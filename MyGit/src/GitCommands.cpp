#include "../include/GitCommands.hpp"
#include "../include/Blob.hpp"
#include "../include/Encryption.hpp"
#include "../include/Colours.hpp"

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
    std::cout << Color::green << "Пользователь установлен: " << Color::cyan << username_ << Color::reset << std::endl;

    fs::create_directories(repoPath_);
    std::ofstream configFile(repoPath_ + "/config.txt");
    configFile << "user=" << username_ << std::endl;
}



void GitCommands::init()
{
    fs::create_directories(repoPath_ + "/objects");

    std::ofstream head(repoPath_ + "/HEAD"); 
    head << "null";

    std::cout << Color::green <<"Инициализировано хранилище в " << Color::blue << repoPath_ << Color::reset << std::endl;
}



void GitCommands::commit(const std::string& message)
{
    if (username_.empty()) 
    {
        std::cout << Color::red << "Сначала выполните config <username>" << Color::reset << std::endl;
        return;
    }

    std::string treeHash = createTree();

    std::string parent = "null";
    {
        std::ifstream head(repoPath_ + "/HEAD");
        if (head)
        {
            std::getline(head, parent);
        }
    }

    std::stringstream data;
    data << "type=commit\n";
    data << "tree=" << treeHash << "\n";
    data << "parent=" << parent << "\n";
    data << "author=" << username_ << "\n";
    data << "message=" << message << "\n";
    

    std::string commitData = data.str();
    std::string commitHash = Encryption::calculateSHA1(commitData);

    std::ofstream out(repoPath_ + "/objects/" + commitHash);
    out << commitData;

    std::ofstream head(repoPath_ + "/HEAD");
    head << commitHash;

    std::cout << Color::green << "Создан коммит: "
        << Color::magenta << commitHash
        << Color::reset << "\n";

}



void GitCommands::checkout(const std::string& hash)
{
    std::string path = repoPath_ + "/objects/" + hash;

    if (!fs::exists(path)) 
    {
        std::cout << Color::red << "Коммит не найден: " << Color::magenta << hash << Color::reset << std::endl;
        return;
    }

    std::ifstream in(path);
    std::string type, tree, parent, author, message;

    std::getline(in, type);
    std::getline(in, tree);
    std::getline(in, parent);
    std::getline(in, author);
    std::getline(in, message);

    std::string treeHash = tree.substr(tree.find('=') + 1);
    restoreTree(treeHash);

    std::ofstream head(repoPath_ + "/HEAD");
    head << hash;

    std::cout << Color::yellow << "Переключено на коммит: "
        << Color::magenta << hash
        << Color::reset << "\n";

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
                addFile(entry.path().string());
            }
        }
        return;
    }

    addFile(filename);
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

        std::string name = entry.path().filename().string();

        if (blobHashes.contains(hash))
        {
            std::cout << Color::green << "[ADDED] " << Color::green << name << Color::reset << "\n";
        }
        else
        {
            std::cout << Color::red << "[UNTRACKED] " << Color::red << name << Color::reset << "\n";
        }
    }
}



void GitCommands::log()
{
    std::ifstream head(repoPath_ + "/HEAD");
    if (!head) 
    {
        std::cout << Color::red << "Нет HEAD файла" << Color::reset << "\n";
        return;
    }

    std::string current;
    std::getline(head, current);

    if (current == "null") 
    {
        std::cout << Color::yellow << "Нет коммитов" << Color::reset << "\n";
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

        std::cout << Color::magenta << current << Color::reset << "\n";
        std::cout << Color::cyan << " " << author << Color::reset << "\n";
        std::cout << Color::white << " " << message << Color::reset << "\n\n";

        parent = parent.substr(parent.find('=') + 1);
        current = parent;
    }
}



