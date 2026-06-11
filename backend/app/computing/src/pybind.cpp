#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <digisign/rsa.h>
#include <digisign/format.h>
#include <digisign/pss.h>
#include <digisign/sha256.h>
#include <digisign/sha3.h>
#include <digisign/elgamal.h>
#include <digisign/ecdsa.h>

// ============================================================
//  BigInt bindings
//  Exposes arbitrary-precision integer operations to Python.
//  Provides conversions between hex, base64, bytes and BigInt.
// ============================================================
void bind_bigint(pybind11::module_ &m) {
    pybind11::class_<digisign::BigInt>(m, "BigInt")
        .def(pybind11::init<>(), "Creates an empty BigInt")
        .def_readwrite("limbs", &digisign::BigInt::limbs,
                       "Internal limb representation (debugging only)")
        .def_readwrite("used", &digisign::BigInt::used,
                       "Number of limbs currently used")
        .def("to_hex", &digisign::BigInt::to_hex,
             "Convert BigInt to hex string",
             pybind11::arg("remove_leading_zeros") = false)
        .def("to_base64", &digisign::BigInt::to_base64,
             "Convert BigInt to Base64 string")
        .def("to_bytes", &digisign::BigInt::to_vectoruint8,
             "Convert BigInt to a Python byte array")
        .def_static("from_bytes", &digisign::BigInt::vectoruint8,
                    "Create BigInt from a byte array")
        .def_static("from_hex", &digisign::BigInt::from_hex,
                    "Create BigInt from a hex string")
        .def_static("from_base64", &digisign::BigInt::from_base64,
                    "Create BigInt from a Base64 string");
}

// ============================================================
//  RSA bindings
//  Exposes RSA key generation, encryption, decryption,
//  and RSA-PSS signing/verification to Python.
// ============================================================
void bind_rsa(pybind11::module_ &rsa) {

    // PSS configuration wrapper
    pybind11::class_<digisign::PSSConfig>(rsa, "PSSConfig")
        .def(pybind11::init([](int salt_length,
                               pybind11::function hash_function,
                               pybind11::function MGF1) {

            // Convert Python callables → std::function
            std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_func =
                [hash_function](const std::vector<uint8_t>& v) {
                    pybind11::object result = hash_function(v);
                    return result.cast<std::vector<uint8_t>>();
                };

            std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> MGF1_hash =
                [MGF1](const std::vector<uint8_t>& v) {
                    pybind11::object result = MGF1(v);
                    return result.cast<std::vector<uint8_t>>();
                };

            return digisign::PSSConfig(salt_length, hash_func, MGF1_hash);
        }),
        pybind11::arg("salt_length"),
        pybind11::arg("hash_function"),
        pybind11::arg("MGF1"));

    // RSA API exposed to Python
    rsa.def("rsa_generate_keys", &digisign::RSA_generate_keys,
            "Generate RSA keypair");
    rsa.def("rsa_generate_keys_parallel", &digisign::RSA_generate_keys_parallel,
            pybind11::call_guard<pybind11::gil_scoped_release>(),
            "Generate RSA keypair using parallel processing");

    rsa.def("encrypt", &digisign::encrypt, "Encrypt message using RSA");
    rsa.def("decrypt", &digisign::decrypt, "Decrypt RSA ciphertext");

    rsa.def("verify",
            pybind11::overload_cast<const std::string&, const std::vector<uint8_t>&,
                                    const digisign::BigInt&, const digisign::BigInt&,
                                    const digisign::PSSConfig&>(&digisign::verify),
            "Verify RSA-PSS signature");

    rsa.def("verify",
            pybind11::overload_cast<const std::string&, const std::string&,
                                    const digisign::BigInt&, const digisign::BigInt&,
                                    const digisign::PSSConfig&>(&digisign::verify),
            "Verify RSA-PSS signature (hex format)");

    rsa.def("sign", &digisign::digital_signature,
            "Create RSA-PSS digital signature");
}

// ============================================================
//  Hash bindings
//  Exposes SHA-256, SHA3-256, SHA3-512 to Python.
// ============================================================
void bind_hash(pybind11::module_ &hash) {
    hash.def("SHA256", pybind11::overload_cast<const std::string&>(&digisign::sha256),
             "SHA256 hash of string");
    hash.def("SHA256", pybind11::overload_cast<const std::vector<uint8_t>&>(&digisign::sha256),
             "SHA256 hash of bytes");

    hash.def("SHA3_256", pybind11::overload_cast<const std::string&>(&digisign::sha3_256),
             "SHA3-256 hash of string");
    hash.def("SHA3_256", pybind11::overload_cast<const std::vector<uint8_t>&>(&digisign::sha3_256),
             "SHA3-256 hash of bytes");

    hash.def("SHA3_512", pybind11::overload_cast<const std::string&>(&digisign::sha3_512),
             "SHA3-512 hash of string");
    hash.def("SHA3_512", pybind11::overload_cast<const std::vector<uint8_t>&>(&digisign::sha3_512),
             "SHA3-512 hash of bytes");
}

