#include "EDS.hpp"
#include <iostream>

int main() {
    EDS signer;

    // Задайте свои пути к файлам здесь
    const std::string inputFile = "./test/test_files/test.txt";
    const std::string privateKey = "./keys/private_key.pem";
    const std::string hashFile = "./test/test_files/test.hash.txt";
    const std::string signatureFile = "./test/test_files/sign.hash.sig";

    // Шаг 1: Создание хэша
    if(!signer.calculateSHA256(inputFile, hashFile)) {
        std::cerr << "Ошибка создания хэша!" << std::endl;
        return 1;
    }
    std::cout << "Хэш успешно создан: " << hashFile << std::endl;

    // Шаг 2: Создание подписи
    if(!signer.signFile(privateKey, hashFile, signatureFile)) {
        std::cerr << "Ошибка создания подписи!" << std::endl;
        return 1;
    }
    std::cout << "Подпись успешно создана: " << signatureFile << std::endl;

    std::cout << "Все операции выполнены успешно!" << std::endl;
    return 0;
}