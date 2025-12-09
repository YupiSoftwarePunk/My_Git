#include "../include/GitCommands.hpp"


#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>


namespace fs = std::filesystem;

GitCommands::GitCommands(const std::string& repoPath)
    : repoPath_(repoPath) { }

void GitCommands::runCommand(const std::string& commandLine)
{
    std::istringstream iss(commandLine);
    std::string cmd;
    iss >> cmd;

    if (cmd == "config") 
    {
        std::string user;
        iss >> user;
        config(user);
    }
    else if (cmd == "init") 
    {
        init();
    }
    else if (cmd == "commit") 
    {
        std::string msg;
        std::getline(iss, msg);
        if (!msg.empty() && msg[0] == ' ') msg.erase(0, 1);
        {
            commit(msg);
        }
    }
    else if (cmd == "checkout") 
    {
        std::string hash;
        iss >> hash;
        checkout(hash);
    }
    else 
    {
        std::cout << "Неизвестная команда: " << cmd << std::endl;
    }
}








void GitCommands::config(const std::string& username)
{
    username_ = username;
    std::cout << "Пользователь установлен: " << username_ << std::endl;

    // сохраняем в config файл
    fs::create_directories(repoPath_);
    std::ofstream configFile(repoPath_ + "/config.txt");
    configFile << "user=" << username_ << std::endl;
}



void GitCommands::init()
{
    fs::create_directories(repoPath_ + "/objects");
    std::cout << "Инициализировано хранилище в " << repoPath_ << std::endl;
}



void GitCommands::commit(const std::string& message)
{
    if (username_.empty()) 
    {
        std::cout << "Сначала выполните config <username>" << std::endl;
        return;
    }

    // простейший коммит: сохраняем сообщение в файл
    std::string commitFile = repoPath_ + "/objects/commit_" + std::to_string(std::time(nullptr)) + ".txt";
    std::ofstream out(commitFile);
    out << "author=" << username_ << "\n";
    out << "message=" << message << "\n";
    out.close();

    std::cout << "Создан коммит: " << commitFile << std::endl;
}



void GitCommands::checkout(const std::string& hash)
{
    // пока просто выводим
    std::cout << "Переключение на коммит: " << hash << std::endl;
}
