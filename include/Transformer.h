#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <string>
#include <queue>
#include <algorithm>
#include <list>
#include <vector>

#ifndef __AUTOMATA_H___
#define __AUTOMATA_H___

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

class Transformer
{
private:
    vector<pair<char, string>> Language;
    int Q_size;
    int *Q;
    int q0;
    bool *F;
    string beg_word;
    unordered_map<int, unordered_map<char, int>> Delta;
    unordered_map<int, unordered_map<char, int>> Delta_rev;
    unordered_map<pair<int, char>, string> Lambda;
    unordered_map<int, string> Pfi;
public:
    Transformer();
    ~Transformer();

    void trim()
    {
        bool * accessible = get_accessible();
        bool * coaccessible = get_coaccessible();
        int Q_trim_size = Q_size;
        unordered_map<int, unordered_map<char, int>> Delta_trim;
        bool * F_trim = new bool[Q_trim_size];
        for (int q = 0; q < Q_size; q++)
        {
            if (accessible[q] && coaccessible[q])
            {
                for(const auto& [a, p] : Delta[q])
                {
                    if (accessible[p] && coaccessible[p])
                    {
                        Delta_trim[q][a] = p;
                    }
                }
                if (F[q])
                {
                    F_trim[q] = true;
                }
            }
        }
        delete accessible;
        delete coaccessible;
        // return the new transformer
    }

    bool * get_accessible()
    {
        bool * accessible = new bool[Q_size];
        queue<int> queue;
        accessible[q0] = true;
        queue.push(q0);
        while (!queue.empty())
        {
            int p = queue.front();
            queue.pop();
            for (const auto& [a, q] : Delta[p])
            {
                if (!accessible[q])
                {
                    accessible[q] = true;
                    queue.push(q);
                }
            }
        }
        return accessible;
    }

    bool * get_coaccessible()
    {
        bool * coaccessible = new bool[Q_size];
        queue<int> queue;
        for (int i = 0; i < Q_size; i++)
        {
            if (F[i])
            {
                coaccessible[i] = true;
                queue.push(i);
            }
        }
        coaccessible[q0] = true;
        queue.push(q0);
        while (!queue.empty())
        {
            int p = queue.front();
            queue.pop();
            for (const auto& [a, q] : Delta_rev[p])
            {
                if (!coaccessible[q])
                {
                    coaccessible[q] = true;
                    queue.push(q);
                }
            }
        }
        return coaccessible;
    }

    void maximal_state_output()
    {
        
    }

    void encode_transformer()
    {
        int Q_enc_size = Q_size + 1;
        bool * F_enc = new bool[Q_enc_size];
        unordered_map<pair<int, string>, int> Delta_enc;
        for (const auto& [key, word]: Lambda)
        {
            string letter;
            letter += key.second;
            letter += "/";
            letter += word;
            Delta_enc[{key.first, letter}] = Delta[key.first][key.second];
        }
        for (int q = 0; q < Q_size; q++)
        {
            if (F[q])
            {
                string letter;
                letter += "@/";
                letter += Pfi[q];
                Delta_enc[{q, letter}] = Q_enc_size;
            }
        }
        // minimize the automata
    }
};

Transformer::Transformer()
{
}

Transformer::~Transformer()
{
}


#endif