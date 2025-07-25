cmake_minimum_required(VERSION 3.20)
project(Bitcoin)

set(CMAKE_CXX_STANDARD 17)

# Find OpenSSL (for crypto functions)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(nlohmann_json REQUIRED)

# Create executable
add_executable(blockchain
    src/main.cpp
    src/crypto/hash.cpp
    src/crypto/keys.cpp
)

# Link OpenSSL
target_link_libraries(blockchain OpenSSL::SSL OpenSSL::Crypto)
target_include_directories(blockchain PRIVATE src)