void GitCommands::help()
{
    std::cout << Color::bold << "\t\tДоступные команды MyGit\n" << Color::reset;
    std::cout << Color::cyan << "cd <path>             "<< Color::reset << "— перейти в директорию\n";
    std::cout << Color::cyan << "config <username>     " << Color::reset << "— установить имя пользователя\n";
    std::cout << Color::cyan << "init                  " << Color::reset << "— инициализировать репозиторий\n";
    std::cout << Color::cyan << "add <file>            " << Color::reset << "— добавить файл в blob\n";
    std::cout << Color::cyan << "add .                 " << Color::reset << "— добавить все файлы\n";
    std::cout << Color::cyan << "add --all             " << Color::reset << "— добавить все файлы\n";
    std::cout << Color::cyan << "commit <message>      " << Color::reset << "— создать коммит\n";
    std::cout << Color::cyan << "checkout <hash>       " << Color::reset << "— переключиться на коммит\n";
    std::cout << Color::cyan << "status                " << Color::reset << "— статус репозитория\n";
    std::cout << Color::cyan << "log                   " << Color::reset << "— история коммитов\n";
    std::cout << Color::cyan << "help                  " << Color::reset << "— список команд\n";
    std::cout << Color::cyan << "exit                  " << Color::reset << "— выход из программы\n";
    std::cout << Color::cyan << "Ctrl+C                " << Color::reset << "- выход из программы\n";
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
        std::cout << Color::red <<"Директория не найдена: " << Color::yellow << path << Color::reset <<std::endl;
        return;
    }

    workDir_ = fs::absolute(path).string();
    fs::current_path(workDir_);
    repoPath_ = workDir_ + "/.mygit";

    std::cout << Color::green <<"Текущая директория: " << Color::blue << workDir_ << Color::reset << std::endl;

    if (fs::exists(repoPath_))
    {
        std::cout << Color::cyan <<"Репозиторий найден: " << Color::blue << repoPath_ << Color::reset << std::endl;
    }
    else
    {
        std::cout << Color::yellow << "Репозиторий не создан. "
            << Color::white << "Используйте " << Color::cyan << "init" << Color::white
            << " для создания." << Color::reset << std::endl;
    }
}



void GitCommands::addFile(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file)
    {
        std::cout << Color::red <<"Файл не найден: " << filePath << Color::reset << std::endl;
        return;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    Blob blob(content);
    std::string serialized = blob.serialize();
    std::string hash = Encryption::calculateSHA1(serialized);

    std::string outPath = repoPath_ + "/objects/" + hash;

    if (fs::exists(outPath))
    {
        return;
    }

    std::ofstream out(outPath, std::ios::binary);
    out << serialized;
    out.close();

    std::cout << Color::green <<"Добавлен blob: " << filePath << " -> " << hash << Color::reset << std::endl;
}



std::string GitCommands::createTree()
{
    std::stringstream ss;
    ss << "type=tree\n";

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

        std::string blobPath = repoPath_ + "/objects/" + hash;
        if (!fs::exists(blobPath))
        {
            std::ofstream out(blobPath, std::ios::binary);
            out << serialized;
        }

        ss << entry.path().filename().string() << " " << hash << "\n";
    }

    std::string treeData = ss.str();
    std::string treeHash = Encryption::calculateSHA1(treeData);

    std::ofstream out(repoPath_ + "/objects/" + treeHash);
    out << treeData;

    return treeHash;
}



void GitCommands::restoreTree(const std::string& treeHash)
{
    std::ifstream in(repoPath_ + "/objects/" + treeHash);
    if (!in)
        return;

    std::string line;
    std::getline(in, line); 

    while (std::getline(in, line))
    {
        std::istringstream iss(line);
        std::string filename, hash;
        iss >> filename >> hash;

        std::ifstream blobFile(repoPath_ + "/objects/" + hash, std::ios::binary);
        std::string blobData((std::istreambuf_iterator<char>(blobFile)),
            std::istreambuf_iterator<char>());

        Blob blob;
        blob.deserialize(blobData);

        std::ofstream out(workDir_ + "/" + filename, std::ios::binary);
        out << blob.getContent();
    }
}