// ============================================================
//  Format bindings
//  Exposes hex/base64 encoding and decoding utilities.
// ============================================================
void bind_format(pybind11::module_ &format) {
    format.def("bytes_to_hex", &digisign::bytes_to_hex,
               "Convert byte vector to hex string");
    format.def("bytes_to_base64", &digisign::base64_encode,
               "Convert byte vector to Base64 string");

    format.def("hex_to_bytes", &digisign::hex_to_bytes,
               "Convert hex string to bytes");
    format.def("base64_to_bytes", &digisign::base64_to_bytes,
               "Convert Base64 string to bytes");
}

// ============================================================
//  ElGamal bindings
//  Exposes key generation, signing, verification,
//  and DER encoding/decoding to Python.
// ============================================================
void bind_elgamal(pybind11::module_ &elgamal) {

    // Public key
    pybind11::class_<digisign::ElGamalPublicKey>(elgamal, "ElGamalPublicKey")
        .def(pybind11::init<>())
        .def(pybind11::init([](digisign::BigInt p, digisign::BigInt q,
                               digisign::BigInt g, digisign::BigInt y) {
            return digisign::ElGamalPublicKey(p, q, g, y);
        }),
        pybind11::arg("p"), pybind11::arg("q"),
        pybind11::arg("g"), pybind11::arg("y"))
        .def_readwrite("p", &digisign::ElGamalPublicKey::p)
        .def_readwrite("q", &digisign::ElGamalPublicKey::q)
        .def_readwrite("g", &digisign::ElGamalPublicKey::g)
        .def_readwrite("y", &digisign::ElGamalPublicKey::y);

    // Signature
    pybind11::class_<digisign::ElGamalSignature>(elgamal, "ElGamalSignature")
        .def(pybind11::init<>())
        .def(pybind11::init([](digisign::BigInt r, digisign::BigInt s) {
            return digisign::ElGamalSignature(r, s);
        }),
        pybind11::arg("r"), pybind11::arg("s"))
        .def_readwrite("r", &digisign::ElGamalSignature::r)
        .def_readwrite("s", &digisign::ElGamalSignature::s);

    // Key generation
    elgamal.def("ElGamal_generate_keys", &digisign::ElGamal_generate_keys,
                "Generate ElGamal keypair");
    elgamal.def("ElGamal_generate_keys_parallel",
                &digisign::ElGamal_generate_keys_parallel,
                pybind11::call_guard<pybind11::gil_scoped_release>(),
                "Generate ElGamal keypair using parallel processing");

    // Signing
    elgamal.def("sign",
        [](const std::string& message,
           const digisign::ElGamalPublicKey& key_pub,
           const digisign::BigInt& key_priv,
           pybind11::function hash_function) {

            std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash =
                [hash_function](std::vector<uint8_t> v) {
                    pybind11::object result = hash_function(v);
                    return result.cast<std::vector<uint8_t>>();
                };

            return digisign::elgamal_sign(message, key_pub, key_priv, hash);
        },
        "Create ElGamal digital signature");

    // Verification
    elgamal.def("verify",
        [](const std::string& message,
           const digisign::ElGamalPublicKey& key_pub,
           const digisign::ElGamalSignature& sign,
           pybind11::function hash_function) {

            std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash =
                [hash_function](std::vector<uint8_t> v) {
                    pybind11::object result = hash_function(v);
                    return result.cast<std::vector<uint8_t>>();
                };

            return digisign::elgamal_verify(message, key_pub, sign, hash);
        },
        "Verify ElGamal digital signature");

    elgamal.def("verify",
        [](const std::string& message,
           const digisign::ElGamalPublicKey& key_pub,
           const std::string& hex_signature,
           pybind11::function hash_function) {

            std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash =
                [hash_function](std::vector<uint8_t> v) {
                    pybind11::object result = hash_function(v);
                    return result.cast<std::vector<uint8_t>>();
                };

            return digisign::elgamal_verify(message, key_pub, hex_signature, hash);
        },
        "Verify ElGamal signature (DER hex format)");

    // DER encoding/decoding
    elgamal.def("DER_decode_signature",
                pybind11::overload_cast<const std::vector<uint8_t>&>(&digisign::DER_decode_signature),
                "Decode ElGamal signature from DER bytes");
    elgamal.def("DER_decode_signature",
                pybind11::overload_cast<const std::string&>(&digisign::DER_decode_signature),
                "Decode ElGamal signature from DER hex");
    elgamal.def("DER_encode_signature",
                pybind11::overload_cast<const digisign::ElGamalSignature&>(&digisign::DER_encode_signature),
                "Encode ElGamal signature to DER bytes");
    elgamal.def("DER_encode_signature_hex",
                pybind11::overload_cast<const digisign::ElGamalSignature&>(&digisign::DER_encode_signature_hex),
                "Encode ElGamal signature to DER hex");
}

