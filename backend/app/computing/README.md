# Set up for developers (Computing module)

1. Enter into `./app/backend/computing`
2. Run CMD as admistrator.
3. Enter into `C:/`
4. Run following commands:

- `export PATH="/c/Program Files/CMake/bin:$PATH"`
- `git clone https://github.com/microsoft/vcpkg.git`
- `cd vcpkg`
- `bootstrap-vcpkg.bat`
- `vcpkg install libffi:x64-windows`
- `vcpkg install openssl:x64-windows-static`
- `vcpkg install pybind11:x64-windows-static`

5. After installation process compile C++ files using folowing commands:

- Enter into `./app/backend/computing`
- Delete `./build` directory.
- `cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static`
- `cmake --build build`

6. Application backend has been installed correctly, you can now move back and run app: [Main manual](../../../README.md).
