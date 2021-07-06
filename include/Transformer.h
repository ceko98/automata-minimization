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
#include "../include/HashDefinitions.h"
#include "../include/Automata.h"

#ifndef __TRANSFORMER_H___
#define __TRANSFORMER_H___

using Transitions = unordered_map<int, unordered_map<char, int>>;
using Transitions_Rev = unordered_map<int, unordered_set<pair<char, int>>>;
using Transitions_Exit = unordered_map<pair<int, char>, string>;
using Finals_Exit = unordered_map<int, string>;

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
        unordered_map<pair<int, char>, string> Lambda,
        unordered_map<int, string> Pfi) : Q_size(Q_size), q0(q0), F(F), beg_word(beg_word), Delta(Delta), Lambda(Lambda), Pfi(Pfi)
    {
        for (const auto &[p, trans] : Delta)
        {
            for (const auto &[a, q] : trans)
            {
                Delta_rev[q].insert({a, p});
            }
        }
    };
    ~Transformer();

    Transformer trim()
    {
        vector<bool> accessible = get_accessible();
        vector<bool> coaccessible = get_coaccessible();

        unordered_map<int, unordered_map<char, int>> Delta_trim;
        unordered_map<pair<int, char>, string> Lambda_trim;

        unordered_map<int, int> old_new;
        int leftmost = 0;
        for (int q = 0; q < Q_size; q++)
        {
            if (accessible[q] && coaccessible[q])
            {
                old_new[q] = leftmost;
                leftmost++;
            }
        }
        int Q_trim_size = leftmost;
        vector<bool> F_trim(Q_trim_size, false);

        for (int q = 0; q < Q_size; q++)
        {
            int q_new = old_new[q];
            if (accessible[q] && coaccessible[q])
            {
                for (const auto &[a, p] : Delta[q])
                {
                    if (accessible[p] && coaccessible[p])
                    {
                        Delta_trim[q_new][a] = old_new[p];
                        Lambda_trim[{q_new, a}] = Lambda[{q, a}];
                    }
                }
                if (F[q])
                {
                    F_trim[q_new] = true;
                }
            }
        }

        unordered_map<int, string> Pfi_trim;
        for (auto [f, word] : Pfi)
        {
            Pfi_trim[old_new[f]] = word;
        }

        return Transformer(
            Q_trim_size, q0, F_trim, beg_word, Delta_trim, Lambda_trim, Pfi_trim);
    }

    vector<bool> get_accessible()
    {
        vector<bool> accessible(Q_size, false);
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

    vector<bool> get_coaccessible()
    {
        vector<bool> coaccessible(Q_size, false);
        queue<int> queue;
        for (int i = 0; i < Q_size; i++)
        {
            if (F[i])
            {
                coaccessible[i] = true;
                queue.push(i);
            }
        }
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

    Transformer canonize()
    {
        vector<string> mso = maximal_state_output();
        // cout << "==============" << endl;

        // for (int i = 0; i < mso.size(); i++)
        // {
        //     cout << i << " " << mso[i] << endl;
        //     ;
        // }
        // cout << "==============" << endl;

        string beg_word_C = beg_word + mso[q0];
        Transitions_Exit Lambda_C;
        for (const auto &[key, word] : Lambda)
        {
            int p = key.first;
            char a = key.second;
            string new_word = (word + mso[Delta[p][a]]).substr(mso[p].size());
            // cout << p << " " << a << " " << word << " " << mso[Delta[p][a]] << " " << new_word << endl;
            Lambda_C[key] = (word + mso[Delta[p][a]]).substr(mso[p].size());
        }

        Finals_Exit Pfi_C;
        for (const auto &[p, word] : Pfi)
        {
            Pfi_C[p] = word.substr(mso[p].size());
        }

        // cout << "==============" << endl;
        // for (auto &[p, word] : Lambda_C)
        // {
        //     cout << "(" << p.first << " " << p.second << " " << word << ")" << endl;
        // }

        return Transformer(
            Q_size, q0, F, beg_word_C, Delta, Lambda_C, Pfi_C
        );
    }

    vector<string> maximal_state_output()
    {
        int Q_A_size = Q_size + 1;
        int F_A = Q_A_size - 1;
        unordered_map<int, unordered_map<string, int>> Delta_A;
        unordered_map<int, unordered_set<pair<string, int>>> Delta_A_rev;
        for (const auto &[p, letter_to_state] : Delta)
        {
            for (const auto &[a, q] : letter_to_state)
            {
                Delta_A[p].insert({Lambda[{p, a}], q});
                Delta_A_rev[q].insert({Lambda[{p, a}], p});
            }
        }
        for (int q = 0; q < Q_size; q++)
        {
            if (F[q])
            {

                Delta_A[q].insert({Pfi[q], F_A});
                Delta_A_rev[F_A].insert({Pfi[q], q});
            }
        }

        vector<string> words(Q_A_size);
        vector<bool> visited(Q_A_size, false);
        find_words(F_A, "", visited, q0, Delta_A_rev, words);
        unordered_map<int, unordered_set<pair<string, int>>> Delta_A_rev_plus(Delta_A_rev);
        for (int p = 0; p < Q_A_size; p++)
        {
            vector<string> for_add;
            for (const auto &[a, q] : Delta_A[p])
            {
                for_add.push_back(a + words[q]);
            }
            string prefix = largest_common_prefix(for_add);
            Delta_A_rev_plus[F_A].insert({prefix, p});
        }

        vector<int> heap;
        vector<int> prev(Q_A_size, -1);
        vector<int> dist(Q_A_size, numeric_limits<int>::max());
        dist[F_A] = 0;
        for (int q = 0; q < Q_A_size; q++)
        {
            heap.push_back(q);
        }
        auto cmp = [dist](int l, int r)
        { return dist[l] > dist[r]; };
        make_heap(heap.begin(), heap.end(), cmp);

        while (heap.size())
        {
            pop_heap(heap.begin(), heap.end(), cmp);
            int u = heap.back();
            heap.pop_back();

            cout << u << endl;

            for (const auto &[word, p] : Delta_A_rev_plus[u])
            {
                int alt = dist[u] + word.size();
                if (alt < dist[p])
                {
                    dist[p] = alt;
                    prev[p] = u;
                }
            }
        }

        vector<string> maximal_state_output(Q_A_size);
        for (int q = 0; q < maximal_state_output.size(); q++)
        {
            maximal_state_output[q] = words[q].substr(0, dist[q]);
        }
        
        return maximal_state_output;
    }

    string largest_common_prefix(vector<string> &words)
    {
        if (words.size() == 0)
        {
            return "";
        }
        int to_size = numeric_limits<int>::max();
        for (auto &word : words)
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

        for (const auto &[word, next] : transitions[current])
        {
            if (!visited[next])
            {
                find_words(next, word + current_word, visited, initial, transitions, words);
            }
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
};

// Transformer::Transformer(
//     int Q_size, int q0, vector<bool> F, string beg_word,
//     unordered_map<int, unordered_map<char, int>> Delta,
//     unordered_map<int, unordered_set<pair<char, int>>> Delta_rev,
//     unordered_map<pair<int, char>, string> Lambda,
//     unordered_map<int, string> Pfi
// )
// {

// }

Transformer::~Transformer()
{
}

#endif