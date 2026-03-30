# Set up for developers (App frontend)

1. Move into `./frontend` directory.

2. Paste following commands:

- `npm install`
- `npm install tailwindcss @tailwindcss/vite `
- `npm i -D daisyui@latest`
- `npm install --save-dev @tauri-apps/cli`
- `npm install axios`

3. Build frontend:

- `npm run build`

4. Application frontend has been installed correctly, you can now move back and install backend dependencies: [Main manual](../README.md).

### Tips

1. If you want to only run application frontend follow the steps:

- Enter into `./frontend` directory.
- Paste command: `npm run dev`

2. If you want to only run tauri follow the steps:

- Enter into `./frontend` directory.
- Paste command: `npm run build`
- Paste command: `npm run tauri dev`
