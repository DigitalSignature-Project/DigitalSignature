import subprocess
import time
import sys

import requests

backend = subprocess.Popen([sys.executable, "main.py"], cwd="backend")

while True:
    try:
        requests.get("http://127.0.0.1:8000")
        break
    except:
        time.sleep(0.5)

frontend = subprocess.Popen(["npm", "run", "tauri", "dev"], cwd="frontend", shell=True)

# frontend = subprocess.Popen(
#     ["npm", "run", "tauri", "build"],
#     cwd="frontend",
#     shell=True
# )

try:
    frontend.wait()
finally:
    backend.kill()
