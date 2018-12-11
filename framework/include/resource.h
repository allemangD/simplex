#ifndef GL_TEMPLATE_RESOURCE_H
#define GL_TEMPLATE_RESOURCE_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

std::string readFile(const std::string &path) {
    std::ifstream file(path);
    if (!file) return std::string();

    file.ignore(std::numeric_limits<std::streamsize>::max());
    auto size = file.gcount();

    if (size > 0x10000) return std::string();

    file.clear();
    file.seekg(0, std::ios_base::beg);

    std::stringstream sstr;
    sstr << file.rdbuf();
    file.close();

    return sstr.str();
}

#endif //GL_TEMPLATE_RESOURCE_H
