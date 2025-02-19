#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <iostream>
#include <string>
#include <ctime>
#include <sstream>
#include <vector>
#include <fstream>
#include <filesystem>
#include "SHA1/sha1.hpp"
#include "Transaction.hpp"

class Block {
public:
    int index;                          // Индекс блока
    std::string previousHash;           // Хэш предыдущего блока
    std::string timestamp;              // Временная метка создания блока
    std::vector<Transaction> transactions; // Список транзакций
    std::string hash;                   // Хэш текущего блока
    int nonce;                          // Нонс для доказательства работы
    static const int difficulty = 4;    // Уровень сложности (количество ведущих нулей)
    int ProofOfWorkCoinAddress;         // Адрес. Добавляет транзакцию, отправляющую майнеру 3 монеты


    Block(int idx, const std::string& prevHash, const std::vector<Transaction>& txs, int MinerAdrs) 
        : index(idx), previousHash(prevHash), transactions(txs), nonce(0), ProofOfWorkCoinAddress(MinerAdrs) {
        timestamp = getCurrentTime();
        hash = mineBlock();
    }

    std::string calculateHash(bool save) const {
        std::stringstream ss;
        ss << index << previousHash << timestamp << std::endl;

        for (const auto& tx : transactions) {
            ss << tx.publicKey << " " << tx.howMany << " " << tx.receiver_publicKey << " " << tx.signature << std::endl;
        }

        ss << nonce;
        std::string does_hash = hashing(ss.str());
        if (save == true) saveBlockAtFile(ss.str());
        return does_hash;
    }

    void saveBlockAtFile(const std::string& BlockData) const {
        std::string blocks_folder = "Blocks";
        std::string file_name = "block" + std::to_string(index) + ".ast";
        std::string file_path = blocks_folder + "/" + file_name;
        std::filesystem::create_directory(blocks_folder);
        std::string code_snippet = BlockData;

        std::ofstream outFile(file_path);
        if (outFile.is_open()) {
            outFile << code_snippet;
            outFile.close();
        } else {
            std::cerr << "Не удалось открыть файл для записи." << std::endl;
        }
    }

    std::string getCurrentTime() const {
        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    static std::string hashing(const std::string& str) {
        SHA1 checksum;
        checksum.update(str);
        return checksum.final();
    }

    std::string mineBlock() {
        transactions.push_back(Transaction("0", 3, std::to_string(ProofOfWorkCoinAddress)));

        std::string target(difficulty, '0');
        do {
            hash = calculateHash(false);
            nonce++;
        } while (hash.substr(0, difficulty) != target);
        calculateHash(true); // для сохранения файла, да, не оптимизировано, но кому какое дело :)
        return hash;
    }

    void displayBlock() const {
        std::cout << "Блок #" << index << std::endl;
        std::cout << "Временная метка: " << timestamp << std::endl;
        std::cout << "Транзакции: " << std::endl;
        // for (const auto& tx : transactions) {
        //     tx.displayTransaction();
        // }
        std::cout << "Предыдущий хэш: " << previousHash << std::endl;
        std::cout << "Хэш: " << hash << std::endl;
        std::cout << "Нонс: " << nonce << std::endl;
        std::cout << "-----------------------------------" << std::endl;
    }
};

#endif // BLOCK_HPP

