#ifndef ENCRYPTION_HPP
#define ENCRYPTION_HPP

#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>


class Encryption
{
public:
    static std::string calculateSHA1(const std::string& content);
    static std::string compressData(const std::string& data);
    static std::string decompressData(const std::string& compressed);

private:
    static std::string toHex(const unsigned char* data, size_t length);
};

#endif