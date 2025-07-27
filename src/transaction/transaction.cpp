// src/transaction/transaction.cpp
#include "transaction.h"
#include "../crypto/hash.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace bitcoin {

void TransactionInput::print() const {
    std::cout << "  Input:" << std::endl;
    std::cout << "      Previous TXID:  " << previous_txid << std::endl;
    std::cout << "      Output Index:   " << vout << std::endl;
    std::cout << "      Script Sig      " << script_sig << std::endl;
    std::cout << "      Sequence:       " << sequence  << std::endl;
}

void TransactionOutput::print() const {
    std::cout << "  Output:" << std::endl;
    std::cout << "      Value:         " << value << " satoshis ("
              << std::fixed << std::setprecision(8) << get_btc_amount() << " BTC" << std::endl;
    std::cout << "      Script Pubkey: " << script_pubkey << std::endl;
}

std::string Transaction::calculate_txid() const {
    // In real Bitcoin, this would serialize the entire transaction
    // and double SHA256 it. For simplicity, we'll create a representative hash

    std::stringstream tx_data;
    tx_data << version;

    // Add all inputs
    for (const auto& input : inputs) {
        tx_data << input.previous_txid << input.vout << input.script_sig << input.sequence;
    }

    // Add all outputs
    for (const auto& output : outputs) {
        tx_data << output.value << output.script_pubkey;
    }

    tx_data << locktime;

    // Calculate double SAH256
    std::string data = tx_data.str();
    txid = crypto::Hash::double_sha256(data);
    return txid;
}

uint64_t Transaction::get_total_input_value() const {
    // Note: In real Bitcoin, you'd need to look up the previous transactions
    // to know the input values. For demo purposes, we'll estimate.
    uint64_t total = 0;
    // This is simplified - real implementation would look up previous outputs
    return total;
}

uint64_t Transaction::get_total_output_value() const {
    uint64_t total = 0; 
    for (const auto& output : outputs) {
        total += output.value;
    }
    return total;
}

uint64_t Transaction::get_fee() const {
    // Fee = Total INputs - Total Outputs
    // For now, return 0 since we can't easaily calculate input values
    return 0;
}

bool Transaction::is_coinbase() const {
    // Coinbase transaction has exactly one input with:
    // - previous_txid of all zeros
    // - vout of 0xFFFFFFFF
    return inputs.size() == 1 &&
        inputs[0].previous_txid == std::string(64, '0') &&
        inputs[0].vout == 0xFFFFFFFF;
}

void Transaction::print() const {
    std::cout << "Transaction:" << std::endl;
    std::cout << "  TXID:       " << (txid.empty() ? calculate_txid() : txid) << std::endl;
    std::cout << "  Version:    " << version << std::endl;
    std::cout << "  Locktime:   " << locktime << std::endl;
    std::cout << "  Inputs (" << inputs.size() << "):" << std::endl;
    for (const auto& input : inputs) {
        input.print();
    }
    std::cout << "  Outputs (" << outputs.size() << "):" << std::endl;
    for (const auto& output : outputs) {
        output.print();
    }
    std::cout << "  Total Output Value: " << get_total_output_value()
              << " satoshis" << std::endl;
    std::cout << "  Is Coinbase: " << (is_coinbase() ? "Yes" : "No") << std::endl;
}

}