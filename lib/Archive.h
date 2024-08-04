#pragma once
#include "LZWArchiver.h"
#include "ZipEntry.h"
#include <string>
#include <vector>

/// Each file in archive has the following structure:
/// (Relative path)\n 
/// (Full path)\n 
/// (Flag if directory - 1 byte)(Original size - 4 bytes)(Compressed data)(CRC32 code of compressed data - 4 bytes)(Total size including metadata - 4 bytes)
class Archive
{
    LZWArchiver archiver;

    void readZip(std::istream &is, ZipEntry &z) const;
    int writeZip(std::ostream &os, ZipEntry &z) const;

public:
    /// Creates archive.
    /// @param where - the destination of compressed data
    /// @param from - the sources of uncompressed data
    /// @param zips - the ZipEntry-s with meta information about zipped data
    void create(std::iostream &where, std::vector<std::istream *> &from, std::vector<ZipEntry> &zips) const;

    /// Unzips data from archive.
    /// @param where - the place where uncompressed data will be stored
    /// @param from - the archive input stream
    /// @param what - the patterns. If one a file or folder in the archive satisfies a pattern it is unzipped.
    void unzip(std::string &where, std::istream &from, std::vector<std::string> &what) const;

    /// Checks whether archive has been changed.
    /// @param where - the archie input stream
    /// @param out - output stream where results are saved
    void checkDamaged(std::istream &archive, std::ostream &out) const;

    /// Changes file in the archive.
    /// @param archiePath - path to the archive
    /// @param what - the file path to the file to be modified.
    void refresh(const std::string &archivePath, const std::string &what) const;

    /// Show information about files in archive and level of compression.
    /// @param from - input stream of archive
    /// @param out - output stream where information is saved
    void info(std::istream &from, std::ostream &out) const;
};