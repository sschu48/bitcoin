// src/crypto/base58.cpp
#include "base58.h"
#include <openssl/sha.h>
#include <algorithm>

namespace crypto {

static const std::string BASE58_ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

std::string Base58::encode(const std::vector<unsigned char>& data) {
    // Count leading zeros 
    int leading_zeros = 0;
    for (unsigned char byte : data) {
        if (byte != 0) break;
        leading_zeros++;
    }

    // Convert to base58
    std::vector<unsigned char> temp_data = data;
    std::string result;

    while (!temp_data.empty()) {
        // Divide by 58
        int remainder = 0; 
        for (auto& byte : temp_data) {
            int temp = remainder * 256 + byte;
            byte = temp / 58;
            remainder = temp % 58;
        }

        result = BASE58_ALPHABET[remainder] + result;

        // Remove leading zeros
        while (!temp_data.empty() && temp_data[0] == 0) {
            temp_data.erase(temp_data.begin());
        }
    }

    // Add leading 1s for leading zeros
    result = std::string(leading_zeros, '1') + result;

    return result;
}

std::vector<unsigned char> Base58::double_sha256_bytes(const std::vector<unsigned char>& data) {
    // First SHA256
    unsigned char first_hash[SHA256_DIGEST_LENGTH];
    SHA256(data.data(), data.size(), first_hash);

    // Second SHA256
    unsigned char second_hash[SHA256_DIGEST_LENGTH];
    SHA256(first_hash, SHA_DIGEST_LENGTH, second_hash);

    return std::vector<unsigned char>(second_hash, second_hash + SHA256_DIGEST_LENGTH);
}

std::string Base58::encode_check(const std::vector<unsigned char>& data) {
    // Step 1: Calculate checksum (first 4 vytes of double SHA256)
    auto checksum_full = double_sha256_bytes(data);
    std::vector<unsigned char> checksum(checksum_full.begin(), checksum_full.begin() + 4);

    // Step 2: Append checksum to data
    std::vector<unsigned char> data_with_checksum = data;
    data_with_checksum.insert(data_with_checksum.end(), checksum.begin(), checksum.end());

    // Step 3: Encode in Base58
    return encode(data_with_checksum);
}

}