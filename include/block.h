// what is a block? 
// a container that groups multiple transactions. it is timestamped and hashed
// with a linkage to the previous block
// this forms a "blockchain"

// the blockchain is the public ledger of all transactions 
// each block included a nonce for proof of work

// transactions are collected by nodes once sent out to the network then 
// stored in a block
// block is hashed then linked to previous blocl

#ifndef BLOCK_H
#define BLOCK_H

#include "transaction.h"
#include "balance.h"
#include <openssl/evp.h>

#define HASH_LENGTH 32 // 32 bytes for SHA-256
#define MAX_TRANSACTIONS 10 // max transactionls per block
#define DIFFICULTY 2 // number of leading zero bytes in hash

typedef struct {
    Transaction transactions[MAX_TRANSACTIONS]; // array of transactions
    int transaction_count; // number of transactions
    unsigned char prev_hash[HASH_LENGTH]; // previous block's hash
    unsigned char hash[HASH_LENGTH]; // current block's hash
    long timestamp; // creation time
    unsigned long nonce; // for PoW
} Block;

// function delcarations
void block_init(Block *block, const Transaction *transactions, int transaction_count, const unsigned char *prev_hash, EC_KEY **pubkeys, BalanceStore *store);
void block_compute_hash(Block *block);
void block_mine(Block *block);
void block_print(const Block *block);

#endif //BLOCK_H