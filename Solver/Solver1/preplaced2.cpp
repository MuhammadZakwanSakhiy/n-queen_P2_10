#include <vector>
#include <utility>
#include "nqueens.h"

using namespace std;

class PreplacedMRV {
public:
    int n;
    unsigned int full;
    vector<int> solution;
    vector<vector<int>> answers;
    unsigned int usedCols = 0, usedD1 = 0, usedD2 = 0;
    bool valid = true;

    PreplacedMRV(int size, const vector<pair<int,int>>& fixed)
        : n(size), full((1u << size) - 1u), solution(size, -1) {
        for (auto [r, c] : fixed) {
            if (r < 0 || r >= n || c < 0 || c >= n) {
                valid = false;
                return;
            }
            if (solution[r] != -1) {
                if (solution[r] != c) valid = false;
                continue;
            }
            unsigned int column = 1u << c;
            unsigned int diag1 = 1u << (r - c + n - 1);
            unsigned int diag2 = 1u << (r + c);
            if ((usedCols & column) || (usedD1 & diag1) || (usedD2 & diag2)) {
                valid = false;
                return;
            }
            solution[r] = c;
            usedCols |= column;
            usedD1 |= diag1;
            usedD2 |= diag2;
        }
    }

    unsigned int candidates(int r) const {
        unsigned int result = 0;
        unsigned int columns = full & ~usedCols;
        while (columns) {
            unsigned int bit = columns & (0u - columns);
            columns -= bit;
            int c = __builtin_ctz(bit);
            if (!(usedD1 & (1u << (r - c + n - 1))) &&
                !(usedD2 & (1u << (r + c)))) {
                result |= bit;
            }
        }
        return result;
    }

    void dfs(int remaining) {
        if (remaining == 0) {
            answers.push_back(solution);
            return;
        }

        int chosenRow = -1;
        int smallest = n + 1;
        unsigned int choices = 0;
        for (int r = 0; r < n; ++r) {
            if (solution[r] != -1) continue;
            unsigned int options = candidates(r);
            int count = __builtin_popcount(options);
            if (count == 0) return; // forward checking: dead end
            if (count < smallest) {
                smallest = count;
                chosenRow = r;
                choices = options;
            }
        }

        while (choices) {
            unsigned int bit = choices & (0u - choices);
            choices -= bit;
            int c = __builtin_ctz(bit);
            unsigned int d1 = 1u << (chosenRow - c + n - 1);
            unsigned int d2 = 1u << (chosenRow + c);
            solution[chosenRow] = c;
            usedCols |= bit;
            usedD1 |= d1;
            usedD2 |= d2;
            dfs(remaining - 1);
            usedCols ^= bit;
            usedD1 ^= d1;
            usedD2 ^= d2;
            solution[chosenRow] = -1;
        }
    }

    vector<vector<int>> run() {
        if (!valid) return {};
        int remaining = 0;
        for (int c : solution) remaining += (c == -1);
        dfs(remaining);
        return move(answers);
    }
};

vector<vector<int>> solvePreplaced(
    int n, const vector<pair<int, int>>& preplaced) {
    if (n < 1 || n > 12) return {};
    return PreplacedMRV(n, preplaced).run();
}
