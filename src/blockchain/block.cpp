// src/blockchain/block.cpp
#include "block.h"
#include "../crypto/hash.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace bitcoin
{

std::string BlockHeader::calculate_hash() const {
    // In real Bitcoin, this serializes the 80-byte blcok header and double SHA256s it
    std::stringstream header_data;
    header_data << version << previous_block_hash << merkle_root
                << timestamp << bits << nonce;

    std::string data = header_data.str();
    return crypto::Hash::double_sha256(data);
}

bool BlockHeader::has_valid_proof_of_work() const {
    std::string block_hash = calculate_hash();

    // Very simple proof-of-work check: hash must start with at least one zero
    // Real Bitcoin uses the 'bits' field to determine required difficulty
    return block_hash.substr(0, 1) == "0";
}

std::string BlockHeader::get_target() const {
    // This is a simplified version. Real Bitcoin uses compact target format. 
    // For now, just return a simple target based on bits value
    std::stringstream target;
    target << std::string(bits / 4, '0') << std::string(64 - bits / 4, 'f');
    return target.str();
}

void BlockHeader::print() const {
    std::cout << "Block Header:" << std::endl;
    std::cout << "  Version:           " << version << std::endl;
    std::cout << "  Previous Hash:     " << previous_block_hash << std::endl;
    std::cout << "  Merkle Root:       " << merkle_root << std::endl;
    
    // Convert timestamp to readable format
    std::time_t time = static_cast<std::time_t>(timestamp);
    std::cout << "  Timestamp:         " << timestamp << " (" << std::ctime(&time) << ")" << std::endl;
    std::cout << "  Bits (Difficulty): " << bits << std::endl;
    std::cout << "  Nonce:             " << nonce << std::endl;
    std::cout << "  Block Hash:        " << calculate_hash() << std::endl;
    std::cout << "  Valid PoW:         " << (has_valid_proof_of_work() ? "Yes" : "No") << std::endl;
}

std::string Block::calculate_merkle_root() const {
    if (transactions.empty()) {
        return std::string(64, '0'); // All zeros if not transactions
    }

    // Simplified merkle root calculation
    // Real Bitcoin builds a binary tree of transaction hashes
    std::stringstream all_txids;
    for (const auto& tx : transactions) {
        all_txids << tx.calculate_txid();
    }

    return crypto::Hash::double_sha256(all_txids.str());
}

uint64_t Block::get_block_reward() const {
    if (transactions.empty() || !transactions[0].is_coinbase()) {
        return 0;
    }

    // The coinbase transaction (first transaction) contains the block reward
    return transactions[0].get_total_output_value();
}

uint64_t Block::get_total_fees() const {
    uint64_t total_fees = 0;

    // Skip coinbase transaction (index 0)
    for (size_t i = 0; i < transactions.size(); ++i) {
        total_fees += transactions[i].get_fee();
    }

    return total_fees;
}

bool Block::validate_transactions() const {
    if (transactions.empty()) {
        return false; // Block must have at least coinbase transaction
    }

    // First transaction must be coinbase
    if (!transactions[0].is_coinbase()) {
        return false;
    }

    // All other transactions must not be coinbase
    for (size_t i = 1; i < transactions.size(); ++i) {
        if (transactions[i].is_coinbase()) {
            return false;
        }
    }

    // Additional validation would go here in real implementation
    return true;
}

bool Block::is_genesis_block() const {
    // Genesis block has previous hash of all zeros
    return header.previous_block_hash == std::string(64, '0');
}

void Block::print() const {
    std::cout << "=================== BLOCK ===================" << std::endl;
    header.print();
    std::cout << "\nBlock Details:" << std::endl;
    std::cout << "  Transaction Count: " << transactions.size() << std::endl;
    std::cout << "  Block Reward:      " << get_block_reward() << " satoshis" << std::endl;
    std::cout << "  Total Fees:        " << get_total_fees() << " satoshis" << std::endl;
    std::cout << "  Is Genesis:        " << (is_genesis_block() ? "Yes" : "No") << std::endl;
    std::cout << "  Valid Transactions:" << (validate_transactions() ? "Yes" : "No") << std::endl;
    
    std::cout << "\nTransactions:" << std::endl;
    for (size_t i = 0; i < transactions.size(); ++i) {
        std::cout << "\n--- Transaction " << i << " ---" << std::endl;
        transactions[i].print();
    }
    std::cout << "=============================================" << std::endl;
}
    
} // namespace bitcoin
