#ifndef GITCOMMANDS_HPP
#define GITCOMMANDS_HPP


#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>


class GitCommands
{
public:
    explicit GitCommands(const std::string& repoPath = ".mygit");

    void runCommand(const std::string& commandLine);

    // команды
    void config(const std::string& username);
    void init();
    void commit(const std::string& message);
    void checkout(const std::string& hash);

private:
    std::string repoPath_;
    std::string username_;
};

#endif