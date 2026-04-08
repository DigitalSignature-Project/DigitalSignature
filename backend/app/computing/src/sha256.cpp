#include <digisign/sha256.h>

namespace digisign {

uint32_t rotr(uint32_t x, int n) {
    return (x >> n) | (x << (32 - n));
}

uint32_t sigma0(uint32_t x) {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

uint32_t sigma1(uint32_t x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

uint32_t ch(uint32_t e, uint32_t f, uint32_t g) {
    return (e & f) ^ (~e & g);
}

uint32_t maj(uint32_t a, uint32_t b, uint32_t c) {
    return (a & b) ^ (a & c) ^ (b & c);
}

uint32_t Sigma0(uint32_t a) {
    return rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
}

uint32_t Sigma1(uint32_t e) {
    return rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
}

std::vector<uint8_t> pad(const std::vector<uint8_t>& input) {
    std::vector<uint8_t> msg = input;

    uint64_t bit_len = msg.size() * 8;

    msg.push_back(0x80);

    while ((msg.size() % 64) != 56) {
        msg.push_back(0x00);
    }

    for (int i = 7; i >= 0; --i) {
        msg.push_back((bit_len >> (i * 8)) & 0xFF);
    }

    return msg;
}

std::vector<uint32_t> parse_block(const std::vector<uint8_t>& msg, size_t blockIndex) {
    std::vector<uint32_t> words(16);

    size_t offset = blockIndex * 64;

    for (size_t i = 0; i < 16; i++) {
        size_t j = offset + i * 4;

        words[i] =
            (uint32_t(msg[j]) << 24) |
            (uint32_t(msg[j + 1]) << 16) |
            (uint32_t(msg[j + 2]) << 8) |
            (uint32_t(msg[j + 3]));
    }

    return words;
}

void expand_w(std::vector<uint32_t>& W) {
    W.resize(64);

    for (size_t i = 16; i < 64; i++) {
        W[i] = sigma1(W[i - 2])
            + W[i - 7]
            + sigma0(W[i - 15])
            + W[i - 16];
    }
}

void compress_block(std::vector<uint32_t>& H, const std::vector<uint32_t>& W) {
    static const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    uint32_t a = H[0];
    uint32_t b = H[1];
    uint32_t c = H[2];
    uint32_t d = H[3];
    uint32_t e = H[4];
    uint32_t f = H[5];
    uint32_t g = H[6];
    uint32_t h = H[7];

    for (size_t i = 0; i < 64; i++) {
        uint32_t T1 = h + Sigma1(e) + ch(e, f, g) + K[i] + W[i];
        uint32_t T2 = Sigma0(a) + maj(a, b, c);

        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;
    }

    H[0] += a;
    H[1] += b;
    H[2] += c;
    H[3] += d;
    H[4] += e;
    H[5] += f;
    H[6] += g;
    H[7] += h;
}

std::vector<uint8_t> sha256(const std::vector<uint8_t>& message) {
    std::vector<uint32_t> H = {
        0x6a09e667, 0xbb67ae85,
        0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c,
        0x1f83d9ab, 0x5be0cd19
    };

    std::vector<uint8_t> padded = pad(message);

    size_t numBlocks = padded.size() / 64;

    for (size_t b = 0; b < numBlocks; b++) {
        std::vector<uint32_t> W = parse_block(padded, b);
        expand_w(W);
        compress_block(H, W);
    }

    std::vector<uint8_t> hash(32);

    for (size_t i = 0; i < 8; i++) {
        hash[i * 4] = (H[i] >> 24) & 0xFF;
        hash[i * 4 + 1] = (H[i] >> 16) & 0xFF;
        hash[i * 4 + 2] = (H[i] >> 8) & 0xFF;
        hash[i * 4 + 3] = (H[i]) & 0xFF;
    }

    return hash;
}

}