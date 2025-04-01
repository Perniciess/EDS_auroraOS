#include <fstream>
#include <iomanip>
#include <iostream>
#include <openssl/evp.h>


void calculateSHA256(const std::string &inputFilename, const std::string &outputFilename) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLength;

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if(!mdctx) {
        return;
    }

    if(EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(mdctx);
        return;
    }

    std::ifstream file(inputFilename, std::ios::binary);
    if(!file) {
        EVP_MD_CTX_free(mdctx);
        return;
    }

    char buffer[8192];
    while(file.read(buffer, sizeof(buffer))) {
        EVP_DigestUpdate(mdctx, buffer, file.gcount());
    }
    EVP_DigestUpdate(mdctx, buffer, file.gcount());

    if(EVP_DigestFinal_ex(mdctx, hash, &hashLength) != 1) {
        EVP_MD_CTX_free(mdctx);
        return;
    }

    std::ofstream outputFile(outputFilename);
    if(outputFile) {
        for(unsigned int i = 0; i < hashLength; ++i) {
            outputFile << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
        }
        outputFile << std::endl;
    }

    EVP_MD_CTX_free(mdctx);
}


int main() {
    std::string inputFilename = "./test/test.pdf";
    std::string outputFilename = "./test/test.hash.txt";
    calculateSHA256(inputFilename, outputFilename);
    return 0;
}
