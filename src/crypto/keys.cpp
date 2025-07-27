// src/crypto/keys.cpp
#include "keys.h"
#include "hash.h"
#include "base58.h"
#include <openssl/ec.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include <openssl/sha.h>
#include <openssl/ecdsa.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace crypto {
// how this works: 
// 1. keys pairs public/private are generated
// 2. bitcoin is sent to your public key (synonymous to address)
// 3. a transaction must be made to spend the bitcoin you received
// 4. you sign the transaction with private key
// 5. everyone can verify signature w/ public key


// helper function to convert bytes to hex
std::string bytes_to_hex(const std::vector<unsigned char>& bytes) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (unsigned char byte : bytes) {
        ss << std::setw(2) << (int)byte;
    }
    return ss.str();
}

// helper function to convert hex to bytes
std::vector<unsigned char> hex_to_bytes(const std::string& hex) {
    std::vector<unsigned char> bytes;
    for (size_t i = 0; i < hex.length(); i +=2) {
        std::string byte_str = hex.substr(i, 2);
        unsigned char byte = (unsigned char)strtol(byte_str.c_str(), nullptr, 16);
        bytes.push_back(byte);
    }
    return bytes;
}

// PriateKey implementation
PrivateKey::PrivateKey() {
    // generate 32 random bytes
    key_data.resize(32);
    if (RAND_bytes(key_data.data(), 32) != 1) {
        throw std::runtime_error("failed to generate random private key");
    }
}

PrivateKey::PrivateKey(const std::string& hex) {
    // cretae private key from existing hex string
    key_data = hex_to_bytes(hex);
    if (key_data.size() != 32) {
        throw std::runtime_error("private key must be 32 bytes");
    }
}

std::string PrivateKey::to_hex() const {
    return bytes_to_hex(key_data);
}

// DIGITAL SIGNATURES IMPLEMENTATION
std::string PrivateKey::sign_message(const std::string& message) const {
    // Step 1: Hash the message (Bitcoin signs the hash and not the raw message)
    std::vector<unsigned char> message_bytes(message.begin(), message.end());
    unsigned char hash[32];
    SHA256(message_bytes.data(), message_bytes.size(), hash);

    // Step 2: Setup elliptic curve for signing
    EC_KEY* ec_key = EC_KEY_new_by_curve_name(NID_secp256k1);
    if (!ec_key) {
        throw std::runtime_error("Failedto create EC_KEY for signing");
    }

    // Step 3: Set private key
    BIGNUM* priv_bn = BN_bin2bn(key_data.data(), 32, nullptr);
    EC_KEY_set_private_key(ec_key, priv_bn);

    // Step 4: Create the signature
    // This is the mathematical proof that we own the private key
    ECDSA_SIG* sig = ECDSA_do_sign(hash, 32, ec_key);
    if (!sig) {
        BN_free(priv_bn);
        EC_KEY_free(ec_key);
        throw std::runtime_error("Failed to create signature");
    }

    // Step 5: Convert signature to bytes
    int sig_len = i2d_ECDSA_SIG(sig, nullptr);
    std::vector<unsigned char> sig_bytes(sig_len);
    unsigned char* sig_ptr = sig_bytes.data();
    i2d_ECDSA_SIG(sig, &sig_ptr);

    // Clean up
    ECDSA_SIG_free(sig);
    BN_free(priv_bn);
    EC_KEY_free(ec_key);

    // Return signature as hex string
    return bytes_to_hex(sig_bytes);
}

bool PublicKey::verify_signature(const std::string& message, const std::string& signature) const{
    try {
        // Step 1: Hash the message (same as signing)
        std::vector<unsigned char> message_bytes(message.begin(), message.end());
        unsigned char hash[32];
        SHA256(message_bytes.data(), message_bytes.size(), hash);

        // Step 2: Convert signature grom hex to bytes
        std::vector<unsigned char> sig_bytes = hex_to_bytes(signature);

        // Step 3: Set up elliptic curve for verification
        EC_KEY* ec_key = EC_KEY_new_by_curve_name(NID_secp256k1);
        if (!ec_key) return false;

        // Step 4: Set out public key
        const EC_GROUP* group = EC_KEY_get0_group(ec_key);
        EC_POINT* pub_point = EC_POINT_new(group);
        if (EC_POINT_oct2point(group, pub_point, key_data.data(), key_data.size(), nullptr) != 1) {
                EC_POINT_free(pub_point);
                EC_KEY_free(ec_key);
                return false;
        }
        EC_KEY_set_public_key(ec_key, pub_point);

        // Step 5: Parse the signature
        const unsigned char* sig_ptr = sig_bytes.data();
        ECDSA_SIG* sig = d2i_ECDSA_SIG(nullptr, &sig_ptr, sig_bytes.size());
        if (!sig) {
            EC_POINT_free(pub_point);
            EC_KEY_free(ec_key);
            return false;
        }

        // Step 6: Verification - Does this signature match this public key + message
        int result = ECDSA_do_verify(hash, 32, sig, ec_key);

        // Clean up
        ECDSA_SIG_free(sig);
        EC_POINT_free(pub_point);
        EC_KEY_free(ec_key);

        return result == 1; // 1 = valid signature, 0 = invalid
    } catch (...) {
        return false;
    }
}

