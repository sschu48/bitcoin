#include <iostream>
#include "crypto/hash.h"
#include "crypto/keys.h"

int main() {
    std::cout << "Bitcoin - key demo" << std::endl;

    /// generate a private key
    crypto::PrivateKey private_key;
    std::string priv_hex = private_key.to_hex();
    std::cout << "Private Key: " << priv_hex << std::endl;
    
    // generate public key from it
    crypto::PublicKey public_key1(private_key);
    std::cout << "Public Key 1: " << public_key1.to_hex() << std::endl;

    // now recreate the same private key from the hex
    crypto::PrivateKey same_private_key(priv_hex);
    crypto::PublicKey public_key2(same_private_key);
    std::cout << "Public Key 2: " << public_key2.to_hex() << std::endl;

    // they should be identical
    bool same = (public_key1.to_hex() == public_key2.to_hex());
    std::cout << "Same public keys? " << (same ? "Yes" : "No") << std::endl;

    std::cout << "\nwhat happened:" << std::endl;
    std::cout << "- same private key always produces the same public key" << std::endl;
    std::cout << "- different private keys produce different public keys" << std::endl;
    std::cout << "- this is the foundation of bitcoin signatures" << std::endl;

    return 0;
}