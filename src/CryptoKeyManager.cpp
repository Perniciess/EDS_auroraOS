#include <openssl/pkcs12.h>
#include <iostream>
#include <filesystem> 
#include <vector>

#include <sys/stat.h>



#include <CryptoKeyManager.hpp>


namespace fs = std::filesystem;


int CryptoKeyManager::checkFolder() {
	std::string folderPath = "keys";

	if (!fs::exists(folderPath)) {
        std::cerr << "Папка не существует!" << std::endl;
            return 1;
    }
    return 0;
}


int CryptoKeyManager::checkKeys() {
	std::string privateKeyPath = "keys/private_key.pem";

	if (!fs::exists(privateKeyPath)) {
        std::cerr << "Приватного ключа нет" << std::endl;
        std::cout << "Старт генерации ключей" << std::endl;
        generateKeys();
    }
	return 0;
}


int CryptoKeyManager::generateFolder() {
    if (mkdir("keys", 0777) == 0) {  // 0777 - права доступа
        std::cout << "Папка создана успешно!" << std::endl;
    } else {
        perror("Ошибка");
    }
    return 0;
}

int CryptoKeyManager::generateKeys() {
    if(OPENSSL_init_crypto(0, nullptr) == 0) {
        return 1;
    }

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if(!ctx) {
        return 1;
    }

    if(EVP_PKEY_keygen_init(ctx) <= 0 || EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0) {
        return 1;
    }

    EVP_PKEY *pkey = nullptr;
    if(EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        return 1;
    }

    FILE *privateKeyFile = fopen("keys/private_key.pem", "wb");
    if(!privateKeyFile) {
        EVP_PKEY_free(pkey); 
        return 1;
    }

    if(PEM_write_PrivateKey(privateKeyFile, pkey, nullptr, nullptr, 0, nullptr, nullptr) != 1) {
        fclose(privateKeyFile); 
        return 1;
    }
    fclose(privateKeyFile);


    FILE *publicKeyFile = fopen("keys/public_key.pem", "wb");
    if(!publicKeyFile) {
        return 1;
    }
    if(PEM_write_PUBKEY(publicKeyFile, pkey) != 1) {
        return 1;
    }
    fclose(publicKeyFile);


    std::cout << "Старт генерации сертификата" << std::endl;
    generateCertificate(pkey, "1234", "certificate.p12");


    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    ERR_free_strings();
    OPENSSL_cleanup();

    return 0;
}



bool CryptoKeyManager::generateCertificate(EVP_PKEY* pkey, const char* password, const char* filename) {
    X509* x509 = X509_new();
    if (!x509) return false;

    // Устанавливаем версию (v3)
    X509_set_version(x509, 2);

    // Серийный номер
    ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);

    // Срок действия (1 год)
    X509_gmtime_adj(X509_get_notBefore(x509), 0);
    X509_gmtime_adj(X509_get_notAfter(x509), 31536000L);

    // Публичный ключ, извлекает из приватного
    X509_set_pubkey(x509, pkey);

    // Информация о субъекте
    X509_NAME* name = X509_get_subject_name(x509);
    X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC, (const unsigned char*)"RU", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC, (const unsigned char*)"My Company", -1, -1, 0);

	// Common name
    //X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (const unsigned char*)"example.com", -1, -1, 0);

    // Издатель = субъект (самоподписанный)
    X509_set_issuer_name(x509, name);

    // Подпись
    if (!X509_sign(x509, pkey, EVP_sha256())) {
        X509_free(x509);
        return false;
    }


    PKCS12* p12 = PKCS12_create(
		password,       // Пароль для защиты
		"My Certificate", // Friendly name
		pkey,          // Приватный ключ
		x509,          // Сертификат
		nullptr,       // Цепочка CA (можно nullptr)
		0,             // Флаги (0 - по умолчанию)
		0,             // Итерации для шифрования ключа (0 - стандартные)
		0,             // Итерации для MAC (0 - стандартные)
		PKCS12_DEFAULT_ITER, // Алгоритм шифрования
		PKCS12_DEFAULT_ITER  // Алгоритм MAC
	);

    if (!p12) return false;

    FILE* p12File = fopen(filename, "wb");
    if (!p12File) {
        PKCS12_free(p12);
        return false;
    }

    bool success = i2d_PKCS12_fp(p12File, p12);
    fclose(p12File);
    PKCS12_free(p12);
    std::cout << "Сертификат сгенерирован" << std::endl;
    return success;
}