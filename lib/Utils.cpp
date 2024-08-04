#include "Utils.h"
#include "Archive.h"
#include <vector>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <cstring>

int fileSize(const std::string &path)
{
    std::ifstream in(path, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

int calculateCRC(std::istream &source, int size, int n, long long divisor)
{
    long long res = 0;
    char ch;
    int pos = -1;
    for (int i = 0; i < size && source; i++)
    {
        source.get(ch);
        unsigned char c = ch;
        if (!source)
        {
            res <<= (n);
            pos += (n);
        }
        else if (i == size - 1)
        {
            res <<= 8;
            res = res | c;
            pos += 8;
            res <<= (n);
            pos += (n);
        }
        else
        {
            res <<= 8;
            res = res | c;
            pos += 8;
        }
        while (pos >= (n))
        {
            if ((res & (1ll << (pos))) > 0)
            {
                res = res ^ (divisor << (pos - (n)));
            }
            pos--;
        }
    }
    return (res & ~(~0ll << (n)));
}

std::string getCanonicalPath(const std::string &path)
{
    std::vector<std::string> parts;
    std::string cur;
    for (char c : path)
    {
        if (c == std::filesystem::path::preferred_separator)
        {
            if (cur == "..")
            {
                parts.pop_back();
            }
            else if (cur != ".")
            {

                parts.push_back(cur);
            }
            cur.clear();
        }
        else
        {
            cur += c;
        }
    }
    if (cur == "..")
    {
        parts.pop_back();
    }
    else if (cur != ".")
    {
        parts.push_back(cur);
    }
    std::string canonical;
    for (int i = 0; i < parts.size(); i++)
    {
        canonical += parts[i];
        if (i < parts.size() - 1)
        {
            canonical += std::filesystem::path::preferred_separator;
        }
    }
    return canonical;
}

bool partialCheck(const char *text, const char *pattern)
{
    if (*text == '\0')
    {
        return true;
    }
    if (*text != '\0' && *pattern == '\0')
    {
        return false;
    }
    if (*pattern == '?')
    {
        return partialCheck(text + 1, pattern + 1);
    }
    if (*pattern == '*')
    {
        return partialCheck(text, pattern + 1) || partialCheck(text + 1, pattern) || partialCheck(text + 1, pattern + 1);
    }
    return *text == *pattern && partialCheck(text + 1, pattern + 1);
}

bool partialCheck(const std::string &text, const std::string &pattern)
{
    return partialCheck(text.c_str(), pattern.c_str());
}

bool check(const char *text, const char *pattern)
{
    if (*text == '\0')
    {
        return *pattern == '\0' || *pattern == '*' && check(text, pattern + 1);
    }
    if (*text != '\0' && *pattern == '\0')
    {
        return false;
    }
    if (*pattern == '?')
    {
        return check(text + 1, pattern + 1);
    }
    if (*pattern == '*')
    {
        return check(text, pattern + 1) || check(text + 1, pattern) || check(text + 1, pattern + 1);
    }
    return *text == *pattern && check(text + 1, pattern + 1);
}

bool check(const std::string &text, const std::string &pattern)
{
    return check(text.c_str(), pattern.c_str());
}

bool check(const std::string &text, const std::vector<std::string> &patterns)
{
    for (const std::string &pattern : patterns)
    {
        if (check(text, pattern))
        {
            return true;
        }
    }
    return false;
}

std::vector<ZipEntry> match(const std::string &pattern)
{
    std::string fullPattern = std::filesystem::current_path().string();
    fullPattern += std::filesystem::path::preferred_separator;
    fullPattern += pattern;
    fullPattern = getCanonicalPath(fullPattern);
    std::filesystem::path cur = std::filesystem::current_path();

    while (!partialCheck(cur.string(), fullPattern))
    {
        cur = cur.parent_path();
    }
    int size = cur.string().size();
    std::vector<ZipEntry> v;
    for (const std::filesystem::directory_entry &entry :
         std::filesystem::recursive_directory_iterator{cur})
    {
        std::filesystem::path path = entry.path();
        std::string str = path.string();
        if ((std::filesystem::is_directory(path) && std::filesystem::is_empty(path) || !std::filesystem::is_directory(path)) && check(str, fullPattern))
        {
            ZipEntry z;
            z.relativePath = str.substr(size);
            z.fullPath = path;
            z.isDirectory = std::filesystem::is_directory(path);
            if (!z.isDirectory)
            {
                z.size = fileSize(path);
            }
            else
            {
                z.size = 0;
            }
            v.push_back(z);
        }
    }
    return v;
}

void executeCommands(int argc, char **argv)
{
    Archive a;
    if (argc < 3)
    {
        std::cout << "Invalid operation" << std::endl;
        return;
    }
    if (strcmp(argv[1], "ZIP") == 0)
    {
        std::string str = getCanonicalPath(std::filesystem::current_path().string() + std::filesystem::path::preferred_separator + argv[2]);
        std::ofstream of(str);
        std::fstream where(str);

        std::vector<ZipEntry> zips;
        for (int i = 3; i < argc; i++)
        {
            std::vector<ZipEntry> cur = match(argv[i]);
            zips.insert(zips.end(), cur.begin(), cur.end());
        }
        std::vector<std::istream *> from;
        for (ZipEntry &z : zips)
        {
            std::ifstream *ifs = new std::ifstream(z.fullPath);
            from.push_back(ifs);
        }
        a.create(where, from, zips);
        for (int i = from.size() - 1; i >= 0; i--)
        {
            delete from[i];
        }
    }
    else if (strcmp(argv[1], "UNZIP") == 0)
    {
        std::ifstream from(argv[3]);
        std::vector<std::string> what;
        for (int i = 4; i < argc; i++)
        {
            what.push_back(argv[i]);
        }
        std::string where = argv[2];
        a.unzip(where, from, what);
    }
    else if (strcmp(argv[1], "REFRESH") == 0)
    {
        a.refresh(argv[2], argv[3]);
    }
    else if (strcmp(argv[1], "INFO") == 0)
    {
        std::string str = getCanonicalPath(std::filesystem::current_path().string() + std::filesystem::path::preferred_separator + argv[2]);
        std::ifstream ifs(str);
        a.info(ifs, std::cout);
    }
    else if (strcmp(argv[1], "EC") == 0)
    {
        std::string str = getCanonicalPath(std::filesystem::current_path().string() + std::filesystem::path::preferred_separator + argv[2]);
        std::ifstream ifs(str);
        a.checkDamaged(ifs, std::cout);
    }
    else
    {
        std::cout << "Invalid operation" << std::endl;
    }
}