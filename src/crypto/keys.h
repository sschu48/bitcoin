// src/crypto/keys.h
#pragma once
#include <string>
#include <vector>

namespace crypto {

class PrivateKey {
private: 
    std::vector<unsigned char> key_data; // 32 bytes

public: 
    // generate a random private key
    PrivateKey();

    // create from hex string
    PrivateKey(const std::string& hex);

    // get as hex string
    std::string to_hex() const;

    // get the raw bytes
    const std::vector<unsigned char>& get_bytes() const { return key_data;}

    // DIGITAL SIGNATURES
    // Sign a message with private key
    std::string sign_message(const std::string& message) const;
};

class PublicKey {
private:
    std::vector<unsigned char> key_data; // 33 bytes (compressed)

public: 
    // create from private key
    PublicKey(const PrivateKey& private_key);

    // create from hex string
    PublicKey(const std::string& hex);

    // get as hex string
    std::string to_hex() const;

    // get the raw bytes
    const std::vector<unsigned char>& get_bytes() const { return key_data;}

    // DIGITAL SIGNATURES - Verify signatures made  by the private key
    // Verify that a signature was made by the private key that matches this public key
    bool verify_signature(const std::string& message, const std::string& signature) const;

    // BITCOIN ADDRESS GENERATION
    // Convert this public key into a Bitcoin address
    std::string to_bitcoin_address() const;
};

}