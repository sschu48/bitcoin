#include <stdio.h>
#include <string.h>
#include <time.h>
#include "block.h"

void block_init(Block *block, const Transaction *transactions, int transaction_count, const unsigned char *prev_hash, EC_KEY **pubkeys, BalanceStore *store) {
    if (transaction_count > MAX_TRANSACTIONS) {
        printf("Error: too many transactions\n");
        return;
    }
    block->transaction_count = 0;
    for (int i = 0; i < transaction_count; i++) {
        // verify transaction simplifedd assuming senders public key is available
        if (pubkeys && pubkeys[i] && transactions[i].signature_len > 0) {
            if (!transaction_verify(&transactions[i], pubkeys[i])) {
                printf("Error: transaction %d verification failed\n", i);
                continue;
            }
        } else if (transactions[i].signature_len > 0) {
            printf("Warning: transactions %d signature validation skipped (no pubkey)\n", i);
        }
        if (store && !balance_store_update(store, &transactions[i])) {
            printf("Error: transactions %d balance validation failed\n", i);
            continue;
        }
        block->transactions[i] = transactions[i]; // copy transactions
        block->transaction_count ++;
    }
    if (prev_hash) {
        memcpy(block->prev_hash, prev_hash, HASH_LENGTH); // copy previous hash
    } else {
        memset(block->prev_hash, 0, HASH_LENGTH); // zero for genesis block
    }
    block->timestamp = time(NULL); // current time
    block->nonce = 0;
    block_mine(block); // mine block instead of just computing hash
}

void block_compute_hash(Block *block) {
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if(!ctx) {
        printf("Error: failed to create EVP context\n");
        return;
    }
    if (!EVP_DigestInit_ex(ctx, EVP_sha256(), NULL)) {
        printf("Error: failed to initialize SHA-256\n");
        return;
    }

    // hash transactions
    for (int i = 0; i < block->transaction_count; i++) {
        EVP_DigestUpdate(ctx, block->transactions[i].sender, strlen(block->transactions[i].sender));
        EVP_DigestUpdate(ctx, block->transactions[i].receiver, strlen(block->transactions[i].receiver));
        EVP_DigestUpdate(ctx, &block->transactions[i].amount, sizeof(double));
        EVP_DigestUpdate(ctx, block->transactions[i].signature, block->transactions[i].signature_len);
    }
    // hash prev_hash, timestamp, nonce
    EVP_DigestUpdate(ctx, block->prev_hash, HASH_LENGTH);
    EVP_DigestUpdate(ctx, &block->timestamp, sizeof(long));
    EVP_DigestUpdate(ctx, &block->nonce, sizeof(unsigned long));

    unsigned int hash_len;
    if (!EVP_DigestFinal_ex(ctx, block->hash, &hash_len)) {
        printf("Error: failed to finalize hash\n");
        EVP_MD_CTX_free(ctx);
        return;
    }
    EVP_MD_CTX_free(ctx);
}

void block_mine(Block *block) {
    do {
        block->nonce++;
        block_compute_hash(block);
    } while (block->hash[0] != 0 || block->hash[1] != 0); // check for DIFFICULTY leading zeros
    printf("mined block with nonce: %lu\n", block->nonce);
}

void block_print(const Block *block) {
    printf("    Timestamp: %ld\n", block->timestamp);
    printf("    Previous hash: ");
    for (int i = 0; i < HASH_LENGTH; i++) {
        printf("%02x", block->prev_hash[i]);
    }
    printf("\n    Hash: ");
    for (int i = 0; i < HASH_LENGTH; i++) {
        printf("%02x", block->hash[i]);
    }
    printf("\n    Transactions (%d):\n", block->transaction_count);
    for (int i = 0; i < block->transaction_count; i++) {
        transaction_print(&block->transactions[i]);
    }
    printf("    Nonce: %lu\n", block->nonce);
} 