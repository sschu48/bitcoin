#include <stdio.h>
#include "block.h"
#include "balance.h"

#define CHAIN_LENGTH 2 // three blocks

int main() {
    // generate key pairs
    EC_KEY *alice_key = generate_key_pair();
    EC_KEY *bob_key = generate_key_pair();
    if (!alice_key || !bob_key) {
        printf("Error: key generation failed\n");
        free_key_pair(alice_key);
        free_key_pair(bob_key);
        return 1;
    }

    char alice_addr[ADDRESS_LENGTH], bob_addr[ADDRESS_LENGTH];
    get_address_from_pubkey(alice_key, alice_addr, ADDRESS_LENGTH);
    get_address_from_pubkey(bob_key, bob_addr, ADDRESS_LENGTH);
    printf("Alice address: %s\n", alice_addr);
    printf("Bob address: %s\n", bob_addr);
    if (strncmp(alice_addr, bob_addr, ADDRESS_LENGTH) == 0) { // verify address aren't empty
        printf("Error: Alice and Bob addresses are identical\n");
        free_key_pair(alice_key);
        free_key_pair(bob_key);
        return 1;
    }

    BalanceStore store;
    balance_store_init(&store);
    balance_store_add(&store, alice_addr, 10.0);
    balance_store_add(&store, bob_addr, 10.0);
    balance_store_add(&store, "Charlie", 0.0);

    // create transactions
    Transaction txns[2];
    transaction_init(&txns[0], alice_key, bob_addr, 5.0);
    transaction_init(&txns[1], bob_key, "Charlie", 3.0);

    EC_KEY *pubkeys[2] = {alice_key, bob_key};

    // verify transactions
    if (!transaction_verify(&txns[0], alice_key)) {
        printf("Error: transaction 0 verification failed\n");
    } else {
        printf("Transaction 0 verified\n");
    } 
    if (!transaction_verify(&txns[1], bob_key)) {
        printf("Error: transactions 1 verification failed\n");
    } else {
        printf("Transaction 1 verified\n");
    }

    // create blockchain
    Block chain[CHAIN_LENGTH];
    block_init(&chain[0], NULL, 0, NULL, NULL, NULL); // no transactions
    block_init(&chain[1], txns, 2, chain[0].hash, pubkeys, &store);

    // print blockchain
    for (int i = 0; i < CHAIN_LENGTH; i++) {
        printf("Block %d:\n", i);
        block_print(&chain[i]);
        printf("\n\n");
    }

    balance_store_print(&store);

    free_key_pair(alice_key);
    free_key_pair(bob_key);
    return 0;
}