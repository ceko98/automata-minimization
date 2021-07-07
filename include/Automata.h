#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <string>
#include <queue>
#include <algorithm>
#include <list>
#include <vector>
#include <iostream>
#include "../include/HashDefinitions.h"

#ifndef __AUTOMATA_H___
#define __AUTOMATA_H___

using namespace std;

class Automata
{
public:
    vector<string> Alphabet;
    int Q_size;
    int q0;
    vector<bool> F;
    unordered_map<int, unordered_map<string, int>> Delta;
    unordered_map<int, unordered_set<pair<string, int>>> Delta_rev;

public:
    Automata(
        int Q_size,
        int q0,
        vector<bool> F,
        unordered_map<int, unordered_map<string, int>> Delta);
    ~Automata();

    pair<vector<int>, int> get_ranks()
    {
        vector<int> rank(Q_size, -1);
        vector<int> degree(Q_size, 0);
        for (const auto &[p, trans] : Delta)
        {
            degree[p] += trans.size();
        }
        queue<int> queue;
        for (int i = 0; i < Q_size; i++)
        {
            if (F[i] && degree[i] == 0)
            {
                queue.push(i);
                rank[i] = 0;
            }
        }
        int R = 0;
        while (!queue.empty())
        {
            int q = queue.front();
            queue.pop();
            for (const auto &[a, p] : Delta_rev[q])
            {
                rank[p] = max(rank[p], rank[q] + 1);
                degree[p] -= 1;
                if (degree[p] == 0)
                {
                    queue.push(p);
                }
                R = max(R, rank[p]);
            }
        }
        return {rank, R};
    }

    vector<int> minimize()
    {
        const auto &[rank, R] = get_ranks();
        vector<vector<int>> B_classes(2 * R);
        vector<int> class_arr(Q_size);
        int next_class = 0;
        for (int q = 0; q < Q_size; q++)
        {
            int r = rank[q];
            int B_ind = 0;
            if (r == R)
            {
                B_ind = 1;
            }
            else if (r == 0)
            {
                B_ind = 0;
            }
            else
            {
                int f = F[q] ? 0 : 1;
                B_ind = 2 * r + f;
            }

            if (B_classes[B_ind].size() == 0)
            {
                class_arr[q] = next_class;
                next_class += 1;
            }
            else
            {
                class_arr[q] = class_arr[B_classes[B_ind][0]];
            }
            B_classes[B_ind].push_back(q);
        }

        for (int t = 1; t < R; t++)
        {
            next_class = partition(B_classes[2 * t], class_arr, next_class);
            next_class = partition(B_classes[2 * t + 1], class_arr, next_class);
        }

        // for (int i = 0; i < Q_size; i++)
        // {
        //     cout << i << " " << class_arr[i] << endl;
        // }

        return class_arr;
    }

    int partition(vector<int> &B, vector<int> &class_arr, int next_class)
    {
        if (B.size() == 0)
        {
            return next_class;
        }
        vector<vector<int>> M(B.size(), vector<int>(Alphabet.size()));
        for (int i = 0; i < B.size(); i++)
        {
            for (int j = 0; j < Alphabet.size(); j++)
            {
                int class_value = Delta.find(B[i]) != Delta.end() && Delta[B[i]].find(Alphabet[j]) != Delta[B[i]].end() ? class_arr[Delta[B[i]][Alphabet[j]]] : next_class;
                M[i][j] = class_value;
            }
        }
        vector<int> sorted_rows = sort(M, next_class + 1);
        int current_class = class_arr[B[0]];

        for (int i = 0; i < B.size(); i++)
        {
            if (i != 0 && !same_vectors(M[sorted_rows[i]], M[sorted_rows[i - 1]]))
            {
                current_class = next_class;
                next_class += 1;
            }
            class_arr[B[sorted_rows[i]]] = current_class;
        }

        return next_class;
    }

    vector<int> sort(vector<vector<int>> &M, int max_classes)
    {
        vector<int> sorted_rows(M.size());
        for (int j = Alphabet.size() - 1; j >= 0; j--)
        {
            vector<queue<int>> buckets(max_classes);
            for (int i = 0; i < M.size(); i++)
            {
                buckets[M[i][j]].push(i);
            }
            int k = 0;
            for (int i = 0; i < buckets.size(); i++)
            {
                while (!buckets[i].empty())
                {
                    sorted_rows[k++] = buckets[i].front();
                    buckets[i].pop();
                }
            }
        }
        return sorted_rows;
    }

    bool same_vectors(vector<int> &a, vector<int> &b)
    {
        for (int i = 0; i < a.size(); i++)
        {
            if (a[i] != b[i])
            {
                return false;
            }
        }

        return true;
    }

    void print()
    {
        cout << "automata ============ "<< endl;
        for (int i = 0; i < Alphabet.size(); i++)
        {
            cout << Alphabet[i] << " ";
        }
        cout << endl;
        
        cout << "0..." << Q_size - 1 << endl;
        cout << q0 << endl;
        cout << "f: ";
        for (int i = 0; i < Q_size; i++)
        {
            if (F[i])
            {
                cout << i << " ";
            }
        }
        cout << "============ "<< endl;
        for(auto &[p, trans] : Delta)
        {
            for(auto &[a, q] : trans)
            {
                cout << "(" << p << " " << a << " " << q << ")" << endl; 
            }
        }
        cout << "automata ============ "<< endl;
    }
};

Automata::Automata(
    int Q_size,
    int q0,
    vector<bool> F,
    unordered_map<int, unordered_map<string, int>> Delta) : Q_size(Q_size), q0(q0), F(F), Delta(Delta)
{
    unordered_set<string> Alphabet_set;
    for (const auto &[p, trans] : Delta)
    {
        for (const auto &[a, q] : trans)
        {
            Delta_rev[q].insert({a, p});
            Alphabet_set.insert(a);
        }
    }

    for (auto &a : Alphabet_set)
    {
        Alphabet.push_back(a);
    }
}

Automata::~Automata()
{
}

#endif