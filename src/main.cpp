#include <iostream>
#include "crypto/hash.h"

int main() {
    std::cout << "Bitcoin - hash function test" << std::endl;

    // test basic functionality
    std::string message = "hello world";
    std::cout << "message: " << message << std::endl;

    // test sha-256
    std::string hash1 = crypto::Hash::sha256(message);
    std::cout << "SHA256:           " << hash1 << std::endl;

    // test double sha-256
    std::string hash2 = crypto::Hash::double_sha256(message);
    std::cout << "Double SHA-256:   " << hash2 << std::endl;

    // show difference 
    std::cout << "Different? " << (hash1 != hash2 ? "Yes" : "No") << std::endl;

    return 0;
}
