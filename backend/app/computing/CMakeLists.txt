cmake_minimum_required(VERSION 3.15)
project(DigitalSignature)

set(CMAKE_CXX_STANDARD 20)

# pybind11
find_package(pybind11 REQUIRED)

# OpenSSL
find_package(OpenSSL REQUIRED)

# OpenMP
find_package(OpenMP REQUIRED)

# Python module
pybind11_add_module(DigiSign
    src/rsa.cpp
    src/rng.cpp
    src/prime.cpp
    src/mod_arith.cpp
    src/BigInt.cpp
    src/format.cpp
    src/pss.cpp
    src/sha256.cpp
    src/pybind.cpp
    src/sha3.cpp
)

# include
target_include_directories(DigiSign PRIVATE include)

# OpenSSL OpenMP Link
target_link_libraries(DigiSign PRIVATE OpenSSL::SSL OpenSSL::Crypto OpenMP::OpenMP_CXX)