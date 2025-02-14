#include <iostream>
#include <vector>
#include "block.hpp"

int main() {
    int n, public_key, private_key;
    generateKeys(n, public_key, private_key); // Generate keys for signing
    std::cout << "N for Publickey and Privatekey: " << n << std::endl;
    std::cout << "Publickey: " << public_key << std::endl;
    std::cout << "Privatekey: " << private_key << std::endl;

    int userBalance = 0;
    std::vector<Transaction> transactions;
    std::vector<Transaction> transactions_all;
    if (userBalance >= 10) {
        transactions.emplace_back(to_string(public_key), 10, to_string(public_key));
        transactions.back().signTransaction(private_key, n); // Sign the transaction
        transactions.emplace_back(to_string(public_key), 5, to_string(public_key));
        transactions.back().signTransaction(private_key, n); // Sign the transaction
    }
    
    // Create the genesis block
    Block genesisBlock(0, "0", transactions, public_key);
    genesisBlock.displayBlock();
    transactions_all.insert(transactions_all.end(), genesisBlock.transactions.begin(), genesisBlock.transactions.end());
    userBalance = getBalance(to_string(public_key), transactions_all);

    int blockCount = 1; // To keep track of the number of blocks created

    while (true) {
        // Create new transactions for the next block
        transactions.clear();
        if (userBalance >= 13) {
            transactions.emplace_back(to_string(public_key), 12, to_string(public_key));
            transactions.back().signTransaction(private_key, n); // Sign the transaction
            transactions.emplace_back(to_string(public_key), 13, to_string(public_key));
            transactions.back().signTransaction(private_key, n); // Sign the transaction
        }
        // Create a new block
        Block newBlock(blockCount, genesisBlock.hash, transactions, public_key);
        newBlock.displayBlock();
        transactions_all.insert(transactions_all.end(), genesisBlock.transactions.begin(), genesisBlock.transactions.end());

        // Verify transactions in the new block
        for (const auto& tx : newBlock.transactions) {
            if (tx.publicKey == "0"){
                std::cout << "Mined block coins: " << tx.howMany << ", to: " << tx.receiver_publicKey << std::endl;
            }
            else if (tx.verifyTransaction(n)) {
                std::cout << "Transaction verified: " << tx.publicKey << " -> " << tx.receiver_publicKey << " How much coins: " << tx.howMany << std::endl;
            } 
            else {
                std::cout << "Transaction verification failed: " << tx.publicKey << " -> " << tx.receiver_publicKey << " How much coins: " << tx.howMany << std::endl;
            }
        }
        userBalance = getBalance(to_string(public_key), transactions_all);
        std::cout << "User balance: " << userBalance << std::endl;
        std::cout << "------------------" << std::endl;

        // Update the previous block to the new block
        genesisBlock = newBlock;

        // Increment the block count
        blockCount++;
    }

    return 0;
}