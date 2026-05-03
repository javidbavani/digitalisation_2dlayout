cmake_minimum_required(VERSION 3.16)
project(OdbParser)

set(CMAKE_CXX_STANDARD 17)

find_package(nlohmann_json REQUIRED)

add_executable(odb_parser
    src/main.cpp
    src/OdbParser.cpp
    src/MatrixParser.cpp
    src/ProfileParser.cpp
    src/SymbolTable.cpp
    src/FeaturesParser.cpp
)

target_include_directories(odb_parser PRIVATE include)
target_link_libraries(odb_parser PRIVATE nlohmann_json::nlohmann_json)