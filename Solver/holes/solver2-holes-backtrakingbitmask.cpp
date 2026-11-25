#include <vector>
#include <string>

using namespace std;

class HolesBitmask {
public:
    int n;
    unsigned int full;

    vector<unsigned int> allowed;
    vector<int> position;
    vector<vector<int>> answers;

    explicit HolesBitmask(int size) : n(size) {
        full = (1u << n) - 1u;
        allowed.resize(n, full);
        position.resize(n, -1);
    }

    void dfs(int row,
             unsigned int columns,
             unsigned int diagLeft,
             unsigned int diagRight) {

        if (row == n) {
            answers.push_back(position);
            return;
        }

        unsigned int available =
            allowed[row] &
            ~(columns | diagLeft | diagRight);

        while (available != 0) {

            unsigned int bit =
                available & (0u - available);

            available -= bit;

            position[row] = __builtin_ctz(bit);

            dfs(
                row + 1,
                columns | bit,
                (diagLeft | bit) << 1,
                (diagRight | bit) >> 1
            );
        }
    }

    vector<vector<int>> run(
        const vector<string>& grid) {

        if ((int)grid.size() != n)
            return {};

        for (int r = 0; r < n; ++r) {

            if ((int)grid[r].size() != n)
                return {};

            for (int c = 0; c < n; ++c) {

                if (grid[r][c] == 'H')
                    allowed[r] &= ~(1u << c);
            }

            if (allowed[r] == 0)
                return {};
        }

        dfs(0, 0, 0, 0);

        return answers;
    }
};

vector<vector<int>> solveHoles(
    int n, const vector<string>& grid) {

    if (n < 1 || n > 12)
        return {};

    return HolesBitmask(n).run(grid);
}