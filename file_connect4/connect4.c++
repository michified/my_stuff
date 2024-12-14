#include <bits/stdc++.h>
#pragma GCC optimize("O3")
#pragma GCC optimize("Ofast,unroll-loops")
#define ll long long
using namespace std;
using ms = chrono::duration<double, milli>;

const int height = 6, width = 7, win = 1e9;
const pair<int, int> directions[7] = {{0, 1}, {-1, 0}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
const int valueTable[2][4] = {{0, 1, 6, 100}, {0, 6, 100, 10000}};

class BoardState {
private:
	int player; // player is -1, machine is 1
	int board[height][width];
	int numPieces[width];

	bool valid(int row, int col) {
		return row >= 0 and row < height and col >= 0 and col < width;
	}

	int count(int me) {
		int i, j, k, row, col, value = 0, mecnt, oppcnt, remaining;
		vector<pair<int, int>> emptyCurLine;
		int candidates[height][width];
		memset(candidates, 0, sizeof(candidates));
		// for locations that can add to a line, first is row, second is column

		for (i = 0; i < height; i++) {
			for (j = 0; j < width; j++) {
				for (auto& direction : directions) {
					if (not valid(i + direction.first * 3, j + direction.second * 3)) continue;

					row = i;
					col = j;
					mecnt = 0;
					oppcnt = 0;
					emptyCurLine.clear();

					for (k = 0; k < 4; k++) {
						if (board[row][col] == me) mecnt++;
						else if (board[row][col] == -me) oppcnt++;
						else emptyCurLine.push_back({row, col});
						row += direction.first;
						col += direction.second;
					}

					if (mecnt == 4) return win;
					if (oppcnt == 4) return -win;
					if (oppcnt == 0 and mecnt != 0) {
						for (auto& cell : emptyCurLine) candidates[cell.first][cell.second] = max(candidates[cell.first][cell.second], mecnt);
					}
				}
			}
		}
		
		int cell1, cell2;
		for (j = 0; j < width; j++) {
			for (i = 0; i < height - 1; i++) {
				cell1 = candidates[i][j];
				cell2 = candidates[i + 1][j];
				value += (valueTable[0][cell1] + valueTable[0][cell2] + valueTable[1][min(cell1, cell2)]) * (height - i);
			}
		}

		return value;
	}

public:
	BoardState(int player) {
		this->player = player;
		memset(board, 0, sizeof(board));
		memset(numPieces, 0, sizeof(numPieces));
	}

	int getPlayer() {
		return player;
	}

	void flipPlayer() {
		player = -player;
	}

	bool canAdd(int col) {
		return numPieces[col] < height;
	}

	void add(int col) {
		board[numPieces[col]][col] = player;
		numPieces[col]++;
	}

	void remove(int col) {
		numPieces[col]--;
		board[numPieces[col]][col] = 0;
	}

	int staticEval() {
		int res = count(1);
		if (abs(res) == win) return res;
		flipPlayer();
		res -= count(-1);
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

// first elem is eval value, second is pos of best move
pair<int, int> minimax(BoardState& state, int alpha, int beta, int depth) {
	int eval = state.staticEval();
	if (depth == 0 or abs(eval) == win) return {eval, 0};

	int valid = 0, best = -1, bestEval, col;
	if (state.getPlayer() == 1) bestEval = -win;
	else bestEval = win;

	vector<pair<int, int>> order; // first is value after added, second is column number
	for (int i = 0; i < width; i++) {
		if (state.canAdd(i)) {
			state.add(i);
			order.push_back({-state.staticEval(), i});
			state.remove(i);
		}
	}
	sort(order.begin(), order.end());

	for (auto& elem : order) {
		col = elem.second;
		if (not state.canAdd(col)) continue;

		state.add(col);
		state.flipPlayer();
		eval = minimax(state, alpha, beta, depth - 1).first;
		state.remove(col);
		state.flipPlayer();
		
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
			state.add(col);
		} else {
			cout << "Computer is thinking..." << endl;
			int allowedDepth = 1;
			ms elapsed, totalElapsed;

			const auto before = chrono::system_clock::now();
			col = minimax(state, -win, win, allowedDepth).second;
			totalElapsed = chrono::system_clock::now() - before;

			while (totalElapsed.count() < allowedMS and allowedDepth < movesLeft) {
				allowedDepth++;
				const auto before2 = chrono::system_clock::now();
				col = minimax(state, -win, win, allowedDepth).second;
				totalElapsed = chrono::system_clock::now() - before;
			}

			cout << "Piece placed in column " << col + 1 << ". (Depth: " << allowedDepth << ") (" << totalElapsed.count() << "ms)"<< endl << endl;
			state.add(col);
		}

		if (abs(state.staticEval()) == win) {
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