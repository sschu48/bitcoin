#include <stdio.h>
#include "transaction.h"

int main() {
    Transaction txn;
    transaction_init(&txn, "Sean", "Devon", 50.0);
    transaction_print(&txn);
    return 0;
}