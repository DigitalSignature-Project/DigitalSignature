#include <digisign/format.h>
#include <sstream>
#include <iomanip>

namespace digisign {

std::string base64_encode(const std::vector<uint8_t>& data)
{
    static const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string result;
    int val = 0;
    int valb = -6;

    for (uint8_t c : data) {
        val = (val << 8) + c;
        valb += 8;

        while (valb >= 0) {
            result.push_back(table[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }

    if (valb > -6) {
        result.push_back(table[((val << 8) >> (valb + 8)) & 0x3F]);
    }

    while (result.size() % 4) {
        result.push_back('=');
    }

    return result;
}

std::string bytes_to_hex(const std::vector<uint8_t>& data)
{
    std::ostringstream oss;

    for (uint8_t byte : data) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }

    return oss.str();
}

}