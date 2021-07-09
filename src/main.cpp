#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "../include/Transformer.h"

using namespace std;

int main()
{
    ifstream file;
    // numbers.nsfst
    file.open("numbers.nsfst");
    int lines = 1 + count(
        istreambuf_iterator<char>(file),
        istreambuf_iterator<char>(), '\n');
    file.close();

    file.open("numbers.nsfst");
    int p, q;
    char a;
    int exit;

    int q0;
    file >> q0;
    string f_line;
    getline(file, f_line);
    getline(file, f_line);

    unordered_map<int, unordered_map<char, int>> delta;
    unordered_map<pair<int, char>, int> lambda;
    unordered_map<int, int> pfi;
    int Q_size = 0;
    for (int i = 2; i < lines; i++)
    {
        file >> p >> q >> a >> exit;
        delta[p - 1].insert({a, q - 1});
        lambda[{p - 1, a}] = exit;
        Q_size = max(Q_size, max(p - 1, q - 1));
    }

    Q_size++;
    vector<bool> F(Q_size, false);
    vector<int> F_set;

    stringstream ss(f_line);
    int f;
    int f_exit;
    while (ss >> f >> f_exit)
    {
        F_set.push_back(f - 1);
        pfi[f - 1] = f_exit;
        F[f - 1] = true;
    }

    file.close();

    Transformer t(
        Q_size, q0 - 1, F, 0, delta, lambda, pfi
    );

    /*
    states: 127
    finals: 
    transistions:
    */

    // t.print_transformer();
    Transformer trimmed = t.trim();
    // trimmed.print_transformer();
    Transformer canonized = trimmed.canonize();
    // canonized.print_transformer();
    Automata a_enc = canonized.encode_transformer();
    vector<int> classes = a_enc.minimize();
    Transformer minimized = Transformer::decode(a_enc, classes, canonized.get_beg_word());

    minimized.print_transformer();

    return 0;
}