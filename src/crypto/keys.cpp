// src/crypto/keys.cpp
#include "keys.h"
#include <openssl/ec.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
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

// PrivateKey implementation
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