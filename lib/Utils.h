#pragma once
#include "ZipEntry.h"
#include <string>
#include <vector>
#include <climits>

/// Return the file size in bytes.
/// @param path - the path to the file
int fileSize(const std::string &path);

/// Calculates n-bit binary cyclic redundancy check (CRC) code.
/// @param source - the source of data
/// @param size - the maximum size of bytes to read from source
/// @param n - the bit size of CRC
/// @param divisor - the divisor used to calculate CRC
int calculateCRC(std::istream &source, int size = INT_MAX, int n = 32, long long divisor = 0x04C11DB7);

/// Return the canonical path(without .. and .).
/// @param path - the path to be canonized
std::string getCanonicalPath(const std::string &path);

/// Checks whether text matches with some prefix of pattern.
/// @param text - the text
/// @param pattern - the pattern(may contain * and ?)
bool partialCheck(const char *text, const char *pattern);

/// Checks whether text matches with some prefix of pattern.
/// @param text - the text
/// @param pattern - the pattern(may contain * and ?)
bool partialCheck(const std::string &text, const std::string &pattern);

/// Checks whether text matches pattern.
/// @param text - the text
/// @param pattern - the pattern(may contain * and ?)
bool check(const char *text, const char *pattern);

/// Checks whether text matches pattern.
/// @param text - the text
/// @param pattern - the pattern(may contain * and ?)
bool check(const std::string &text, const std::string &pattern);

/// Checks whether text matches any of the patterns.
/// @param text - the text
/// @param pattern - the patterns(may contain * and ?)
bool check(const std::string &text, const std::vector<std::string> &patterns);

/// Returns ZipEntry-s of all files matching the given pattern.
/// @param pattern - the pattern used for matching
std::vector<ZipEntry> match(const std::string &pattern);

/// Executes commands given from command line. If symbols * and ? are used they should have \ in front.
void executeCommands(int argc, char **argv);