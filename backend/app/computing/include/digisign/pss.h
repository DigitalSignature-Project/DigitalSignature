#pragma once
#include <digisign/BigInt.h>
#include <string>
#include <functional>

namespace digisign {

/**
 * @brief Configuration for RSA‑PSS encoding.
 *
 * Contains:
 *   • salt_length — length of random salt
 *   • hash_function — hash used for mHash and H
 *   • MGF1_hash — hash used inside MGF1
 *
 * For SHA‑256 PSS:
 *   salt_length = 32
 *   hash_function = sha256
 *   MGF1_hash = sha256
 */
struct PSSConfig {

    int salt_length;
    std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function;
    std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> MGF1_hash;

    PSSConfig(int salt_length,
              std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function,
              std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> MGF1_hash);
};

/**
 * @brief XOR of two byte arrays of equal length.
 */
std::vector<uint8_t> xor_bytes(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b);

/**
 * @brief Builds DB = PS || 0x01 || salt for RSA‑PSS.
 *
 * PS = zero padding of length (emLen − hashLen − saltLen − 1)
 */
std::vector<uint8_t> build_db(const std::vector<uint8_t>& salt, size_t emLen, size_t hashLen);

/**
 * @brief MGF1 mask generation function (RFC 8017).
 *
 * mask = Hash(seed || counter)
 */
std::vector<uint8_t> MGF1(const std::vector<uint8_t>& seed, size_t maskLen,
                          std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function);

/**
 * @brief Applies the emBits mask required by RSA‑PSS.
 *
 * Ensures that the leftmost unused bits of the encoded message are zero.
 */
void apply_embit_mask(std::vector<uint8_t>& maskedDB, size_t emLen, const BigInt& n);

/**
 * @brief Builds final encoded message EM = maskedDB || H || 0xBC.
 */
std::vector<uint8_t> build_em(const std::vector<uint8_t>& maskedDB, const std::vector<uint8_t>& H);

/**
 * @brief RSA‑PSS encoding (EMSA‑PSS‑ENCODE).
 */
std::vector<uint8_t> pss_encode(const std::string& message, size_t emLen,
                                const BigInt& n, const PSSConfig& pss_config);

/**
 * @brief RSA‑PSS decoding (EMSA‑PSS‑VERIFY).
 */
bool pss_decode(const std::vector<uint8_t>& EM, const std::vector<uint8_t>& mHash,
                size_t emLen, const BigInt& n, const PSSConfig& pss_config);

/**
 * @brief Creates RSA‑PSS signature: s = EMᵈ mod n.
 */
std::vector<uint8_t> digital_signature(const std::string& message,
                                       const BigInt& priv_key, const BigInt& n,
                                       const PSSConfig& pss_config);

/**
 * @brief Verifies RSA‑PSS signature.
 */
bool verify(const std::string& message, const std::vector<uint8_t>& signature,
            const BigInt& e, const BigInt& n, const PSSConfig& pss_config);

/**
 * @brief Hex‑string overload of verify().
 */
bool verify(const std::string& message, const std::string& hex_signature,
            const BigInt& e, const BigInt& n, const PSSConfig& pss_config);

}
