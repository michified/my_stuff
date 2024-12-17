#include <bits/stdc++.h>

// optimization flags for the compiler for higher efficiency (i am not convinced that they actually help though)
// #pragma GCC optimize("O3")
// #pragma GCC optimize("Ofast,unroll-loops")

#define ll long long
using namespace std;
using ms = chrono::duration<double, milli>;

const int height = 6, width = 7, win = 1e9;
const pair<int, int> directions[7] = {{0, 1}, {-1, 0}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
const int valueTable[2][4] = {{0, 1, 6, 100}, {0, 6, 100, 10000}};
const int bitwiseDirs[4] = {1, 7, 8, 6}; // bitwise shift constants for vertical, horizontal, and diagonal movement
const int order[7] = {3, 2, 4, 5, 1, 0, 6}; // the order in which the bot checks columns

class BoardState {
private:
	int player; // player is -1, machine is 1
	int board[height][width];
	// turns columns into implicit, random-accessible stacks (as is the nature of the connect 4 columns)
	// allows the current height of the column to be instantly obtained instead of looping through each row (O(1) instead of O(H))
	// results in a ~7x speedup when adding/removing pieces from columns as H = 7
	int numPieces[width];
	ll cmpBitBoard;
	ll hmnBitBoard;
	/* 
	0s and 1s to track which positions have pieces of each side
	bit powers correspond to the grid:
	5 12 19 26 33 40 47
	4 11 18 25 32 39 46
	3 10 17 24 31 38 45
	2 9  16 23 30 37 44
	1 8  15 22 29 36 43
	0 7  14 21 28 35 42
	there is a row skipped at the top to ensure that there are no false wins when checking
	more specifically to prevent things like 4 5 7 8 to be counted as a win
	*/ 

	// checks if the coordinates are inside the grid
	bool valid(int row, int col) {
		return row >= 0 and row < height and col >= 0 and col < width;
	}

	// function that takes a player and calculates their score; higher means more advantageous for the player
	int score(int me) {
		int i, j, k, row, col, value = 0, mecnt, oppcnt, remaining;
		vector<pair<int, int>> emptyCurLine;
		int candidates[height][width];
		memset(candidates, 0, sizeof(candidates));

		// iterates through each direction that a line can be connected for each square in the grid
		for (i = 0; i < height; i++) {
			for (j = 0; j < width; j++) {
				for (auto& direction : directions) {
					// checks wheter the endpoint is inside the grid
					if (not valid(i + direction.first * 3, j + direction.second * 3)) continue;

					row = i;
					col = j;
					mecnt = 0;
					oppcnt = 0;
					emptyCurLine.clear();

					// counds the number of pieces of each side in the line
					for (k = 0; k < 4; k++) {
						if (board[row][col] == me) mecnt++;
						else if (board[row][col] == -me) oppcnt++;
						else emptyCurLine.push_back({row, col});
						row += direction.first;
						col += direction.second;
					}

					// decides whether someone won or the pieces contribute to the current player's score
					if (mecnt == 4) return win;
					if (oppcnt == 4) return -win;
					if (oppcnt == 0 and mecnt != 0) {
						for (auto& cell : emptyCurLine) candidates[cell.first][cell.second] = max(candidates[cell.first][cell.second], mecnt);
					}
				}
			}
		}
		
		// a procedure that calculates how 'valuable' each piece is and adds it to the total score
		int cell1, cell2;
		for (j = 0; j < width; j++) {
			for (i = 0; i < height - 1; i++) {
				cell1 = candidates[i][j];
				cell2 = candidates[i + 1][j];
				value += (valueTable[0][cell1] + valueTable[0][cell2] + valueTable[1][min(cell1, cell2)]) * (height - i);
			}
			value += valueTable[0][candidates[height - 1][j]];
		}

		return value;
	}

public:
	// constructor
	BoardState(int player) {
		this->player = player;
		memset(board, 0, sizeof(board));
		memset(numPieces, 0, sizeof(numPieces));
		cmpBitBoard = 0;
		hmnBitBoard = 0;
	}

	// accessor and mutator methods
	int getPlayer() {
		return player;
	}

	void flipPlayer() {
		player = -player;
	}

	bool canAdd(int col) {
		return numPieces[col] < height;
	}

	void addToCol(int col) {
		board[numPieces[col]][col] = player;
		if (player == 1) cmpBitBoard ^= 1ll << (col * 7 + numPieces[col]);
		else hmnBitBoard ^= 1ll << (col * 7 + numPieces[col]);
		numPieces[col]++;
	}

	void removeFromCol(int col) {
		numPieces[col]--;
		if (player == 1) cmpBitBoard ^= 1ll << (col * 7 + numPieces[col]);
		else hmnBitBoard ^= 1ll << (col * 7 + numPieces[col]);
		board[numPieces[col]][col] = 0;
	}

	pair<ll, ll> getBitBoards() {
		return {cmpBitBoard, hmnBitBoard};
	}

	// uses the bitboards to quickly determine whether someone won (O(1) instead of O(WH)) 
	// results in an ~100x speedup during checking for wins
	int isWin() {
		for (int direction : bitwiseDirs) {
			if ((cmpBitBoard & (cmpBitBoard >> direction) & (cmpBitBoard >> (direction * 2)) & (cmpBitBoard >> (direction * 3))) != 0) return win;
		}
		for (int direction : bitwiseDirs) {
			if ((hmnBitBoard & (hmnBitBoard >> direction) & (hmnBitBoard >> (direction * 2)) & (hmnBitBoard >> (direction * 3))) != 0) return -win;
		}
		return 0;
	}

	// evaluates the overall score from the perspective of the computer
	int staticEval() {
		int res = score(1);
		if (abs(res) == win) return res;
		flipPlayer();
		res -= score(-1);
		flipPlayer();
		return res;
	}

	void printBoard() {
		int row, col;
		for (int i = 1; i <= width; i++) cout << i << ' ';
		cout << endl;

		for (row = height - 1; row >= 0; row--) {
			for (col = 0; col < width; col++) {
				if (board[row][col] == 1) cout << "O ";
				else if (board[row][col] == -1) cout << "X ";
				else cout << "_ ";
			}
			cout << endl;
		}
	}
};

/*
transposition tables are
a dynamic programming approach to reduce recomputations by storing the bitboard : result as key-value pairs to avoid 
processing the same subtree again; however, maps (binary search trees) introduce an extra log factor
seenTTable stores the best moves calculated during previous turns to suggest columns to search first as alpha-beta 
search prunes away suboptimal branches faster when encountering a near-optimal branch first
results in an ~8x speedup from testing
first.first: evalVal, first.second: bestChoice, second: depth
*/
map<pair<ll, ll>, pair<pair<int, int>, int>> trnspTable, seenTTable;

/*
the minimax algorithm is the central idea that allows computers to exhaust positions and find optimal moves
it assumes that the computer tries to maximize the score of the board while the player tries to minimize it
*/
pair<int, int> minimax(BoardState& state, int alpha, int beta, int depth) {
	// checks wheter this board has been calculated before
	if (trnspTable.find(state.getBitBoards()) != trnspTable.end()) return trnspTable[state.getBitBoards()].first;

	// (base case) checks whether the depth limit has been reached or someone has won already
	int eval = state.isWin();
	if (abs(eval) == win) return {eval, 0};
	if (depth == 0) return {state.staticEval(), 0};

	int valid = 0, best = -1, bestEval;
	if (state.getPlayer() == 1) bestEval = -win;
	else bestEval = win;

	// best is set to the closest non-full row to the center
	for (int col : order) {
		if (state.canAdd(col)) {
			best = col;
			break;
		}
	}

	/*
	checks the children of the current node of the game tree in descending order of predicted score
	before this, it checks the best move suggested by searches done in previous turns
	by searching in this fashion, the alpha-beta concept can prune away suboptimal branches quicker, which
	results in a ~50x speedup from testing
	*/
	int toSearch = 3;
	if (seenTTable.find(state.getBitBoards()) != seenTTable.end()) toSearch = seenTTable[state.getBitBoards()].first.second;

	if (state.canAdd(toSearch)) {
		state.addToCol(toSearch);
		state.flipPlayer();
		eval = minimax(state, alpha, beta, depth - 1).first;
		state.flipPlayer();
		state.removeFromCol(toSearch);
		
		if (state.getPlayer() == 1) {
			if (eval > bestEval) {
				bestEval = eval;
				best = toSearch;
			}
			alpha = max(alpha, eval);
		} else {
			if (eval < bestEval) {
				bestEval = eval;
				best = toSearch;
			}
			beta = min(beta, eval);
		}

		if (beta <= alpha) {
			trnspTable[state.getBitBoards()] = {{bestEval, best}, depth};
			return {bestEval, best};
		}
	}

	for (int col : order) {
		if (not state.canAdd(col) or col == toSearch) continue;

		state.addToCol(col);
		state.flipPlayer();
		eval = minimax(state, alpha, beta, depth - 1).first;
		state.flipPlayer();
		state.removeFromCol(col);
		
		if (state.getPlayer() == 1) {
			if (eval > bestEval) {
				bestEval = eval;
				best = col;
			}
			alpha = max(alpha, eval);
		} else {
			if (eval < bestEval) {
				bestEval = eval;
				best = col;
			}
			beta = min(beta, eval);
		}

		if (beta <= alpha) break;
	}

	// stores this state in the transposition table to be used later (potentially)
	trnspTable[state.getBitBoards()] = {{bestEval, best}, depth};
	return {bestEval, best};
}

const int allowedMS = 3000;

int main() {
	int starting, col, movesLeft = width * height;
	char inp;
	cout << "Do you want to start first? Type 'y' if you do, otherwise type 'n'." << endl;
	cin >> inp;
	if (inp == 'y') starting = -1;
	else starting = 1;

	BoardState state(starting);

	while (movesLeft--) {
		state.printBoard();

		if (state.getPlayer() == -1) {
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
			int allowedDepth = 1;
			ms elapsed, totalElapsed;

			for (auto& elem : trnspTable) {
				if (seenTTable.find(elem.first) == seenTTable.end() or
					seenTTable[elem.first].second < elem.second.second) seenTTable[elem.first] = elem.second;
			}
			trnspTable.clear();
			const auto before = chrono::system_clock::now();
			col = minimax(state, -win - 1, win + 1, allowedDepth).second;
			totalElapsed = chrono::system_clock::now() - before;

			/*
			this is the concept of iterative deepening depth-first search (IDDFS)
			allows DFS to go deeper and deeper if time allows
			which may actually be faster than just directly searching at the most optimal depth (which is hard to predict too)
			when combined with transposition tables
			a timer is also kept to prevent the search for taking too long; the timeout value can be modified
			*/
			while (totalElapsed.count() < allowedMS and allowedDepth < movesLeft) {
				if (movesLeft < 16) trnspTable.clear();
				for (auto& elem : trnspTable) {
					if (seenTTable.find(elem.first) == seenTTable.end() or
						seenTTable[elem.first].second < elem.second.second) seenTTable[elem.first] = elem.second;
				}
				trnspTable.clear();
				allowedDepth++;
				const auto before2 = chrono::system_clock::now();
				col = minimax(state, -win - 1, win + 1, allowedDepth).second;
				totalElapsed = chrono::system_clock::now() - before;
			}

			cout << "Piece placed in column " << col + 1 << ". (Depth: " << allowedDepth << ") (" << totalElapsed.count() << "ms)"<< endl << endl;
		}

		state.addToCol(col);

		if (abs(state.isWin()) == win) {
			state.printBoard();
			if (state.getPlayer() == 1) cout << "Computer wins.";
			else cout << "You win.";
			return 0;
		}

		state.flipPlayer();
	}

	state.printBoard();
	cout << "Tie!";
	return 0;
}

/*
overall, with the addition of the aforementioned optimization techniques, the program can search up to depth 12
in the first few moves of the game, which is a (W = 7)^(D = 5) = ~17,000x speedup compared to a naive method that can 
only search up to a depth of 7 when both are halted after 3 seconds

note: when the computer takes less than 3 seconds to make a move, it usually means that the computer has
'solved' the position: it found a strategy to win no matter what the player plays
*/