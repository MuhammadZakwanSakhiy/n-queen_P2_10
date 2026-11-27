#include <algorithm>
#include <utility>
#include <vector>
#include "nqueens.h"

using namespace std;

class PreplacedBitwiseSolver {
public:
    int n;
    unsigned int full;
    unsigned int allowed[16];
    int nextFree[16];
    int stepShift[16];
    int position[16];
    int firstFree;
    int lastFree;
    int lastForced;
    vector<vector<int>> answers;
    bool flipped;
    bool valid;
    bool isEmpty;

    PreplacedBitwiseSolver(int size, const vector<pair<int, int>>& queens)
        : n(size), firstFree(-1), lastFree(-1), lastForced(-1),
          flipped(false), valid(true), isEmpty(queens.empty()) {
        full = (1u << n) - 1u;
        if (isEmpty) return;

        int forcedCol[16];
        for (int r = 0; r < n; ++r) forcedCol[r] = -1;

        // 1. Validate coordinates and detect conflicts among preplaced queens
        unsigned int usedCols = 0, usedD1 = 0, usedD2 = 0;
        for (const auto& q : queens) {
            int row = q.first, col = q.second;
            if ((unsigned int)row >= (unsigned int)n || (unsigned int)col >= (unsigned int)n) {
                valid = false;
                return;
            }
            if (forcedCol[row] != -1) {
                if (forcedCol[row] != col) {
                    valid = false;
                    return;
                }
                continue; // duplicate coordinate
            }

            unsigned int colBit = 1u << col;
            unsigned int d1Bit = 1u << (row - col + n - 1);
            unsigned int d2Bit = 1u << (row + col);
            if ((usedCols & colBit) || (usedD1 & d1Bit) || (usedD2 & d2Bit)) {
                valid = false;
                return;
            }

            usedCols |= colBit;
            usedD1 |= d1Bit;
            usedD2 |= d2Bit;
            forcedCol[row] = col;
        }

        // 2. Flip board vertically if forced queens sit closer to the bottom edge,
        //    so tighter constraints prune at the top of the search tree.
        int topScore = 0, botScore = 0;
        for (int r = 0; r < n; ++r) {
            if (forcedCol[r] != -1) {
                topScore += r;
                botScore += (n - 1 - r);
            }
        }
        flipped = (botScore < topScore);
        if (flipped) {
            for (int l = 0, r = n - 1; l < r; ++l, --r) {
                int tmp = forcedCol[l];
                forcedCol[l] = forcedCol[r];
                forcedCol[r] = tmp;
            }
        }

        int numFree = 0;
        for (int r = 0; r < n; ++r) {
            if (forcedCol[r] != -1) {
                position[r] = forcedCol[r];
                lastForced = r;
            } else {
                if (firstFree == -1) firstFree = r;
                lastFree = r;
                ++numFree;
            }
            allowed[r] = full & ~usedCols;
        }

        // 3. Precompute static diagonal masks from preplaced queens into allowed[r]
        for (int pr = 0; pr < n; ++pr) {
            int pc = forcedCol[pr];
            if (pc == -1) continue;
            unsigned int bit = 1u << pc;
            for (int r = 0; r < n; ++r) {
                if (r == pr) continue;
                int d = r > pr ? r - pr : pr - r;
                allowed[r] &= ~((bit << d) | (bit >> d));
            }
        }

        // 4. Static forward check + build jump table over forced rows
        int prev = -1;
        for (int r = 0; r < n; ++r) {
            if (forcedCol[r] == -1) {
                if (allowed[r] == 0) {
                    valid = false;
                    return;
                }
                if (prev != -1) {
                    nextFree[prev] = r;
                    stepShift[prev] = r - prev;
                }
                prev = r;
            }
        }

        if (numFree >= 8) answers.reserve(64);
        else if (n >= 9 && numFree >= 6) answers.reserve(16);
    }

    // Mirror-symmetry search when no queens are preplaced
    void dfsSym(int row, unsigned int cols, unsigned int dl, unsigned int dr, bool reflect) {
        unsigned int av = full & ~(cols | dl | dr);
        if (row == n - 1) {
            if (av) {
                position[row] = __builtin_ctz(av);
                answers.emplace_back(position, position + n);
                if (reflect) {
                    answers.emplace_back(n);
                    int* dst = answers.back().data();
                    for (int r = 0; r < n; ++r) dst[r] = n - 1 - position[r];
                }
            }
            return;
        }
        while (av) {
            unsigned int bit = av & (0u - av);
            av -= bit;
            position[row] = __builtin_ctz(bit);
            dfsSym(row + 1, cols | bit, (dl | bit) << 1, (dr | bit) >> 1, reflect);
        }
    }

    // Pure consecutive-row DFS once past the last preplaced queen (constant 1-bit shifts)
    void dfsPure(int row, unsigned int cols, unsigned int dl, unsigned int dr) {
        unsigned int av = allowed[row] & ~(cols | dl | dr);
        if (row == lastFree) {
            if (av) {
                position[row] = __builtin_ctz(av);
                answers.emplace_back(position, position + n);
            }
            return;
        }
        while (av) {
            unsigned int bit = av & (0u - av);
            av -= bit;
            position[row] = __builtin_ctz(bit);
            dfsPure(row + 1, cols | bit, (dl | bit) << 1, (dr | bit) >> 1);
        }
    }

    // Jump-table DFS while preplaced rows remain ahead; transitions to dfsPure once past lastForced
    void dfsJump(int row, unsigned int cols, unsigned int dl, unsigned int dr) {
        unsigned int av = allowed[row] & ~(cols | dl | dr);
        if (row == lastFree) {
            if (av) {
                position[row] = __builtin_ctz(av);
                answers.emplace_back(position, position + n);
            }
            return;
        }
        int nr = nextFree[row];
        int sh = stepShift[row];
        if (nr > lastForced) {
            while (av) {
                unsigned int bit = av & (0u - av);
                av -= bit;
                position[row] = __builtin_ctz(bit);
                dfsPure(nr, cols | bit, (dl | bit) << sh, (dr | bit) >> sh);
            }
        } else {
            while (av) {
                unsigned int bit = av & (0u - av);
                av -= bit;
                position[row] = __builtin_ctz(bit);
                dfsJump(nr, cols | bit, (dl | bit) << sh, (dr | bit) >> sh);
            }
        }
    }

    vector<vector<int>> run() {
        if (!valid) return {};
        if (isEmpty) {
            if (n == 1) {
                answers.push_back({0});
                return move(answers);
            }
            if (n >= 8) answers.reserve(64);
            for (int c = 0; c < n / 2; ++c) {
                unsigned int bit = 1u << c;
                position[0] = c;
                dfsSym(1, bit, bit << 1, bit >> 1, true);
            }
            if (n & 1) {
                unsigned int bit = 1u << (n / 2);
                position[0] = n / 2;
                dfsSym(1, bit, bit << 1, bit >> 1, false);
            }
            return move(answers);
        }

        if (firstFree == -1) {
            answers.emplace_back(position, position + n);
        } else if (firstFree > lastForced) {
            dfsPure(firstFree, 0, 0, 0);
        } else {
            dfsJump(firstFree, 0, 0, 0);
        }

        if (flipped) {
            for (auto& sol : answers) {
                reverse(sol.begin(), sol.end());
            }
        }
        return move(answers);
    }
};

vector<vector<int>> solvePreplaced(int n, const vector<pair<int, int>>& preplaced) {
    if (n < 1 || n > 12) return {};
    return PreplacedBitwiseSolver(n, preplaced).run();
}
