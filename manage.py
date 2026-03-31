import os
import sys
import subprocess
import platform
import shutil
import json
from pathlib import Path

# Terminal colors
class Colors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'

def print_step(msg):
    print(f"\n{Colors.OKCYAN}==> {msg}{Colors.ENDC}")

def print_success(msg):
    print(f"{Colors.OKGREEN}[SUCCESS] {msg}{Colors.ENDC}")

def print_error(msg):
    print(f"{Colors.FAIL}[ERROR] {msg}{Colors.ENDC}")
    sys.exit(1)

def check_tools():
    tools = ['git', 'cmake', 'npm']
    for tool in tools:
        if shutil.which(tool) is None:
            print_error(f"Tool not found: {tool}. Please install it before continuing.")
    print_success("All required tools are installed.")

def run_cmd(cmd, cwd=None, env=None):
    use_shell = platform.system() == "Windows" and cmd[0] == "npm"
    result = subprocess.run(cmd, cwd=cwd, shell=use_shell, env=env)
    if result.returncode != 0:
        print_error(f"Command {' '.join(cmd)} failed.")

def setup_project():
    print_step("Phase 1/4: Environment and Git verification")
    check_tools()
    
    print("Switching to develop branch...")
    checkout_result = subprocess.run(['git', 'checkout', 'develop'], capture_output=True, text=True)
    
    if checkout_result.returncode != 0:
        print(f"{Colors.WARNING}Local 'develop' branch not found. Attempting to fetch from origin...{Colors.ENDC}")
        run_cmd(['git', 'switch', '-c', 'develop', 'origin/develop'])
        
    run_cmd(['git', 'pull'])

    print_step("Phase 2/4: Backend Configuration (Python)")
    backend_dir = Path('backend')
    venv_dir = backend_dir / '.venv'
    
    if not venv_dir.exists():
        print("Creating virtual environment...")
        run_cmd([sys.executable, '-m', 'venv', str(venv_dir)])
    
    is_windows = platform.system() == "Windows"
    pip_exe = venv_dir / 'Scripts' / 'pip.exe' if is_windows else venv_dir / 'bin' / 'pip'
    
    print("Installing Python dependencies...")
    run_cmd([str(pip_exe), 'install', '-r', str(backend_dir / 'requirements.txt')])

    print_step("Phase 3/4: Frontend Configuration (NPM)")
    run_cmd(['npm', 'install'], cwd='frontend')

    print_step("Phase 4/4: Generating VS Code Configuration (F5)")
    vscode_dir = Path('.vscode')
    vscode_dir.mkdir(exist_ok=True)

    python_exe_path = "${workspaceFolder}/backend/.venv/Scripts/python.exe" if is_windows else "${workspaceFolder}/backend/.venv/bin/python"

    tasks_json = {
        "version": "2.0.0",
        "tasks": [
            {
                "label": "Build project (C++ and Frontend)",
                "type": "shell",
                "command": sys.executable,
                "args": ["manage.py", "build"],
                "presentation": {
                    "reveal": "always",
                    "panel": "shared"
                }
            }
        ]
    }

    launch_json = {
        "version": "0.2.0",
        "configurations": [
            {
                "name": "Run Application (F5)",
                "type": "debugpy",
                "request": "launch",
                "program": "${workspaceFolder}/run_app.py",
                "console": "integratedTerminal",
                "preLaunchTask": "Build project (C++ and Frontend)",
                "python": python_exe_path
            }
        ]
    }

    with open(vscode_dir / 'tasks.json', 'w', encoding='utf-8') as f:
        json.dump(tasks_json, f, indent=4, ensure_ascii=False)
    
    with open(vscode_dir / 'launch.json', 'w', encoding='utf-8') as f:
        json.dump(launch_json, f, indent=4, ensure_ascii=False)

    print_success("Configuration complete! You can now press F5 in VS Code.")

