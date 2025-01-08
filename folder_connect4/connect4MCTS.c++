#include <bits/stdc++.h>
// #pragma GCC optimize("O3")
// #pragma GCC optimize("Ofast,unroll-loops")
#define ll long long
using namespace std;
using ms = chrono::duration<double, milli>;

const int height = 6, width = 7;
const int bitwiseDirs[4] = {1, 7, 8, 6}; // bitwise shift constants for vertical, horizontal, and diagonal movement

class BoardState {
public:
	int player; // player is -1, machine is 1
	int numPieces[width];
	int placed;
	ll cmpBitBoard;
	ll hmnBitBoard;
	double wins;
	int total;
	vector<BoardState*> children;

	BoardState(int player) { // create new instance
		this->player = player;
		memset(numPieces, 0, sizeof(numPieces));
		cmpBitBoard = 0;
		hmnBitBoard = 0;
		wins = 0;
		total = 0;
		placed = 0;
	}

	BoardState(BoardState* toClone, int move) { // creates instance of next move
		for (int i = 0; i < width; i++) numPieces[i] = toClone->numPieces[i];
		placed = toClone->placed;
		cmpBitBoard = toClone->cmpBitBoard;
		hmnBitBoard = toClone->hmnBitBoard;
		player = -toClone->player;
		addToCol(move);
		wins = 0;
		total = 0;
	}

	BoardState(BoardState* toClone) { // clones current instance
		player = toClone->player;
		for (int i = 0; i < width; i++) numPieces[i] = toClone->numPieces[i];
		cmpBitBoard = toClone->cmpBitBoard;
		hmnBitBoard = toClone->hmnBitBoard;
		wins = 0;
		total = 0;
		placed = toClone->placed;
	}	

	bool canAdd(int col) {
		return numPieces[col] < height;
	}

	void addToCol(int col) {
		if (player == 1) cmpBitBoard ^= 1ll << (col * 7 + numPieces[col]);
		else hmnBitBoard ^= 1ll << (col * 7 + numPieces[col]);
		numPieces[col]++;
		placed++;
	}

	int isWin() {
		for (int direction : bitwiseDirs) {
			if ((cmpBitBoard & (cmpBitBoard >> direction) & (cmpBitBoard >> (direction * 2)) & (cmpBitBoard >> (direction * 3))) != 0) return 1;
		}
		for (int direction : bitwiseDirs) {
			if ((hmnBitBoard & (hmnBitBoard >> direction) & (hmnBitBoard >> (direction * 2)) & (hmnBitBoard >> (direction * 3))) != 0) return -1;
		}
		return 0;
	}
};

double c = sqrt(2);

double uct(BoardState* board, int curRan) {
	return board->wins / (board->total == 0 ? (double) 0.00001 : board->total) + c * sqrt(log(max(curRan, 3)) / (board->total == 0 ? (double) 0.00001 : board->total));
}

void traverse(vector<BoardState*>& path) {
	while (not path.back()->children.empty()) {
		double bestUCT = -1, res;
		BoardState* best;
		for (auto child : path.back()->children) {
			res = uct(child, path.back()->total);
			if (res > bestUCT) {
				bestUCT = res;
				best = child;
			}
		}
		path.push_back(best);
	}
}

int rollout(BoardState* board) {
	BoardState copy(board);
	int move, played = 0;
	for (int i = 0; i < width; i++) played += copy.numPieces[i];
	while (not copy.isWin() and played < width * height) {
		move = rand() % width;
		while (not copy.canAdd(move)) move = rand() % width;
		copy.addToCol(move);
		copy.player = -copy.player;
		played++;
	}
	return copy.isWin();
}

void monteCarlo(BoardState* board) {
	vector<BoardState*> path = {board};
	traverse(path);
	int verdict;
	if (path.back()->placed == width * height) {
		verdict = board->isWin();
	} else if (path.back()->total == 0) {
		verdict = rollout(path.back());
	} else {
		for (int i = 0; i < width; i++) {
			if (not path.back()->canAdd(i)) continue;
			path.back()->children.push_back(new BoardState(path.back(), i));
		}
		auto cur = path.back()->children[0];
		verdict = rollout(cur);
		cur->total++;
		if (cur->player == verdict) cur->wins++;
		else if (verdict == 0) cur->wins += 0.5;
	}
	for (auto node : path) {
		node->total++;
		if (node->player == verdict) node->wins++;
		else if (verdict == 0) node->wins += 0.5;
	}
}

const int allowedMS = 2000;

void printBoard(BoardState board) {
	cout << "1 2 3 4 5 6 7" << endl;
	for (ll row = 1 << (height - 1); row >= 1; row >>= 1) {
		for (ll cell = row; cell <= (1ll << 47); cell <<= 7) {
			if (board.cmpBitBoard & cell) cout << "O ";
			else if (board.hmnBitBoard & cell) cout << "X ";
			else cout << "_ ";
		}
		cout << endl;
	}
}

int main() {
	int starting, col, movesLeft = width * height;
	char inp;
	cout << "Do you want to start first? Type 'y' if you do, otherwise type 'n'." << endl;
	cin >> inp;
	if (inp == 'y') starting = -1;
	else starting = 1;

	BoardState state(-1), dummy(0);

	while (movesLeft--) {
		printBoard(state);

		if (starting == -1) {
			cout << "Your turn! Which column?" << endl;
			cin >> inp;
			col = inp - '0' - 1;
			while (col < 0 or col >= width or not state.canAdd(col)) {
				cout << "Invalid column! Please try again." << endl;
				cin >> inp;
				col = inp - '0' - 1;
			}
			cout << "Piece placed in column " << col + 1 << "." << endl << endl;
		} else {
			cout << "Computer is thinking..." << endl;
			const auto before = chrono::system_clock::now();
			ms totalElapsed = chrono::system_clock::now() - before;
			dummy.children.push_back(new BoardState(state));
			for (int i = 0; i < width; i++) {
				if (dummy.children[0]->canAdd(i)) dummy.children[0]->children.push_back(new BoardState(dummy.children[0], i));
			}
			while (totalElapsed.count() < allowedMS) {
				monteCarlo(dummy.children[0]);
				totalElapsed = chrono::system_clock::now() - before;
			}
			int best = 0, cur = 0;
			for (int i = 0; i < width; i++) {
				if (not dummy.children[0]->canAdd(i)) continue;
				auto child = dummy.children[0]->children[cur];
				cur++;
				cout << i << ' ' << child->total << ' ';
				if (child->total > best) {
					best = child->total;
					col = i;
				}
			}
			dummy.children.pop_back();
			cout << endl;
			cout << "Piece placed in column " << col + 1 << endl << endl;
		}

		state.player = starting;
		state.addToCol(col);
		state.player = -1;

		if (abs(state.isWin()) == 1) {
			printBoard(state);
			if (starting == 1) cout << "Computer wins.";
			else cout << "You win.";
			return 0;
		}

		starting = -starting;
	}

	printBoard(state);
	cout << "Tie!";
	return 0;
}