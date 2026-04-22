#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <digisign/rsa.h>
#include <digisign/format.h>
#include <digisign/pss.h>
#include <digisign/sha256.h>


void bind_bigint(pybind11::module_ &m) {
    pybind11::class_<digisign::BigInt>(m, "BigInt")
    .def(pybind11::init<>())
    .def_readwrite("limbs", &digisign::BigInt::limbs)
    .def_readwrite("used", &digisign::BigInt::used)
    .def("to_hex", &digisign::BigInt::to_hex, "converts BigInt to string in hex format", pybind11::arg("remove_leading_zeros") = false)
    .def("to_base64", &digisign::BigInt::to_base64, "converts BigInt to string in base64 format")
    .def("to_bytes", &digisign::BigInt::to_vectoruint8, "converts BigInt to byte vector")
    .def_static("from_bytes", &digisign::BigInt::vectoruint8, "converts byte vector to BigInt")
    .def_static("from_hex", &digisign::BigInt::from_hex, "converts string in hex format to BigInt")
    .def_static("from_base64", &digisign::BigInt::from_base64, "converts string in base64 format to BigInt");
}

void bind_rsa(pybind11::module_ &rsa) {
    pybind11::class_<digisign::PSSConfig>(rsa, "PSSConfig")
    .def(pybind11::init([](int salt_length, pybind11::function hash_function, pybind11::function MGF1) {

        std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_func = [hash_function](const std::vector<uint8_t>& v) {
            pybind11::object result = hash_function(v);
            return result.cast<std::vector<uint8_t>>();
        };

        std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> MGF1_hash = [MGF1](const std::vector<uint8_t>& v) {
            pybind11::object result = MGF1(v);
            return result.cast<std::vector<uint8_t>>();
        };

        return digisign::PSSConfig(salt_length, hash_func, MGF1_hash);
    }),
    pybind11::arg("salt_length"),
    pybind11::arg("hash_function"),
    pybind11::arg("MGF1")    
    );

    rsa.def("rsa_generate_keys", &digisign::RSA_generate_keys, "rsa_generate_keys");
    rsa.def("rsa_generate_keys_parallel", &digisign::RSA_generate_keys_parallel, pybind11::call_guard<pybind11::gil_scoped_release>(), "rsa_generate_keys_parallel");
    rsa.def("encrypt", &digisign::encrypt, "encrypt");
    rsa.def("decrypt", &digisign::decrypt, "decrypt");
    rsa.def("pss_encode", &digisign::pss_encode, "creates a rsa-pss sha256 digital signature");
    rsa.def("verify", &digisign::verify, "verifies rsa-pss sha256 digital signature");
    rsa.def("sign", &digisign::digital_signature, "creates digital signature");
}

void bind_hash(pybind11::module_ &hash) {
    hash.def("SHA256", pybind11::overload_cast<const std::string&>(&digisign::sha256), "SHA256 hash function for string");
    hash.def("SHA256", pybind11::overload_cast<const std::vector<uint8_t>&>(&digisign::sha256), "SHA256 hash function for bytes");
}

void bind_format(pybind11::module_ &format) {
    format.def("bytes_to_hex", &digisign::bytes_to_hex, "converts byte vector to string in hex format");
    format.def("bytes_to_base64", &digisign::base64_encode, "converts byte vector to string in base64 format");
    format.def("hex_to_bytes", &digisign::base64_encode, "converts string in hex format to byte vector");
    format.def("base64_to_bytes", &digisign::base64_encode, "converts string in base64 format to byte vector");
}


PYBIND11_MODULE(DigiSign, m) {
    auto rsa = m.def_submodule("RSA");
    auto hash = m.def_submodule("HASH");
    auto format = m.def_submodule("Format"); 

    bind_bigint(m);
    bind_rsa(rsa);
    bind_hash(hash);
    bind_format(format);
}