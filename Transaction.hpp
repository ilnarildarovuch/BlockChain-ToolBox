#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <string>
#include <iostream>
#include "SCS/SC.cpp"

class Transaction {
public:
    std::string publicKey; // публичный ключ отправителя
    int howMany;            // количество монет, отправленных в транзакции
    std::string receiver_publicKey; // кому предназначены монеты
    int signature;          // подпись транзакции

    
    Transaction(const std::string& pubKey, const int count, const std::string& recvKey)
        : publicKey(pubKey), howMany(count), receiver_publicKey(recvKey), signature(0) {}

    // Подписать транзакцию
    void signTransaction(int privateKey, int n) {
        signature = signMessage(privateKey, n, receiver_publicKey + to_string(howMany));
    }

    // Проверить транзакцию
    bool verifyTransaction(int n) const {
        return verifySignature(stoi(publicKey), n, receiver_publicKey + to_string(howMany), signature);
    }

    // Отобразить детали транзакции
    void displayTransaction() const {
        std::cout << "Публичный ключ: " << publicKey << std::endl;
        std::cout << "Сколько монет: " << howMany << std::endl;
        std::cout << "Ключ получателя: " << receiver_publicKey << std::endl;
        std::cout << "Подпись: " << signature << std::endl;
    }
};

int getBalance(const std::string& publicKey, const std::vector<Transaction>& transactions) {
    int balance = 0;

    for (const auto& tx : transactions) {
        if (tx.publicKey == publicKey) {
            balance -= tx.howMany; // вычесть сумму, если это расход
        }
        if (tx.receiver_publicKey == publicKey) {
            balance += tx.howMany; // добавить сумму, если это доход
        }
    }

    return balance;
}

#endif // TRANSACTION_HPP

