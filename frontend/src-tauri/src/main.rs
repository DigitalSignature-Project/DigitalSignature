#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

use keyring::Entry;
use tauri::command;

#[command]
fn save_credentials(login: &str, passphrase: &str) -> Result<(), String> {
    let entry = Entry::new("SecureSignApp", login).map_err(|e| e.to_string())?;
    entry.set_password(passphrase).map_err(|e| e.to_string())?;
    Ok(())
}

#[command]
fn get_credentials(login: &str) -> Result<String, String> {
    let entry = Entry::new("SecureSignApp", login).map_err(|e| e.to_string())?;
    let password = entry.get_password().map_err(|e| e.to_string())?;
    Ok(password)
}

#[command]
fn delete_credentials(login: &str) -> Result<(), String> {
    let entry = Entry::new("SecureSignApp", login).map_err(|e| e.to_string())?;
    entry.delete_password().map_err(|e| e.to_string())?;
    Ok(())
}

fn main() {
    app_lib::run();
    tauri::Builder::default()
        .plugin(tauri_plugin_fs::init())
        .plugin(tauri_plugin_dialog::init())
        .invoke_handler(tauri::generate_handler![
            save_credentials,
            get_credentials,
            delete_credentials
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
