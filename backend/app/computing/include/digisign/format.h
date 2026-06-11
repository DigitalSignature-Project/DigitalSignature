#pragma once
#include <string>
#include <vector>

namespace digisign {

/**
 * @brief Encodes binary data into Base64 (RFC 4648).
 */
std::string base64_encode(const std::vector<uint8_t>& data);

/**
 * @brief Converts a byte vector to a lowercase hexadecimal string.
 */
std::string bytes_to_hex(const std::vector<uint8_t>& data);

/**
 * @brief Converts a single hex character to its numeric value.
 *
 * @throws std::invalid_argument if the character is not [0-9A-Fa-f].
 */
uint8_t hex_char_to_val(char c);

/**
 * @brief Converts a hex string (with or without 0x prefix) to bytes.
 *
 * Accepts odd-length hex strings by implicitly padding with a leading zero.
 */
std::vector<uint8_t> hex_to_bytes(const std::string& hex);

/**
 * @brief Decodes Base64 (RFC 4648) into bytes.
 *
 * Rejects:
 *  • invalid characters  
 *  • whitespace after padding  
 *  • data after '=' padding  
 */
std::vector<uint8_t> base64_to_bytes(const std::string& input);

/**
 * @brief Decodes DER length field.
 *
 * Supports:
 *  • short form (0xxxxxxx)
 *  • long form (1xxxxxxx)
 *
 * Rejects indefinite length (not allowed in DER).
 *
 * @param data   DER buffer
 * @param offset Current parsing offset (updated)
 */
size_t DER_decode_length(const std::vector<uint8_t>& data, size_t& offset);

/**
 * @brief Decodes a DER INTEGER and returns its raw bytes.
 *
 * Removes leading zero padding if present.
 */
std::vector<uint8_t> DER_decode_integer(const std::vector<uint8_t>& data, size_t& offset);

/**
 * @brief Encodes a DER length field.
 */
std::vector<uint8_t> DER_encode_length(const size_t& len);

/**
 * @brief Encodes a raw integer as DER INTEGER.
 *
 * Ensures:
 *  • no unnecessary leading zeros  
 *  • adds a leading 0x00 if MSB would indicate a negative number  
 */
std::vector<uint8_t> DER_encode_integer(const std::vector<uint8_t>& val);

}
