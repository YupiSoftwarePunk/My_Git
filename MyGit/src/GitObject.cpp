#include "../include/GitObject.hpp"
#include "../include/Encryption.hpp"


std::string GitObject::getHash() const
{
    return Encryption::calculateSHA1(serialize());
}