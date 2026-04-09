#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(
            tauri_plugin_log::Builder::default()
                .targets([
                    tauri_plugin_log::Target::new(
                        tauri_plugin_log::TargetKind::Stdout
                    )
                ])
                .level(log::LevelFilter::Info)
                .build()
        )
        .setup(|_app| {
            #[cfg(all(not(debug_assertions), target_os = "windows"))]
            {
                let exe_path = std::env::current_exe()
                    .expect("failed to get current exe path")
                    .parent()
                    .expect("failed to get exe directory")
                    .join("backend.exe");

                Command::new(exe_path)
                    .spawn()
                    .expect("failed to spawn backend.exe");
            }

            Ok(())
        })
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}