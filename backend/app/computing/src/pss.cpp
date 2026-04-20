#include <digisign/pss.h>
#include <digisign/sha256.h>
#include <iostream>
#include <openssl/rand.h>
#include <digisign/rsa.h>

namespace digisign {


PSSConfig::PSSConfig(int salt_length, std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function, std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> MGF1_hash) {
    this->salt_length = salt_length;
    this->hash_function = hash_function;
    this->MGF1_hash = MGF1_hash;
}

std::vector<uint8_t> xor_bytes(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b) {
    std::vector<uint8_t> result(a.size());

    for (size_t i = 0; i < a.size(); i++) {
        result[i] = a[i] ^ b[i];
    }

    return result;
}

std::vector<uint8_t> build_db(const std::vector<uint8_t>& salt, size_t emLen, size_t hashLen) {
    size_t dbLen = emLen - hashLen - 1;

    size_t psLen = dbLen - salt.size() - 1;

    std::vector<uint8_t> DB;

    DB.insert(DB.end(), psLen, 0x00);

    DB.push_back(0x01);

    DB.insert(DB.end(), salt.begin(), salt.end());

    return DB;
}

std::vector<uint8_t> MGF1(const std::vector<uint8_t>& seed, size_t maskLen, std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function) {
    const size_t hashLen = hash_function(seed).size();

    std::vector<uint8_t> mask;
    mask.reserve(maskLen);

    size_t blocks = (maskLen + hashLen - 1) / hashLen;

    for (size_t i = 0; i < blocks; i++) {
        std::vector<uint8_t> data;

        data.insert(data.end(), seed.begin(), seed.end());

        uint32_t counter = static_cast<uint32_t>(i);

        data.push_back((counter >> 24) & 0xFF);
        data.push_back((counter >> 16) & 0xFF);
        data.push_back((counter >> 8) & 0xFF);
        data.push_back(counter & 0xFF);

        auto h = hash_function(data);

        mask.insert(mask.end(), h.begin(), h.end());
    }

    mask.resize(maskLen);

    return mask;
}

void apply_embit_mask(std::vector<uint8_t>& maskedDB, size_t emLen, const BigInt& n)
{
    size_t emBits = n.bit_length() - 1;
    size_t totalBits = emLen * 8;

    size_t unusedBits = totalBits - emBits;

    if (unusedBits > 7) {
        throw std::runtime_error("Too many unused bits");
    }

    if (unusedBits > 0) {
        uint8_t mask = 0xFF >> unusedBits;
        maskedDB[0] &= mask;
    }
}

std::vector<uint8_t> build_em(const std::vector<uint8_t>& maskedDB, const std::vector<uint8_t>& H) {
    std::vector<uint8_t> EM;

    EM.insert(EM.end(), maskedDB.begin(), maskedDB.end());

    EM.insert(EM.end(), H.begin(), H.end());

    EM.push_back(0xBC);

    return EM;
}

std::vector<uint8_t> pss_encode(const std::string& message, size_t emLen, const BigInt& n, const PSSConfig& pss_config) {
    const size_t saltLen = pss_config.salt_length;

    std::vector<uint8_t> messageBytes(message.begin(), message.end());
    auto mHash = pss_config.hash_function(messageBytes);

    const size_t hashLen = mHash.size();

    if (emLen < hashLen + saltLen + 2) {
        throw std::runtime_error("Encoded message length too short");
    }

    std::vector<uint8_t> salt(saltLen);
    if (RAND_bytes(salt.data(), saltLen) != 1) {
        throw std::runtime_error("CSPRNG failure");
    }

    std::vector<uint8_t> M_prime(8, 0x00); // 8 zer
    M_prime.insert(M_prime.end(), mHash.begin(), mHash.end());  
    M_prime.insert(M_prime.end(), salt.begin(), salt.end());

    auto H = pss_config.hash_function(M_prime);

    auto DB = build_db(salt, emLen, hashLen);

    auto dbMask = MGF1(H, DB.size(), pss_config.MGF1_hash);

    auto maskedDB = xor_bytes(DB, dbMask);

    apply_embit_mask(maskedDB, emLen, n);

    auto EM = build_em(maskedDB, H);

    return EM;
}

bool pss_decode(const std::vector<uint8_t>& EM, const std::vector<uint8_t>& mHash, size_t emLen, const BigInt& n, const PSSConfig& pss_config) {
    const size_t hashLen = pss_config.hash_function(EM).size();
    const size_t saltLen = pss_config.salt_length;

    if (EM.size() != emLen) return false;

    if (EM.back() != 0xBC) return false;

    size_t hIndex = EM.size() - hashLen - 1;

    std::vector<uint8_t> maskedDB(EM.begin(), EM.begin() + hIndex);
    std::vector<uint8_t> H(EM.begin() + hIndex, EM.begin() + hIndex + hashLen);

    apply_embit_mask(maskedDB, emLen, n);

    auto dbMask = MGF1(H, maskedDB.size(), pss_config.MGF1_hash);
    auto DB = xor_bytes(maskedDB, dbMask);

    apply_embit_mask(DB, emLen, n);

    size_t psEnd = DB.size() - saltLen - 1;

    for (size_t i = 0; i < psEnd; i++) {
        if (DB[i] != 0x00) return false;
    }

    if (DB[psEnd] != 0x01) return false;

    std::vector<uint8_t> salt(DB.begin() + psEnd + 1, DB.end());

    std::vector<uint8_t> M_prime(8, 0x00);
    auto mHashCheck = pss_config.hash_function(mHash);

    M_prime.insert(M_prime.end(), mHashCheck.begin(), mHashCheck.end());
    M_prime.insert(M_prime.end(), salt.begin(), salt.end());

    auto H_prime = pss_config.hash_function(M_prime);

    return H == H_prime;
}

std::vector<uint8_t> digital_signature(const std::string& message, const BigInt& priv_key, const BigInt& n, const PSSConfig& pss_config) {
    int bits = n.used * 64 / 8;

    std::vector<uint8_t> pss = pss_encode(message, bits, n, pss_config);

    BigInt bipss = BigInt::vectoruint8(pss);

    BigInt bisign = encrypt(bipss, priv_key, n);

    return bisign.to_vectoruint8();
}

bool verify(const std::string& message, const std::vector<uint8_t>& signature, const BigInt& e, const BigInt& n, const PSSConfig& pss_config) {
    const size_t emBits = n.bit_length() - 1;
    const size_t emLen = (emBits + 7) / 8;

    if (signature.size() != emLen) {
        return false;
    }

    BigInt s = BigInt::vectoruint8(signature);

    BigInt m = decrypt(s, e, n);

    std::vector<uint8_t> EM = m.to_vectoruint8();

    if (EM.size() < emLen) {
        std::vector<uint8_t> padded(emLen - EM.size(), 0x00);
        padded.insert(padded.end(), EM.begin(), EM.end());
        EM = padded;
    }

    if (EM.size() != emLen) {
        return false;
    }

    std::vector<uint8_t> mHash(message.begin(), message.end());

    return pss_decode(EM, mHash, emLen, n, pss_config);
}

}