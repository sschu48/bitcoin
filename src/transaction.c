#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include "transaction.h"

EC_KEY *generate_key_pair(void) {
    EC_KEY *key = EC_KEY_new_by_curve_name(NID_secp256k1);
    if (!key || !EC_KEY_generate_key(key)) {
        printf("Error: failed to generate ECDSA key\n");
        return NULL;
    }
    return key;
}

void free_key_pair(EC_KEY *key) {
    if (key) EC_KEY_free(key);
}

void get_address_from_pubkey(EC_KEY *key, char *address, size_t address_len) {
    unsigned char *pubkey = NULL;
    int pubkey_len = i2o_ECPublicKey(key, &pubkey);
    if (!pubkey || pubkey_len <= 0) {
        printf("Error: failed to get public key\n");
        strncpy(address, "invalid", address_len);
        address[address_len - 1] = '\0';
        return;
    }
    printf("public key length: %d\n", pubkey_len);
    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned int hash_len;
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx || !EVP_DigestInit_ex(ctx, EVP_sha256(), NULL)) {
        printf("Error: failed to initialize SHA-256\n");
        free(pubkey);
        EVP_MD_CTX_free(ctx);
        return;
    }
    EVP_DigestUpdate(ctx, pubkey, pubkey_len);
    EVP_DigestFinal_ex(ctx, hash, &hash_len);
    EVP_MD_CTX_free(ctx);
    free(pubkey);
    for (int i = 0; i < ADDRESS_LENGTH - 1 && i < hash_len * 2; i += 2) {
        snprintf(address + i, address_len - i, "%02x", hash[i / 2]);
    }
    address[ADDRESS_LENGTH - 1] = '\0';
    printf("generated address: %s\n", address);
}

void transaction_init(Transaction *txn, EC_KEY *sender_key, const char *receiver, double amount) {
    get_address_from_pubkey(sender_key, txn->sender, ADDRESS_LENGTH);
    strncpy(txn->receiver, receiver, ADDRESS_LENGTH - 1);
    txn->receiver[ADDRESS_LENGTH - 1] = '\0'; // ensure null-termination
    txn->amount = amount;
    memset(txn->signature, 0, SIGNATURE_LENGTH); 
    txn->signature_len = 0;
    transaction_sign(txn, sender_key);
}

int transaction_sign(Transaction *txn, EC_KEY *key) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned int hash_len;
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx || !EVP_DigestInit_ex(ctx, EVP_sha256(), NULL)) {
        printf("Error: failed to initialize SHA-256\n");
        EVP_MD_CTX_free(ctx);
        return 0;
    }
    EVP_DigestUpdate(ctx, txn->sender, strlen(txn->sender));
    EVP_DigestUpdate(ctx, txn->receiver, strlen(txn->receiver));
    EVP_DigestUpdate(ctx, &txn->amount, sizeof(double));
    EVP_DigestFinal_ex(ctx, hash, &hash_len);
    EVP_MD_CTX_free(ctx);

    unsigned char *sig = txn->signature;
    unsigned int sig_len = SIGNATURE_LENGTH;
    if (!ECDSA_sign(0, hash, hash_len, sig, &sig_len, key)) {
        printf("Error: failed to sign transactions\n");
        return 0;
    }
    txn->signature_len = sig_len;
    return 1;
}

int transaction_verify(const Transaction *txn, EC_KEY *pubkey) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned int hash_len;
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx || !EVP_DigestInit_ex(ctx, EVP_sha256(), NULL)) {
        printf("Error: failed to initialize SHA-256\n");
        EVP_MD_CTX_free(ctx);
        return 0;
    }
    EVP_DigestUpdate(ctx, txn->sender, strlen(txn->sender));
    EVP_DigestUpdate(ctx, txn->receiver, strlen(txn->receiver));
    EVP_DigestUpdate(ctx, &txn->amount, sizeof(double));
    EVP_DigestFinal_ex(ctx, hash, &hash_len);
    EVP_MD_CTX_free(ctx);

    int result = ECDSA_verify(0, hash, hash_len, txn->signature, txn->signature_len, pubkey);
    return result == 1;
}

void transaction_print(const Transaction *txn) {
    printf("Transaction:\n");
    printf("    Sender: %s\n", txn->sender);
    printf("    Receiver: %s\n", txn->receiver); 
    printf("    Amount: %.2f\n", txn->amount);
    printf("    Signature: ");
    for (unsigned int i = 0; i < txn->signature_len; i++) {
        printf("%02x", txn->signature[i]);
    }
    printf("\n");
}