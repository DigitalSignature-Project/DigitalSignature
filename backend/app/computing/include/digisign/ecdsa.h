#pragma once
#include <string>
#include <functional>
#include <digisign/BigInt.h>

namespace digisign {

struct CurvePoint;

struct Curve {

// ===============================
//  Curve: secp256k1
// ===============================

    static BigInt p;
    static BigInt a;
    static BigInt b;
    static BigInt n;
    static CurvePoint G;

    static bool contains(const BigInt& x, const BigInt& y);

    static uint64_t p_inv();
    static const BigInt& p_R2();
    static uint64_t nc_inv();
    static const BigInt& n_R2();
    static BigInt to_mont(const BigInt& a);
    static BigInt from_mont(const BigInt& aM);
    static BigInt mod_add_p(const BigInt& a, const BigInt& b);
    static BigInt mod_sub_p(const BigInt& a, const BigInt& b);
    static BigInt mod_mul_p(const BigInt& a, const BigInt& b);

};

struct JacobianPoint {
    BigInt X;
    BigInt Y;
    BigInt Z;
    bool infinity;

    JacobianPoint();

    JacobianPoint(const BigInt& X, const BigInt& Y, const BigInt& Z);

    CurvePoint to_affine() const;

    JacobianPoint dbl() const;

    JacobianPoint operator+(const JacobianPoint& Q) const;

    JacobianPoint operator*(const BigInt& k) const;

};

struct CurvePoint {

    BigInt x;
    BigInt y;
    bool infinity;

    CurvePoint();

    CurvePoint(const BigInt& x, const BigInt& y);

    JacobianPoint to_jacobian() const;

    CurvePoint operator+(const CurvePoint& Q) const;

    CurvePoint dbl() const;

    CurvePoint operator*(const BigInt& k) const;

};

struct ECDSAPublicKey {
    CurvePoint key_public;

    ECDSAPublicKey();

    ECDSAPublicKey(const CurvePoint& key_pub);
};

struct ECDSASignature {

    BigInt r;
    BigInt s;

    ECDSASignature();

    ECDSASignature(const BigInt& r, const BigInt& s);

};

void ECDSA_generate_keys(ECDSAPublicKey& key_pub, BigInt& key_priv);

ECDSASignature ecdsa_sign(const std::string& message, const BigInt& key_priv, std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function);

bool ecdsa_verify(const std::string& message, const ECDSAPublicKey& key_pub, const ECDSASignature& signature, std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function);

bool ecdsa_verify(const std::string& message, const ECDSAPublicKey& key_pub, const std::string& hex_signature, std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function);

ECDSASignature DER_decode_signature_ecdsa(const std::vector<uint8_t>& sig);

ECDSASignature DER_decode_signature_ecdsa(const std::string& hex_sig);

std::vector<uint8_t> DER_encode_signature(const ECDSASignature& sig);

std::string DER_encode_signature_hex(const ECDSASignature& sig);

std::vector<uint8_t> encode_public_key(const ECDSAPublicKey& key_pub, bool compressed = false);

std::string encode_public_key_hex(const ECDSAPublicKey& key_pub, bool compressed = false);

ECDSAPublicKey decode_public_key(const std::vector<uint8_t>& pub_key, bool compressed);

ECDSAPublicKey decode_public_key(const std::string& hex_pub_key, bool compressed);

}
