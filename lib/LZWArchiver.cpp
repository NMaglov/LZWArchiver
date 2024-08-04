#include "LZWArchiver.h"
#include <unordered_map>
#include <iostream>
#include <stack>
#include <vector>

int LZWArchiver::getBits(char x, std::size_t from, std::size_t to) const
{
    return (x >> from) & ~(~0 << (to - from));
}
int LZWArchiver::getBits(int x, std::size_t from, std::size_t to) const
{
    return (x >> from) & ~(~0 << (to - from));
}

int LZWArchiver::flushBuffer(int &buffer, int &endPos, int &pos, std::ostream &out) const
{
    int size = 0;
    while (pos < endPos - 8)
    {
        out.put(getBits(buffer, endPos - 8, endPos));
        buffer <<= 8;
        pos += 8;
        size++;
    }
    return size;
}

void LZWArchiver::resetTable(std::unordered_map<std::pair<int, int>, int, hash_pair> &table) const
{
    table.clear();
    try
    {
        table.reserve(MAX_DICTIONARY_SIZE);
    }
    catch (...)
    {
        std::cerr << "error allocating memory\n";
        throw;
    }

    for (std::size_t i = 0; i < INITIAL_CODE; i++)
    {
        table[{i, -1}] = i;
    }
}

std::size_t LZWArchiver::zip(std::istream &source, std::ostream &target) const
{
    std::unordered_map<std::pair<int, int>, int, hash_pair> table;
    resetTable(table);

    std::size_t code = INITIAL_CODE;
    std::size_t nextBitCntIncrease = 1 << 9;
    std::size_t bitCount = 9;
    std::size_t size = 0;
    unsigned char c, prev;
    int sequence = -1;
    int outputCode;
    char ch;
    source.get(ch);
    if (!source)
    {
        return 0;
    }
    int buffer = 0;
    int endPos = 30;
    outputCode = ch;
    int pos = endPos - 1;
    while (source)
    {
        prev = ch;
        source.get(ch);
        c = (unsigned char)ch;

        if (!source)
        {
            if (sequence == -1)
            {
                outputCode = c;
            }
            break;
        }
        std::pair<int, int> key{sequence, c};
        if (sequence == -1)
        {
            outputCode = prev;
            key = {prev, c};
        }
        if (table.find(key) != table.end())
        {
            outputCode = table[key];
            sequence = table[key];
        }
        else
        {
            if (code < MAX_DICTIONARY_SIZE)
            {
                table[key] = code;
                code++;
            }
            sequence = -1;
            buffer = buffer | ((getBits(outputCode, 0, bitCount)) << (pos - bitCount + 1));
            pos -= bitCount;
            size += flushBuffer(buffer, endPos, pos, target);
            if (code > MAX_DICTIONARY_SIZE)
            {
                buffer = buffer | (getBits((int)RESTART, 0, bitCount) << (pos - bitCount + 1));
                pos -= bitCount;
                bitCount = 9;
                code = INITIAL_CODE;
                nextBitCntIncrease = (1 << 9);
                resetTable(table);
            }
            if (code == nextBitCntIncrease)
            {
                buffer = buffer | (getBits((int)INCREASE_BITS, 0, bitCount) << (pos - bitCount + 1));
                pos -= bitCount;
                bitCount++;
                nextBitCntIncrease *= 2;
            }
            size += flushBuffer(buffer, endPos, pos, target);
        }
    }

    buffer = buffer | ((getBits(outputCode, 0, bitCount)) << (pos - bitCount + 1));
    pos -= bitCount;
    while (pos < endPos)
    {
        target.put(getBits(buffer, endPos - 8, endPos));
        buffer <<= 8;
        pos += 8;
        size++;
    }
    return size;
}

void LZWArchiver::unzip(std::istream &source, std::ostream &target, std::size_t size) const
{
    // std::pair<int, int> table[MAX_DICTIONARY_SIZE];
    std::vector<std::pair<int, int>> table;
    try
    {
        table.reserve(MAX_DICTIONARY_SIZE);
    }
    catch (...)
    {
        std::cerr << "error allocating memory\n";
        throw;
    }
    for (std::size_t i = 0; i < (1 << 8); i++)
    {
        table[i] = {i, -1};
    }
    std::size_t code = INITIAL_CODE;

    char ch;
    char prev;
    source.get(prev);
    if (!source)
    {
        return;
    }
    source.get(ch);
    if (!source)
    {
        target.put(prev);
        return;
    }
    int buffer = 0;
    const int endPos = 30;
    buffer = (1 * getBits(prev, 0, 8) << (endPos - 8)) | (1 * getBits(ch, 0, 8) << (endPos - 16));
    int pos = endPos - 17;
    std::size_t bitCount = 9;
    int cur;
    cur = ((1 * getBits(buffer, endPos - bitCount, endPos)) & ~(~0 << bitCount));
    pos += bitCount;
    buffer <<= bitCount;
    target << (char)cur;
    int pr;
    for (std::size_t i = 2; i < size; i++)
    {
        source.get(ch);
        if (!source)
        {
            return;
        }

        buffer = buffer | (1 * getBits(ch, 0, 8) << (pos - 7));
        pos -= 8;
        while (pos + bitCount < endPos)
        {
            int x = (((1 * getBits(buffer, endPos - bitCount, endPos))));
            buffer <<= bitCount;
            pos += bitCount;
            if (x == INCREASE_BITS)
            {
                bitCount++;
                continue;
            }
            if (x == RESTART)
            {
                code = INITIAL_CODE;
                bitCount = 9;
                continue;
            }
            bool flag = true;
            pr = cur;
            cur = x;
            if (code <= MAX_DICTIONARY_SIZE && code == cur)
            {
                flag = false;
                int tmp = pr;
                while (tmp >= INITIAL_CODE)
                {
                    tmp = table[tmp].first;
                }
                table[code] = {pr, tmp};
                code++;
            }
            std::stack<char> st;
            while (x >= (1 << 8))
            {
                st.push(table[x].second);
                x = table[x].first;
            }
            st.push(x);
            while (st.size())
            {
                target << st.top();
                st.pop();
            }
            if (code < MAX_DICTIONARY_SIZE && flag)
            {
                table[code] = {pr, x};
                code++;
            }
        }
    }
}