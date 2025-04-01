#include <fstream>
#include <iostream>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <sstream>


void handleErrors() {
    ERR_print_errors_fp(stderr);
    abort();
}


bool signFile(const std::string &privateKeyFile, const std::string &hashFile, const std::string &outputFile) {
    BIO *bio = BIO_new_file(privateKeyFile.c_str(), "r");
    if(!bio) {
        return false;
    }

    EVP_PKEY *privateKey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    if(!privateKey) {
        handleErrors();
        return false;
    }

    std::ifstream hashInput(hashFile);
    if(!hashInput.is_open()) {
        EVP_PKEY_free(privateKey);
        return false;
    }

    std::string hash((std::istreambuf_iterator<char>(hashInput)), std::istreambuf_iterator<char>());
    hashInput.close();


    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if(!ctx) {
        EVP_PKEY_free(privateKey);
        return false;
    }

    if(EVP_SignInit(ctx, EVP_sha256()) != 1) {
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(privateKey);
        return false;
    }


    if(EVP_SignUpdate(ctx, hash.c_str(), hash.size()) != 1) {
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(privateKey);
        return false;
    }


    unsigned char signature[256];
    unsigned int signatureLength;
    if(EVP_SignFinal(ctx, signature, &signatureLength, privateKey) != 1) {
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(privateKey);
        return false;
    }


    std::ofstream output(outputFile, std::ios::binary);
    if(!output.is_open()) {
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(privateKey);
        return false;
    }

    output.write(reinterpret_cast<char *>(signature), signatureLength);
    output.close();

    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(privateKey);
    return true;
}

int main() {
    std::string privateKeyFile = "./keys/private_key.pem";
    std::string hashFile = "./test/test.hash.txt";
    std::string outputFile = "./test/signature.sig";

    if(!signFile(privateKeyFile, hashFile, outputFile)) {
        return 1;
    }
    return 0;
}
