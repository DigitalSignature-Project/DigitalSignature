#pragma once
#include <digisign/BigInt.h>
#include <string>
#include <functional>

namespace digisign {

struct PSSConfig {

	int salt_length;
	std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function;
	std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> MGF1_hash;

	PSSConfig(int salt_length, std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function, std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> MGF1_hash);

};

std::vector<uint8_t> xor_bytes(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b);

std::vector<uint8_t> build_db(const std::vector<uint8_t>& salt, size_t emLen, size_t hashLen);

std::vector<uint8_t> MGF1(const std::vector<uint8_t>& seed, size_t maskLen, std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function);

void apply_embit_mask(std::vector<uint8_t>& maskedDB, size_t emLen, const BigInt& n);

std::vector<uint8_t> build_em(const std::vector<uint8_t>& maskedDB, const std::vector<uint8_t>& H);

std::vector<uint8_t> pss_encode(const std::string& message, size_t emLen, const BigInt& n, const PSSConfig& pss_config);

bool pss_decode(const std::vector<uint8_t>& EM, const std::vector<uint8_t>& mHash, size_t emLen, const BigInt& n, const PSSConfig& pss_config);

std::vector<uint8_t> digital_signature(const std::string& message, const BigInt& priv_key, const BigInt& n, const PSSConfig& pss_config);

bool verify(const std::string& message, const std::vector<uint8_t>& signature, const BigInt& e, const BigInt& n, const PSSConfig& pss_config);

bool verify(const std::string& message, const std::string& hex_signature, const BigInt& e, const BigInt& n, const PSSConfig& pss_config);

}