#include <iostream>
#include "../include/Transformer.h"

using namespace std;

int main() {
    int Q_size = 11;
    int q0 = 0;
    vector<bool> F(Q_size, false);
    F[4] = true; F[6] = true;
    unordered_map<int, unordered_map<char, int>> delta = {
        {0, { {'a', 1}, {'b', 7}, {'c', 10} }},
        {1, { {'a', 3}, {'b', 2} }},
        {3, { {'b', 4}, {'a', 5} }},
        {5, { {'a', 6} }},
        {7, { {'a', 9} }},
        {8, { {'a', 7} }},
        {9, { {'a', 6} }},
        {10, { {'a', 6} }},
    };

    unordered_map<pair<int, char>, string> lambda = {
        {{0, 'a'}, "b"}, {{0, 'b'}, "b"}, {{0, 'c'}, "b"},
        {{1, 'a'}, "a"},
        {{3, 'a'}, "a"}, {{3, 'b'}, "b"},
        {{5, 'a'}, ""},
        {{7, 'a'}, ""},
        {{9, 'a'}, "a"},
        {{10, 'a'}, ""},
    };
    string start = "";
    unordered_map<int, string> pfi = {
        {4, "aa"}, {6, "aa"}
    };
    Transformer t(
        Q_size, q0, F, start, delta, lambda, pfi
    );

    Transformer trimmed = t.trim();
    trimmed.canonize();

    // int Q_size = 8, q0 = 0;
    // vector<bool> F = {0,0,0,0,0,0,0,1};
    // unordered_map<int, unordered_map<string, int>> Delta = {
    //     {0, {{"a", 1}} },
    //     {1, {{"a", 2}} },
    //     {2, {{"b", 3}, {"a", 4}} },
    //     {3, {{"b", 5}} },
    //     {4, {{"b", 6}} },
    //     {5, {{"a", 7}} },
    //     {6, {{"a", 7}} },
    // };
    // unordered_map<int, vector<pair<string, int>>> Delta_rev = {
    //     {1, {{"a", 0}} },
    //     {2, {{"b", 1}} },
    //     {3, {{"b", 2}} },
    //     {4, {{"a", 2}} },
    //     {5, {{"b", 3}} },
    //     {6, {{"b", 4}} },
    //     {7, {{"a", 5}, {"a", 6}} },
    // };

    // vector<string> sigma = { "a", "b" };
    // Automata a(
    //     sigma, Q_size, q0, F, Delta, Delta_rev
    // );
    // a.minimize();
    return 0;
}