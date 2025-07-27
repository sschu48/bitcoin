// src/crypto/hash.h
#pragma once
#include <string>
#include <vector>

namespace crypto {

class Hash {
public:
    // basic SHA-256 - main hash function for bitcoin
    static std::string sha256(const std::string& input);

    // double SHA 256 - what bitcoin actually uses for most things
    static std::string double_sha256(const std::string& input);

    // RIPEM160 - used in Bitcoin address generation
    static std::string ripemd160(const std::string& input);

    // Hash160 - SHA256 + RIPEMD160 (Bitcoin's address hash)
    static std::string hash160(const std::string& input);
};

}