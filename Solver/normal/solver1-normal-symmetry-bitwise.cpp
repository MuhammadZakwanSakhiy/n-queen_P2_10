#include <vector>
#include <utility>
#include "nqueens.h"

using namespace std;

class SymmetrySolver {
public:
    int n;
    unsigned int full;
    vector<int> pos;
    vector<vector<int>> answers;
    bool reflect = false;

    explicit SymmetrySolver(int size) : n(size), full((1u << size) - 1u), pos(size) {}

    void dfs(int row, unsigned int cols, unsigned int dLeft,
             unsigned int dRight) {
        if (row == n) {
            answers.push_back(pos);
            if (reflect) {
                vector<int> mirror(n);
                for (int r = 0; r < n; ++r) mirror[r] = n - 1 - pos[r];
                answers.push_back(move(mirror));
            }
            return;
        }

        unsigned int choices = full & ~(cols | dLeft | dRight);
        while (choices) {
            unsigned int bit = choices & (0u - choices);
            choices -= bit;
            pos[row] = __builtin_ctz(bit);
            dfs(row + 1, cols | bit, (dLeft | bit) << 1,
                (dRight | bit) >> 1);
        }
    }

    vector<vector<int>> run() {
        // First-column choices in left half; mirror solutions to right half.
        reflect = true;
        for (int c = 0; c < n / 2; ++c) {
            unsigned int bit = 1u << c;
            pos[0] = c;
            dfs(1, bit, bit << 1, bit >> 1);
        }
        // Odd N: center column reflects to itself; enumerate directly.
        if (n & 1) {
            reflect = false;
            unsigned int bit = 1u << (n / 2);
            pos[0] = n / 2;
            dfs(1, bit, bit << 1, bit >> 1);
        }
        return move(answers);
    }
};

vector<vector<int>> solveNQueens(int n) {
    if (n < 1 || n > 12) return {};
    return SymmetrySolver(n).run();
}
