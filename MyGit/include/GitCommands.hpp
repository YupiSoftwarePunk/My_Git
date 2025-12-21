#ifndef GITCOMMANDS_HPP
#define GITCOMMANDS_HPP


#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>


class GitCommands
{
public:
    explicit GitCommands();

    void runCommand(const std::string& commandLine);

    // команды
    void config(const std::string& username);
    void init();
    void commit(const std::string& message);
    void checkout(const std::string& hash);
    void add(const std::string& filename);
    void status();
    void log();
    void help();
    void cd(const std::string& path);
    void addFile(const std::string& filename);

private:
    std::string repoPath_;
    std::string username_;
    std::string workDir_;
};

#endif