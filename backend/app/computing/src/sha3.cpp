#include <digisign/sha3.h>
#include <digisign/format.h>

namespace digisign {

uint64_t rotl(uint64_t x, int n) {
    n %= 64;
    return (x << n) | (x >> (64 - n));
}

void theta(uint64_t A[5][5]) {
    uint64_t C[5];
    uint64_t D[5];

    for (int x = 0; x < 5; x++) {
        C[x] = A[x][0] ^ A[x][1] ^ A[x][2] ^ A[x][3] ^ A[x][4];
    }

    for (int x = 0; x < 5; x++) {
        D[x] = C[(x + 4) % 5] ^ rotl(C[(x + 1) % 5], 1);
    }

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            A[x][y] ^= D[x];
        }
    }
}

void rho(uint64_t A[5][5]) {
    static const int R[5][5] = {
        {  0, 36,  3, 41, 18 },
        {  1, 44, 10, 45,  2 },
        { 62,  6, 43, 15, 61 },
        { 28, 55, 25, 21, 56 },
        { 27, 20, 39,  8, 14 }
    };

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            A[x][y] = rotl(A[x][y], R[x][y]);
        }
    }
}

void pi(uint64_t A[5][5]) {
    uint64_t B[5][5];

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            B[y][(2 * x + 3 * y) % 5] = A[x][y];
        }
    }

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            A[x][y] = B[x][y];
        }
    }
}

void chi(uint64_t A[5][5]) {
    uint64_t B[5][5];

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            B[x][y] = A[x][y];
        }
    }

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            A[x][y] =
                B[x][y] ^
                ((~B[(x + 1) % 5][y]) & B[(x + 2) % 5][y]);
        }
    }
}

void iota(uint64_t A[5][5], int round) {
    static const uint64_t RC[24] = {
        0x0000000000000001ULL,
        0x0000000000008082ULL,
        0x800000000000808AULL,
        0x8000000080008000ULL,
        0x000000000000808BULL,
        0x0000000080000001ULL,
        0x8000000080008081ULL,
        0x8000000000008009ULL,
        0x000000000000008AULL,
        0x0000000000000088ULL,
        0x0000000080008009ULL,
        0x000000008000000AULL,
        0x000000008000808BULL,
        0x800000000000008BULL,
        0x8000000000008089ULL,
        0x8000000000008003ULL,
        0x8000000000008002ULL,
        0x8000000000000080ULL,
        0x000000000000800AULL,
        0x800000008000000AULL,
        0x8000000080008081ULL,
        0x8000000000008080ULL,
        0x0000000080000001ULL,
        0x8000000080008008ULL
    };

    A[0][0] ^= RC[round];
}

void keccak_f(uint64_t A[5][5]) {
    for (int r = 0; r < 24; r++) {
        theta(A);
        rho(A);
        pi(A);
        chi(A);
        iota(A, r);
    }
}

std::vector<uint8_t> sha3_pad(const std::vector<uint8_t>& message, size_t rate, uint8_t domain)
{
    std::vector<uint8_t> padded = message;

    padded.push_back(domain);

    while (padded.size() % rate != rate - 1) {
        padded.push_back(0x00);
    }

    padded.push_back(0x80);

    return padded;
}

void absorb_block(uint64_t A[5][5], const uint8_t* block, size_t rate) {
    int idx = 0;
    size_t lanes = rate / 8;

    for (int i = 0; i < lanes; i++) {

        uint64_t lane = 0;

        for (int b = 0; b < 8; b++) {
            lane |= (uint64_t)block[idx++] << (8 * b);
        }

        int x = i % 5;
        int y = i / 5;

        A[x][y] ^= lane;
    }
}

std::vector<uint8_t> sha3_256(const std::vector<uint8_t>& msg) {

    static const size_t rate = 136;
    static const size_t output = 32;
    static const uint8_t domain = 0x06;

    auto padded = sha3_pad(msg, rate, domain);


    uint64_t A[5][5] = { 0 };

    for (size_t offset = 0; offset < padded.size(); offset += rate) {

        absorb_block(A, &padded[offset], rate);

        keccak_f(A);
    }

    std::vector<uint8_t> out;
    out.reserve(output);

    for (int i = 0; i < 25 && out.size() < output; i++) {

        int x = i % 5;
        int y = i / 5;

        uint64_t lane = A[x][y];

        for (int b = 0; b < 8 && out.size() < output; b++) {
            out.push_back((lane >> (8 * b)) & 0xFF);
        }
    }

    return out;
}

std::vector<uint8_t> sha3_512(const std::vector<uint8_t>& msg) {

    static const size_t rate = 72;
    static const size_t output = 64;
    static const uint8_t domain = 0x06;

    auto padded = sha3_pad(msg, rate, domain);


    uint64_t A[5][5] = { 0 };

    for (size_t offset = 0; offset < padded.size(); offset += rate) {

        absorb_block(A, &padded[offset], rate);

        keccak_f(A);
    }

    // squeeze
    std::vector<uint8_t> out;
    out.reserve(output);

    for (int i = 0; i < 25 && out.size() < output; i++) {

        int x = i % 5;
        int y = i / 5;

        uint64_t lane = A[x][y];

        for (int b = 0; b < 8 && out.size() < output; b++) {
            out.push_back((lane >> (8 * b)) & 0xFF);
        }
    }

    return out;
}

std::string sha3_256(const std::string& message) {
    std::vector<uint8_t> msgbytes(message.begin(), message.end());
    return bytes_to_hex(sha3_256(msgbytes));
}

std::string sha3_512(const std::string& message) {
    std::vector<uint8_t> msgbytes(message.begin(), message.end());
    return bytes_to_hex(sha3_512(msgbytes));
}

}