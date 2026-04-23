#pragma once
#include <vector>
#include <string>

namespace digisign {

    std::vector<uint8_t> sha3_pad(const std::vector<uint8_t>& message, size_t rate, uint8_t domain);

    void absorb_block(uint64_t A[5][5], const uint8_t* block, size_t rate);

    std::vector<uint8_t> sha3_256(const std::vector<uint8_t>& msg);

    std::string sha3_256(const std::string& message);

    std::vector<uint8_t> sha3_512(const std::vector<uint8_t>& msg);

    std::string sha3_512(const std::string& message);

    void keccak_f(uint64_t A[5][5]);

    void iota(uint64_t A[5][5], int round);

    void chi(uint64_t A[5][5]);

    void pi(uint64_t A[5][5]);

    void rho(uint64_t A[5][5]);

    void theta(uint64_t A[5][5]);

    uint64_t rotl(uint64_t x, int n);

}