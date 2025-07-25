#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <openssl/ec.h>
#include <openssl/evp.h>

#define ADDRESS_LENGTH 40 //simplified address length
#define SIGNATURE_LENGTH 672// max ECDSA signature size (DER format)

typedef struct {
    char sender[ADDRESS_LENGTH];    // senders address
    char receiver[ADDRESS_LENGTH];  // receivers address
    double amount; // amount of coins
    unsigned char signature[SIGNATURE_LENGTH];
    unsigned int signature_len; 
} Transaction;

// key management
EC_KEY *generate_key_pair(void);
void free_key_pair(EC_KEY *key);
void get_address_from_pubkey(EC_KEY *key, char *address, size_t address_len);

// transaction functions
void transaction_init(Transaction *txn, EC_KEY *sender_key, const char *receiver, double amount);
int transaction_sign(Transaction *txn, EC_KEY *key);
int transaction_verify(const Transaction *txn, EC_KEY *pubkey);
void transaction_print(const Transaction *txn);

#endif // Transaction_H