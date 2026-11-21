#include <iostream>
#include <vector>

using namespace std;

class NormalSolver {
public:
    int n;
    unsigned int full;
    vector<int> position;
    vector<vector<int>> answers;

    NormalSolver(int n) {
        this->n = n;
        full = (1u << n) - 1u;
        position.resize(n, 0);
    }

    void dfs(int row, unsigned int columns,
             unsigned int diagLeft, unsigned int diagRight) {

        if (row == n) {
            answers.push_back(position);
            return;
        }

        unsigned int available = full &
            ~(columns | diagLeft | diagRight);

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
        dfs(0, 0, 0, 0);
        return answers;
    }
};

vector<vector<int>> solveNQueens(int n) {
    if (n < 1 || n > 12) {
        return {};
    }

    NormalSolver solver(n);
    return solver.run();
}




