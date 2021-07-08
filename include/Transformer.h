#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <string>
#include <queue>
#include <algorithm>
#include <list>
#include <vector>
#include <limits>
#include <stack>
#include <set>
#include <iostream>
#include "../include/HashDefinitions.h"
#include "../include/Automata.h"

#ifndef __TRANSFORMER_H___
#define __TRANSFORMER_H___

using Transitions = unordered_map<int, unordered_map<char, int>>;
using Transitions_Rev = unordered_map<int, unordered_set<pair<char, int>>>;
using Transitions_Exit = unordered_map<pair<int, char>, int>;
using Finals_Exit = unordered_map<int, int>;

class Transformer
{
private:
    int Q_size;
    int q0;
    vector<bool> F;
    int beg_word;
    unordered_map<int, unordered_map<char, int>> Delta;
    unordered_map<int, unordered_set<pair<char, int>>> Delta_rev;
    unordered_map<pair<int, char>, int> Lambda;
    unordered_map<int, int> Pfi;

public:
    Transformer();
    Transformer(
        int Q_size, int q0, vector<bool> F, int beg_word,
        unordered_map<int, unordered_map<char, int>> Delta,
        unordered_map<pair<int, char>, int> Lambda,
        unordered_map<int, int> Pfi) : Q_size(Q_size), q0(q0), F(F), beg_word(beg_word), Delta(Delta), Lambda(Lambda), Pfi(Pfi)
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

    int get_beg_word()
    {
        return beg_word;
    }

    Transformer trim()
    {
        vector<bool> accessible = get_accessible();
        vector<bool> coaccessible = get_coaccessible();

        unordered_map<int, unordered_map<char, int>> Delta_trim;
        unordered_map<pair<int, char>, int> Lambda_trim;

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

        unordered_map<int, int> Pfi_trim;
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
        vector<int> mso = maximal_state_output();

        int beg_word_C = beg_word + mso[q0];
        Transitions_Exit Lambda_C;
        for (const auto &[key, word] : Lambda)
        {
            int p = key.first;
            char a = key.second;
            int new_word = (word + mso[Delta[p][a]] - mso[p]);
            Lambda_C[key] = (word + mso[Delta[p][a]] - mso[p]);
        }

        Finals_Exit Pfi_C;
        for (const auto &[p, word] : Pfi)
        {
            Pfi_C[p] = word - mso[p];
        }

        return Transformer(
            Q_size, q0, F, beg_word_C, Delta, Lambda_C, Pfi_C
        );
    }

    vector<int> maximal_state_output()
    {
        int Q_A_size = Q_size + 1;
        int F_A = Q_A_size - 1;
        unordered_map<int, unordered_map<int, int>> Delta_A;
        unordered_map<int, unordered_set<pair<int, int>>> Delta_A_rev;
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

        vector<int> words(Q_A_size, 0);
        vector<bool> visited(Q_A_size, false);
        find_words(F_A, 0, visited, q0, Delta_A_rev, words);

        unordered_map<int, unordered_set<pair<int, int>>> Delta_A_rev_plus(Delta_A_rev);
        for (int p = 0; p < Q_A_size; p++)
        {
            int prefix = numeric_limits<int>::max();
            for (const auto &[a, q] : Delta_A[p])
            {
                prefix = min(prefix, a + words[q]);
            }
            prefix = Delta_A[p].size() == 0 ? 0 : prefix;
            Delta_A_rev_plus[F_A].insert({prefix, p});
        }

        vector<int> dist(Q_A_size, numeric_limits<int>::max());
        set<int> s;
        dist[F_A] = 0;
        for (int q = 0; q < Q_A_size; q++)
        {
            s.insert(q);
        }

        while (!s.empty())
        {
            int u = *s.begin();
            for (auto i : s)
            {
                if (dist[i] < dist[u])
                {
                    u = i;
                }
            }

            s.erase(u);
            // cout << u << endl;
            for (const auto &[word, p] : Delta_A_rev_plus[u])
            {
                int alt = dist[u] + word;
                if (alt < dist[p])
                {
                    dist[p] = alt;
                }
            }
        }

        vector<int> maximal_state_output(Q_A_size, 0);
        for (int q = 0; q < maximal_state_output.size(); q++)
        {
            maximal_state_output[q] = dist[q];
        }

        return maximal_state_output;
    }

