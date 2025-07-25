#include <stdio.h>
#include <string.h>
#include "balance.h"

void balance_store_init(BalanceStore *store) {
    store->account_count = 0;
}

void balance_store_add(BalanceStore *store, const char *address, double balance) {
    if (store->account_count >= MAX_ACCOUNTS) {
        printf("Error: balance store full\n");
        return;
    }
    strncpy(store->accounts[store->account_count].address, address, ADDRESS_LENGTH -1);
    store->accounts[store->account_count].address[ADDRESS_LENGTH - 1] = '\0';
    store->accounts[store->account_count].balance = balance;
    store->account_count++;
}

double balance_store_get(BalanceStore *store, const char *address) {
    for (int i = 0; i < store->account_count; i++) {
        if (strncmp(store->accounts[i].address, address, ADDRESS_LENGTH) == 0) {
            return store->accounts[i].balance;
        }
    }
    return 0.0; // default: no balance
}

int balance_store_update(BalanceStore *store, const Transaction *txn) {
    double sender_balance = balance_store_get(store, txn->sender);
    if (sender_balance < txn->amount) {
        printf("Error: insufficient balance for %s (%.2f < %.2f)\n", txn->sender, sender_balance, txn->amount);
        return 0;
    }
    int sender_found = 0, receiver_found = 0;
    for (int i = 0; i < store->account_count; i++) { 
        if (strncmp(store->accounts[i].address, txn->sender, ADDRESS_LENGTH) == 0) {
            store->accounts[i].balance -= txn->amount;
            sender_found = 1;
            printf("Updated %s balance: %.2f\n", txn->sender, store->accounts[i].balance);
        }
        if (strncmp(store->accounts[i].address, txn->receiver, ADDRESS_LENGTH) == 0) {
            store->accounts[i].balance += txn->amount;
            receiver_found = 1;
            printf("Updated %s balance: %.2f\n", txn->receiver, store->accounts[i].balance);
        }
    }
    if (!sender_found || !receiver_found) {
        printf("Error: sender or receiver not found in balance store\n");
        return 0;
    }
    return 1;
}

void balance_store_print(const BalanceStore *store) {
    printf("Balance Store:\n");
    for (int i = 0; i < store->account_count; i++) {
        printf("    %s: %.2f\n", store->accounts[i].address, store->accounts[i].balance);
    }
}