std::string PublicKey::to_bitcoin_address() const {
    // Step 1: Get public key as hex string
    std::string pub_key_hex = to_hex();

    // Step 2: Convert hex to binary for hashing
    std::string pub_key_binary;
    for (size_t i = 0; i < pub_key_hex.length(); i += 2) {
        std::string byte_str = pub_key_hex.substr(i, 2);
        char byte = (char)strtol(byte_str.c_str(), nullptr, 16);
        pub_key_binary.push_back(byte);
    }

    // Step 3: Hash160 the public key (SHA256 + RIPEMD160)
    std::string hash160_result = crypto::Hash::hash160(pub_key_binary);

    // Step 4: Add verion byte (0x00 for mainet)
    std::vector<unsigned char> versionned_hash;
    versionned_hash.push_back(0x00); // Version byte for mainned P2PKH

    // Convert hash160 hex to bytes
    for (size_t i = 0; i < hash160_result.length(); i += 2) {
        std::string byte_str = hash160_result.substr(i, 2);
        unsigned char byte = (unsigned char)strtol(byte_str.c_str(), nullptr, 16);
        versionned_hash.push_back(byte);
    }

    // Step 5: Base58Check encode
    return crypto::Base58::encode_check(versionned_hash);
}

// PublicKey implementation
// elliptic curve math: Public = Private * G (G is a location on a curve)
PublicKey::PublicKey(const PrivateKey& private_key) {
    // create EC_KEY object for secp256k1 curve
    EC_KEY* ec_key = EC_KEY_new_by_curve_name(NID_secp256k1);
    if (!ec_key) {
        throw std::runtime_error("failed to create EC_KEY");
    }

    // convert private key bytes to BIGNUM
    BIGNUM* priv_bn = BN_bin2bn(private_key.get_bytes().data(), 32, nullptr);
    if (!priv_bn) {
        EC_KEY_free(ec_key);
        throw std::runtime_error("failed to create BIGNUM from private keys");
    }

    // set the private key
    if (EC_KEY_set_private_key(ec_key, priv_bn) != 1) {
        BN_free(priv_bn);
        EC_KEY_free(ec_key);
        throw std::runtime_error("failed to set private key");
    }

    // generate public key using elliptic curve math
    // the follow does: Public Key = Private * G (where G is the generated point)
    const EC_GROUP* group = EC_KEY_get0_group(ec_key);
    EC_POINT* pub_point = EC_POINT_new(group);
    if (EC_POINT_mul(group, pub_point, priv_bn, nullptr, nullptr, nullptr) != 1) {
        EC_POINT_free(pub_point);
        BN_free(priv_bn);
        EC_KEY_free(ec_key);
        throw std::runtime_error("failed to generate public key");
    }

    // convert to compressed format (33 bytes)
    size_t pub_key_len = EC_POINT_point2oct(group, pub_point, POINT_CONVERSION_COMPRESSED, nullptr, 0, nullptr);
    key_data.resize(pub_key_len);
    EC_POINT_point2oct(group, pub_point, POINT_CONVERSION_COMPRESSED, key_data.data(), pub_key_len, nullptr);

    // cleanup
    EC_POINT_free(pub_point);
    BN_free(priv_bn);
    EC_KEY_free(ec_key);

    // result: there is now a public key that mathematically relates to the private key
    // this process can't be reversed to get the private key though
}

PublicKey::PublicKey(const std::string& hex) {
    key_data = hex_to_bytes(hex);
    if (key_data.size() != 33 && key_data.size() != 65) {
        throw std::invalid_argument("public key must be 33 or 65 bytes");
    }
}

std::string PublicKey::to_hex() const {
    return bytes_to_hex(key_data);
}

}