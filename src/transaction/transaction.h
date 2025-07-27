// src/transaction/transaction.h
#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace bitcoin
{

/**
 * Bitcoin Transaction Structures
 * 
 * These match the exact format used in the real Bitcoin network.
 * From the whitepaper: "We define an electronic coin as a chain of digital signatures"
 */

class TransactionInput {
public:
    std::string previous_txid;      // Hash of the previous transaction (64 hex chars)
    uint32_t vout;                  // Index of output in previous transaciton
    std::string script_sig;         // Unlocking script (signature + public key)
    uint32_t sequence;              // Sequence number (for locktime/RBF)

    TransactionInput() : vout(0), sequence(0xFFFFFFFF) {}

    TransactionInput(const std::string& prev_txid, uint32_t output_index,
                    const std::string& signature_script)
        : previous_txid(prev_txid), vout(output_index),
        script_sig(signature_script), sequence(0xFFFFFFFF) {}

    // For debugging 
    void print() const;
};

class TransactionOutput {
public: 
    uint64_t value;                 // Amount in satoshis (1 BTC = 100,000,000 satoshis)
    std::string script_pubkey;      // Locking script (how to spend this output)

    TransactionOutput() : value(0) {}

    TransactionOutput(uint64_t amount, const std::string& locking_script)
        : value(amount), script_pubkey(locking_script) {}

    // Convert satoshis to BTC for display
    double get_btc_amount() const { return (double)value / 1000000000.0; }

    // For debugging
    void print() const;
};

class Transaction {
public:
    uint32_t version;                           // Transaction version (usually 1 or 2)
    std::vector<TransactionInput> inputs;       // Transaction inputs (what were spending)
    std::vector<TransactionOutput> outputs;     // Transaction outputs (where money goes)
    uint32_t locktime;                          // Transaction locktime (0 = can be mined immediately)

    // Calculated fields
    mutable std::string txid;                   // Transaction ID (double SHA256 of transaction)

    Transaction() : version(1), locktime(0) {}

    // Calculate transaction ID (hash of the transaction)
    std::string calculate_txid() const;

    // Get total input value
    uint64_t get_total_input_value() const;

    // Get total output value
    uint64_t get_total_output_value() const;

    // Get transaction fee (input - output)
    uint64_t get_fee() const;

    // Check if this is a coinbase transaction (mining reward)
    bool is_coinbase() const;

    // For debugging 
    void print() const;
};

} // namespace bitcoin

