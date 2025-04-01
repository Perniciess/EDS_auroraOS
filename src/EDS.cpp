#include "EDS.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <openssl/err.h>

void EDS::handleErrors() {
    ERR_print_errors_fp(stderr);
    abort();
}

bool EDS::calculateSHA256(const std::string &inputFilename, const std::string &outputFilename) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLength = 0;

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if(!mdctx)
        return false;

    if(EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(mdctx);
        return false;
    }

    std::ifstream file(inputFilename, std::ios::binary);
    if(!file) {
        EVP_MD_CTX_free(mdctx);
        return false;
    }

    char buffer[BUFFER_SIZE];
    while(file.read(buffer, sizeof(buffer))) {
        EVP_DigestUpdate(mdctx, buffer, file.gcount());
    }
    EVP_DigestUpdate(mdctx, buffer, file.gcount());

    if(EVP_DigestFinal_ex(mdctx, hash, &hashLength) != 1) {
        EVP_MD_CTX_free(mdctx);
        return false;
    }

    std::ofstream outputFile(outputFilename);
    if(!outputFile) {
        EVP_MD_CTX_free(mdctx);
        return false;
    }

    for(unsigned int i = 0; i < hashLength; ++i) {
        outputFile << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    outputFile << std::endl;

    EVP_MD_CTX_free(mdctx);
    return true;
}

bool EDS::signFile(const std::string &privateKeyFile, const std::string &hashFile, const std::string &outputFile) {
    BIO *bio = BIO_new_file(privateKeyFile.c_str(), "r");
    if(!bio) {
        std::cerr << "Error: Cannot open private key file" << std::endl;
        return false;
    }

    EVP_PKEY *privateKey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    if(!privateKey) {
        std::cerr << "Error: Failed to read private key" << std::endl;
        handleErrors();
        return false;
    }

    std::ifstream hashInput(hashFile, std::ios::binary);
    if(!hashInput.is_open()) {
        std::cerr << "Error: Cannot open hash file" << std::endl;
        EVP_PKEY_free(privateKey);
        return false;
    }

    std::vector<unsigned char> hash(
        (std::istreambuf_iterator<char>(hashInput)),
        std::istreambuf_iterator<char>());
    hashInput.close();

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if(!ctx) {
        std::cerr << "Error: Failed to create signing context" << std::endl;
        EVP_PKEY_free(privateKey);
        return false;
    }

    if(EVP_SignInit(ctx, EVP_sha256()) != 1 ||
       EVP_SignUpdate(ctx, hash.data(), hash.size()) != 1) {
        std::cerr << "Error: Signing operation failed" << std::endl;
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(privateKey);
        return false;
    }

    std::vector<unsigned char> signature(EVP_PKEY_size(privateKey));
    unsigned int signatureLength;

    if(EVP_SignFinal(ctx, signature.data(), &signatureLength, privateKey) != 1) {
        std::cerr << "Error: Failed to finalize signature" << std::endl;
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(privateKey);
        return false;
    }

    std::ofstream output(outputFile, std::ios::binary);
    if(!output.write(reinterpret_cast<char *>(signature.data()), signatureLength)) {
        std::cerr << "Error: Failed to write signature file" << std::endl;
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(privateKey);
        return false;
    }

    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(privateKey);
    std::cout << "Success: File signed - " << outputFile << std::endl;
    return true;
}