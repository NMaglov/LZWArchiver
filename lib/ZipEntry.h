#pragma once
#include <string>

struct ZipEntry
{
    std::string relativePath;
    std::string fullPath;
    bool isDirectory;
    int size;
};