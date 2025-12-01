#ifndef GITOBJECT_HPP
#define GITOBJECT_HPP

#include <string>

class GitObject
{
public:
    virtual ~GitObject() = default;
    virtual std::string serialize() const = 0;
    virtual void deserialize(const std::string& data) = 0;
    virtual std::string getType() const = 0;
    virtual std::string getHash() const;

private:
	
};

#endif