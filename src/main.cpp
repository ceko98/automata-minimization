#include <unordered_map>
#include <utility>
#include <string>
#include <iostream>
#include "../include/Automata.h"

using namespace std;

vector<int> sort(vector<vector<int>> &M)
{
    vector<int> sorted_rows(M.size());
    for (int j = 3 - 1; j >= 0; j--)
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
    return sorted_rows;
}

int main() {
    vector<vector<int>> m = {
        {1, 2, 1},
        {0, 0, 1},
        {2, 2, 2},
        {0, 3, 3},
        {1, 4, 4},
        {2, 5, 5},
    };
    vector<int> sorted = sort(m);
    for (size_t i = 0; i < sorted.size(); i++)
    {
        std::cout << sorted[i] << " " << std::endl;
    }
    
    // Automata a;
    // a.sort();
    return 0;
}