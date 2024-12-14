#include <bits/stdc++.h>
using namespace std;

const int wordsCount = 44392;

int main() {
    ifstream file("words.txt");
    vector<string> words(wordsCount);
    for (int i = 0; i < wordsCount; i++) file >> words[i];
    file.close();

    char yellow;
    cout << "Enter the yellow letter in lowercase:" << endl;
    cin >> yellow;
    string whites;
    cout << "Enter the white letters in one word in lowercase (e.g. abcdef):";
    cin >> whites;
    vector<bool> whitesLookup(120);
    for (char c : whites) whitesLookup[c] = true;

    vector<string> valid;
    for (string& word : words) {
        bool hasYellow = false, allWhite = true;
        for (char c : word) {
            if (not whitesLookup[c]) {
                allWhite = false;
                break;
            }
            if (c == yellow) hasYellow = true;
        }
        if (hasYellow and allWhite) valid.push_back(word);
    }

    cout << "Type any letter for a new word.";
    reverse(valid.begin(), valid.end());
    while (not valid.empty()) {
        char inp;
        cin >> inp;
        cout << valid.back() << endl;
        valid.pop_back();
    }
    cout << "No more words.";
    return 0;
}