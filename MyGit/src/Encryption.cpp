#include "../include/Encryption.hpp"


std::string Encryption::calculateSHA1(const std::string& content)
{
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(content.c_str()), content.size(), hash);

    return toHex(hash, SHA_DIGEST_LENGTH);
}


std::string Encryption::compressData(const std::string& data)
{
    return data;
}


std::string Encryption::decompressData(const std::string& compressed)
{
    return compressed;
}


std::string Encryption::toHex(const unsigned char* data, size_t length)
{
    std::ostringstream oss;
    for (size_t i = 0; i < length; ++i)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
    }
    return oss.str();
}
