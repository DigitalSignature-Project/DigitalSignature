use std::process::{Command, Child};
use std::sync::Mutex;
use tauri::{Manager, WindowEvent}; 

static BACKEND_PROCESS: Mutex<Option<Child>> = Mutex::new(None);

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .setup(|_app| {
            #[cfg(all(not(debug_assertions), target_os = "windows"))]
            {
                let exe_path = std::env::current_exe()
                    .expect("failed to get current exe path")
                    .parent()
                    .expect("failed to get exe directory")
                    .join("backend.exe");

                let child = Command::new(exe_path)
                    .spawn()
                    .expect("failed to spawn backend.exe");

                if let Ok(mut process_lock) = BACKEND_PROCESS.lock() {
                    *process_lock = Some(child);
                }
            }

            Ok(())
        })
        .on_window_event(|window, event| {
            if let WindowEvent::CloseRequested { api, .. } = event {
                api.prevent_close();

                let app_handle = window.app_handle().clone(); 

                std::thread::spawn(move || {
                    if let Ok(mut process_lock) = BACKEND_PROCESS.lock() {
                        #[allow(unused_mut)]
                        if let Some(mut child) = process_lock.take() {
                            
                            #[cfg(target_os = "windows")]
                            {
                                let pid = child.id();
                                let _ = Command::new("taskkill")
                                    .args(["/F", "/T", "/PID", &pid.to_string()])
                                    .output();
                            }

                            #[cfg(not(target_os = "windows"))]
                            {
                                let _ = child.kill();
                                let _ = child.wait();
                            }
                        }
                    }

                    app_handle.exit(0);
                });
            }
        })
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}