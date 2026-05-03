#include <iostream>
#include <fstream>
#include <filesystem>
#include "../include/OdbParser.h"

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{

    if (argc < 3)
    {
        std::cerr << "Usage: odb_parser <odb_directory> <output.json>\n";
        return 1;
    }

    fs::path odbPath = argv[1];
    fs::path outPath = argv[2];

    if (!fs::exists(odbPath))
    {
        std::cerr << "Error: path not found: " << odbPath << "\n";
        return 1;
    }
    if (!fs::is_directory(odbPath))
    {
        std::cerr << "Error: must be a directory: " << odbPath << "\n";
        return 1;
    }

    try
    {
        std::cout << "Parsing " << odbPath << " ...\n";

        OdbParser parser(odbPath);
        nlohmann::json result = parser.parse();

        std::ofstream out(outPath);
        if (!out.is_open())
        {
            std::cerr << "Error: cannot write to " << outPath << "\n";
            return 1;
        }

        out << result.dump(2);
        std::cout << "Done → " << outPath << "\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }

    return 0;
}