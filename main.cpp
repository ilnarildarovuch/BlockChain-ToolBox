#include <iostream>
#include <vector>
#include "block.hpp"

int main() {
    int n, public_key, private_key;
    generateKeys(n, public_key, private_key); // Генерируем ключи для подписи
    std::cout << "N для открытого и закрытого ключей: " << n << std::endl;
    std::cout << "Открытый ключ: " << public_key << std::endl;
    std::cout << "Закрытый ключ: " << private_key << std::endl;

    int userBalance = 0;
    std::vector<Transaction> transactions;
    std::vector<Transaction> transactions_all;
    if (userBalance >= 10) {
        transactions.emplace_back(to_string(public_key), 10, to_string(public_key));
        transactions.back().signTransaction(private_key, n); // Подписываем транзакцию
        transactions.emplace_back(to_string(public_key), 5, to_string(public_key));
        transactions.back().signTransaction(private_key, n); // Подписываем транзакцию
    }
    
    // Создаем блок генезис
    Block genesisBlock(0, "0", transactions, public_key);
    genesisBlock.displayBlock();
    transactions_all.insert(transactions_all.end(), genesisBlock.transactions.begin(), genesisBlock.transactions.end());
    userBalance = getBalance(to_string(public_key), transactions_all);

    int blockCount = 1; // счетчик созданных блоков

    while (true) {
        // Создаем новые транзакции для следующего блока
        transactions.clear();
        if (userBalance >= 13) {
            transactions.emplace_back(to_string(public_key), 12, to_string(public_key));
            transactions.back().signTransaction(private_key, n); // Подписываем транзакцию
            transactions.emplace_back(to_string(public_key), 13, to_string(public_key));
            transactions.back().signTransaction(private_key, n); // Подписываем транзакцию
        }
        // Создаем новый блок
        Block newBlock(blockCount, genesisBlock.hash, transactions, public_key);
        newBlock.displayBlock();
        transactions_all.insert(transactions_all.end(), genesisBlock.transactions.begin(), genesisBlock.transactions.end());

        // Проверяем транзакции в новом блоке
        for (const auto& tx : newBlock.transactions) {
            if (tx.publicKey == "0"){
                std::cout << "Монеты, добытые в блоке: " << tx.howMany << ", на адрес: " << tx.receiver_publicKey << std::endl;
            }
            else if (tx.verifyTransaction(n)) {
                std::cout << "Транзакция подтверждена: " << tx.publicKey << " -> " << tx.receiver_publicKey << " Количество монет: " << tx.howMany << std::endl;
            } 
            else {
                std::cout << "Подтверждение транзакции неудалось: " << tx.publicKey << " -> " << tx.receiver_publicKey << " Количество монет: " << tx.howMany << std::endl;
            }
        }
        userBalance = getBalance(to_string(public_key), transactions_all);
        std::cout << "Баланс пользователя: " << userBalance << std::endl;
        std::cout << "------------------" << std::endl;

        // Обновляем предыдущий блок до нового блока
        genesisBlock = newBlock;

        // Увеличиваем счетчик блоков
        blockCount++;
    }

    return 0;
}
