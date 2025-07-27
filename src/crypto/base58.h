// src/crypto/base58.h
#pragma once
#include <string>
#include <vector>

namespace crypto
{

class Base58 {
public: 
    // Simple Base58Check encoding for Bitcoin address
    static std::string encode_check(const std::vector<unsigned char>& data);

private:
    static std::string encode(const std::vector<unsigned char>& data);
    static std::vector<unsigned char> double_sha256_bytes(const std::vector<unsigned char>& data);
};

} // namespace crypto

