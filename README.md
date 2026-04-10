# DigitalSignature

Application for creating a digital signature

# Table of content

- [Project set up for developers](#set-up-for-developers)

# Set up for developers

1. Clone git repository using following command: `git clone https://github.com/DigitalSignature-Project/DigitalSignature.git`

2. Download and install interpreter for `Python 3.12.0` from: `https://www.python.org/downloads/`.

3. Download and install Rust from: `https://rust-lang.org/tools/install/`.

4. Download Node.js from: `https://nodejs.org/en/download`.

5. Download CMake from: `https://cmake.org/download/`.

6. Download Visual Studio BuildTools with powershell command: `winget install Microsoft.VisualStudio.2022.BuildTools --silent --override "--wait --quiet --add Microsoft.VisualStudio.Workload.NativeDesktop --includeRecommended" `

7. Install all dependencies for application frontend. For more details check this manual: [Frontend manual](frontend/README.md)

8. Install all dependencies for application backend. For more details check this manual: [Backend manual](backend/README.md)

# Alternative setup for developers
# After completing the first 6 steps in the instructions above.

1. Install requests module for python: `pip install requests`

2. Use the `manage.py` script to install the remaining dependencies and run the project using the following commands:

- `python manage.py setup` - this command installs all necessary dependencies.
- `python manage.py build` - this command installs the developer version of the application.
- `python manage.py run` - this command launch application.
- `python manage.py prod` - this command builds utility applications
