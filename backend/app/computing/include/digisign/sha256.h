#pragma once
#include <vector>
#include <string>

namespace digisign {

uint32_t rotr(uint32_t x, int n);

uint32_t sigma0(uint32_t x);

uint32_t sigma1(uint32_t x);

uint32_t ch(uint32_t e, uint32_t f, uint32_t g);

uint32_t maj(uint32_t a, uint32_t b, uint32_t c);

uint32_t Sigma0(uint32_t a);

uint32_t Sigma1(uint32_t e);

std::vector<uint8_t> pad(const std::vector<uint8_t>& input);

std::vector<uint32_t> parse_block(const std::vector<uint8_t>& msg, size_t blockIndex);

void expand_w(std::vector<uint32_t>& W);

void compress_block(std::vector<uint32_t>& H, const std::vector<uint32_t>& W);

std::vector<uint8_t> sha256(const std::vector<uint8_t>& message);

std::string sha256(const std::string& message);

}