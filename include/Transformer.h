#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <string>
#include <queue>
#include <algorithm>
#include <list>
#include <vector>
#include <limits>
#include <iostream>

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

class Transformer
{
private:
    // vector<pair<char, string>> Language;
    int Q_size;
    int q0;
    vector<bool> F;
    string beg_word;
    unordered_map<int, unordered_map<char, int>> Delta;
    unordered_map<int, unordered_set<pair<char, int>>> Delta_rev;
    unordered_map<pair<int, char>, string> Lambda;
    unordered_map<int, string> Pfi;

public:
    Transformer();
    Transformer(
        int Q_size, int q0, vector<bool> F, string beg_word,
        unordered_map<int, unordered_map<char, int>> Delta,
        unordered_map<int, unordered_set<pair<char, int>>> Delta_rev,
        unordered_map<pair<int, char>, string> Lambda,
        unordered_map<int, string> Pfi
    );
    ~Transformer();

    void trim()
    {
        bool *accessible = get_accessible();
        bool *coaccessible = get_coaccessible();
        int Q_trim_size = Q_size;
        unordered_map<int, unordered_map<char, int>> Delta_trim;
        bool *F_trim = new bool[Q_trim_size];
        for (int q = 0; q < Q_size; q++)
        {
            if (accessible[q] && coaccessible[q])
            {
                for (const auto &[a, p] : Delta[q])
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

        for(auto & [p, trans]: Delta_trim)
        {
            for(auto & [a, q]: trans)
            {
                cout << "(" << p << " " << a << " " << q << ")" << endl;
            }
        }

        cout << "==============" << endl;
        for(auto & [p, trans]: Delta)
        {
            for(auto & [a, q]: trans)
            {
                cout << "(" << p << " " << a << " " << q << ")" << endl;
            }
        }
        // return the new transformer
    }

    bool *get_accessible()
    {
        bool *accessible = new bool[Q_size];
        queue<int> queue;
        accessible[q0] = true;
        queue.push(q0);
        while (!queue.empty())
        {
            int p = queue.front();
            queue.pop();
            for (const auto &[a, q] : Delta[p])
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

    bool *get_coaccessible()
    {
        bool *coaccessible = new bool[Q_size];
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
            for (const auto &[a, q] : Delta_rev[p])
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
        int Q_A_size = Q_size + 1;
        int F_A = Q_A_size - 1;
        unordered_map<int, unordered_map<string, int>> Delta_A;
        unordered_map<int, unordered_set<pair<string, int>>> Delta_A_rev;
        for (const auto &[p, letter_to_state] : Delta)
        {
            for (const auto &[a, q] : letter_to_state)
            {
                Delta_A[p].insert({ Lambda[{p, a}], q });
                Delta_A_rev[q].insert({ Lambda[{p, a}], p });
                cout << "(" << p << " " << Lambda[{p, a}] << " " << q << ")" << endl;
            }
        }
        cout << "==============" << endl;
        for (int q = 0; q < Q_size; q++)
        {
            if (F[q])
            {

                Delta_A[q].insert({ Pfi[q], F_A });
                Delta_A_rev[F_A].insert({ Pfi[q], q });
            }
        }

        for(auto & [p, trans]: Delta_A)
        {
            for(auto & [a, q]: trans)
            {
                cout << "(" << p << " " << a << " " << q << ")" << endl;
            }
        }
        cout << "==============" << endl;

        for(auto & [p, trans]: Delta_A_rev)
        {
            for(auto & [a, q]: trans)
            {
                cout << "(" << p << " " << a << " " << q << ")" << endl;
            }
        }

        cout << "==============" << endl;

        vector<string> words(Q_A_size);
        vector<bool> visited(Q_A_size, false);
        find_words(F_A, "", visited, q0, Delta_A_rev, words);
        unordered_map<int, unordered_map<string, int>> Delta_A_plus(Delta_A);
        for (int p = 0; p < Q_A_size; p++)
        {
            vector<string> for_add;
            for(const auto& [a, q] : Delta_A[p])
            {
                for_add.push_back(a + words[q]);
            }
            string prefix = largest_common_prefix(for_add);

        }
        
        vector<int> heap;
        vector<int> prev(Q_A_size, -1);
        vector<int> dist(Q_A_size, numeric_limits<int>::max());
        dist[q0] = 0;
        for (int q = 0; q < Q_A_size; q++)
        {
            heap.push_back(q);
        }
        auto cmp = [dist](int l, int r) { return dist[l] < dist[r]; };
        make_heap(heap.begin(), heap.end(), cmp);

        while (heap.size())
        {
            pop_heap(heap.begin(), heap.end(), cmp);
            int u = heap.back();
            heap.pop_back();

            for(const auto& [word, p] : Delta_A_plus[u])
            {
                int alt = dist[u] + word.size();
                if (alt < dist[p])
                {
                    dist[p] = alt;
                    prev[p] = u;
                }
            }
        }
        
        vector<string> omega(Q_A_size);
        for (int q = 0; q < omega.size(); q++)
        {
            omega[q] = words[q].substr(0, dist[q]);
        }

        for (int i = 0; i < omega.size(); i++)
        {
            cout << i << " " << omega[i] << endl;
        }
    }

    void encode_transformer()
    {
        int Q_enc_size = Q_size + 1;
        bool *F_enc = new bool[Q_enc_size];
        unordered_map<pair<int, string>, int> Delta_enc;
        for (const auto &[key, word] : Lambda)
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

    string largest_common_prefix(vector<string> &words)
    {
        if (words.size() == 0)
        {
            return "";
        }
        int to_size = numeric_limits<int>::max();
        for(auto& word : words)
        {
            to_size = min(to_size, (int)word.size());
        }
        string result = "";
        for (int i = 0; i < to_size; i++)
        {
            char cmp = words[0][i];
            for (int j = 1; j < words.size(); j++)
            {
                if (words[j][i] != cmp)
                {
                    return result;
                }
            }
            result += cmp;
        }
        return result;
    }

    void find_words(
        int current,
        string current_word,
        vector<bool> &visited,
        int initial,
        unordered_map<int, unordered_set<pair<string, int>>> &transitions,
        vector<string> &words)
    {
        words[current] = current_word;
        visited[current] = true;
        if (initial == current)
        {
            return;
        }

        for(const auto& [word, next]: transitions[current])
        {
            find_words(next, word + current_word, visited, initial, transitions, words);
        }
    }
};

Transformer::Transformer(
    int Q_size, int q0, vector<bool> F, string beg_word,
    unordered_map<int, unordered_map<char, int>> Delta,
    unordered_map<int, unordered_set<pair<char, int>>> Delta_rev,
    unordered_map<pair<int, char>, string> Lambda,
    unordered_map<int, string> Pfi
) : Q_size(Q_size), q0(q0), F(F), beg_word(beg_word), Delta(Delta), Delta_rev(Delta_rev), Lambda(Lambda), Pfi(Pfi)
{

}

Transformer::~Transformer()
{
}

#endif