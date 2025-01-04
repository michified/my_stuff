#include <bits/stdc++.h>
#define ll long long
using namespace std;

const int wordCount = 2316, allowedWords = 10656;

vector<string> words(wordCount);
map<string, int> identifiers;

int score(vector<string>& valid, string& target) {
    identifiers.clear();
    int i, j;
    ll ret = 0;
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
    for (auto& elem : identifiers) ret += (ll) elem.second * elem.second;
    return ret;
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

    string guess = "crane", inp, copy;
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
        if (inp == "ggggg") return 0;
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
        if (valid.size() == 2) guess = valid[0];
        else guess = findBest(valid);
    }
    return 0;
}