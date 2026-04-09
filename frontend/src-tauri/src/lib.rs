use std::process::{Command, Child};
use std::sync::{Arc, Mutex}; // Dodano import Arc
use tauri::{Manager, WindowEvent};

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .setup(|app| {
            #[cfg(all(not(debug_assertions), target_os = "windows"))]
            {
                let exe_path = std::env::current_exe()
                    .expect("failed to get current exe path")
                    .parent()
                    .expect("failed to get exe directory")
                    .join("backend.exe");

                let child: Child = Command::new(exe_path)
                    .spawn()
                    .expect("failed to spawn backend.exe");

                // KROK 1: Mutex musi być owinięty w Arc, abyśmy mogli bezpiecznie go przenosić między wątkami
                app.manage(Arc::new(Mutex::new(Some(child))));
            }

            Ok(())
        })
        .on_window_event(|window, event| {
            if let WindowEvent::CloseRequested { api, .. } = event {
                // Wstrzymujemy natychmiastowe zamknięcie okna (aby uniknąć osierocenia backendu)
                api.prevent_close();

                let state = window.app_handle()
                    .state::<Arc<Mutex<Option<Child>>>>()
                    .inner()
                    .clone();

                let app_handle = window.app_handle().clone(); 

                std::thread::spawn(move || {
                    // Zabijamy backend
                    if let Ok(mut backend_lock) = state.lock() {
                        if let Some(mut child) = backend_lock.take() {
                            
                            #[cfg(target_os = "windows")]
                            {
                                // Pobieramy ID procesu (PID)
                                let pid = child.id();
                                
                                // Używamy taskkill, aby wymusić zamknięcie całego drzewa procesów
                                let _ = Command::new("taskkill")
                                    .args(["/F", "/T", "/PID", &pid.to_string()])
                                    .output();
                            }

                            #[cfg(not(target_os = "windows"))]
                            {
                                // Standardowe zabijanie dla systemów Unix/Linux/Mac
                                let _ = child.kill();
                                let _ = child.wait();
                            }
                        }
                    }

                    // Zamykamy aplikację poprawnie
                    app_handle.exit(0);
                });
            }
        })
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}