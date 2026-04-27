#include <digisign/format.h>
#include <sstream>
#include <iomanip>
#include <array>

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

uint8_t hex_char_to_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    throw std::invalid_argument("Invalid hex character");
}

std::vector<uint8_t> hex_to_bytes(const std::string& hex) {
    std::string clean = hex;

    if (clean.rfind("0x", 0) == 0 || clean.rfind("0X", 0) == 0) {
        clean = clean.substr(2);
    }

    if (clean.size() % 2 != 0) {
        throw std::invalid_argument("Hex string must have even length");
    }

    std::vector<uint8_t> result;
    result.reserve(clean.size() / 2);

    for (size_t i = 0; i < clean.size(); i += 2) {
        uint8_t high = hex_char_to_val(clean[i]);
        uint8_t low = hex_char_to_val(clean[i + 1]);
        result.push_back((high << 4) | low);
    }

    return result;
}

std::vector<uint8_t> base64_to_bytes(const std::string& input) {
    static const std::array<int, 256> T = [] {
        std::array<int, 256> table{};
        table.fill(-1);

        const std::string chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

        for (int i = 0; i < 64; i++) {
            table[static_cast<unsigned char>(chars[i])] = i;
        }

        return table;
        }();

    std::vector<uint8_t> out;
    int val = 0;
    int valb = -8;

    bool padding_started = false;

    for (unsigned char c : input) {
        if (std::isspace(c)) continue;
        if (c == '=') {
            padding_started = true;
            continue;
        }

        if (padding_started) {
            throw std::invalid_argument("Invalid base64: data after padding");
        }

        int decoded = T[c];
        if (decoded == -1) {
            throw std::invalid_argument("Invalid base64 character");
        }

        val = (val << 6) + decoded;
        valb += 6;

        if (valb >= 0) {
            out.push_back(static_cast<uint8_t>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }

    return out;
}

size_t DER_decode_length(const std::vector<uint8_t>& data, size_t& offset) {
    if (offset >= data.size())
        throw std::runtime_error("Out of bounds");

    uint8_t first = data[offset++];

    if ((first & 0x80) == 0) {
        return first;
    }

    size_t numBytes = first & 0x7F;

    if (numBytes == 0)
        throw std::runtime_error("Indefinite length not allowed in DER");

    if (offset + numBytes > data.size())
        throw std::runtime_error("Out of bounds");

    size_t length = 0;

    for (size_t i = 0; i < numBytes; i++) {
        length = (length << 8) | data[offset++];
    }

    return length;
}

std::vector<uint8_t> DER_decode_integer(const std::vector<uint8_t>& data, size_t& offset) {
    if (offset >= data.size())
        throw std::runtime_error("Out of bounds");

    if (data[offset++] != 0x02)
        throw std::runtime_error("Expected INTEGER");

    size_t len = DER_decode_length(data, offset);

    if (offset + len > data.size())
        throw std::runtime_error("INTEGER out of bounds");

    std::vector<uint8_t> value(
        data.begin() + offset,
        data.begin() + offset + len
    );

    offset += len;

    if (value.size() > 1 && value[0] == 0x00) {
        value.erase(value.begin());
    }

    return value;
}

std::vector<uint8_t> DER_encode_length(const size_t& l) {
    size_t len = l;
    std::vector<uint8_t> out;

    if (len < 128) {
        out.push_back(static_cast<uint8_t>(len));
    }
    else {
        std::vector<uint8_t> tmp;

        while (len > 0) {
            tmp.insert(tmp.begin(), len & 0xFF);
            len >>= 8;
        }

        out.push_back(0x80 | tmp.size());
        out.insert(out.end(), tmp.begin(), tmp.end());
    }

    return out;
}

std::vector<uint8_t> DER_encode_integer(const std::vector<uint8_t>& v) {
    std::vector<uint8_t> val = v;
    
    while (val.size() > 1 && val[0] == 0x00)
        val.erase(val.begin());

    if (!val.empty() && (val[0] & 0x80)) {
        val.insert(val.begin(), 0x00);
    }

    std::vector<uint8_t> out;
    out.push_back(0x02);

    std::vector<uint8_t> len = DER_encode_length(val.size());

    out.insert(out.end(), len.begin(), len.end());

    out.insert(out.end(), val.begin(), val.end());

    return out;
}

}