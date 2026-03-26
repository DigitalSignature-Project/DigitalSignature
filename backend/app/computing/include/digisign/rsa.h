#pragma once

#include <digisign/BigInt.h>

namespace digisign {

void RSA_generate_keys(BigInt& key_pub, BigInt& key_priv, BigInt& n, int bits);

void RSA_generate_keys_parallel(BigInt& key_pub, BigInt& key_priv, BigInt& n, int bits, int max_threads);

BigInt encrypt(uint64_t message, const BigInt& pub_key, const BigInt& n);

uint64_t decrypt(BigInt& encrypted_message, const BigInt& priv_key, const BigInt& n);

}