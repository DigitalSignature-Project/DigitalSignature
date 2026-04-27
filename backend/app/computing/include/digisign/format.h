#pragma once
#include <string>
#include <vector>

namespace digisign {

std::string base64_encode(const std::vector<uint8_t>& data);

std::string bytes_to_hex(const std::vector<uint8_t>& data);

uint8_t hex_char_to_val(char c);

std::vector<uint8_t> hex_to_bytes(const std::string& hex);

std::vector<uint8_t> base64_to_bytes(const std::string& input);

size_t DER_decode_length(const std::vector<uint8_t>& data, size_t& offset);

std::vector<uint8_t> DER_decode_integer(const std::vector<uint8_t>& data, size_t& offset);

std::vector<uint8_t> DER_encode_length(const size_t& len);

std::vector<uint8_t> DER_encode_integer(const std::vector<uint8_t>& val);

}