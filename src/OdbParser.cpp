#include "../include/OdbParser.h"
#include "../include/MatrixParser.h"
#include "../include/ProfileParser.h"
#include "../include/FeaturesParser.h"
#include <iostream>

OdbParser::OdbParser(const fs::path &odbRoot) : m_root(odbRoot)
{
    if (!fs::exists(odbRoot))
        throw std::runtime_error("ODB++ root not found: " + odbRoot.string());
}

fs::path OdbParser::stepPath() const
{
    fs::path steps = m_root / "steps";
    for (auto &entry : fs::directory_iterator(steps))
        if (entry.is_directory())
            return entry.path();
    throw std::runtime_error("No step directory found under steps/");
}

nlohmann::json OdbParser::parseStackup()
{
    return ::parseMatrix(m_root / "matrix" / "matrix");
}

nlohmann::json OdbParser::parseProfile()
{
    return ::parseProfile(stepPath() / "profile");
}

nlohmann::json OdbParser::parseLayers(const nlohmann::json &stackup)
{
    nlohmann::json layers;
    fs::path layersDir = stepPath() / "layers";

    for (auto &entry : fs::directory_iterator(layersDir))
    {
        if (!entry.is_directory())
            continue;

        fs::path featuresFile = entry.path() / "features";
        if (!fs::exists(featuresFile))
            continue;

        std::string layerName = entry.path().filename().string();
        std::cout << "  Parsing layer: " << layerName << "\n";

        FeaturesParser fp;
        layers[layerName] = fp.parse(featuresFile, layerName);
    }
    return layers;
}

nlohmann::json OdbParser::parse()
{
    std::cout << "Parsing stackup...\n";
    auto stackup = parseStackup();

    std::cout << "Parsing profile...\n";
    auto profile = parseProfile();

    std::cout << "Parsing layers...\n";
    auto layers = parseLayers(stackup);

    std::cout << "Extracting vias...\n";
    auto vias = extractVias(layers, stackup);

    return {
        {"stackup", stackup},
        {"profile", profile},
        {"layers", layers},
        {"vias", vias}};
}

// Call this after parseLayers() has run.
// Pass the full layers JSON and the stackup JSON.
nlohmann::json OdbParser::extractVias(const nlohmann::json &layers,
                                      const nlohmann::json &stackup)
{
    nlohmann::json vias = nlohmann::json::array();

    for (auto &[layerName, layerData] : layers.items())
    {

        // Drill layers are identified by name or by type in the stackup
        bool isDrill = layerName.find("drill") != std::string::npos;

        if (!isDrill)
        {
            // Also check the stackup type field as a fallback
            for (auto &entry : stackup)
            {
                if (entry["name"] == layerName &&
                    entry["type"] == "DRILL")
                {
                    isDrill = true;
                    break;
                }
            }
        }
        if (!isDrill)
            continue;

        for (auto &pad : layerData["pads"])
        {
            nlohmann::json via;
            via["x"] = pad["x"];
            via["y"] = pad["y"];
            via["drill_mm"] = pad.value("diameter_mm", 0.0);
            via["net"] = pad["net"];
            via["type"] = "through"; // refine to blind/buried later
            vias.push_back(via);
        }
    }
    return vias;
}