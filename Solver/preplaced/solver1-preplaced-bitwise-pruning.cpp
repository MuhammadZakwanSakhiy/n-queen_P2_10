#include <utility>
#include <vector>
#include "nqueens.h"

using namespace std;

class PreplacedBitwiseSolver {
public:
    int n;
    unsigned int full;
    int numFree;
    int freeRows[12];
    int s1Arr[12];
    int s2Arr[12];
    unsigned int allowed[12];
    int position[12];
    vector<vector<int>> answers;
    bool valid;
    bool isEmpty;

    PreplacedBitwiseSolver(int size, const vector<pair<int, int>>& queens)
        : n(size), numFree(0), valid(true), isEmpty(queens.empty()) {
        full = (1u << n) - 1u;
        if (isEmpty) return;

        // 1. Validate coordinates and detect conflicts among preplaced queens
        unsigned int usedCols = 0, usedD1 = 0, usedD2 = 0;
        unsigned int forcedRowMask = 0;

        for (const auto& q : queens) {
            int row = q.first, col = q.second;
            if ((unsigned int)row >= (unsigned int)n || (unsigned int)col >= (unsigned int)n) {
                valid = false;
                return;
            }
            unsigned int rowBit = 1u << row;
            if (forcedRowMask & rowBit) {
                if (position[row] != col) {
                    valid = false;
                    return;
                }
                continue; // duplicate coordinate
            }
            unsigned int colBit = 1u << col;
            unsigned int d1Bit = colBit << (n - 1 - row);
            unsigned int d2Bit = colBit << row;
            if ((usedCols & colBit) || (usedD1 & d1Bit) || (usedD2 & d2Bit)) {
                valid = false;
                return;
            }
            forcedRowMask |= rowBit;
            usedCols |= colBit;
            usedD1 |= d1Bit;
            usedD2 |= d2Bit;
            position[row] = col;
        }

        // 2. Collect free rows and precompute valid column masks against preplaced queens
        int popc[12];
        for (int r = 0; r < n; ++r) {
            if (forcedRowMask & (1u << r)) continue;
            unsigned int av = full & ~(usedCols | (usedD1 >> (n - 1 - r)) | (usedD2 >> r));
            if (!av) {
                valid = false;
                return;
            }
            freeRows[numFree] = r;
            allowed[numFree] = av;
            popc[numFree] = __builtin_popcount(av);
            ++numFree;
        }

        // 3. Static MRV + adjacency tie-breaking: place most constrained rows first,
        //    preferring rows adjacent to already chosen/preplaced rows to maximize diagonal pruning.
        unsigned int chosenMask = forcedRowMask;
        for (int i = 0; i < numFree; ++i) {
            int best = i;
            int bestScore = 100000;
            for (int j = i; j < numFree; ++j) {
                int r = freeRows[j];
                int adj = 0;
                if (r > 0 && (chosenMask & (1u << (r - 1)))) ++adj;
                if (r + 1 < n && (chosenMask & (1u << (r + 1)))) ++adj;
                int score = (popc[j] << 4) - (adj << 2);
                if (score < bestScore) {
                    bestScore = score;
                    best = j;
                }
            }
            if (best != i) {
                swap(freeRows[i], freeRows[best]);
                swap(allowed[i], allowed[best]);
                swap(popc[i], popc[best]);
            }
            chosenMask |= (1u << freeRows[i]);
            s1Arr[i] = n - 1 - freeRows[i];
            s2Arr[i] = freeRows[i];
        }
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

    // Arbitrary-order bitwise DFS over free rows only
    void dfs(int idx, unsigned int cols, unsigned int d1, unsigned int d2) {
        int r = freeRows[idx];
        unsigned int avail = allowed[idx] & ~(cols | (d1 >> s1Arr[idx]) | (d2 >> s2Arr[idx]));
        if (idx == numFree - 1) {
            if (avail) {
                position[r] = __builtin_ctz(avail);
                answers.emplace_back(position, position + n);
            }
            return;
        }
        int s1 = s1Arr[idx];
        int s2 = s2Arr[idx];
        while (avail) {
            unsigned int bit = avail & (0u - avail);
            avail -= bit;
            position[r] = __builtin_ctz(bit);
            dfs(idx + 1, cols | bit, d1 | (bit << s1), d2 | (bit << s2));
        }
    }

    vector<vector<int>> run() {
        if (!valid) return {};
        if (isEmpty) {
            if (n == 1) {
                answers.push_back({0});
                return move(answers);
            }
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
        if (numFree == 0) {
            answers.emplace_back(position, position + n);
            return move(answers);
        }
        dfs(0, 0, 0, 0);
        return move(answers);
    }
};

vector<vector<int>> solvePreplaced(int n, const vector<pair<int, int>>& preplaced) {
    if (n < 1 || n > 12) return {};
    return PreplacedBitwiseSolver(n, preplaced).run();
}
