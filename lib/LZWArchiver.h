#pragma once
#include <fstream>
#include <climits>
#include <unordered_map>
struct hash_pair
{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &p) const
    {
        std::size_t hash1 = std::hash<T1>{}(p.first);
        std::size_t hash2 = std::hash<T2>{}(p.second);
        return hash1 ^ hash2;
    }

    std::size_t operator()(const std::pair<int, int> &p) const
    {
        return (p.first << 16) ^ p.second;
    }
};
class LZWArchiver
{
    static const std::size_t MAX_DICTIONARY_SIZE = (1 << 14) - 1;
    static const std::size_t INCREASE_BITS = (1 << 8);
    static const std::size_t RESTART = (1 << 8) + 1;
    static const std::size_t INITIAL_CODE = (1 << 8) + 2;
    int getBits(char x, std::size_t from, std::size_t to) const;
    int getBits(int x, std::size_t from, std::size_t to) const;
    int flushBuffer(int &buffer, int &endPos, int &pos, std::ostream &out) const;
    void resetTable(std::unordered_map<std::pair<int, int>, int, hash_pair> &table) const;

public:
    /// Compresses data from source and stores the result in target and returns size of compressed data
    /// @param source - the source of data
    /// @param target - the destination of compressed data
    std::size_t zip(std::istream &source, std::ostream &target) const;

    /// Reads compressed data from source and uncompresses it in target
    /// @param source - the source of compressed data
    /// @param target - the destination of uncompressed data
    /// @param size - maximum number of bytes that can be uncompressed
    void unzip(std::istream &source, std::ostream &target, std::size_t size = INT_MAX) const;
};