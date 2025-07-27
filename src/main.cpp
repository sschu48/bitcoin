// src/explained_demo.cpp - Understanding What We Built
#include <iostream>
#include <ctime>
#include "crypto/hash.h"
#include "crypto/keys.h"
#include "transaction/transaction.h"
#include "blockchain/block.h"

int main() {
    std::cout << "🤔 === UNDERSTANDING WHAT WE BUILT === 🤔" << std::endl;
    
    // ====================================================================
    // PART 1: COINBASE TRANSACTION (How new bitcoins are created)
    // ====================================================================
    std::cout << "\n🏦 PART 1: COINBASE TRANSACTION" << std::endl;
    std::cout << "This is how NEW bitcoins enter the system!" << std::endl;
    
    bitcoin::Transaction coinbase;
    
    // COINBASE INPUT: Special input that doesn't spend existing bitcoins
    std::cout << "\n--- Coinbase Input (Special!) ---" << std::endl;
    bitcoin::TransactionInput coinbase_input;
    coinbase_input.previous_txid = std::string(64, '0'); // All zeros = "no previous transaction"
    coinbase_input.vout = 0xFFFFFFFF;                    // Special number = "creating new money"
    coinbase_input.script_sig = "Mining reward for block #123456";
    coinbase.inputs.push_back(coinbase_input);
    
    std::cout << "Previous TXID: " << coinbase_input.previous_txid.substr(0, 16) << "... (all zeros = new money)" << std::endl;
    std::cout << "Message: " << coinbase_input.script_sig << std::endl;
    
    // COINBASE OUTPUT: Where the new bitcoins go (to the miner)
    std::cout << "\n--- Coinbase Output (Miner's Reward) ---" << std::endl;
    crypto::PrivateKey miner_key;
    crypto::PublicKey miner_public(miner_key);
    std::string miner_address = miner_public.to_bitcoin_address();
    
    bitcoin::TransactionOutput coinbase_output;
    coinbase_output.value = 5000000000; // 50 BTC (early Bitcoin reward)
    coinbase_output.script_pubkey = "Pay 50 BTC to: " + miner_address;
    coinbase.outputs.push_back(coinbase_output);
    
    std::cout << "Miner gets: " << coinbase_output.get_btc_amount() << " BTC" << std::endl;
    std::cout << "Miner's address: " << miner_address << std::endl;
    std::cout << "💡 This is how bitcoins are created - miners get rewarded!" << std::endl;
    
    // ====================================================================
    // PART 2: REGULAR TRANSACTION (Normal payment between people)
    // ====================================================================
    std::cout << "\n💳 PART 2: REGULAR TRANSACTION" << std::endl;
    std::cout << "Alice sends bitcoin to Bob (normal payment)" << std::endl;
    
    bitcoin::Transaction payment;
    
    // Create Alice and Bob
    crypto::PrivateKey alice_key;
    crypto::PublicKey alice_public(alice_key);
    crypto::PrivateKey bob_key;
    crypto::PublicKey bob_public(bob_key);
    
    std::cout << "\nAlice's address: " << alice_public.to_bitcoin_address() << std::endl;
    std::cout << "Bob's address:   " << bob_public.to_bitcoin_address() << std::endl;
    
    // ALICE'S INPUT: She's spending bitcoins she received before
    std::cout << "\n--- Alice's Input (What she's spending) ---" << std::endl;
    bitcoin::TransactionInput alice_input;
    alice_input.previous_txid = "abc123def456..."; // Some previous transaction where Alice received bitcoins
    alice_input.vout = 0;                          // She's spending output #0 from that transaction
    alice_input.script_sig = "Alice's signature proving she owns those bitcoins";
    payment.inputs.push_back(alice_input);
    
    std::cout << "Alice is spending from transaction: " << alice_input.previous_txid << std::endl;
    std::cout << "Output index: " << alice_input.vout << std::endl;
    std::cout << "💡 Alice proves ownership with her digital signature!" << std::endl;
    
    // PAYMENT OUTPUTS: Where the money goes
    std::cout << "\n--- Payment Outputs (Where money goes) ---" << std::endl;
    
    // Output 1: Payment to Bob
    bitcoin::TransactionOutput to_bob;
    to_bob.value = 100000000; // 1 BTC
    to_bob.script_pubkey = "Pay 1 BTC to: " + bob_public.to_bitcoin_address();
    payment.outputs.push_back(to_bob);
    
    // Output 2: Change back to Alice (if she had more than 1 BTC)
    bitcoin::TransactionOutput change_to_alice;
    change_to_alice.value = 50000000; // 0.5 BTC change
    change_to_alice.script_pubkey = "Pay 0.5 BTC back to: " + alice_public.to_bitcoin_address();
    payment.outputs.push_back(change_to_alice);
    
    std::cout << "Bob receives: " << to_bob.get_btc_amount() << " BTC" << std::endl;
    std::cout << "Alice gets change: " << change_to_alice.get_btc_amount() << " BTC" << std::endl;
    std::cout << "💡 Like getting change when you pay with a $20 bill for a $15 item!" << std::endl;
    
    // ====================================================================
    // PART 3: BLOCK (Bundling transactions together)
    // ====================================================================
    std::cout << "\n📦 PART 3: BLOCK" << std::endl;
    std::cout << "Bundling transactions into a block (like a page in a ledger)" << std::endl;
    
    bitcoin::Block block;
    block.transactions.push_back(coinbase);  // First transaction = coinbase (mining reward)
    block.transactions.push_back(payment);   // Other transactions = regular payments
    
    // BLOCK HEADER: Metadata about this block
    std::cout << "\n--- Block Header (Block's ID card) ---" << std::endl;
    block.header.version = 1;
    block.header.previous_block_hash = "000000b873e79784647a6c82962c70d228557d24a747ea4d1b8bbe878e1206";
    block.header.merkle_root = block.calculate_merkle_root();
    block.header.timestamp = static_cast<uint32_t>(std::time(nullptr));
    block.header.bits = 4; // Difficulty
    block.header.nonce = 0; // We'll find this through mining
    
    std::cout << "Previous block: " << block.header.previous_block_hash.substr(0, 16) << "..." << std::endl;
    std::cout << "Merkle root: " << block.header.merkle_root.substr(0, 16) << "... (summary of all transactions)" << std::endl;
    std::cout << "Timestamp: " << block.header.timestamp << std::endl;
    std::cout << "💡 This links to the previous block, creating a chain!" << std::endl;
    
    // ====================================================================
    // PART 4: MINING (Finding the magic number)
    // ====================================================================
    std::cout << "\n⛏️ PART 4: MINING" << std::endl;
    std::cout << "Finding a nonce that makes the block hash start with zeros" << std::endl;
    
    std::cout << "\nBefore mining:" << std::endl;
    std::cout << "Block hash: " << block.calculate_hash() << std::endl;
    std::cout << "Starts with zero? " << (block.header.has_valid_proof_of_work() ? "YES" : "NO") << std::endl;
    
    std::cout << "\nMining..." << std::endl;
    uint32_t attempts = 0;
    while (!block.header.has_valid_proof_of_work() && attempts < 100000) {
        block.header.nonce++;
        attempts++;
    }
    
    std::cout << "\nAfter mining:" << std::endl;
    std::cout << "Found nonce: " << block.header.nonce << " after " << attempts << " attempts" << std::endl;
    std::cout << "Block hash: " << block.calculate_hash() << std::endl;
    std::cout << "Starts with zero? " << (block.header.has_valid_proof_of_work() ? "YES ✅" : "NO") << std::endl;
    std::cout << "💡 This proves the miner did computational work!" << std::endl;
    
    // ====================================================================
    // SUMMARY: What we built and why
    // ====================================================================
    std::cout << "\n🎉 === WHAT WE BUILT === 🎉" << std::endl;
    std::cout << "✅ Coinbase Transaction: How new bitcoins are created" << std::endl;
    std::cout << "✅ Regular Transaction: How people send bitcoins to each other" << std::endl;
    std::cout << "✅ Digital Signatures: How we prove ownership (from Phase 1)" << std::endl;
    std::cout << "✅ Bitcoin Addresses: Where bitcoins are sent (from Phase 1)" << std::endl;
    std::cout << "✅ Blocks: How transactions are bundled together" << std::endl;
    std::cout << "✅ Mining: How the network agrees on what happened" << std::endl;
    
    std::cout << "\n🔗 THE BITCOIN CHAIN:" << std::endl;
    std::cout << "Block 1 → Block 2 → Block 3 → ... (each links to previous)" << std::endl;
    std::cout << "Each block contains transactions" << std::endl;
    std::cout << "Each transaction moves bitcoins between addresses" << std::endl;
    std::cout << "Digital signatures prove who authorized each transaction" << std::endl;
    
    return 0;
}