#include <digisign/ecdsa.h>
#include <stdexcept>
#include <digisign/mod_arith.h>
#include <digisign/rng.h>
#include <digisign/format.h>

namespace digisign {

// p = 2^256 - 2^32 - 977
BigInt Curve::p = BigInt::from_hex("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F");

// a = 0
BigInt Curve::a = BigInt::from_hex("0");

// b = 7
BigInt Curve::b = BigInt::from_hex("7");

// n = Order of point G
BigInt Curve::n = BigInt::from_hex("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141");

// G Generator 
CurvePoint Curve::G(
    BigInt::from_hex("0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798"),
    BigInt::from_hex("0x483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8")
);

bool Curve::contains(const BigInt& x, const BigInt& y) {
    BigInt left = (y * y) % p;
    BigInt right = (x * x * x + a * x + b) % p;
    right = right % p;
    return left == right;
}

uint64_t Curve::p_inv() {
    static uint64_t p_inv = n_inv(Curve::p, 6);
    return p_inv;
}

const BigInt& Curve::p_R2() {
    static const BigInt r = []() {
        size_t p_limbs = p.used;
        BigInt R((p_limbs + 1) * 64);
        R.limbs[p_limbs] = 1;
        R.used = p_limbs + 1;
        return (R * R) % p;
    }();
    return r;
}

uint64_t Curve::nc_inv() {
    static uint64_t nc_inv = n_inv(Curve::n, 6);
    return nc_inv;
}

const BigInt& Curve::n_R2() {
    static const BigInt r = []() {
        size_t n_limbs = n.used;
        BigInt R((n_limbs + 1) * 64);
        R.limbs[n_limbs] = 1;
        R.used = n_limbs + 1;
        return (R * R) % n;
        }();
    return r;
}

BigInt Curve::to_mont(const BigInt& a) {
    BigInt tmp = a % Curve::p;
    return montgomery_reduce(tmp * Curve::p_R2(), Curve::p, Curve::p_inv());
}

BigInt Curve::from_mont(const BigInt& aM) {
    return montgomery_reduce(aM, Curve::p, Curve::p_inv());
}

inline BigInt Curve::mod_add_p(const BigInt& a, const BigInt& b) {
    BigInt r = a + b;
    if (!(r < Curve::p)) r = r - Curve::p;
    return r;
}

inline BigInt Curve::mod_sub_p(const BigInt& a, const BigInt& b) {
    if (a < b) return (a + Curve::p) - b;
    return a - b;
}

inline BigInt Curve::mod_mul_p(const BigInt& a, const BigInt& b) {
    return montgomery_reduce(a * b, Curve::p, Curve::p_inv());
}

JacobianPoint::JacobianPoint() : infinity(true) {}

JacobianPoint::JacobianPoint(const BigInt& X, const BigInt& Y, const BigInt& Z) : X(X), Y(Y), Z(Z), infinity(false) {}

CurvePoint JacobianPoint::to_affine() const {
    if (infinity || Z.isZero()) return CurvePoint();

    BigInt Xn = Curve::from_mont(X);
    BigInt Yn = Curve::from_mont(Y);
    BigInt Zn = Curve::from_mont(Z);

    BigInt inv_z, tmp;
    extended_gcd(Zn, Curve::p, inv_z, tmp);
    inv_z = inv_z % Curve::p;

    BigInt z2 = (inv_z * inv_z) % Curve::p;
    BigInt z3 = (z2 * inv_z) % Curve::p;

    BigInt x = (Xn * z2) % Curve::p;
    BigInt y = (Yn * z3) % Curve::p;

    return CurvePoint(x, y);
}

JacobianPoint JacobianPoint::dbl() const {
    if (infinity || Y.isZero()) return JacobianPoint();

    BigInt A = Curve::mod_mul_p(X, X);

    BigInt B = Curve::mod_mul_p(Y, Y);

    BigInt C = Curve::mod_mul_p(B, B);

    BigInt S = Curve::mod_mul_p(X, B);
    S = Curve::mod_add_p(S, S); 
    S = Curve::mod_add_p(S, S); 

    BigInt M = Curve::mod_add_p(A, A);
    M = Curve::mod_add_p(M, A); 

    BigInt X3 = Curve::mod_mul_p(M, M);
    BigInt twoS = Curve::mod_add_p(S, S);
    X3 = Curve::mod_sub_p(X3, twoS);

    BigInt S_minus_X3 = Curve::mod_sub_p(S, X3);
    BigInt Y3 = Curve::mod_mul_p(M, S_minus_X3);

    BigInt eightC = C;
    eightC = Curve::mod_add_p(eightC, eightC);
    eightC = Curve::mod_add_p(eightC, eightC);
    eightC = Curve::mod_add_p(eightC, eightC);

    Y3 = Curve::mod_sub_p(Y3, eightC);

    BigInt Z3 = Curve::mod_mul_p(Y, Z);
    Z3 = Curve::mod_add_p(Z3, Z3);

    return JacobianPoint(X3, Y3, Z3);
}

JacobianPoint JacobianPoint::operator+(const JacobianPoint& Q) const {
    if (infinity) return Q;
    if (Q.infinity) return *this;

    BigInt Z1Z1 = Curve::mod_mul_p(Z, Z);

    BigInt Z2Z2 = Curve::mod_mul_p(Q.Z, Q.Z);

    BigInt U1 = Curve::mod_mul_p(X, Z2Z2);

    BigInt U2 = Curve::mod_mul_p(Q.X, Z1Z1);

    BigInt S1 = Curve::mod_mul_p(Y, Q.Z);
    S1 = Curve::mod_mul_p(S1, Z2Z2);

    BigInt S2 = Curve::mod_mul_p(Q.Y, Z);
    S2 = Curve::mod_mul_p(S2, Z1Z1);

    if (U1 == U2) {
        if (S1 == S2) return this->dbl();
        else return JacobianPoint();
    }

    BigInt H = Curve::mod_sub_p(U2, U1);

    BigInt I = Curve::mod_add_p(H, H);
    I = Curve::mod_mul_p(I, I);

    BigInt J = Curve::mod_mul_p(H, I);

    BigInt r = Curve::mod_sub_p(S2, S1);
    r = Curve::mod_add_p(r, r);

    BigInt V = Curve::mod_mul_p(U1, I);

    BigInt X3 = Curve::mod_mul_p(r, r);
    X3 = Curve::mod_sub_p(X3, J);
    BigInt twoV = Curve::mod_add_p(V, V);
    X3 = Curve::mod_sub_p(X3, twoV);

    BigInt Y3 = Curve::mod_sub_p(V, X3);
    Y3 = Curve::mod_mul_p(Y3, r);

    BigInt twoS1 = Curve::mod_add_p(S1, S1);
    BigInt twoS1J = Curve::mod_mul_p(twoS1, J);
    Y3 = Curve::mod_sub_p(Y3, twoS1J);

    BigInt Z3 = Curve::mod_add_p(Z, Q.Z);
    Z3 = Curve::mod_mul_p(Z3, Z3);
    Z3 = Curve::mod_sub_p(Z3, Z1Z1);
    Z3 = Curve::mod_sub_p(Z3, Z2Z2);
    Z3 = Curve::mod_mul_p(Z3, H);

    return JacobianPoint(X3, Y3, Z3);
}

JacobianPoint JacobianPoint::operator*(const BigInt& k) const {
    JacobianPoint R;         
    JacobianPoint P = *this;

    BigInt e = k;

    while (!e.isZero()) {
        if (e.isOdd()) {
            R = R.infinity ? P : R+P;
        }
        P = P.dbl();
        e = e >> 1;
    }

    return R;
}


CurvePoint::CurvePoint() : infinity(true) {}

CurvePoint::CurvePoint(const BigInt& x, const BigInt& y) : x(x), y(y), infinity(false) {
    if (!Curve::contains(x, y)) {
        throw std::runtime_error("Point not on curve");
    }
}

JacobianPoint CurvePoint::to_jacobian() const {
    if (infinity) return JacobianPoint();

    return JacobianPoint(Curve::to_mont(x), Curve::to_mont(y), Curve::to_mont(BigInt::one()));
}

CurvePoint CurvePoint::operator+(const CurvePoint& Q) const {
    JacobianPoint jakP = this->to_jacobian();
    JacobianPoint jakQ = Q.to_jacobian();
    return (jakP + jakQ).to_affine();
}

CurvePoint CurvePoint::dbl() const {
    JacobianPoint jakP = this->to_jacobian();

    return jakP.dbl().to_affine();
}

CurvePoint CurvePoint::operator*(const BigInt& k) const {
    JacobianPoint jakP = this->to_jacobian();

    return (jakP*k).to_affine();
}

ECDSAPublicKey::ECDSAPublicKey() : key_public() {}

ECDSAPublicKey::ECDSAPublicKey(const CurvePoint& key_pub) : key_public(key_pub) {}

ECDSASignature::ECDSASignature() {};

ECDSASignature::ECDSASignature(const BigInt& r, const BigInt& s) : r(r), s(s) {}

void ECDSA_generate_keys(ECDSAPublicKey& key_pub, BigInt& key_priv) {
    RandomGenerator rng;
    key_priv = rng.random_range(BigInt::one(), Curve::n - BigInt::one());
    key_pub = ECDSAPublicKey(Curve::G * key_priv);
}

ECDSASignature ecdsa_sign(const std::string& message, const BigInt& key_priv, std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function) {
    RandomGenerator rng;

    std::vector<uint8_t> messageBytes(message.begin(), message.end());

    std::vector<uint8_t> hashedMessage = hash_function(messageBytes);

    BigInt z = BigInt::vectoruint8(hashedMessage);

    if (z.bit_length() > Curve::n.bit_length()) {
        z = z >> (z.bit_length() - Curve::n.bit_length());
    }

    BigInt z_R = montgomery_reduce( (z % Curve::n) * Curve::n_R2(), Curve::n, Curve::nc_inv());

    while (true) {

        BigInt k = rng.random_range(BigInt::one(), Curve::n - BigInt::one());

        CurvePoint R = Curve::G * k;

        BigInt r = R.x % Curve::n;
        if (r.isZero()) continue;

        BigInt k_inv, tmp;
        extended_gcd(k, Curve::n, k_inv, tmp);

        k_inv = k_inv % Curve::n;

        BigInt k_inv_R = montgomery_reduce(k_inv * Curve::n_R2(), Curve::n, Curve::nc_inv());

        BigInt r_R = montgomery_reduce(r * Curve::n_R2(), Curve::n, Curve::nc_inv());

        BigInt d_R = montgomery_reduce(key_priv * Curve::n_R2(), Curve::n, Curve::nc_inv());

        BigInt rd_R = montgomery_reduce(r_R * d_R, Curve::n, Curve::nc_inv());
        BigInt s_R = montgomery_reduce(k_inv_R * (z_R + rd_R), Curve::n, Curve::nc_inv());

        BigInt s = montgomery_reduce(s_R, Curve::n, Curve::nc_inv());

        if (s.isZero()) continue;

        return ECDSASignature(r, s);
    }
}

bool ecdsa_verify(const std::string& message, const ECDSAPublicKey& key_pub, const ECDSASignature& signature, std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function) {

    if (signature.r.isZero() || signature.s.isZero()) return false;
    if (!(signature.r < Curve::n) || !(signature.s < Curve::n)) return false;

    std::vector<uint8_t> messageBytes(message.begin(), message.end());

    std::vector<uint8_t> hashedMessage = hash_function(messageBytes);

    BigInt z = BigInt::vectoruint8(hashedMessage);

    if (z.bit_length() > Curve::n.bit_length()) {
        z = z >> (z.bit_length() - Curve::n.bit_length());
    }

    BigInt z_R = montgomery_reduce( (z % Curve::n) * Curve::n_R2(), Curve::n, Curve::nc_inv());

    BigInt w, tmp;
    extended_gcd(signature.s, Curve::n, w, tmp);

    w = w % Curve::n;

    BigInt w_R = montgomery_reduce(w * Curve::n_R2(), Curve::n, Curve::nc_inv());

    BigInt r_R = montgomery_reduce(signature.r * Curve::n_R2(), Curve::n, Curve::nc_inv());

    BigInt u1_R = montgomery_reduce(z_R * w_R, Curve::n, Curve::nc_inv());
    BigInt u2_R = montgomery_reduce(r_R * w_R, Curve::n, Curve::nc_inv());

    BigInt u1 = montgomery_reduce(u1_R, Curve::n, Curve::nc_inv());
    BigInt u2 = montgomery_reduce(u2_R, Curve::n, Curve::nc_inv());

    CurvePoint Q = key_pub.key_public;

    CurvePoint P1 = Curve::G * u1;
    CurvePoint P2 = Q * u2;

    CurvePoint P = P1 + P2;

    if (P.infinity) return false;

    BigInt xP_mod_n = P.x % Curve::n;

    return xP_mod_n == signature.r;
}

bool ecdsa_verify(const std::string& message, const ECDSAPublicKey& key_pub, const std::string& hex_signature, std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function) {
    return ecdsa_verify(message, key_pub, DER_decode_signature_ecdsa(hex_signature), hash_function);
}

ECDSASignature DER_decode_signature_ecdsa(const std::vector<uint8_t>& sig) {
    size_t offset = 0;

    if (sig[offset++] != 0x30)
        throw std::runtime_error("Invalid signature: expected SEQUENCE");

    size_t seqLen = DER_decode_length(sig, offset);

    size_t seqEnd = offset + seqLen;

    if (seqEnd > sig.size())
        throw std::runtime_error("Invalid signature: length overflow");

    std::vector<uint8_t> r = DER_decode_integer(sig, offset);

    std::vector<uint8_t> s = DER_decode_integer(sig, offset);

    if (offset != seqEnd)
        throw std::runtime_error("Invalid signature: trailing bytes");

    return ECDSASignature(BigInt::vectoruint8(r), BigInt::vectoruint8(s));
}

ECDSASignature DER_decode_signature_ecdsa(const std::string& hex_sig) {
    return DER_decode_signature_ecdsa(hex_to_bytes(hex_sig));
}

std::vector<uint8_t> DER_encode_signature(const ECDSASignature& sig) {
    std::vector<uint8_t> r_bytes = sig.r.to_vectoruint8();
    std::vector<uint8_t> s_bytes = sig.s.to_vectoruint8();

    std::vector<uint8_t> r_enc = DER_encode_integer(r_bytes);
    std::vector<uint8_t> s_enc = DER_encode_integer(s_bytes);

    std::vector<uint8_t> body;
    body.insert(body.end(), r_enc.begin(), r_enc.end());
    body.insert(body.end(), s_enc.begin(), s_enc.end());

    std::vector<uint8_t> out;
    out.push_back(0x30);

    std::vector<uint8_t> len = DER_encode_length(body.size());
    out.insert(out.end(), len.begin(), len.end());

    out.insert(out.end(), body.begin(), body.end());

    return out;
}

std::string DER_encode_signature_hex(const ECDSASignature& sig) {
    return bytes_to_hex(DER_encode_signature(sig));
}

std::vector<uint8_t> encode_public_key(const ECDSAPublicKey& key_pub, bool compressed) {
    if (!compressed) {
        std::vector<uint8_t> out;
        out.reserve(65);

        out.push_back(0x04);

        auto x_bytes = key_pub.key_public.x.to_vectoruint8();
        auto y_bytes = key_pub.key_public.y.to_vectoruint8();

        out.insert(out.end(), x_bytes.begin(), x_bytes.end());
        out.insert(out.end(), y_bytes.begin(), y_bytes.end());

        return out;
    }
    else {
        std::vector<uint8_t> out;
        out.reserve(33);

        uint8_t prefix = (!(key_pub.key_public.y.isOdd()) ? 0x02 : 0x03);
        out.push_back(prefix);

        auto x_bytes = key_pub.key_public.x.to_vectoruint8();
        out.insert(out.end(), x_bytes.begin(), x_bytes.end());

        return out;
    }
}

std::string encode_public_key_hex(const ECDSAPublicKey& key_pub, bool compressed) {
    return bytes_to_hex(encode_public_key(key_pub, compressed));
}

ECDSAPublicKey decode_public_key(const std::vector<uint8_t>& pub_key, bool compressed) {
    if (!compressed) {
        if (pub_key.size() != 65 || pub_key[0] != 0x04) throw std::runtime_error("Invalid uncompressed public key");

        std::vector<uint8_t> x_bytes(&pub_key[1], &pub_key[1] + 32);
        std::vector<uint8_t> y_bytes(&pub_key[33], &pub_key[33] + 32);

        BigInt x = BigInt::vectoruint8(x_bytes);
        BigInt y = BigInt::vectoruint8(y_bytes);

        return ECDSAPublicKey(CurvePoint(x, y));
    } 
    else {
        if (pub_key.size() != 33) throw std::runtime_error("Invalid compressed public key");

        uint8_t prefix = pub_key[0];
        if (prefix != 0x02 && prefix != 0x03) throw std::runtime_error("Invalid compressed key prefix");

        std::vector<uint8_t> x_bytes(&pub_key[1], &pub_key[1] + 32);

        BigInt x = BigInt::vectoruint8(x_bytes);

        BigInt exp = (Curve::p + BigInt::one()) >> 2;

        BigInt y2 = (x * x * x + Curve::a * x + Curve::b) % Curve::p;

        BigInt y = mod_pow(y2, exp, Curve::p);

        bool y_even = !y.isOdd();
        bool prefix_even = (prefix == 0x02);

        if (y_even != prefix_even)
            y = Curve::p - y;

        return ECDSAPublicKey(CurvePoint(x, y));
    }
}

ECDSAPublicKey decode_public_key(const std::string& hex_pub_key, bool compressed) {
    return decode_public_key(hex_to_bytes(hex_pub_key), compressed);
}

}