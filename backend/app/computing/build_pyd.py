import subprocess
import sys


def run_command(cmd):
    print(f"Run: {cmd}")
    result = subprocess.run(cmd, shell=True)
    if result.returncode != 0:
        print(f"Error: {cmd}")
        sys.exit(result.returncode)


run_command("python setup.py clean --all")
run_command("python setup.py build_ext --build-lib dist")
print("Finish")
