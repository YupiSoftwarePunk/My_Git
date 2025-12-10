#ifndef BLOB_HPP
#define BLOB_HPP

#include <string>
#include "GitObject.hpp"

class Blob : public GitObject
{
public:
    explicit Blob(const std::string& content = "") : content_(content) {}

    std::string serialize() const override;
    void deserialize(const std::string& data) override;
    std::string getType() const override { return "blob"; }
    std::string getContent() const { return content_; }

private:
	std::string content_;
};

#endif