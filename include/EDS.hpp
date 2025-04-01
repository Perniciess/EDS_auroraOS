#ifndef EDS_HPP
#define EDS_HPP


#include <openssl/evp.h>
#include <openssl/pem.h>
#include <string>
#include <vector>


class EDS {
public:
    bool calculateSHA256(const std::string &inputFilename, const std::string &outputFilename);
    bool signFile(const std::string &privateKeyFile, const std::string &hashFile, const std::string &outputFile);

private:
    void handleErrors();
    static const size_t BUFFER_SIZE = 8192;
};

#endif