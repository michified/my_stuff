#include <bits/stdc++.h>
#define ll long long
using namespace std;

const int wordCount = 2315, allowedWords = 10657;

vector<string> words(wordCount);
map<string, ll> identifiers;

int score(vector<string>& valid, string& target) {
    identifiers.clear();
    int i, j, ret = 0;
    ll tot = 0;
    string colors;
    for (auto& word : valid) {
        colors = "bbbbb";
        for (i = 0; i < 5; i++) {
            if (target[i] == word[i]) colors[i] = 'g';
        }
        for (i = 0; i < 5; i++) {
            if (colors[i] == 'g') continue;
            for (j = 0; j < 5; j++) {
                if (i == j) continue;
                if (word[j] == target[i]) colors[i] = 'y';
            }
        }
        identifiers[colors]++;
    }
    for (auto& elem : identifiers) tot += elem.second * elem.second * elem.second;
    return tot / (ll) identifiers.size();
}

string findBest(vector<string>& valid) {
    string best, seq;
    ll bestScore = LLONG_MAX, i, j, k, res, curCnt;
    for (auto& word : words) {
        res = score(valid, word);
        if (res < bestScore) {
            bestScore = res;
            best = word;
        }
    }
    return best;
}

int main() {
    int i, j;

    ifstream file("possibleWords.txt");
    for (i = 0; i < wordCount; i++) file >> words[i];
    file.close();

    string guess = "raise", inp, copy;
    vector<string> valid = words, tmp, valid2, tmp2;

    ifstream file2("allowedGuesses.txt");
    for (i = 0; i < allowedWords; i++) {
        file2 >> inp;
        words.push_back(inp);
    }
    file2.close();

    while (inp != "ggggg") {
        cout << guess << endl;
        cin >> inp;
        bool good, seen;
        for (auto& word : valid) {
            good = true;
            copy = word;
            for (i = 0; i < 5; i++) {
                if (inp[i] == 'g') {
                    if (guess[i] == copy[i]) copy[i] = '#';
                    else {
                        good = false;
                        break;
                    }
                }
            }
            for (i = 0; i < 5; i++) {
                if (inp[i] == 'y') {
                    if (copy[i] == guess[i]) {
                        good = false;
                        break;
                    }
                    seen = false;
                    for (j = 0; j < 5; j++) {
                        if (i == j) continue;
                        if (copy[j] == guess[i]) {
                            copy[j] = '#';
                            seen = true;
                            break;
                        }
                    }
                    if (not seen) {
                        good = false;
                        break;
                    }
                }
            }
            for (i = 0; i < 5; i++) {
                if (inp[i] == 'b') {
                    for (j = 0; j < 5; j++) {
                        if (copy[j] == guess[i]) {
                            good = false;
                            break;
                        }
                    }
                }
            }
            if (good) tmp.push_back(word);
        }
        swap(valid, tmp);
        tmp.clear();
        if (valid.size() == 1) {
            cout << valid[0] << endl;
            return 0;
        }
        guess = findBest(valid);
    }
    return 0;
}