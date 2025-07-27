# Bitcoin Network Implementation Plan

## Project Overview
Build a functional Bitcoin-like cryptocurrency network in C++ that demonstrates:
- Peer-to-peer networking
- Transaction creation and validation
- Proof-of-work mining
- Blockchain consensus
- Digital signatures and cryptographic security

## Phase 1: Core Cryptographic Foundation (Week 1-2)

### 1.1 Hash Functions (Bitcoin's Core Tool)
- **SHA-256**: Primary hash function used throughout Bitcoin
- **RIPEMD-160**: Secondary hash function for shortening public keys
- **Double SHA-256**: Used for transaction IDs and block hashes
- **Hash160**: Combination of SHA-256 + RIPEMD-160 for addresses

### 1.2 Digital Signatures (ECDSA)
- **secp256k1** elliptic curve (Bitcoin's specific curve)
- Private key generation (256-bit random number)
- Public key derivation from private key
- **DER encoding** for signature serialization
- Message signing and signature verification

### 1.3 Address Generation System
- **Public Key → Address conversion**
- **Base58Check encoding** (Bitcoin's address format)
- **WIF (Wallet Import Format)** for private keys
- Address validation and checksum verification

### 1.4 Basic Data Structures
```cpp
// Transaction structure matching Bitcoin exactly
class Transaction {
    uint32_t version;               // Transaction version (1 or 2)
    vector<TransactionInput> inputs;
    vector<TransactionOutput> outputs;
    uint32_t locktime;             // Transaction locktime
    string txid;                   // SHA-256 hash of transaction
};

class TransactionInput {
    string previousTxid;           // Reference to previous transaction
    uint32_t vout;                // Output index in previous transaction  
    Script scriptSig;             // Unlocking script
    uint32_t sequence;            // Sequence number (for RBF/locktime)
};

class TransactionOutput {
    uint64_t value;               // Amount in satoshis
    Script scriptPubKey;          // Locking script
};

// Block structure matching Bitcoin format
class Block {
    BlockHeader header;
    vector<Transaction> transactions;
};

class BlockHeader {
    uint32_t version;             // Block version
    string previousBlockHash;     // Hash of previous block
    string merkleRoot;           // Merkle root of transactions
    uint32_t timestamp;          // Unix timestamp
    uint32_t bits;               // Difficulty target (compact format)
    uint32_t nonce;              // Proof-of-work nonce
};
```

## Phase 2: Bitcoin Script System & Transaction Logic (Week 3-4)

### 2.1 Script Engine Implementation
- **Script opcodes**: OP_DUP, OP_HASH160, OP_EQUALVERIFY, OP_CHECKSIG, etc.
- **Stack-based execution model** (like Bitcoin)
- **Standard script types**:
  - **P2PKH** (Pay-to-Public-Key-Hash) - most common
  - **P2SH** (Pay-to-Script-Hash) - for complex scripts
  - **P2PK** (Pay-to-Public-Key) - legacy format
  - **Multisig** scripts for multiple signatures

### 2.2 UTXO (Unspent Transaction Output) System
- **UTXO set management** (core to Bitcoin's design)
- **Coinbase transactions** (mining rewards)
- **Transaction validation rules**:
  - Input amounts must equal output amounts + fees
  - All inputs must reference valid UTXOs
  - Scripts must execute successfully
- **Double-spending prevention**

### 2.3 Transaction Construction & Validation
- **Raw transaction format** (exactly like Bitcoin)
- **Transaction serialization/deserialization**
- **SIGHASH types** (ALL, NONE, SINGLE, ANYONECANPAY)
- **Signature creation and verification**
- **Transaction fee calculation**
- **Replace-by-Fee (RBF)** support via sequence numbers

### 2.4 Advanced Transaction Features
- **Locktime support** (time-based transaction delays)
- **Sequence numbers** for relative timelocks
- **Multi-input/multi-output transactions**
- **Change outputs** (returning excess to sender)

## Phase 3: Mining & Proof-of-Work System (Week 5-6)

### 3.1 Bitcoin's Proof-of-Work Algorithm
- **SHA-256 double hashing** for block headers
- **Target difficulty system** (bits field conversion)
- **Difficulty adjustment** every 2016 blocks (~2 weeks)
- **Nonce searching** (0 to 4.2 billion range)

### 3.2 Merkle Tree Implementation
- **Binary tree structure** for transaction hashing
- **Merkle root calculation** (for block headers)
- **Merkle branch verification** (for SPV)
- **Transaction ordering** (coinbase first, then by fee rate)

### 3.3 Block Construction & Mining
- **Block template creation**:
  - Coinbase transaction (mining reward + fees)
  - Transaction selection from mempool
  - Merkle root calculation
  - Block header construction
- **Mining loop**:
  - Increment nonce
  - Hash block header
  - Check if hash meets target
  - Broadcast successful block

### 3.4 Difficulty & Target System
- **Compact target format** (bits field)
- **Difficulty calculation** from target
- **Block time targeting** (10 minutes average)
- **Difficulty adjustment algorithm**

### 3.5 Mining Economics
- **Block reward system** (starts at 50 BTC, halves every 210,000 blocks)
- **Transaction fee prioritization**
- **Mining profitability calculations**

## Phase 4: Blockchain & Consensus (Week 7-8)

### 4.1 Blockchain Data Structure
- **Chain of blocks** with cryptographic links
- **Genesis block** (hardcoded first block)
- **Block validation rules**:
  - Valid proof-of-work
  - Valid previous block hash
  - All transactions valid
  - Proper merkle root
  - Timestamp validation

### 4.2 Chain Reorganization & Forks
- **Longest chain rule** (most cumulative work)
- **Orphan block handling**
- **Chain reorganization** when longer chain found
- **Fork resolution** mechanism
- **Temporary disagreement** handling

### 4.3 Block Storage & Indexing
- **Raw block data storage** (blk*.dat files like Bitcoin Core)
- **Block index** for fast lookups
- **UTXO set persistence**
- **Transaction indexing** by TXID
- **Block height tracking**

### 4.4 Consensus Rules Implementation
- **Block size limits** (1MB for legacy compatibility)
- **Transaction validation rules**
- **Script execution limits**
- **Coinbase maturity** (100 confirmations)
- **Maximum money supply** (21 million coins)

## Phase 5: Peer-to-Peer Network Protocol (Week 9-10)

### 5.1 Bitcoin Network Protocol
- **TCP connections** on port 8333 (mainnet) / 18333 (testnet)
- **Message structure**: magic bytes + command + length + checksum + payload
- **Network message types**:
  - `version`/`verack` - connection handshake
  - `inv` - inventory announcements
  - `getdata` - request specific data
  - `tx` - transaction broadcasts
  - `block` - block broadcasts
  - `ping`/`pong` - keep-alive

### 5.2 Peer Discovery & Management
- **DNS seed nodes** for initial peer discovery
- **addr** messages for peer address sharing
- **Connection management** (max 8 outbound, 125 inbound)
- **Peer scoring** and ban management
- **Connection encryption** (optional BIP 151)

### 5.3 Data Synchronization
- **Initial Block Download (IBD)**
- **Headers-first synchronization**
- **Block relay** and validation
- **Transaction relay** and mempool management
- **Compact block relay** (BIP 152)

### 5.4 Network Types
- **Mainnet** (production network)
- **Testnet** (testing network)
- **Regtest** (regression testing, local only)
- **Signet** (newer test network with controlled block signing)

## Phase 6: Wallet & Advanced Features (Week 11-12)

### 6.1 HD (Hierarchical Deterministic) Wallet System
- **BIP 32** - HD wallet structure
- **BIP 39** - Mnemonic seed phrases
- **BIP 44** - Multi-account hierarchy
- **Master key derivation** from seed
- **Child key derivation** (hardened and non-hardened)

### 6.2 Simplified Payment Verification (SPV)
- **Merkle branch verification** without full blockchain
- **Block header chain** verification
- **Bloom filters** for transaction filtering (BIP 37)
- **Lightweight client** implementation

### 6.3 Modern Bitcoin Features (Optional Extensions)
- **Segregated Witness (SegWit)** support:
  - P2WPKH and P2WSH script types
  - Witness data structure
  - Weight-based block size calculation
- **Bech32 addresses** (native SegWit format)
- **Replace-by-Fee (RBF)** transaction replacement

### 6.4 Development Tools & Interfaces
- **Command-line interface** (like bitcoin-cli)
- **RPC server** for external tool integration
- **Block explorer** functionality
- **Raw transaction tools**
- **Address and key utilities**

## Implementation Architecture

### Core Bitcoin Components We'll Implement

Based on LearnMeABitcoin's comprehensive coverage, our implementation will include all these essential Bitcoin components:

**Cryptographic Layer:**
- SHA-256 and RIPEMD-160 hash functions
- ECDSA with secp256k1 curve
- Base58Check encoding for addresses
- Merkle tree construction and verification

**Transaction System:**
- UTXO model with proper tracking
- Script engine with all standard opcodes
- P2PKH, P2SH, P2PK, and multisig support
- Transaction serialization matching Bitcoin format exactly
- Proper SIGHASH implementation

**Mining & Consensus:**
- SHA-256 proof-of-work with target difficulty
- Difficulty adjustment every 2016 blocks
- Coinbase transactions with proper reward halving
- Merkle root calculation for block headers
- Longest chain rule with fork resolution

**Network Protocol:**
- Full Bitcoin P2P message protocol implementation
- Proper handshake with version/verack messages
- Transaction and block relay mechanisms
- Initial block download (IBD) process
- Mempool management

**Data Structures:**
- Block format identical to Bitcoin
- Transaction format with all fields
- Raw block storage system
- UTXO set persistence
- Block and transaction indexing

**Wallet Features:**
- HD wallet with BIP 32/39/44 support
- Address generation and management
- Transaction construction and signing
- Balance calculation from UTXO set
- SPV client capabilities
```cpp
// External dependencies
#include <openssl/sha.h>     // SHA-256 hashing
#include <openssl/ecdsa.h>   // Digital signatures
#include <openssl/ripemd.h>  // RIPEMD160 for addresses
#include <boost/asio.hpp>    // Networking
#include <nlohmann/json.hpp> // JSON serialization
```

### Project Structure
```
bitcoin-clone/
├── src/
│   ├── crypto/           # Cryptographic functions
│   ├── transaction/      # Transaction logic
│   ├── mining/          # Proof-of-work and mining
│   ├── blockchain/      # Blockchain and consensus
│   ├── network/         # P2P networking
│   ├── wallet/          # Wallet functionality
│   └── main.cpp         # Entry point
├── tests/               # Unit tests
├── docs/                # Documentation
└── CMakeLists.txt       # Build configuration
```

## Key Learning Milestones

### Milestone 1: Digital Money
- Create and verify digital signatures
- Understand public-key cryptography
- Build basic transaction structure

### Milestone 2: Prevent Double-Spending
- Implement UTXO tracking
- Validate transaction chains
- Understand consensus importance

### Milestone 3: Decentralized Consensus
- Build proof-of-work system
- Implement longest chain rule
- Handle blockchain forks

### Milestone 4: Network Effects
- Create peer-to-peer communication
- Broadcast transactions and blocks
- Achieve network consensus

## Development Best Practices

### Security Considerations
- Never store private keys in plain text
- Validate all inputs thoroughly
- Use secure random number generation
- Implement proper error handling

### Testing Strategy
- Unit tests for each component
- Integration tests for system interactions
- Stress tests for network performance
- Security tests for attack scenarios

### Performance Optimization
- Efficient UTXO set management
- Fast block validation
- Optimized network communication
- Memory usage optimization

## Suggested Tools and Libraries

### Development Environment
- **Compiler**: GCC or Clang with C++17 support
- **Build System**: CMake
- **Testing**: Google Test framework
- **Debugging**: GDB or LLDB

### External Libraries
- **OpenSSL**: Cryptographic functions
- **Boost**: Networking and utilities
- **nlohmann/json**: JSON handling
- **spdlog**: Logging framework

## Learning Resources Integration

### Understanding Bitcoin Concepts
- Study the whitepaper sections as you implement each phase
- Compare your implementation with Bitcoin Core source code
- Test interoperability with Bitcoin testnet protocols

### Debugging and Validation
- Create test scenarios for each attack vector mentioned in the paper
- Implement the probability calculations from Section 11
- Verify your implementation matches expected cryptographic outputs

## Expected Timeline: 12 Weeks
- **Weeks 1-2**: Cryptographic foundation
- **Weeks 3-4**: Transaction system
- **Weeks 5-6**: Mining and proof-of-work
- **Weeks 7-8**: Blockchain consensus
- **Weeks 9-10**: Peer-to-peer networking
- **Weeks 11-12**: Advanced features and optimization

This plan will give you a deep understanding of every component that makes Bitcoin work, from cryptographic primitives to network consensus mechanisms.