#include "../include/Blob.hpp"

std::string Blob::serialize() const
{
    std::string header = "blob " + std::to_string(content_.size()) + '\0';
    return header + content_;
}


void Blob::deserialize(const std::string& data)
{
    auto pos = data.find('\0');
    if (pos != std::string::npos)
    {
        content_ = data.substr(pos + 1);
    }
    else
    {
        content_.clear(); 
        return;
    }
}