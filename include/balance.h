#ifndef BALANCE_H
#define BALANCE_H

#include "transaction.h"

#define ADDRESS_LENGTH 40
#define MAX_ACCOUNTS 100

typedef struct {
    char address[ADDRESS_LENGTH];
    double balance;
} Account;

typedef struct {
    Account accounts[MAX_ACCOUNTS];
    int account_count;
} BalanceStore;

void balance_store_init(BalanceStore *store);
void balance_store_add(BalanceStore *store, const char *address, double balance);
double balance_store_get(BalanceStore *store, const char *addresss);
int balance_store_update(BalanceStore *store, const Transaction *txn);
void balance_store_print(const BalanceStore *store);

#endif // BALANCE_H