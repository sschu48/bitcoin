// src/crypto/hash/cpp
#include "hash.h"
#include <openssl/sha.h>
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

}