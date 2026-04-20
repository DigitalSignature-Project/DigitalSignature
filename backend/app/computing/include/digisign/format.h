#pragma once
#include <string>
#include <vector>

namespace digisign {

std::string base64_encode(const std::vector<uint8_t>& data);

std::string bytes_to_hex(const std::vector<uint8_t>& data);

}