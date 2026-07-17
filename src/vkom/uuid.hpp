#pragma once

#include <cstdint>
#include <sstream>
#include <string>
#include <stdexcept>

namespace vkom {

/* adapted from krisvers/k32 */
class UUID {
private:
    uint8_t _bytes[16];

    static uint8_t nibbleFromChar(char c) {
        if (c >= '0' && c <= '9') {
            return c - '0';
        } else if (c >= 'a' && c <= 'f') {
            return c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F') {
            return c - 'A' + 10;
        }

        return 0xff;
    }

    static char charFromNibble(uint8_t n) {
        static const char* hexChars = "0123456789abcdef";
        if (n >= 0x10) {
            return '\0';
        }

        return hexChars[n];
    }

    static UUID fromString(std::string const& str) {
        if (str.length() != 36) {
            throw std::runtime_error("Invalid UUID string");
        }

        if (str[8] != '-' || str[13] != '-' || str[18] != '-' || str[23] != '-') {
            throw std::runtime_error("Invalid UUID string");
        }

        uint8_t bytes[16];
        for (size_t i = 0; i < 4; i += 1) {
            uint8_t nibbles[2] = {
                nibbleFromChar(str[i * 2 + 0]),
                nibbleFromChar(str[i * 2 + 1]),
            };

            if (nibbles[0] >= 0x10 || nibbles[1] >= 0x10) {
                throw std::runtime_error("Invalid UUID string");
            }

            bytes[i] = (nibbles[0] << 4) | nibbles[1];
        }

        for (size_t i = 4; i < 6; i += 1) {
            uint8_t nibbles[2] = {
                nibbleFromChar(str[1 + i * 2 + 0]),
                nibbleFromChar(str[1 + i * 2 + 1]),
            };

            if (nibbles[0] >= 0x10 || nibbles[1] >= 0x10) {
                throw std::runtime_error("Invalid UUID string");
            }

            bytes[i] = (nibbles[0] << 4) | nibbles[1];
        }

        for (size_t i = 6; i < 8; i += 1) {
            uint8_t nibbles[2] = {
                nibbleFromChar(str[2 + i * 2 + 0]),
                nibbleFromChar(str[2 + i * 2 + 1]),
            };

            if (nibbles[0] >= 0x10 || nibbles[1] >= 0x10) {
                throw std::runtime_error("Invalid UUID string");
            }

            bytes[i] = (nibbles[0] << 4) | nibbles[1];
        }

        for (size_t i = 8; i < 10; i += 1) {
            uint8_t nibbles[2] = {
                nibbleFromChar(str[3 + i * 2 + 0]),
                nibbleFromChar(str[3 + i * 2 + 1]),
            };

            if (nibbles[0] >= 0x10 || nibbles[1] >= 0x10) {
                throw std::runtime_error("Invalid UUID string");
            }

            bytes[i] = (nibbles[0] << 4) | nibbles[1];
        }

        for (size_t i = 10; i < 16; i += 1) {
            uint8_t nibbles[2] = {
                nibbleFromChar(str[4 + i * 2 + 0]),
                nibbleFromChar(str[4 + i * 2 + 1]),
            };

            if (nibbles[0] >= 0x10 || nibbles[1] >= 0x10) {
                throw std::runtime_error("Invalid UUID string");
            }

            bytes[i] = (nibbles[0] << 4) | nibbles[1];
        }

        return UUID(bytes);
    }

public:
    UUID(uint8_t bytes[16]) {
        for (size_t i = 0; i < 16; i += 1) {
            _bytes[i] = bytes[i];
        }
    }

    UUID(std::string const& str) {
        *this = UUID::fromString(str);
    }

    UUID(UUID const& other) {
        for (size_t i = 0; i < 16; i += 1) {
            _bytes[i] = other._bytes[i];
        }
    }

    UUID& operator=(UUID const& other) {
        for (size_t i = 0; i < 16; i += 1) {
            _bytes[i] = other._bytes[i];
        }

        return *this;
    }

    bool operator==(UUID const& other) const {
        return std::memcmp(_bytes, other._bytes, 16) == 0;
    }

    std::string str() {
        std::stringstream ss;
        for (size_t i = 0; i < 16; i += 1) {
            if (i == 4 || i == 6 || i == 8 || i == 10) {
                ss << '-';
            }

            ss << charFromNibble(_bytes[i] >> 4) << charFromNibble(_bytes[i] & 0xf);
        }

        return ss.str();
    }

    static UUID null() {
        uint8_t bytes[16] = {};
        return UUID(bytes);
    }
};

}