    void find_words(
        int current,
        int current_word,
        vector<bool> &visited,
        int initial,
        unordered_map<int, unordered_set<pair<int, int>>> &transitions,
        vector<int> &words)
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

    Automata encode_transformer()
    {
        int Q_enc_size = Q_size + 1;
        vector<bool> F_enc(Q_enc_size, false);
        F_enc[Q_enc_size - 1] = true;
        unordered_map<int, unordered_map<string, int>> Delta_enc;
        for (const auto &[key, word] : Lambda)
        {
            auto &[p, a] = key;
            string letter;
            letter += key.second;
            letter += "/";
            letter += to_string(word);
            Delta_enc[p].insert({letter, Delta[p][a]});
        }
        for (int q = 0; q < Q_size; q++)
        {
            if (F[q])
            {
                string letter;
                letter += "@/";
                letter += to_string(Pfi[q]);
                Delta_enc[q].insert({letter, Q_enc_size - 1});
            }
        }

        return Automata(
            Q_enc_size, q0, F_enc, Delta_enc  
        );
    }

    static Transformer decode(Automata &aut, vector<int> &class_arr, int beg_word)
    {
        int Q_size_dec = 0;
        class_arr.pop_back();
        for (int i = 0; i < class_arr.size(); i++)
        {
            Q_size_dec = max(Q_size_dec, class_arr[i]);
        }
        Q_size_dec++;

        unordered_map<int, unordered_map<char, int>> Delta_dec;
        unordered_map<pair<int, char>, int> Lambda_dec;
        unordered_map<int, int> Pfi_dec;
        vector<bool> F_dec(Q_size_dec, false);

        for (auto &[p, trans] : aut.Delta)
        {
            for (auto &[word, q] : trans)
            {
                char a = word[0];
                
                string exit_word = word.substr(2);
                int exit = (exit_word.size() == 0 ? 0 : stoi(exit_word));
                if (aut.F[q])
                {
                    Pfi_dec[class_arr[p]] = exit;
                    F_dec[class_arr[p]] = true;
                }
                else
                {
                    Delta_dec[class_arr[p]][a] = class_arr[q];
                    Lambda_dec[{class_arr[p], a}] = exit;
                }
            }
        }

        return Transformer(
            Q_size_dec, aut.q0, F_dec, beg_word, Delta_dec, Lambda_dec, Pfi_dec
        );
    }

    void print_transformer()
    {
        cout << "states 0..." << Q_size - 1 << endl;
        cout << q0 << endl;
        for (int i = 0; i < Q_size; i++)
        {
            if (F[i])
            {
                cout << i << " ";
            }
        }
        cout << endl;
        for(auto &[p, trans] : Delta)
        {
            for(auto &[a, q] : trans)
            {
                cout << "(" << p << " " << q << " " << a << " " << Lambda[{p,a}] << ")" << endl; 
            }
        }
        cout << "==========" << endl;
        for(auto &[f, w] : Pfi)
        {
            cout << "(" << f << " " << w << ")" << endl;
        }
        cout << "==========" << endl;
        cout << "beg word: " << beg_word << endl;
    }

    bool is_cyclic(int curr, vector<bool> &passed)
    {
        vector<int> colors(Q_size, -1);
        
        for(auto &[a, p] : Delta[curr])
        {
            if (passed[p]) {
                return true;
            } else {
                return is_cyclic(p, passed);
            }
        }
        passed[curr] = false;
        
        return false;
    }
};

Transformer::~Transformer()
{
}

#endif