def build_project():
    print_step("Compilation: Building C++ module")
    
    cmake_file = next(Path('.').rglob('computing/CMakeLists.txt'), None)
    if not cmake_file:
        cmake_file = next(Path('.').rglob('*computing*/CMakeLists.txt'), None)
        if not cmake_file:
            print_error("CMakeLists.txt for C++ module not found!")
    
    computing_dir = cmake_file.parent

    vcpkg_root = os.environ.get('VCPKG_ROOT')
    is_windows = platform.system() == "Windows"
    
    if not vcpkg_root:
        default_vcpkg = "C:\\vcpkg" if is_windows else f"{os.environ.get('HOME')}/vcpkg"
        print(f"{Colors.WARNING}VCPKG_ROOT variable is missing. Attempting to use: {default_vcpkg}{Colors.ENDC}")
        vcpkg_root = default_vcpkg

    vcpkg_toolchain = Path(vcpkg_root) / "scripts" / "buildsystems" / "vcpkg.cmake"
    if not vcpkg_toolchain.exists():
        print_error(f"VCPKG toolchain file not found: {vcpkg_toolchain}")

    build_dir = computing_dir / "build"
    build_dir.mkdir(exist_ok=True) 

    clean_env = os.environ.copy()
    if is_windows:
        clean_env["PATH"] = ";".join([p for p in clean_env.get("PATH", "").split(";") if "msys64" not in p.lower()])

    triplet = "x64-windows" if is_windows else "x64-linux"

    vcpkg_exe = Path(vcpkg_root) / ("vcpkg.exe" if is_windows else "vcpkg")
    if vcpkg_exe.exists():
        print_step(f"Checking/Installing C++ dependencies for triplet: {triplet}...")
        run_cmd([str(vcpkg_exe), 'install', f'pybind11:{triplet}'])
    else:
        print(f"{Colors.WARNING}vcpkg executable not found in {vcpkg_root}. Skipping auto-installation.{Colors.ENDC}")

    if not (build_dir / "CMakeCache.txt").exists():
        print("Configuring CMake for the first time or after a clean...")
        cmake_config_cmd = [
            'cmake', '-S', str(computing_dir), '-B', str(build_dir),
            f'-DCMAKE_TOOLCHAIN_FILE={vcpkg_toolchain}',
            f'-DVCPKG_TARGET_TRIPLET={triplet}',
            '-DPYBIND11_FINDPYTHON=ON', 
            f'-DPython_EXECUTABLE={sys.executable}'
        ]
        run_cmd(cmake_config_cmd, env=clean_env)

    print("Building CMake targets...")
    run_cmd(['cmake', '--build', str(build_dir), '--config', 'Release'], env=clean_env)

    dist_dir = Path('dist')
    dist_dir.mkdir(exist_ok=True)
    
    ext = "*.pyd" if is_windows else "*.so"
    for compiled_file in build_dir.rglob(ext):
        shutil.copy(compiled_file, dist_dir)
        print_success(f"Copied module: {compiled_file.name} to dist/ directory")

    print_step("Compilation: Building Frontend (Vite/React)")
    run_cmd(['npm', 'run', 'build'], cwd='frontend')
    
    print_success("Build process completed successfully!")

def run_project():
    """Nowa funkcja do uruchamiania aplikacji z poprawnego środowiska."""
    print_step("Starting the application...")
    
    is_windows = platform.system() == "Windows"
    python_exe = Path('backend') / '.venv' / ('Scripts' if is_windows else 'bin') / ('python.exe' if is_windows else 'python')
    
    if not python_exe.exists():
        print_error("Virtual environment not found! Please run 'python manage.py setup' first.")
        
    # Zakładam, że Twój główny plik nazywa się run_app.py zgodnie z poprzednimi logami
    app_script = "run_app.py"
    if not Path(app_script).exists():
        print_error(f"Cannot find the entry point: {app_script}")
        
    print(f"{Colors.OKGREEN}Running {app_script}... (Press CTRL+C to quit){Colors.ENDC}\n")
    try:
        # Uruchamiamy serwer bezpośrednio z .venv bez konieczności jego ręcznej aktywacji
        subprocess.run([str(python_exe), app_script])
    except KeyboardInterrupt:
        print(f"\n{Colors.WARNING}Application stopped by user.{Colors.ENDC}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python manage.py [setup|build|run]")
        sys.exit(1)

    command = sys.argv[1].lower()

    if command == "setup":
        setup_project()
    elif command == "build":
        build_project()
    elif command == "run":
        run_project()
    else:
        print_error("Unknown command. Use 'setup', 'build', or 'run'.")