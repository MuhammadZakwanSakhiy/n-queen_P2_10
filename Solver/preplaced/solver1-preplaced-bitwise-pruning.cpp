#include <vector>
#include <utility>
#include <string>
#include "nqueens.h"

using namespace std;

class PreplacedBitwiseSolver {
public:
    int n;
    unsigned int full;
    unsigned int rowForbidden[16];
    int forcedCol[16];
    int position[16];
    vector<vector<int>> answers;
    bool valid;

    PreplacedBitwiseSolver(int size, const vector<pair<int, int>>& queens) : n(size) {
        full = (1u << n) - 1u;
        valid = true;

        for (int r = 0; r < n; ++r) {
            rowForbidden[r] = 0;
            forcedCol[r] = -1;
            position[r] = 0;
        }

        // 1. Initial validation & conflict detection among preplaced queens
        unsigned int usedCols = 0, usedD1 = 0, usedD2 = 0;
        for (auto [row, col] : queens) {
            if (row < 0 || row >= n || col < 0 || col >= n) {
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
        }

        // 2. Precompute static ray attack masks across all rows
        for (auto [pr, pc] : queens) {
            for (int r = 0; r < n; ++r) {
                if (r == pr) continue;
                // Vertical column attack
                rowForbidden[r] |= (1u << pc);
                // Main diagonal attack (\)
                int c1 = pc + (r - pr);
                if (c1 >= 0 && c1 < n) rowForbidden[r] |= (1u << c1);
                // Anti-diagonal attack (/)
                int c2 = pc - (r - pr);
                if (c2 >= 0 && c2 < n) rowForbidden[r] |= (1u << c2);
            }
        }

        // 3. Static forward check: any unforced row with 0 available spots means no solution
        for (int r = 0; r < n; ++r) {
            if (forcedCol[r] == -1 && (full & ~rowForbidden[r]) == 0) {
                valid = false;
                return;
            }
        }

        answers.reserve(32);
    }

    void dfs(int row, unsigned int columns, unsigned int diagLeft, unsigned int diagRight) {
        if (row == n) {
            answers.emplace_back(position, position + n);
            return;
        }

        // If this row has a preplaced queen, check only that single column
        int fc = forcedCol[row];
        if (fc != -1) {
            unsigned int bit = 1u << fc;
            if (!((columns | diagLeft | diagRight) & bit)) {
                position[row] = fc;
                dfs(row + 1, columns | bit, (diagLeft | bit) << 1, (diagRight | bit) >> 1);
            }
            return;
        }

        // Free row: filter columns using dynamic masks AND precomputed static forbidden mask
        unsigned int available = full & ~(columns | diagLeft | diagRight | rowForbidden[row]);

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
