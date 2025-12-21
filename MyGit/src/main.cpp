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

    GitCommands git; 

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