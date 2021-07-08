#include <unordered_map>

#ifndef __HASH_DEFINITIONS__
#define __HASH_DEFINITIONS__

using namespace std;

template <>
struct std::hash<pair<int, char>>
{
    void hash_combine(size_t &seed, size_t value) const
    {
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    size_t operator()(const pair<int, char> &a) const
    {
        size_t seed = 0;
        size_t h1 = hash<int>()(a.first);
        size_t h2 = hash<char>()(a.second);
        hash_combine(seed, h1);
        hash_combine(seed, h2);
        return seed;
    }
};


template <>
struct std::hash<pair<char, int>>
{
    void hash_combine(size_t &seed, size_t value) const
    {
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    size_t operator()(const pair<char, int> &a) const
    {
        size_t seed = 0;
        size_t h1 = hash<char>()(a.first);
        size_t h2 = hash<int>()(a.second);
        hash_combine(seed, h1);
        hash_combine(seed, h2);
        return seed;
    }
};

template <>
struct std::hash<pair<int, string>>
{
    void hash_combine(size_t &seed, size_t value) const
    {
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    size_t operator()(const pair<int, string> &a) const
    {
        size_t seed = 0;
        size_t h1 = hash<int>()(a.first);
        size_t h2 = hash<string>()(a.second);
        hash_combine(seed, h1);
        hash_combine(seed, h2);
        return seed;
    }
};

template <>
struct std::hash<pair<string, int>>
{
    void hash_combine(size_t &seed, size_t value) const
    {
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    size_t operator()(const pair<string, int> &a) const
    {
        size_t seed = 0;
        size_t h1 = hash<string>()(a.first);
        size_t h2 = hash<int>()(a.second);
        hash_combine(seed, h1);
        hash_combine(seed, h2);
        return seed;
    }
};


template <>
struct std::hash<pair<int, int>>
{
    void hash_combine(size_t &seed, size_t value) const
    {
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    size_t operator()(const pair<int, int> &a) const
    {
        size_t seed = 0;
        size_t h1 = hash<int>()(a.first);
        size_t h2 = hash<int>()(a.second);
        hash_combine(seed, h1);
        hash_combine(seed, h2);
        return seed;
    }
};

#endif