#include <vector>
#include <utility>
#include <string>
#include <cstdlib>
#include "nqueens.h"

using namespace std;

class PreplacedBitwiseSolver {
public:
    int n;
    unsigned int full;
    unsigned int allowed[16];
    int forcedCol[16];
    int position[16];
    vector<vector<int>> answers;
    bool valid;

    PreplacedBitwiseSolver(int size, const vector<pair<int, int>>& queens) : n(size) {
        full = (1u << n) - 1u;
        valid = true;
        unsigned int rowForbidden[16];

        for (int r = 0; r < n; ++r) {
            rowForbidden[r] = 0;
            forcedCol[r] = -1;
            position[r] = 0;
        }

        // 1. Initial validation & conflict detection among preplaced queens
        unsigned int usedCols = 0, usedD1 = 0, usedD2 = 0;
        for (auto [row, col] : queens) {
            if ((unsigned int)row >= (unsigned int)n || (unsigned int)col >= (unsigned int)n) {
                valid = false;
                return;
            }
            if (forcedCol[row] != -1) {
                if (forcedCol[row] != col) {
                    valid = false;
                    return;
                }
                continue; // duplicate coordinate in input
            }

            unsigned int colBit = 1u << col;
            unsigned int d1Bit = 1u << (row - col + n - 1);
            unsigned int d2Bit = 1u << (row + col);

            // Preplaced queens attack each other -> immediately invalid
            if ((usedCols & colBit) || (usedD1 & d1Bit) || (usedD2 & d2Bit)) {
                valid = false;
                return;
            }

            usedCols |= colBit;
            usedD1 |= d1Bit;
            usedD2 |= d2Bit;
            forcedCol[row] = col;
            position[row] = col;
        }

        // 2. Precompute static ray attack masks across all rows
        for (int r = 0; r < n; ++r) {
            rowForbidden[r] = usedCols;
        }
        for (auto [pr, pc] : queens) {
            unsigned int bit = 1u << pc;
            for (int r = 0; r < n; ++r) {
                if (r == pr) continue;
                int d = abs(r - pr);
                rowForbidden[r] |= (bit << d) | (bit >> d);
            }
        }

        // 3. Static forward check: any unforced row with 0 available spots means no solution
        for (int r = 0; r < n; ++r) {
            allowed[r] = full & ~rowForbidden[r];
            if (forcedCol[r] == -1 && allowed[r] == 0) {
                valid = false;
                return;
            }
        }
        // Note: Do NOT reserve answers capacity to avoid inflating peak heap memory on small cases.
    }

    void dfs(int row, unsigned int columns, unsigned int diagLeft, unsigned int diagRight) {
        // Last row optimization: direct placement without deeper recursion
        if (row == n - 1) {
            int fc = forcedCol[row];
            if (fc != -1) {
                answers.emplace_back(position, position + n);
            } else {
                unsigned int available = allowed[row] & ~(columns | diagLeft | diagRight);
                if (available) {
                    position[row] = __builtin_ctz(available);
                    answers.emplace_back(position, position + n);
                }
            }
            return;
        }

        int fc = forcedCol[row];
        if (fc != -1) {
            // Preplaced queen is fixed and cannot conflict with free queens because all its
            // column and diagonal constraints are already baked into allowed[r].
            dfs(row + 1, columns, diagLeft << 1, diagRight >> 1);
            return;
        }

        unsigned int available = allowed[row] & ~(columns | diagLeft | diagRight);

        while (available) {
            unsigned int bit = available & (0u - available);
            available -= bit;

            position[row] = __builtin_ctz(bit);
            dfs(row + 1, columns | bit, (diagLeft | bit) << 1, (diagRight | bit) >> 1);
        }
    }

    vector<vector<int>> run() {
        if (valid) {
            dfs(0, 0, 0, 0);
        }
        return move(answers);
    }
};

vector<vector<int>> solvePreplaced(int n, const vector<pair<int, int>>& preplaced) {
    if (n < 1 || n > 12) return {};
    return PreplacedBitwiseSolver(n, preplaced).run();
}
