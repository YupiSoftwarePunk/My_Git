cmake_minimum_required(VERSION 3.15)
project(MyGit)

# Поиск OpenSSL
find_package(OpenSSL REQUIRED)

# Добавить директорию include для заголовочных файлов
include_directories(${CMAKE_SOURCE_DIR}/include)

# Для исполняемого файла
add_executable(MyGit 
    src/main.cpp
    src/Encryption.cpp
    src/Blob.cpp
    src/GitCommands.cpp
    src/GitObject.cpp
)

# Подключение OpenSSL к вашему приложению
target_link_libraries(MyGit PRIVATE OpenSSL::SSL OpenSSL::Crypto)