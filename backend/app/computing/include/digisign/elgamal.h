#pragma once
#include <digisign/BigInt.h>
#include <functional>
#include <string>

namespace digisign {

struct ElGamalPublicKey {

	BigInt p;
	BigInt q;
	BigInt g;
	BigInt y;

	ElGamalPublicKey(const BigInt& p, const BigInt& q , const BigInt& g, const BigInt& y);

	ElGamalPublicKey();

};

struct ElGamalSignature {

	BigInt r;
	BigInt s;

	ElGamalSignature(const BigInt& r, const BigInt& sy);

	ElGamalSignature();

};

BigInt generate_g(const BigInt& p, const BigInt& q, int bits);

void ElGamal_generate_keys(ElGamalPublicKey& key_pub, BigInt& key_priv, const int bits_p, const int bits_q);

void ElGamal_generate_keys_parallel(ElGamalPublicKey& key_pub, BigInt& key_priv, const int bits_p, const int bits_q, const int threads);

BigInt generate_p_parallel(const int bits_p, const int bits_q, const BigInt& q, const int threads);

ElGamalSignature elgamal_sign(const std::string& message, const ElGamalPublicKey& key_pub, const BigInt& key_priv, std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function);

bool elgamal_verify(const std::string& message, const ElGamalPublicKey& key_pub, const ElGamalSignature& signature, std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function);

bool elgamal_verify(const std::string& message, const ElGamalPublicKey& key_pub, const std::string& hex_signature, std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function);

ElGamalSignature DER_decode_signature(const std::vector<uint8_t>& sig);

ElGamalSignature DER_decode_signature(const std::string& hex_sig);

std::vector<uint8_t> DER_encode_signature(const ElGamalSignature& sig);

std::string DER_encode_signature_hex(const ElGamalSignature& sig);

}