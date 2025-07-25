#ifndef UTXO_H
#define UTXO_H

#define ADDRESS_LENGTH 40
#define TXID_LENGTH 65 // 64 hex chars + null
#define MAX_UTXOS 1000

// a UTXO represents an unspent "coin" of a specific value, like a dollar
typedef struct {
    char txid[TXID_LENGTH];         // transaction ID that created this UTXO
    int output_index;               // which output in that transation 
    char owner[ADDRESS_LENGTH];     // address that can spend this UTXO
    double amount;                  // value of UTXO
    int is_spent;                   // 0 = unspent, 1 = spent
} UTXO;

// the UTXO set tracks all unspent transaction outputs in the sytem
typedef struct {
    UTXO utxos[MAX_UTXOS];
    int utxo_count;
} UTXOSet;

// UTXO management functions
void utxo_set_init(UTXOSet *set);
void utxo_set_add(UTXOSet *set, const char *txid, int output_index, const char *owner, double amount);
UTXO* utxo_set_find(UTXOSet *set, const char *txid, int output_index);
int utxo_set_spend(UTXOSet *set, const char *txid, int output_index);
double utxo_set_get_balance(const UTXOSet *set, const char *address);
void utxo_set_print(const UTXOSet *set);

#endif // UTXO_H