// g++ -o generate_keys ./src/genkeys.cpp -lssl -lcrypto

#include <iostream>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>


void handleErrors() {
    ERR_print_errors_fp(stderr);
    abort();
}


int main() {
    if(OPENSSL_init_crypto(0, nullptr) == 0) {
        handleErrors();
    }

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if(!ctx) {
        handleErrors();
    }

    if(EVP_PKEY_keygen_init(ctx) <= 0 and EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0) {
        handleErrors();
    }

    EVP_PKEY *pkey = nullptr;
    if(EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        handleErrors();
    }

    FILE *privateKeyFile = fopen("./keys/private_key.pem", "wb");
    if(!privateKeyFile) {
        handleErrors();
    }
    if(PEM_write_PrivateKey(privateKeyFile, pkey, nullptr, nullptr, 0, nullptr, nullptr) != 1) {
        handleErrors();
    }
    fclose(privateKeyFile);


    FILE *publicKeyFile = fopen("./keys/public_key.pem", "wb");
    if(!publicKeyFile) {
        handleErrors();
    }
    if(PEM_write_PUBKEY(publicKeyFile, pkey) != 1) {
        handleErrors();
    }
    fclose(publicKeyFile);


    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    ERR_free_strings();
    OPENSSL_cleanup();

    return 0;
}