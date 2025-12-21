#include "../include/GitCommands.hpp"
#include "../include/Blob.hpp"
#include "../include/Encryption.hpp"


#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>


namespace fs = std::filesystem;

GitCommands::GitCommands()
{ 
    workDir_ = fs::current_path().string();
    repoPath_ = workDir_ + "/.mygit";
}

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
        if (filename == "." || filename == "--all")
        {
            add(".");
        }
        else
        {
            add(filename);
        }
    }
    else if (cmd == "status") 
    {
        status(); 
    }
    else if (cmd == "log") 
    {
        log(); 
    }
    else if (cmd == "cd")
    {
        std::string path;
        std::getline(iss, path);
        if (!path.empty() && path[0] == ' ') path.erase(0, 1);
        cd(path);
    }
    else if (cmd == "help")
    {
        help();
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

    fs::create_directories(repoPath_);
    std::ofstream configFile(repoPath_ + "/config.txt");
    configFile << "user=" << username_ << std::endl;
}



void GitCommands::init()
{
    fs::create_directories(repoPath_ + "/objects");

    std::ofstream head(repoPath_ + "/HEAD"); 
    head << "null";

    std::cout << "Инициализировано хранилище в " << repoPath_ << std::endl;
}



void GitCommands::commit(const std::string& message)
{
    if (username_.empty()) 
    {
        std::cout << "Сначала выполните config <username>" << std::endl;
        return;
    }

    std::string parent = "null";
    {
        std::ifstream head(repoPath_ + "/HEAD");
        if (head)
        {
            std::getline(head, parent);
        }
    }

    std::string data;
    data += "type=commit\n";
    data += "author=" + username_ + "\n";
    data += "message=" + message + "\n";
    data += "parent=" + parent + "\n";

    std::string hash = Encryption::calculateSHA1(data);

    std::ofstream out(repoPath_ + "/objects/" + hash);
    out << data;
    out.close();

    std::ofstream head(repoPath_ + "/HEAD");
    head << hash;

    std::cout << "Создан коммит: " << hash << std::endl;
}



void GitCommands::checkout(const std::string& hash)
{
    std::string path = repoPath_ + "/objects/" + hash;

    if (!fs::exists(path)) 
    {
        std::cout << "Коммит не найден: " << hash << std::endl;
        return;
    }

    std::ifstream in(path);
    std::string type, author, message, parent;

    std::getline(in, type);
    std::getline(in, author);
    std::getline(in, message);
    std::getline(in, parent);

    std::ofstream head(repoPath_ + "/HEAD");
    head << hash;

    std::cout << "Переключено на коммит: " << hash << std::endl;
    std::cout << "  " << author << std::endl;
    std::cout << "  " << message << std::endl;
    std::cout << "  " << parent << std::endl;
}




void GitCommands::add(const std::string& filename)
{
    if (filename == ".")
    {
        for (const auto& entry : fs::recursive_directory_iterator(workDir_))
        {
            if (entry.is_regular_file())
            {
                std::string absPath = entry.path().string();

                if (absPath.rfind(repoPath_, 0) == 0)
                {
                    continue;
                }

                addFile(absPath);
            }
        }
        return;
    }

    if (fs::is_directory(filename))
    {
        for (const auto& entry : fs::recursive_directory_iterator(filename))
        {
            if (entry.is_regular_file())
            {
                add(entry.path().string());
            }
        }
        return;
    }



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

    std::cout << "Добавлен blob: " << filename << " -> " << hash << std::endl;
}



void GitCommands::status()
{
    std::unordered_set<std::string> blobHashes;

    if (fs::exists(repoPath_ + "/objects"))
    {
        for (const auto& entry : fs::directory_iterator(repoPath_ + "/objects"))
        {
            if (entry.is_regular_file())
            {
                blobHashes.insert(entry.path().filename().string());
            }
        }
    }

    for (const auto& entry : fs::recursive_directory_iterator(workDir_))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        std::string filePath = entry.path().string();

        if (filePath.rfind(repoPath_, 0) == 0)
        {
            continue;
        }

        std::ifstream file(filePath, std::ios::binary);
        std::string content((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());

        Blob blob(content);
        std::string serialized = blob.serialize();
        std::string hash = Encryption::calculateSHA1(serialized);

        if (blobHashes.contains(hash))
        {
            std::cout << "\033[32m[ADDED]   " << filePath << "\033[0m\n";
        }
        else
        {
            std::cout << "\033[31m[UNTRACKED] " << filePath << "\033[0m\n";
        }
    }
}



void GitCommands::log()
{
    std::ifstream head(repoPath_ + "/HEAD");
    if (!head) 
    {
        std::cout << "Нет HEAD файла" << std::endl;
        return;
    }

    std::string current;
    std::getline(head, current);

    if (current == "null") 
    {
        std::cout << "Нет коммитов" << std::endl;
        return;
    }

    while (current != "null")
    {
        std::string path = repoPath_ + "/objects/" + current;
        if (!fs::exists(path)) 
        {
            break;
        }

        std::ifstream in(path);
        std::string type, author, message, parent;

        std::getline(in, type);
        std::getline(in, author);
        std::getline(in, message);
        std::getline(in, parent);

        std::cout << current << "\n  " << author << "\n  " << message << "\n\n";

        parent = parent.substr(parent.find('=') + 1);
        current = parent;
    }
}



void GitCommands::help()
{
    std::cout << "\t\tДоступные команды MyGit\n";
    std::cout << "cd <path>             — перейти в директорию\n";
    std::cout << "config <username>     — установить имя пользователя\n";
    std::cout << "init                  — инициализировать репозиторий\n";
    std::cout << "add <file>            — добавить файл в blob\n";
    std::cout << "add .                 — добавить все файлы\n";
    std::cout << "add --all             — добавить все файлы\n";
    std::cout << "commit <message>      — создать коммит\n";
    std::cout << "checkout <hash>       — переключиться на коммит\n";
    std::cout << "status                — статус репозитория\n";
    std::cout << "log                   — история коммитов\n";
    std::cout << "help                  — список команд\n";
    std::cout << "exit                  — выход из программы\n";
    std::cout << "Ctrl+C                - выход из программы\n";
}



void GitCommands::cd(const std::string& rawPath)
{
    std::string path = rawPath;

    if (!path.empty() && path.front() == '"')
    {
        path.erase(0, 1);
    }
    if (!path.empty() && path.back() == '"')
    {
        path.pop_back();
    }


    if (!fs::exists(path) || !fs::is_directory(path))
    {
        std::cout << "Директория не найдена: " << path << std::endl;
        return;
    }

    workDir_ = fs::absolute(path).string();
    fs::current_path(workDir_);
    repoPath_ = workDir_ + "/.mygit";

    std::cout << "Текущая директория: " << workDir_ << std::endl;

    if (fs::exists(repoPath_))
    {
        std::cout << "Репозиторий найден: " << repoPath_ << std::endl;
    }
    else
    {
        std::cout << "Репозиторий не найден. Используйте init для создания." << std::endl;
    }
}



void GitCommands::addFile(const std::string& filename)
{

}
