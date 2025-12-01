#ifndef ENCRYPTION_HPP
#define ENCRYPTION_HPP

#include <string>


class Encryption
{
public:
    std::string calculateSHA1(const std::string& content);
    std::string compressData(const std::string& data);
    std::string decompressData(const std::string& compressed);

};

#endif