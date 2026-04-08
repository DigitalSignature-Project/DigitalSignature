#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <digisign/rsa.h>
#include <digisign/format.h>
#include <digisign/pss.h>

PYBIND11_MODULE(generate_rsa_key, m) {
        pybind11::class_<digisign::BigInt>(m, "BigInt")
        .def(pybind11::init<>())
        .def_readwrite("limbs", &digisign::BigInt::limbs)
        .def_readwrite("used", &digisign::BigInt::used)
        .def("to_hex", &digisign::BigInt::to_hex, "converts BigInt to string in hex format")
        .def("to_base64", &digisign::BigInt::to_base64, "converts BigInt to string in base64 format")
        .def("to_vectoruint8", &digisign::BigInt::to_vectoruint8, "converts BigInt to byte vector");
    m.def("rsa_generate_keys", &digisign::RSA_generate_keys, "rsa_generate_keys");
    m.def("rsa_generate_keys_parallel", &digisign::RSA_generate_keys_parallel, pybind11::call_guard<pybind11::gil_scoped_release>(), "rsa_generate_keys_parallel");
    m.def("encrypt", &digisign::encrypt, "encrypt");
    m.def("decrypt", &digisign::decrypt, "decrypt");
    m.def("bytes_to_hex", &digisign::bytes_to_hex, "converts byte vector to string in hex format");
    m.def("base64_encode", &digisign::base64_encode, "converts byte vector to string in base64 format");
    m.def("pss_encode", &digisign::pss_encode, "creates a rsa-pss sha256 digital signature");
    m.def("verify", &digisign::verify, "verifies rsa-pss sha256 digital signature");
    m.def("vectoruint8", &digisign::BigInt::vectoruint8, "converts to byte vector to BigInt");
    m.def("sign", &digisign::digital_signature, "creates digital signature");
}