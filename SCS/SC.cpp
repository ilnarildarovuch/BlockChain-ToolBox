#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>

using namespace std;

// Упрощенный алгоритм RSA
// В данном блокчейне используется для подписи транзакций
// И их проверки сответствующим образом

// Функция для вычисления НОД
int gcd(int a, int b) {
    return b == 0 ? a : gcd(b, a % b);
}

bool isPrime(int num) {
    if (num <= 1) return false;
    for (int i = 2; i <= sqrt(num); i++) {
        if (num % i == 0) return false;
    }
    return true;
}

int generateRandomPrime(int lower, int upper) {
    int prime;
    do {
        prime = rand() % (upper - lower + 1) + lower;
    } while (!isPrime(prime));
    return prime;
}

// Функция для нахождения обратного по модулю
int modInverse(int a, int m) {
    a = a % m;
    for (int x = 1; x < m; x++) {
        if ((a * x) % m == 1) {
            return x;
        }
    }
    return -1; // Если обратного нет
}

// Функция для возведения в степень по модулю
int modExp(int base, int exp, int mod) {
    int result = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    return result;
}

// Генерация простых чисел (для упрощения используем фиксированные простые числа)
void generateKeys(int& n, int& e, int& d) {
    int p = generateRandomPrime(50, 100); // Генерация случайного простого числа
    int q = generateRandomPrime(50, 100); // Генерация случайного простого числа
    while (p == q) { // p и q разные
        q = generateRandomPrime(50, 100);
    }
    n = p * q; // n = p * q
    int phi = (p - 1) * (q - 1); // phi(n)

    // Выбор e
    e = 17; // Обычно выбирается 65537, но для простоты используем 17
    while (gcd(e, phi) != 1) {
        e++;
    }

    // Вычисление d
    d = modInverse(e, phi);
}

// Подпись сообщения
int signMessage(int d, int n, const string& message) {
    int m = 0;
    for (char c : message) {
        m = (m * 256 + c) % n; // Преобразуем символы в число и берем по модулю n
    }
    return modExp(m, d, n);
}

// Проверка подписи
bool verifySignature(int e, int n, const string& message, int signature) {
    int m = 0;
    for (char c : message) {
        m = (m * 256 + c) % n; // Преобразуем символы в число и берем по модулю n
    }
    int mPrime = modExp(signature, e, n); // Восстановленное сообщение
    return m == mPrime;
}
