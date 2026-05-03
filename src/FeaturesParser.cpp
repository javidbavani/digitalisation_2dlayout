#include "../include/FeaturesParser.h"
#include <fstream>
#include <sstream>

nlohmann::json FeaturesParser::parse(const fs::path &featuresFile,
                                     const std::string &layerName)
{
    std::ifstream file(featuresFile);
    if (!file.is_open())
        throw std::runtime_error("Cannot open features: " + featuresFile.string());

    // Pass 1 — build symbol table
    parseSymbolTable(file);

    // Pass 2 — parse feature records
    file.clear();
    file.seekg(0);

    nlohmann::json layer;
    layer["name"] = layerName;
    layer["traces"] = nlohmann::json::array();
    layer["pads"] = nlohmann::json::array();
    layer["planes"] = nlohmann::json::array();

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#' || line[0] == '$')
            continue;

        std::istringstream iss(line);
        char type;
        iss >> type;

        if (type == 'P')
        {
            auto p = parsePad(iss);
            if (!p.is_null())
                layer["pads"].push_back(p);
        }
        else if (type == 'L')
        {
            auto t = parseLine(iss);
            if (!t.is_null())
                layer["traces"].push_back(t);
        }
        else if (type == 'A')
        {
            auto a = parseArc(iss);
            if (!a.is_null())
                layer["traces"].push_back(a);
        }
        else if (type == 'S')
        {
            auto s = parseSurface(file, iss);
            if (!s.is_null())
                layer["planes"].push_back(s);
        }
    }
    return layer;
}

void FeaturesParser::parseSymbolTable(std::istream &in)
{
    std::string line;
    while (std::getline(in, line))
    {
        if (line.empty() || line[0] != '$')
            continue;
        std::istringstream iss(line.substr(1));
        int index;
        std::string rest;
        iss >> index;
        std::getline(iss, rest);
        if (!rest.empty() && rest[0] == ' ')
            rest = rest.substr(1);
        m_symbols.add(index, rest);
    }
}

nlohmann::json FeaturesParser::parsePad(std::istringstream &iss)
{
    // P  x  y  sym_num  polarity  rotation  mirror  [net]
    double x, y, rotation = 0;
    int symNum, polarity, mirror = 0, net = -1;
    iss >> x >> y >> symNum >> polarity >> rotation >> mirror;
    if (iss >> net)
    {
    }

    const Symbol &sym = m_symbols.get(symNum);
    nlohmann::json pad;
    pad["x"] = x;
    pad["y"] = y;
    pad["rotation_deg"] = rotation;
    pad["net"] = net;

    switch (sym.type)
    {
    case SymbolType::Round:
        pad["shape"] = "round";
        pad["diameter_mm"] = sym.p1;
        break;
    case SymbolType::Rect:
        pad["shape"] = "rect";
        pad["w_mm"] = sym.p1;
        pad["h_mm"] = sym.p2;
        break;
    case SymbolType::Oval:
        pad["shape"] = "oval";
        pad["w_mm"] = sym.p1;
        pad["h_mm"] = sym.p2;
        break;
    default:
        pad["shape"] = "unknown";
    }
    return pad;
}

nlohmann::json FeaturesParser::parseLine(std::istringstream &iss)
{
    // L  x1 y1  x2 y2  sym_num  polarity  [net]
    double x1, y1, x2, y2;
    int symNum, polarity, net = -1;
    iss >> x1 >> y1 >> x2 >> y2 >> symNum >> polarity;
    if (iss >> net)
    {
    }

    const Symbol &sym = m_symbols.get(symNum);
    return {{"type", "line"},
            {"x1", x1},
            {"y1", y1},
            {"x2", x2},
            {"y2", y2},
            {"width_mm", sym.p1},
            {"net", net}};
}

nlohmann::json FeaturesParser::parseArc(std::istringstream &iss)
{
    // A  x1 y1  x2 y2  cx cy  sym_num  polarity  cw  [net]
    double x1, y1, x2, y2, cx, cy;
    int symNum, polarity, cw = 0, net = -1;
    iss >> x1 >> y1 >> x2 >> y2 >> cx >> cy >> symNum >> polarity >> cw;
    if (iss >> net)
    {
    }

    const Symbol &sym = m_symbols.get(symNum);
    return {{"type", "arc"},
            {"x1", x1},
            {"y1", y1},
            {"x2", x2},
            {"y2", y2},
            {"cx", cx},
            {"cy", cy},
            {"cw", cw == 1},
            {"width_mm", sym.p1},
            {"net", net}};
}

nlohmann::json FeaturesParser::parseSurface(std::istream &in,
                                            std::istringstream &firstLine)
{
    // First line: S  polarity  [net]
    int polarity, net = -1;
    firstLine >> polarity;
    if (firstLine >> net)
    {
    }

    nlohmann::json surface;
    surface["polarity"] = (polarity == 1) ? "positive" : "negative";
    surface["net"] = net;
    surface["boundary"] = nlohmann::json::array();

    std::string line;
    while (std::getline(in, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "OE")
            break; // end of this surface

        if (token == "OB" || token == "OS")
        {
            // Straight vertex
            double x, y;
            iss >> x >> y;
            surface["boundary"].push_back({{"type", "line"}, {"x", x}, {"y", y}});
        }
        else if (token == "OC")
        {
            // Arc vertex
            double x, y, cx, cy;
            int cw;
            iss >> x >> y >> cx >> cy >> cw;
            surface["boundary"].push_back({{"type", "arc"}, {"x", x}, {"y", y}, {"cx", cx}, {"cy", cy}, {"cw", cw == 1}});
        }
        // IS (island/hole) blocks follow same OB/OS/OC/OE pattern
        // handle them the same way if you need cutouts in planes
    }
    return surface;
}