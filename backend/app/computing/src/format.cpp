#include <digisign/format.h>
#include <sstream>
#include <iomanip>
#include <array>

namespace digisign {

// ------------------------------------------------------------
// Base64 encoding (RFC 4648)
// ------------------------------------------------------------
std::string base64_encode(const std::vector<uint8_t>& data)
{
    static const char table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::string result;
    int val = 0;
    int valb = -6;

    // Process input 8 bits at a time, output 6-bit groups
    for (uint8_t c : data) {
        val = (val << 8) + c;
        valb += 8;

        while (valb >= 0) {
            result.push_back(table[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }

    // Handle leftover bits
    if (valb > -6) {
        result.push_back(table[((val << 8) >> (valb + 8)) & 0x3F]);
    }

    // Pad to multiple of 4 chars
    while (result.size() % 4) {
        result.push_back('=');
    }

    return result;
}

// ------------------------------------------------------------
// Hex encoding
// ------------------------------------------------------------
std::string bytes_to_hex(const std::vector<uint8_t>& data)
{
    std::ostringstream oss;

    for (uint8_t byte : data) {
        oss << std::hex << std::setw(2)
            << std::setfill('0') << (int)byte;
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

    // Strip optional 0x prefix
    if (clean.rfind("0x", 0) == 0 || clean.rfind("0X", 0) == 0) {
        clean = clean.substr(2);
    }

    // If odd length, pad with leading zero
    if (clean.size() % 2 != 0) {
        clean = "0" + clean;
    }

    std::vector<uint8_t> result;
    result.reserve(clean.size() / 2);

    for (size_t i = 0; i < clean.size(); i += 2) {
        uint8_t high = hex_char_to_val(clean[i]);
        uint8_t low  = hex_char_to_val(clean[i + 1]);
        result.push_back((high << 4) | low);
    }

    return result;
}

// ------------------------------------------------------------
// Base64 decoding (RFC 4648)
// ------------------------------------------------------------
std::vector<uint8_t> base64_to_bytes(const std::string& input) {
    // Build decode table once
    static const std::array<int, 256> T = [] {
        std::array<int, 256> table{};
        table.fill(-1);

        const std::string chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

        for (int i = 0; i < 64; i++) {
            table[(unsigned char)chars[i]] = i;
        }

        return table;
    }();

    std::vector<uint8_t> out;
    int val = 0;
    int valb = -8;

    bool padding_started = false;

    for (unsigned char c : input) {
        if (std::isspace(c))
            continue;

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
            out.push_back((val >> valb) & 0xFF);
            valb -= 8;
        }
    }

    return out;
}

// ------------------------------------------------------------
// DER length decoding
// ------------------------------------------------------------
size_t DER_decode_length(const std::vector<uint8_t>& data, size_t& offset) {
    if (offset >= data.size())
        throw std::runtime_error("Out of bounds");

    uint8_t first = data[offset++];

    // Short form: 0xxxxxxx
    if ((first & 0x80) == 0) {
        return first;
    }

    // Long form: 1xxxxxxx
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

// ------------------------------------------------------------
// DER INTEGER decoding
// ------------------------------------------------------------
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

    // Remove leading zero padding if present
    if (value.size() > 1 && value[0] == 0x00) {
        value.erase(value.begin());
    }

    return value;
}

// ------------------------------------------------------------
// DER length encoding
// ------------------------------------------------------------
std::vector<uint8_t> DER_encode_length(const size_t& l) {
    size_t len = l;
    std::vector<uint8_t> out;

    // Short form
    if (len < 128) {
        out.push_back((uint8_t)len);
    }
    else {
        // Long form: encode length in big-endian
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

// ------------------------------------------------------------
// DER INTEGER encoding
// ------------------------------------------------------------
std::vector<uint8_t> DER_encode_integer(const std::vector<uint8_t>& v) {
    std::vector<uint8_t> val = v;

    // Remove unnecessary leading zeros
    while (val.size() > 1 && val[0] == 0x00)
        val.erase(val.begin());

    // If MSB is 1, prepend 0x00 to indicate positive integer
    if (!val.empty() && (val[0] & 0x80)) {
        val.insert(val.begin(), 0x00);
    }

    std::vector<uint8_t> out;
    out.push_back(0x02); // INTEGER tag

    std::vector<uint8_t> len = DER_encode_length(val.size());
    out.insert(out.end(), len.begin(), len.end());

    out.insert(out.end(), val.begin(), val.end());

    return out;
}

}
