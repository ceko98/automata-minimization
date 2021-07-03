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

class Automata
{
private:
    vector<string> Sigma;

    int Q_size;
    int *Q;
    bool *I;
    int q;
    bool *F;
    unordered_map<pair<int, string>, int> Delta;
    // unordered_map<pair<int, string>, int> Delta_rev;
    list<pair<int, string>> *Delta_rev;

public:
    Automata(/* args */);
    ~Automata();

    pair<int *, int> get_ranks()
    {
        int *rank = new int[Q_size];
        int *degree = new int[Q_size];
        init_with(rank, Q_size, -1);
        init_with(degree, Q_size, 0);
        for (const auto &[key, val] : Delta)
        {
            degree[key.first] += 1;
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
        while (queue.empty())
        {
            int q = queue.front();
            queue.pop();
            for (auto &[p, a] : Delta_rev[q])
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

    int *minimize()
    {
        const auto &[rank, R] = get_ranks();
        vector<vector<int>> B_classes(2 * (R - 1));
        int *class_arr = new int[Q_size];
        int next_class = 0;
        for (int i = 0; i < Q_size; i++)
        {
            int q = Q[i];
            int r = rank[q];
            int f = F[q] ? 0 : 1;
            vector<int> B_r_f = B_classes[2 * r + f];
            if (B_r_f.size() == 0)
            {
                class_arr[q] = next_class;
                next_class += 1;
            }
            else
            {
                class_arr[q] = class_arr[B_r_f[0]];
            }
            B_r_f.push_back(q);
        }
        for (int t = 1; t < R - 1; t++)
        {
            next_class = partition(B_classes[2 * (t - 1)], class_arr, next_class);
            next_class = partition(B_classes[2 * (t - 1) + 1], class_arr, next_class);
        }
        return class_arr;
    }

    int partition(vector<int> B, int *class_arr, int next_class)
    {
        vector<vector<int>> M(B.size(), vector<int>(Sigma.size()));
        for (int i = 0; i < B.size(); i++)
        {
            for (int j = 0; j < Sigma.size(); j++)
            {
                M[i][j] = class_arr[Delta[{B[i], Sigma[j]}]];
            }
        }
        
        
        return next_class;
    }

    void sort(vector<vector<int>> &M)
    {
        vector<int> sorted_rows(M.size());
        for (int j = Sigma.size() - 1; j >= 0; j--)
        {
            vector<queue<int>> buckets(M.size());
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
    }

    void init_with(int *arr, int sz, int val)
    {
        for (size_t i = 0; i < sz; i++)
        {
            arr[i] = val;
        }
    }
};

Automata::Automata(/* args */)
{
}

Automata::~Automata()
{
}

#endif