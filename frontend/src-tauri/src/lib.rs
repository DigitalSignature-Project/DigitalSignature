use std::process::{Command, Child};
use std::sync::Mutex;
use tauri::{Manager, WindowEvent};

struct BackendProcess(pub Mutex<Option<Child>>);

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .manage(BackendProcess(Mutex::new(None)))
        .setup(|app| {
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

                *app.state::<BackendProcess>().0.lock().unwrap() = Some(child);
            }

            Ok(())
        })
        .on_window_event(|window, event| {
            if let WindowEvent::CloseRequested { api, .. } = event {
                api.prevent_close();

                let app_handle = window.app_handle().clone(); 

                std::thread::spawn(move || {
                    let optional_child = app_handle.state::<BackendProcess>().0.lock().unwrap().take();
                    
                    #[allow(unused_mut)]
                    if let Some(mut child) = optional_child {
                        
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

                    app_handle.exit(0);
                });
            }
        })
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}