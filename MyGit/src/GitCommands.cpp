#include "../include/GitCommands.hpp"


#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../include/Blob.hpp"


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
    else if (cmd == "add") 
    {
        std::string filename; 
        iss >> filename; 
        add(filename); 
    }
    else if (cmd == "status") 
    {
        status(); 
    }
    else if (cmd == "log") 
    {
        log(); 
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
    std::cout << "Переключение на коммит: " << hash << std::endl;
}




void GitCommands::add(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file) 
    {
        std::cout << "Файл не найден: " << filename << std::endl;
        return;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    Blob blob(content);
    std::string hash = blob.getHash();
    std::string path = repoPath_ + "/objects/" + hash;

    std::ofstream out(path, std::ios::binary);
    out << blob.serialize();
    out.close();

    std::cout << "Добавлен blob: " << filename << " → " << hash << std::endl;
}



void GitCommands::status()
{
    std::cout << "=== Статус репозитория ===" << std::endl;

    std::ifstream config(repoPath_ + "/config.txt");
    if (config) 
    {
        std::string line;
        std::getline(config, line);
        std::cout << "Пользователь: " << line << std::endl;
    }
    else 
    {
        std::cout << "Нет конфигурации пользователя.\n";
    }

    size_t count = 0;
    for (const auto& entry : fs::directory_iterator(repoPath_ + "/objects"))
        ++count;

    std::cout << "Объектов в хранилище: " << count << std::endl;

    std::ifstream head(repoPath_ + "/HEAD");
    if (head) 
    {
        std::string last;
        std::getline(head, last);
        std::cout << "Последний коммит: " << last << std::endl;
    }
    else 
    {
        std::cout << "Нет коммитов.\n";
    }

}



void GitCommands::log()
{
    std::cout << "=== История коммитов ===" << std::endl;

    for (const auto& entry : fs::directory_iterator(repoPath_ + "/objects"))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }
        std::string name = entry.path().filename().string();
        if (name.find("commit_") != 0)
        {
            continue;
        }

        std::ifstream in(entry.path());
        std::string author, message;
        std::getline(in, author);
        std::getline(in, message);

        std::cout << name << "\n  " << author << "\n  " << message << "\n\n";
    }

}
