// src/blockchain/block.h
#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "../transaction/transaction.h"

namespace bitcoin
{
    /**
     * Bitcoin Block Structure
     * 
     * From the whitepaper: "The network timestamps transactions by hashing them
     * into an ongoing chain of hash-based proof-of-work"
     */

class BlockHeader {
public:
    uint32_t version;                   // Block version number
    std::string previous_block_hash;    // Hash of the previous block (64 hex)
    std::string merkle_root;            // Merkle root of all transactions in this block
    uint32_t timestamp;                 // Unix timestamp when blcok was created
    uint32_t bits;                      // Difficulty target in compact format
    uint32_t nonce;                     // Proof-of-work nonce

    BlockHeader() : version(1), timestamp(0), bits(0), nonce(0) {}

    // Calculate the hash of this block header
    std::string calculate_hash() const;

    // Check if this block header has valid proof-of-work
    bool has_valid_proof_of_work() const;

    // Get difficulty target from bits field
    std::string get_target() const;

    // For debugging
    void print() const;
};

class Block {
public: 
    BlockHeader header;                     // Block header (80 bytes in real Bitcoin)
    std::vector<Transaction> transactions;   // All transactions in this block

    Block() {}

    // Calculate block hash (hash of the header)
    std::string calculate_hash() const { return header.calculate_hash(); }

    // Calculate merkle root of all transactions
    std::string calculate_merkle_root() const;

    // Get total block reward (coinbase + fees)
    uint64_t get_block_reward() const;

    // Get total transaction fees in this block
    uint64_t get_total_fees() const;

    // Validate all transactions in this block
    bool validate_transactions() const;

    // Check if this is the genesis block
    bool is_genesis_block() const;

    // For debugging
    void print() const;
};

} // namespace bitcoin