// ============================================================
//  ECDSA bindings
//  Exposes secp256k1 key generation, signing, verification,
//  and SEC1 public key encoding/decoding.
// ============================================================
void bind_ecdsa(pybind11::module_ &ecdsa) {

    // Public key
    pybind11::class_<digisign::ECDSAPublicKey>(ecdsa, "PublicKey")
        .def(pybind11::init<>())
        .def(pybind11::init([](digisign::BigInt x, digisign::BigInt y) {
            return digisign::ECDSAPublicKey(digisign::CurvePoint(x, y));
        }),
        pybind11::arg("x"), pybind11::arg("y"))
        // Curve point X coordinatre
        .def_property_readonly("x",
        [](const digisign::ECDSAPublicKey& self) {
            return self.key_public.x;
        })
        // Curve point Y coordinatre
        .def_property_readonly("y",
        [](const digisign::ECDSAPublicKey& self) {
            return self.key_public.y;
        });

    // Signature
    pybind11::class_<digisign::ECDSASignature>(ecdsa, "Signature")
        .def(pybind11::init<>())
        .def(pybind11::init([](digisign::BigInt r, digisign::BigInt s) {
            return digisign::ECDSASignature(r, s);
        }),
        pybind11::arg("r"), pybind11::arg("s"))
        .def_readwrite("r", &digisign::ECDSASignature::r)
        .def_readwrite("s", &digisign::ECDSASignature::s);

    // Key generation
    ecdsa.def("ecdsa_generate_keys", &digisign::ECDSA_generate_keys,
              "Generate ECDSA keypair");

    // Signing
    ecdsa.def("sign",
        [](const std::string& message,
           const digisign::BigInt& key_priv,
           pybind11::function hash_function) {

            std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash =
                [hash_function](const std::vector<uint8_t>& v) {
                    pybind11::object result = hash_function(v);
                    return result.cast<std::vector<uint8_t>>();
                };

            return digisign::ecdsa_sign(message, key_priv, hash);
        },
        "Create ECDSA digital signature");

    // Verification
    ecdsa.def("verify",
        [](const std::string& message,
           const digisign::ECDSAPublicKey& key_pub,
           const digisign::ECDSASignature& signature,
           pybind11::function hash_function) {

            std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash =
                [hash_function](const std::vector<uint8_t>& v) {
                    pybind11::object result = hash_function(v);
                    return result.cast<std::vector<uint8_t>>();
                };

            return digisign::ecdsa_verify(message, key_pub, signature, hash);
        },
        "Verify ECDSA digital signature");

    ecdsa.def("verify",
        [](const std::string& message,
           const digisign::ECDSAPublicKey& key_pub,
           const std::string& hex_signature,
           pybind11::function hash_function) {

            std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash =
                [hash_function](const std::vector<uint8_t>& v) {
                    pybind11::object result = hash_function(v);
                    return result.cast<std::vector<uint8_t>>();
                };

            return digisign::ecdsa_verify(message, key_pub, hex_signature, hash);
        },
        "Verify ECDSA signature (DER hex format)");

    // DER encoding/decoding
    ecdsa.def("DER_decode_signature",
              pybind11::overload_cast<const std::vector<uint8_t>&>(&digisign::DER_decode_signature_ecdsa),
              "Decode ECDSA signature from DER bytes");
    ecdsa.def("DER_decode_signature",
              pybind11::overload_cast<const std::string&>(&digisign::DER_decode_signature_ecdsa),
              "Decode ECDSA signature from DER hex");
    ecdsa.def("DER_encode_signature",
              pybind11::overload_cast<const digisign::ECDSASignature&>(&digisign::DER_encode_signature),
              "Encode ECDSA signature to DER bytes");
    ecdsa.def("DER_encode_signature_hex",
              pybind11::overload_cast<const digisign::ECDSASignature&>(&digisign::DER_encode_signature_hex),
              "Encode ECDSA signature to DER hex");

    // SEC1 public key encoding/decoding
    ecdsa.def("encode_public_key", &digisign::encode_public_key,
              pybind11::arg("public_key"),
              pybind11::arg("compressed") = false,
              "Encode public key to SEC1 byte format");

    ecdsa.def("encode_public_key_hex", &digisign::encode_public_key_hex,
              pybind11::arg("public_key"),
              pybind11::arg("compressed") = false,
              "Encode public key to SEC1 hex format");

    ecdsa.def("decode_public_key",
              pybind11::overload_cast<const std::vector<uint8_t>&, bool>(&digisign::decode_public_key),
              pybind11::arg("pub_bytes"),
              pybind11::arg("compressed"),
              "Decode SEC1 public key from bytes");

    ecdsa.def("decode_public_key",
              pybind11::overload_cast<const std::string&, bool>(&digisign::decode_public_key),
              pybind11::arg("pub_hex"),
              pybind11::arg("compressed"),
              "Decode SEC1 public key from hex");
}

// ============================================================
//  DigiSign module initialization
// ============================================================
PYBIND11_MODULE(DigiSign, m) {
    auto rsa = m.def_submodule("RSA");
    auto hash = m.def_submodule("HASH");
    auto format = m.def_submodule("Format"); 
    auto elgamal = m.def_submodule("ElGamal");
    auto ecdsa = m.def_submodule("ECDSA");

    bind_bigint(m);
    bind_rsa(rsa);
    bind_hash(hash);
    bind_format(format);
    bind_elgamal(elgamal);
    bind_ecdsa(ecdsa);
}