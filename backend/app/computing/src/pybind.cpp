#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <digisign/rsa.h>
#include <digisign/format.h>
#include <digisign/pss.h>
#include <digisign/sha256.h>
#include <digisign/sha3.h>
#include <digisign/elgamal.h>


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

    rsa.def("rsa_generate_keys", &digisign::RSA_generate_keys, "generates a pair of RSA keys with common module n");
    rsa.def("rsa_generate_keys_parallel", &digisign::RSA_generate_keys_parallel, pybind11::call_guard<pybind11::gil_scoped_release>(), "generates a pair of RSA keys with common module n with parallel processing");
    rsa.def("encrypt", &digisign::encrypt, "encrypts message using RSA keys");
    rsa.def("decrypt", &digisign::decrypt, "decrypt message using RSA keys");
    rsa.def("verify", pybind11::overload_cast<const std::string&, const std::vector<uint8_t>&, const digisign::BigInt&, const digisign::BigInt&, const digisign::PSSConfig&>(&digisign::verify), "verifies RSA-pss digital signature");
    rsa.def("verify", pybind11::overload_cast<const std::string&, const std::string&, const digisign::BigInt&, const digisign::BigInt&, const digisign::PSSConfig&>(&digisign::verify), "verifies RSA-pss digital signature in hex format");
    rsa.def("sign", &digisign::digital_signature, "creates RSA-pss digital signature");
}

void bind_hash(pybind11::module_ &hash) {
    hash.def("SHA256", pybind11::overload_cast<const std::string&>(&digisign::sha256), "SHA256 hash function for string");
    hash.def("SHA256", pybind11::overload_cast<const std::vector<uint8_t>&>(&digisign::sha256), "SHA256 hash function for bytes");
    hash.def("SHA3_256", pybind11::overload_cast<const std::string&>(&digisign::sha3_256), "SHA3-256 hash function for string");
    hash.def("SHA3_256", pybind11::overload_cast<const std::vector<uint8_t>&>(&digisign::sha3_256), "SHA3-256 hash function for bytes");
    hash.def("SHA3_512", pybind11::overload_cast<const std::string&>(&digisign::sha3_512), "SHA3-512 hash function for string");
    hash.def("SHA3_512", pybind11::overload_cast<const std::vector<uint8_t>&>(&digisign::sha3_512), "SHA3-512 hash function for bytes");
}

void bind_format(pybind11::module_ &format) {
    format.def("bytes_to_hex", &digisign::bytes_to_hex, "converts byte vector to string in hex format");
    format.def("bytes_to_base64", &digisign::base64_encode, "converts byte vector to string in base64 format");
    format.def("hex_to_bytes", &digisign::base64_encode, "converts string in hex format to byte vector");
    format.def("base64_to_bytes", &digisign::base64_encode, "converts string in base64 format to byte vector");
}

void bind_elgamal(pybind11::module_ &elgamal) {
    pybind11::class_<digisign::ElGamalPublicKey>(elgamal, "ElGamalPublicKey")
    .def(pybind11::init<>())
    .def(pybind11::init([](digisign::BigInt p, digisign::BigInt q, digisign::BigInt g, digisign::BigInt y) {
        return digisign::ElGamalPublicKey(p, q, g, y);}),
        pybind11::arg("p"),
        pybind11::arg("q"),
        pybind11::arg("g"),
        pybind11::arg("y"))
    .def_readwrite("p", &digisign::ElGamalPublicKey::p)
    .def_readwrite("q", &digisign::ElGamalPublicKey::q)
    .def_readwrite("g", &digisign::ElGamalPublicKey::g)
    .def_readwrite("y", &digisign::ElGamalPublicKey::y);

    pybind11::class_<digisign::ElGamalSignature>(elgamal, "ElGamalSignature")
    .def(pybind11::init<>())
    .def(pybind11::init([](digisign::BigInt r, digisign::BigInt s) {
        return digisign::ElGamalSignature(r, s);}),
        pybind11::arg("r"),
        pybind11::arg("s"))
    .def_readwrite("r", &digisign::ElGamalSignature::r)
    .def_readwrite("s", &digisign::ElGamalSignature::s);

    elgamal.def("ElGamal_generate_keys", &digisign::ElGamal_generate_keys, "generates ElGamal keys");
    elgamal.def("ElGamal_generate_keys_parallel", &digisign::ElGamal_generate_keys_parallel, pybind11::call_guard<pybind11::gil_scoped_release>(), "generates ElGamal keys with parallel processing");
    elgamal.def("sign",
         [](const std::string& message,
            const digisign::ElGamalPublicKey& key_pub,
            const digisign::BigInt& key_priv,
            pybind11::function hash_function
        ) {
            std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash = [hash_function](std::vector<uint8_t> v) {
                pybind11::object result = hash_function(v);
                return result.cast<std::vector<uint8_t>>();
            };

            return digisign::elgamal_sign(message, key_pub, key_priv, hash);
        },
         "creates ElGamal digital signature");
    elgamal.def("verify",
         [](const std::string& message,
            const digisign::ElGamalPublicKey& key_pub,
            const digisign::ElGamalSignature& sign,
            pybind11::function hash_function
        ) {
            std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash = [hash_function](std::vector<uint8_t> v) {
                pybind11::object result = hash_function(v);
                return result.cast<std::vector<uint8_t>>();
            };

            return digisign::elgamal_verify(message, key_pub, sign, hash);
        },
          "verifies ElGamal digital signature");
    elgamal.def("verify",
         [](const std::string& message,
            const digisign::ElGamalPublicKey& key_pub,
            const std::string& hex_signature,
            pybind11::function hash_function
        ) {
            std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash = [hash_function](std::vector<uint8_t> v) {
                pybind11::object result = hash_function(v);
                return result.cast<std::vector<uint8_t>>();
            };

            return digisign::elgamal_verify(message, key_pub, hex_signature, hash);
        },
          "verifies ElGamal digital signature in DER structure and hex format");
    elgamal.def("DER_decode_signature", pybind11::overload_cast<const std::vector<uint8_t>&>(&digisign::DER_decode_signature), "decodes ElGamal in DER format from bytes to signature");
    elgamal.def("DER_decode_signature", pybind11::overload_cast<const std::string&>(&digisign::DER_decode_signature), "decodes ElGamal in DER format from hex string to signature");
    elgamal.def("DER_encode_signature", &digisign::DER_encode_signature, "encodes ElGamal digital signature to DER format in bytes");
    elgamal.def("DER_encode_signature_hex", &digisign::DER_encode_signature_hex, "encodes ElGamal digital signature to DER format in hex string");
}


PYBIND11_MODULE(DigiSign, m) {
    auto rsa = m.def_submodule("RSA");
    auto hash = m.def_submodule("HASH");
    auto format = m.def_submodule("Format"); 
    auto elgamal = m.def_submodule("ElGamal");

    bind_bigint(m);
    bind_rsa(rsa);
    bind_hash(hash);
    bind_format(format);
    bind_elgamal(elgamal);
}