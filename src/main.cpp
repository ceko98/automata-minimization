#include <iostream>
#include "../include/Transformer.h"

using namespace std;

int main() {
    int Q_size = 7;
    int q0 = 0;
    vector<bool> F = {0,0,0,1,0,0,1};
    unordered_map<int, unordered_map<char, int>> delta = {
        {0, { {'a', 1}, {'b', 4} }},
        {1, { {'a', 2}, {'b', 3} }},
        {2, { {'b', 3} }},
        {3, { {'b', 3} }},
        {4, { {'b', 5} }},
        {5, { {'a', 6} }},
        {6, { {'a', 6} }},
    };
    unordered_map<int, unordered_set<pair<char, int>>> delta_rev = {
        {1, { {'a', 0} }},
        {2, { {'a', 1} }},
        {3, { {'b', 1}, {'b', 2}, {'b', 3} }},
        {4, { {'b', 0} }},
        {5, { {'a', 4} }},
        {6, { {'a', 5}, {'a', 6} }},
    };
    unordered_map<pair<int, char>, string> lambda = {
        {{0, 'a'}, "a"}, {{0, 'b'}, "b"},
        {{1, 'a'}, ""}, {{1, 'b'}, "a"},
        {{2, 'b'}, "a"},
        {{3, 'b'}, "b"},
        {{4, 'b'}, ""},
        {{5, 'a'}, "a"},
        {{6, 'a'}, "a"},
    };
    string start = "";
    unordered_map<int, string> pfi = {
        {3, "aa"}, {6, "aa"}
    };
    Transformer t(
        Q_size, q0, F, start, delta, delta_rev, lambda, pfi
    );

    // t.trim();
    t.maximal_state_output();

    return 0;
}