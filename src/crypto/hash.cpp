// src/crypto/hash/cpp
#include "hash.h"
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <iomanip>
#include <sstream>

namespace crypto {

std::string Hash::sha256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)input.c_str(), input.length(), hash);

    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

std::string Hash::double_sha256(const std::string& input) {
    // hash once
    std::string first_hash = sha256(input);

    // hash again - this is what bitcoin does
    // prevents certain cryptographic attacks
    return sha256(first_hash);
}

std::string Hash::ripemd160(const std::string& input) {
    unsigned char hash[RIPEMD160_DIGEST_LENGTH];
    RIPEMD160((unsigned char*)input.c_str(), input.length(), hash);

    std::stringstream ss;
    for (int i = 0; i < RIPEMD160_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

std::string Hash::hash160(const std::string& input) {
    // Step 1: SHA256 the input
    std::string sha_result = sha256(input);

    // Step 2: RIPEM160 the SHA256 result
    // Convert hex string back to binary for RIPEMD160
    std::string binary_sha;
    for (size_t i = 0; i < sha_result.length(); i += 2) {
        std::string byte_str = sha_result.substr(i, 2);
        char byte = (char)strtol(byte_str.c_str(), nullptr, 16);
        binary_sha.push_back(byte);
    }
    
    return ripemd160(binary_sha);
}

}