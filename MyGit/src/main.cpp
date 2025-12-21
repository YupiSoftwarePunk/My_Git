#include "../include/Blob.hpp"
#include "../include/GitObject.hpp"
#include "../include/Encryption.hpp"
#include "../include/GitCommands.hpp"

#include <iostream>
#include <Windows.h>


int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    //std::cout << "=== ТЕСТ: Encryption (SHA1) ===" << std::endl;
    //std::string text = "Hello Denis!";
    //std::string hash = Encryption::calculateSHA1(text);
    //std::cout << "SHA1(\"" << text << "\") = " << hash << std::endl;


    //std::cout << "\n=== ТЕСТ: Blob ===" << std::endl;
    //Blob blob("This is blob content");
    //std::cout << "Тип: " << blob.getType() << std::endl;
    //std::cout << "Содержимое: " << blob.getContent() << std::endl;

    //std::string serialized = blob.serialize();
    //std::cout << "Сериализовано: " << serialized << std::endl;

    //Blob blob2;
    //blob2.deserialize(serialized);
    //std::cout << "Десериализовано: " << blob2.getContent() << std::endl;

    //std::cout << "Хэш blob: " << blob.getHash() << std::endl;


    //std::cout << "\n=== ТЕСТ: GitCommands ===" << std::endl;
    //GitCommands git(".mygit_test");

    //git.runCommand("config Denis");
    //git.runCommand("init");
    //git.runCommand("commit First commit from test");
    //git.runCommand("checkout 1234567890abcdef");

    //std::cout << "\n=== ВСЕ ТЕСТЫ ЗАВЕРШЕНЫ ===" << std::endl;


    GitCommands git(".mygit"); 

    std::string input;
    std::cout << "Добро пожаловать в MyGit! \nВведите команды:\n";

    while (true)
    {
        std::cout << "mygit> ";
        if (!std::getline(std::cin, input)) break;

        if (input == "exit" || input == "quit") break;

        git.runCommand(input);
    }

    std::cout << "Выход из MyGit.\n";



	return 0;
}