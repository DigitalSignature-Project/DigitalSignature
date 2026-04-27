#include <digisign/elgamal.h>
#include <digisign/rng.h>
#include <digisign/prime.h>
#include <digisign/mod_arith.h>
#include <iostream>
#include <digisign/format.h>
#include <vector>

namespace digisign {

ElGamalPublicKey::ElGamalPublicKey(const BigInt& p, const BigInt& q, const BigInt& g, const BigInt& y) {
    this->p = p;
    this->q = q;
    this->g = g;
    this->y = y;
}

ElGamalPublicKey::ElGamalPublicKey() {
    this->p = BigInt();
    this->q = BigInt();
    this->g = BigInt();
    this->y = BigInt();
}

ElGamalSignature::ElGamalSignature(const BigInt& r, const BigInt& s) {
    this->r = r;
    this->s = s;
}

ElGamalSignature::ElGamalSignature() {
    this->r = BigInt();
    this->s = BigInt();
}

BigInt generate_g(const BigInt& p, const BigInt& q, int bits) {
    RandomGenerator rng;
    BigInt two = BigInt::two();
    BigInt one = BigInt::one();

    int w;
    if (bits < 256) w = 4;
    else if (bits < 1024) w = 5;
    else if (bits < 2048) w = 6;
    else w = 7;

    int table_size = 1 << (w - 1);
    std::vector<BigInt> table(table_size);

    size_t p_limbs = p.used;

    BigInt R((p_limbs + 1) * 64);
    R.limbs[p_limbs] = 1;
    R.used = p_limbs + 1;

    BigInt R2 = (R * R) % p;

    uint64_t p0_inv = n_inv(p, 6);

    BigInt h;
    BigInt g;
    BigInt exp = (p - one) / q;

    while (true) {
        h = rng.random_range(two, p - two);

        g = montgomery_mod_pow(h, exp, p, w, table, R2, p0_inv);

        if (g == one) continue;
        return g;
    }
}

void ElGamal_generate_keys(ElGamalPublicKey& key_pub, BigInt& key_priv, const int bits_p, const int bits_q) {
    
    if (bits_p < bits_q) throw std::runtime_error("p must be bigger than q");

    RandomGenerator rng;
    BigInt q;
    BigInt p;
    BigInt k;
    BigInt one = BigInt::one();
    BigInt two = BigInt::two();
    int w;
    if (bits_p < 256) w = 4;
    else if (bits_p < 1024) w = 5;
    else if (bits_p < 2048) w = 6;
    else w = 7;

    int table_size = 1 << (w - 1);
    std::vector<BigInt> table(table_size);

    q = generate_prime(bits_q, 40, rng);

    while (true) {
        k = rng.random_range(BigInt::one() << (bits_p - bits_q - 1), (BigInt::one() << (bits_p - bits_q)) - 1);

        p = k * q + one;

        if (p.bit_length() != bits_p) continue;

        if (!small_prime_test(p)) {
            continue;
        }

        if (miller_rabin(p, 40, rng, w, table)) {
            break;
        }
    }

    BigInt g = generate_g(p, q, bits_p);

    key_priv = rng.random_range(one, q - one);

    size_t p_limbs = p.used;

    BigInt R((p_limbs + 1) * 64);
    R.limbs[p_limbs] = 1;
    R.used = p_limbs + 1;

    BigInt R2 = (R * R) % p;

    uint64_t p0_inv = n_inv(p, 6);

    key_pub.y = montgomery_mod_pow(g, key_priv, p, w, table, R2, p0_inv);

    key_pub.p = p;
    key_pub.q = q;
    key_pub.g = g;
}

BigInt generate_p_parallel(const int bits_p, const int bits_q, BigInt& q, const int threads) {
    BigInt p;
    BigInt one = BigInt::one();
    bool found = false;

    int w;
    if (bits_p < 256) w = 4;
    else if (bits_p < 1024) w = 5;
    else if (bits_p < 2048) w = 6;
    else w = 7;

#pragma omp parallel for num_threads(threads)
    for (int i = 0; i < threads; i++) {
        BigInt k;
        BigInt p_local;
        RandomGenerator rng;

        std::vector<BigInt> table_local(1 << (w - 1));

        while (true) {
            if (found) break;

            k = rng.random_range(BigInt::one() << (bits_p - bits_q - 1), (BigInt::one() << (bits_p - bits_q)) - 1);
            p_local = k * q + one;

            if (p_local.bit_length() != bits_p) continue;

            if (!small_prime_test(p_local))
                continue;

            if (miller_rabin(p_local, 40, rng, w, table_local)) {
#pragma omp critical
                {
                    p = p_local;
                    found = true;
                }
                break;
            }
        }
    }
    return p;
}

void ElGamal_generate_keys_parallel(ElGamalPublicKey& key_pub, BigInt& key_priv, const int bits_p, const int bits_q, const int threads) {

    if (bits_p < bits_q) throw std::runtime_error("p must be bigger than q");

    RandomGenerator rng;
    BigInt q;
    BigInt p;
    BigInt k;
    BigInt one = BigInt::one();
    BigInt two = BigInt::two();
    int w;
    if (bits_p < 256) w = 4;
    else if (bits_p < 1024) w = 5;
    else if (bits_p < 2048) w = 6;
    else w = 7;

    int table_size = 1 << (w - 1);
    std::vector<BigInt> table(table_size);

    q = generate_prime_parallel_omp(bits_q, 40, threads);

    p = generate_p_parallel(bits_p, bits_q, q, threads);

    BigInt g = generate_g(p, q, bits_p);

    key_priv = rng.random_range(one, q - one);

    size_t p_limbs = p.used;

    BigInt R((p_limbs + 1) * 64);
    R.limbs[p_limbs] = 1;
    R.used = p_limbs + 1;

    BigInt R2 = (R * R) % p;

    uint64_t p0_inv = n_inv(p, 6);

    key_pub.y = montgomery_mod_pow(g, key_priv, p, w, table, R2, p0_inv);

    key_pub.p = p;
    key_pub.q = q;
    key_pub.g = g;
}

ElGamalSignature elgamal_sign(const std::string& message, const ElGamalPublicKey& key_pub, const BigInt& key_priv, std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function) {
    std::vector<uint8_t> messageBytes(message.begin(), message.end());

    std::vector<uint8_t> hashedMessage = hash_function(messageBytes);

    size_t q_bits = key_pub.q.bit_length();
    size_t q_bytes = (q_bits + 7) / 8;

    if (hashedMessage.size() > q_bytes) {
        hashedMessage.resize(q_bytes);
    }

    int extra_bits = (q_bytes * 8) - q_bits;
    if (extra_bits > 0) {
        hashedMessage[0] &= (0xFF >> extra_bits);
    }

    BigInt hashedMessageBI = BigInt::vectoruint8(hashedMessage);

    hashedMessageBI = hashedMessageBI % key_pub.q;

    RandomGenerator rng;

    BigInt k;

    size_t p_limbs = key_pub.p.used;

    int bits = p_limbs * 64;
    int w;
    if (bits < 256) w = 4;
    else if (bits < 1024) w = 5;
    else if (bits < 2048) w = 6;
    else w = 7;

    int table_size = 1 << (w - 1);
    std::vector<BigInt> table(table_size);

    BigInt R((p_limbs + 1) * 64);
    R.limbs[p_limbs] = 1;
    R.used = p_limbs + 1;

    BigInt R2 = (R * R) % key_pub.p;

    uint64_t p0_inv = n_inv(key_pub.p, 6);

    BigInt r;

    do {
        k = rng.random_range(BigInt::one(), key_pub.q - BigInt::one());
        r = montgomery_mod_pow(key_pub.g, k, key_pub.p, w, table, R2, p0_inv) % key_pub.q;
    } while (r.isZero() || r > key_pub.q - BigInt::one());

    BigInt k_inv, y;

    extended_gcd(k, key_pub.q, k_inv, y);

    if (!(k_inv < key_pub.q)) {
        k_inv = k_inv % key_pub.q;
    }

    BigInt s = (k_inv * (hashedMessageBI + key_priv * r)) % key_pub.q;

    return ElGamalSignature(r, s);
}

bool elgamal_verify(const std::string& message, const ElGamalPublicKey& key_pub, const ElGamalSignature& signature, std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function) {
    if (signature.r.isZero() || !(signature.r < key_pub.q)) return false;
    if (signature.s.isZero() || !(signature.s < key_pub.q)) return false;

    BigInt w, y;

    extended_gcd(signature.s, key_pub.q, w, y);

    if (!(w < key_pub.q)) {
        w = w % key_pub.q;
    }
    
    std::vector<uint8_t> messageBytes(message.begin(), message.end());

    std::vector<uint8_t> hashedMessage = hash_function(messageBytes);

    size_t q_bits = key_pub.q.bit_length();
    size_t q_bytes = (q_bits + 7) / 8;

    if (hashedMessage.size() > q_bytes) {
        hashedMessage.resize(q_bytes);
    }

    int extra_bits = (q_bytes * 8) - q_bits;
    if (extra_bits > 0) {
        hashedMessage[0] &= (0xFF >> extra_bits);
    }

    BigInt hashedMessageBI = BigInt::vectoruint8(hashedMessage);

    hashedMessageBI = hashedMessageBI % key_pub.q;

    BigInt u1 = (hashedMessageBI * w) % key_pub.q;
    BigInt u2 = (signature.r * w) % key_pub.q;

    size_t p_limbs = key_pub.p.used;

    int bits = p_limbs * 64;
    int window;
    if (bits < 256) window = 4;
    else if (bits < 1024) window = 5;
    else if (bits < 2048) window = 6;
    else window = 7;

    int table_size = 1 << (window - 1);
    std::vector<BigInt> table(table_size);

    BigInt R((p_limbs + 1) * 64);
    R.limbs[p_limbs] = 1;
    R.used = p_limbs + 1;

    BigInt R2 = (R * R) % key_pub.p;

    uint64_t p0_inv = n_inv(key_pub.p, 6);

    BigInt gu1 = montgomery_mod_pow(key_pub.g, u1, key_pub.p, window, table, R2, p0_inv);
    BigInt yu2 = montgomery_mod_pow(key_pub.y, u2, key_pub.p, window, table, R2, p0_inv);

    BigInt v = ((gu1 * yu2) % key_pub.p) % key_pub.q;

    return (v == signature.r);
}

bool elgamal_verify(const std::string& message, const ElGamalPublicKey& key_pub, const std::string& hex_signature, std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function) {
    return elgamal_verify(message, key_pub, DER_decode_signature(hex_signature), hash_function);
}

ElGamalSignature DER_decode_signature(const std::vector<uint8_t>& sig) {
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

    return ElGamalSignature(BigInt::vectoruint8(r), BigInt::vectoruint8(s));
}

ElGamalSignature DER_decode_signature(const std::string& hex_sig) {
    return DER_decode_signature(hex_to_bytes(hex_sig));
}

std::vector<uint8_t> DER_encode_signature(const ElGamalSignature& sig) {
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

std::string DER_encode_signature_hex(const ElGamalSignature& sig) {
    return bytes_to_hex(DER_encode_signature(sig));
}

}