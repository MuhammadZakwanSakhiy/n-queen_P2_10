#include <vector>
#include <utility>
#include "nqueens.h"

using namespace std;

class PreplacedSolver {
public:
    int n;
    unsigned int full;
    vector<int> forced;
    vector<int> position;
    vector<vector<int>> answers;
    bool valid;

    PreplacedSolver(int n, const vector<pair<int, int>>& queens) {
        this->n = n;
        full = (1u << n) - 1u;
        forced.assign(n, -1);
        position.resize(n, 0);
        valid = true;

        for (auto [row, col] : queens) {
            if (row < 0 || row >= n || col < 0 || col >= n) {
                valid = false;
                break;
            }

            if (forced[row] != -1 && forced[row] != col) {
                valid = false;
                break;
            }

            forced[row] = col;
        }
    }

    void dfs(int row, unsigned int columns,
             unsigned int diagLeft, unsigned int diagRight) {

        if (row == n) {
            answers.push_back(position);
            return;
        }

        unsigned int available = full &
            ~(columns | diagLeft | diagRight);

        if (forced[row] != -1) {
            available &= (1u << forced[row]);
        }

        while (available != 0) {
            unsigned int bit = available & (0u - available);
            available -= bit;

            position[row] = __builtin_ctz(bit);

            dfs(row + 1,
                columns | bit,
                (diagLeft | bit) << 1,
                (diagRight | bit) >> 1);
        }
    }

    vector<vector<int>> run() {
        if (valid) {
            dfs(0, 0, 0, 0);
        }
        return answers;
    }
};

vector<vector<int>> solvePreplaced(
    int n, const vector<pair<int, int>>& preplaced) {

    if (n < 1 || n > 12) {
        return {};
    }

    PreplacedSolver solver(n, preplaced);
    return solver.run();
}








