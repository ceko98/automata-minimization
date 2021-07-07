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
    Transformer canonized = trimmed.canonize();
    Automata a_enc = canonized.encode_transformer();
    vector<int> classes = a_enc.minimize();
    // a_enc.print();
    Transformer minimized = Transformer::decode(a_enc, classes, canonized.get_beg_word());

    minimized.print_transformer();

    return 0;
}