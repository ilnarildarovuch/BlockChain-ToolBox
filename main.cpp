#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <unordered_map>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include "block.hpp"

#define PORT 5238

std::vector<Transaction> transactions;
std::vector<Transaction> transactions_all;
int _public_key, _private_key, _n;

std::vector<std::string> splitString(const std::string& input, const std::string& delimiter) {
    std::string s = input; // Создаём копию входной строки для модификации
    size_t pos = 0;
    std::vector<std::string> tokens;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        std::string token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length()); // Удаляем найденную часть строки
    }
    tokens.push_back(s); // Добавляем оставшуюся часть строки
    return tokens;
}

std::string handle_request(const std::string& request, Block& lastBlock) {
    if (request.find("GET /create_genesis") != std::string::npos) {
        lastBlock = Block(0, "0", transactions, _public_key);
        transactions_all.insert(transactions_all.end(), lastBlock.transactions.begin(), lastBlock.transactions.end());
        transactions.clear();
        return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n Блок-Генезис вымайнен! Количество транзакций: " + std::to_string(lastBlock.transactions.size()) + "; Хеш блока: " + lastBlock.hash + "\n ";
    } else if (request.find("GET /generate_pair") != std::string::npos) {
        generateKeys(_n, _public_key, _private_key);
        return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n" + std::to_string(_n) + " _ " + std::to_string(_public_key) + " _ " + std::to_string(_private_key);
    } else if (request.find("GET /create_block") != std::string::npos) {
        Block newBlock(lastBlock.index + 1, lastBlock.hash, transactions, _public_key);
        lastBlock = newBlock;
        transactions_all.insert(transactions_all.end(), lastBlock.transactions.begin(), lastBlock.transactions.end());
        transactions.clear();
        return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n Блок #" + std::to_string(newBlock.index) + " вымайнен! Количество транзакций: " + std::to_string(newBlock.transactions.size()) + "; Хеш блока: " + lastBlock.hash + "\n ";
    } else if (request.find("GET /sign_trans") != std::string::npos) {
        size_t d_pos = request.find("d=");
        size_t n_pos = request.find("n=");
        size_t receiver_pos = request.find("receiver=");
        size_t e_pos = request.find("e=");
        size_t amount_pos = request.find("amount=");

        if (d_pos != std::string::npos && n_pos != std::string::npos && receiver_pos != std::string::npos && amount_pos != std::string::npos) {
            std::string d_str = splitString(splitString(request.substr(d_pos, n_pos - d_pos), "=")[1], "&")[0];
            std::string n_str = splitString(splitString(request.substr(n_pos, receiver_pos - n_pos), "=")[1], "&")[0];
            std::string receiver_str = splitString(splitString(request.substr(receiver_pos, e_pos - receiver_pos), "=")[1], "&")[0];
            std::string e_str = splitString(splitString(request.substr(e_pos, amount_pos - e_pos), "=")[1], "&")[0];
            std::string amount_str = splitString(splitString(request.substr(amount_pos), "=")[1], " ")[0];

            std::cout << "d: " << d_str << ", n: " << n_str << ", receiver: " << receiver_str << ", amount: " << amount_str << std::endl;

            int d = stoi(d_str);
            int n = stoi(n_str);
            int receiver = stoi(receiver_str);
            int e = stoi(e_str);
            int amount = stoi(amount_str);

            transactions.emplace_back(std::to_string(e), amount, std::to_string(receiver));
            transactions.back().signTransaction(d, n);
            std::string sign = std::to_string(transactions.back().signature);
            std::cout << "Sign: " << sign << std::endl;
            return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n " + sign + " ";
        }
        return "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nBad Request";
    
    } else if (request.find("GET /check_balance") != std::string::npos) {
        size_t e_posb = request.find("e=");
        if (e_posb != std::string::npos) {
            std::string e_str_balance = splitString(splitString(request.substr(e_posb), "=")[1], "&")[0];
            int e_balance = stoi(e_str_balance);
            int balance = 0;

            for (const auto& transaction : transactions_all) {
                if (transaction.receiver_publicKey == std::to_string(e_balance)) {
                    balance += transaction.howMany;
                }
            }

            std::cout << "Balance: " << balance << std::endl;
            return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n " + std::to_string(balance) + " ";
        }
        return "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nBad Request";

    } else {
        return "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nNot Found";
    }
}

void handle_client(int client_socket, Block& lastBlock) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    
    // Читаем запрос
    std::cout << "Request: " << read(client_socket, buffer, sizeof(buffer) - 1) << std::endl;
    std::string request(buffer);
    
    // Обрабатываем запрос и формируем ответ
    std::string response = handle_request(request, lastBlock);
    
    // Отправляем ответ клиенту
    send(client_socket, response.c_str(), response.size(), 0);
    
    // Закрываем соединение
    close(client_socket);
}

int main() {
    srand(time(0)); // Инициализация генератора случайных чисел
    generateKeys(_n, _public_key, _private_key); // Генерация ключей для подписи
    std::cout << "N for public and private keys: " << _n << std::endl;
    std::cout << "Public key: " << _public_key << std::endl;
    std::cout << "Private key: " << _private_key << std::endl;

    Block lastBlock(0, "0", {}, _public_key); // Инициализация последнего блока

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Could not create socket" << std::endl;
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        close(server_socket);
        return 1;
    }

    listen(server_socket, 5);
    std::cout << "Server is listening on port " << PORT << std::endl;

    while (true) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        std::thread(handle_client, client_socket, std::ref(lastBlock)).detach();  // Обработка клиента в новом потоке
    }

    close(server_socket);
    return 0;
}

