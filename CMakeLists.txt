cmake_minimum_required(VERSION 3.15)
project(MyGit)

set(CMAKE_CXX_STANDARD 17)

# Подключение vcpkg toolchain
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

# Поиск OpenSSL
find_package(OpenSSL REQUIRED)

# Заголовки
# include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories("C:/Users/Denis/vcpkg/vcpkg/installed/x64-windows/include")

# Исходники
add_executable(MyGit
    MyGit/src/main.cpp
    MyGit/src/Encryption.cpp
    MyGit/src/Blob.cpp
    MyGit/src/GitCommands.cpp
    MyGit/src/GitObject.cpp
)

# Привязываем OpenSSL к проекту
target_link_libraries(MyGit PRIVATE OpenSSL::SSL OpenSSL::Crypto)
link_directories("C:/Users/Denis/vcpkg/vcpkg/installed/x64-windows/lib")
target_include_directories(MyGit PRIVATE "C:/Users/Denis/vcpkg/vcpkg/installed/x64-windows/include")

# Добавляем include директории OpenSSL
# target_include_directories(MyGit PRIVATE ${OPENSSL_INCLUDE_DIR})