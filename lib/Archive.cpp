#include "Archive.h"
#include "Utils.h"
#include <fstream>
#include <iostream>
#include <filesystem>

void Archive::readZip(std::istream &is, ZipEntry &z) const
{
    std::getline(is, z.fullPath);
    std::getline(is, z.relativePath);
    is.read((char *)&z.isDirectory, 1);
    is.read((char *)&z.size, 4);
}

int Archive::writeZip(std::ostream &os, ZipEntry &z) const
{
    int size = 0;
    os.write(z.fullPath.c_str(), z.fullPath.size());
    os.put('\n');
    size += z.fullPath.size() + 1;

    os.write(z.relativePath.c_str(), z.relativePath.size());
    os.put('\n');
    size += z.relativePath.size() + 1;

    os.put(z.isDirectory);
    size += 1;

    os.write((char *)&z.size, sizeof(z.size));
    size += sizeof(z.size);
    return size;
}

void Archive::create(std::iostream &where, std::vector<std::istream *> &from, std::vector<ZipEntry> &zips) const
{
    LZWArchiver archiver;
    for (int i = 0; i < zips.size(); i++)
    {
        ZipEntry &z = zips[i];
        std::cout << "Adding :" << z.relativePath << "\n";
        std::istream &in = *from[i];
        int totalSize = writeZip(where, z);

        int compressedSize = 0;
        if (!z.isDirectory)
        {
            int start = where.tellp();
            compressedSize = archiver.zip(in, where);
            int end = where.tellp();
            where.seekg(start, std::ios::beg);
            int crc32 = calculateCRC(where, end - start);
            where.write((char *)&crc32, sizeof(crc32));
            totalSize += sizeof(crc32);
        }

        totalSize += compressedSize + sizeof(totalSize);
        where.write((char *)&totalSize, sizeof(totalSize));
    }
}

void Archive::unzip(std::string &where, std::istream &from, std::vector<std::string> &what) const
{
    from.seekg(0, std::ios::end);
    while (from.tellg() > 0)
    {
        int end = from.tellg();
        end -= 8;

        from.seekg(-4, std::ios::cur);
        char c[4];
        from.read(c, 4);
        int totalSize = *((int *)c);
        from.seekg(-totalSize, std::ios::cur);
        int pos = from.tellg();

        ZipEntry z;
        readZip(from, z);

        if (check(z.relativePath, what))
        {
            std::cout << "Unzipping :" << z.relativePath << "\n";
            std::filesystem::path cur = where;
            cur += z.relativePath;
            if (z.isDirectory)
            {
                std::filesystem::create_directories(cur);
            }
            else
            {
                std::filesystem::create_directories(cur.parent_path());
                std::ofstream out(cur);
                archiver.unzip(from, out, end - from.tellg());
            }
        }

        from.seekg(pos - from.tellg(), std::ios::cur);
    }
}
void Archive::checkDamaged(std::istream &archive, std::ostream &out) const
{
    archive.seekg(0, std::ios::end);
    while (archive.tellg() > 0)
    {
        archive.seekg(-8, std::ios::cur);
        int end = archive.tellg();

        char c[4];
        archive.read(c, 4);
        int expectedCRC32 = *((int *)c);
        archive.read(c, 4);
        int totalSize = *((int *)c);
        archive.seekg(-totalSize, std::ios::cur);
        int pos = archive.tellg();
        ZipEntry z;
        readZip(archive, z);

        int crc32 = calculateCRC(archive, end - archive.tellg());
        if (crc32 != expectedCRC32)
        {
            out << "Bad CRC in file: " << z.relativePath << "\n";
        }
        archive.seekg(pos - archive.tellg(), std::ios::cur);
    }
}

void Archive::refresh(const std::string &archivePath, const std::string &what) const
{
    char name[] = "1987317120381702938";
    std::ofstream f(name);
    std::fstream tmpFile(name);
    std::ifstream ifs(archivePath);
    ifs.seekg(0, std::ios::end);
    while (ifs.tellg() > 0)
    {
        int end = ifs.tellg();
        ifs.seekg(-4, std::ios::cur);

        char c[4];
        ifs.read(c, 4);
        int totalSize = *((int *)c);
        ifs.seekg(-totalSize, std::ios::cur);
        int pos = ifs.tellg();
        ZipEntry z;
        readZip(ifs, z);
        int size = writeZip(tmpFile, z);

        if (!z.isDirectory && check(z.relativePath, what))
        {
            std::cout << "Updating :" << z.relativePath << "\n";
            std::ifstream in(z.fullPath);
            int start = tmpFile.tellp();
            int compressedSize = archiver.zip(in, tmpFile);
            int end = tmpFile.tellp();
            tmpFile.seekg(start, std::ios::beg);
            int crc32 = calculateCRC(tmpFile, end - start);
            tmpFile.write((char *)&crc32, sizeof(crc32));
            size += sizeof(crc32);

            size += compressedSize + sizeof(totalSize);
            tmpFile.write((char *)&size, sizeof(size));
        }
        else
        {
            char c;
            while (ifs.tellg() < end)
            {
                ifs.get(c);
                tmpFile.put(c);
            }
        }

        ifs.seekg(pos - ifs.tellg(), std::ios::cur);
    }

    tmpFile.seekg(0, std::ios::beg);

    char c;
    std::ofstream out(archivePath);
    while (tmpFile)
    {
        tmpFile.get(c);
        if (!tmpFile)
        {
            break;
        }
        out.put(c);
    }
    std::remove(name);
}

void Archive::info(std::istream &from, std::ostream &out) const
{
    from.seekg(0, std::ios::end);
    while (from.tellg() > 0)
    {
        int end = from.tellg();
        end -= 8;
        from.seekg(-4, std::ios::cur);
        char c[4];
        from.read(c, 4);
        int totalSize = *((int *)c);
        from.seekg(-totalSize, std::ios::cur);
        int pos = from.tellg();
        ZipEntry z;
        readZip(from, z);

        int compressedSize = end - from.tellg();
        if (!z.isDirectory)
        {
            out << z.relativePath << "'s size is " << 100.0 * compressedSize / z.size << " % of original size\n";
        }
        from.seekg(pos - from.tellg(), std::ios::cur);
    }
}
