#ifndef CRYPTOKEYMANAGER_HPP
#define CRYPTOKEYMANAGER_HPP

#include <openssl/evp.h> 
#include <openssl/pem.h> 
#include <openssl/err.h> 


class CryptoKeyManager {

public:
	int checkKeys();
	int checkFolder();
	int generateFolder();

private:
	int generateKeys();
	bool generateCertificate(EVP_PKEY* pkey, const char* password, const char* filename);
};


#endif // CRYPTOKEYMANAGER_HPP