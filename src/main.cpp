#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "../include/Transformer.h"

using namespace std;

int main()
{
    ifstream file;
    file.open("test");
    int lines = 1 + count(
        istreambuf_iterator<char>(file),
        istreambuf_iterator<char>(), '\n');
    file.close();

    file.open("test");
    int p, q;
    char a;
    string exit;


    unordered_map<int, unordered_map<char, int>> delta;
    unordered_map<pair<int, char>, string> lambda;
    unordered_map<int, string> pfi;
    int Q_size = 0;
    for (int i = 0; i < lines - 4; i++)
    {
        file >> p >> q >> a >> exit;
        delta[p].insert({a, q});
        lambda[{p, a}] = exit == "*" ? "" : exit;
        Q_size = max(Q_size, max(p, q));
    }

    Q_size++;
    int q0;
    vector<bool> F(Q_size, false);
    vector<int> F_set;
    file >> q0; 

    string line;
    getline(file, line);
    getline(file, line);
    stringstream ss(line);
    int f;
    while (ss >> f)
    {
        F_set.push_back(f);
    }

    string beg_word;
    getline(file, beg_word);
    beg_word = beg_word == "*" ? "" : beg_word;

    vector<string> exits_set;
    getline(file, line);
    stringstream ss_exits(line);
    string exit_w;
    while (ss_exits >> exit_w)
    {
        exits_set.push_back(exit_w);
    }

    for (int i = 0; i < F_set.size(); i++)
    {
        int f = F_set[i];
        F[f] = true;
        pfi[f] = exits_set[i] == "*" ? "" : exits_set[i];
    }

    file.close();

    Transformer t(
        Q_size, q0, F, beg_word, delta, lambda, pfi
    );

    // t.print_transformer();

    Transformer trimmed = t.trim();
    Transformer canonized = trimmed.canonize();
    Automata a_enc = canonized.encode_transformer();
    vector<int> classes = a_enc.minimize();
    // a_enc.print();
    Transformer minimized = Transformer::decode(a_enc, classes, canonized.get_beg_word());

    minimized.print_transformer();

    return 0;
}