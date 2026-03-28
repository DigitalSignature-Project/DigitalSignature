#include <digisign/rsa.h>
#include <digisign/rng.h>
#include <digisign/prime.h>
#include <digisign/mod_arith.h>
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

namespace digisign {

void RSA_generate_keys(BigInt& key_pub, BigInt& key_priv, BigInt& n, int bits) {
    bits /= 2;
    bits = std::max(64, bits);

    BigInt e(64);
    e.limbs[0] = 65537;

    RandomGenerator rng;
    BigInt p1 = generate_prime(bits, 10, rng);
    BigInt p2 = generate_prime(bits, 10, rng);

    while (((p1 - 1) % e).isZero()) {
        p1 = generate_prime(bits, 10, rng);
    }
    while (((p2 - 1) % e).isZero() || p1 == p2) {
        p2 = generate_prime(bits, 10, rng);
    }

    n = p1 * p2;
    BigInt phi_n = (p1 - 1) * (p2 - 1);

    BigInt x, y;
    extended_gcd(e, phi_n, x, y);

    if (!(x < phi_n)) {
        x = x % phi_n;
    }

    key_pub = e;
    key_priv = x;
}

void RSA_generate_keys_parallel(BigInt& key_pub, BigInt& key_priv, BigInt& n, int bits, int threads) {
    bits /= 2;
    bits = std::max(64, bits);

    BigInt e(64);
    e.limbs[0] = 65537;

    BigInt p1 = generate_prime_parallel_omp(bits, 10, threads);
    BigInt p2 = generate_prime_parallel_omp(bits, 10, threads);

    while (((p1 - 1) % e).isZero()) {
        p1 = generate_prime_parallel_omp(bits, 10, threads);
    }
    while (((p2 - 1) % e).isZero() || p1 == p2) {
        p2 = generate_prime_parallel_omp(bits, 10, threads);
    }

    n = p1 * p2;
    BigInt phi_n = (p1 - 1) * (p2 - 1);

    BigInt x, y;
    extended_gcd(e, phi_n, x, y);

    if (!(x < phi_n)) {
        x = x % phi_n;
    }

    key_pub = e;
    key_priv = x;
}

BigInt encrypt(uint64_t message, const BigInt& pub_key, const BigInt& n) {
    BigInt encrypted_message = BigInt::uint64(message);

    size_t n_limbs = n.used;

    int w = 2;

    int table_size = 1 << (w - 1);
    std::vector<BigInt> table(table_size);

    BigInt R((n_limbs + 1) * 64);
    R.limbs[n_limbs] = 1;
    R.used = n_limbs + 1;

    BigInt R2 = (R * R) % n;

    uint64_t n0_inv = n_inv(n, 6);

    return montgomery_mod_pow(encrypted_message, pub_key, n, w, table, R2, n0_inv);
}

uint64_t decrypt(BigInt& encrypted_message, const BigInt& priv_key, const BigInt& n) {
    BigInt message = encrypted_message;

    size_t n_limbs = n.used;

    int w = 2;

    int table_size = 1 << (w - 1);
    std::vector<BigInt> table(table_size);

    BigInt R((n_limbs + 1) * 64);
    R.limbs[n_limbs] = 1;
    R.used = n_limbs + 1;

    BigInt R2 = (R * R) % n;

    uint64_t n0_inv = n_inv(n, 6);

    return montgomery_mod_pow(message, priv_key, n, w, table, R2, n0_inv).limbs[0];
}

}

PYBIND11_MODULE(generate_rsa_key, m) {
        pybind11::class_<digisign::BigInt>(m, "BigInt")
        .def(pybind11::init<>())
        .def_readwrite("limbs", &digisign::BigInt::limbs)
        .def_readwrite("used", &digisign::BigInt::used);
    m.def("rsa_generate_keys", &digisign::RSA_generate_keys, "rsa_generate_keys");
    m.def("rsa_generate_keys_parallel", &digisign::RSA_generate_keys_parallel, pybind11::call_guard<pybind11::gil_scoped_release>(), "rsa_generate_keys_parallel");
    m.def("encrypt", &digisign::encrypt, "encrypt");
    m.def("decrypt", &digisign::decrypt, "decrypt");
}