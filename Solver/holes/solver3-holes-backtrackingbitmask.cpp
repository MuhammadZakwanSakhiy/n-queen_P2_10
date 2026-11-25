#include <vector>
#include <string>
#include <utility>
#include "nqueens.h"

using namespace std;

class HolesOptimized {
private:
    int n;

    unsigned int halfMask;
    unsigned int middleBit;

    bool transposed = false;

    vector<unsigned int> allowed;
    vector<int> position;
    vector<vector<int>> answers;

    enum class State {
        FULL,       // Pencarian biasa
        PENDING,    // Simetri belum dipecahkan
        PAIR        // Bangkitkan pasangan refleksi
    };

    // Menyimpan solusi asli atau refleksi
    void saveSolution(bool mirror) {

        if (!transposed && !mirror) {
            answers.push_back(position);
            return;
        }

        answers.emplace_back(n);
        auto& result = answers.back();

        for (int r = 0; r < n; ++r) {

            int c = position[r];

            if (mirror)
                c = n - 1 - c;

            if (transposed)
                result[c] = r;
            else
                result[r] = c;
        }
    }

    void dfs(
        int row,
        unsigned int columns,
        unsigned int diagLeft,
        unsigned int diagRight,
        State state
    ) {

        // Base case
        if (row == n) {
            saveSolution(false);

            if (state == State::PAIR)
                saveSolution(true);

            return;
        }

        // Kandidat yang valid
        unsigned int available =
            allowed[row] &
            ~(columns | diagLeft | diagRight);

        // Symmetry reduction
        if (state == State::PENDING)
            available &= halfMask;

        while (available != 0) {

            // Ambil bit aktif paling kanan
            unsigned int bit =
                available & (0u - available);

            available -= bit;

            position[row] = __builtin_ctz(bit);

            State nextState = state;

            // Tangani simetri, termasuk N ganjil
            if (state == State::PENDING) {

                if (bit != middleBit)
                    nextState = State::PAIR;
            }

            dfs(
                row + 1,
                columns | bit,
                (diagLeft | bit) << 1,
                (diagRight | bit) >> 1,
                nextState
            );
        }
    }

public:

    explicit HolesOptimized(int size)
        : n(size),
          halfMask((1u << ((size + 1) / 2)) - 1u),
          middleBit(size % 2 ? 1u << (size / 2) : 0u),
          allowed(size, 0),
          position(size, -1) {}

    vector<vector<int>> run(
        const vector<string>& grid
    ) {

        // Validasi ukuran papan
        if ((int)grid.size() != n)
            return {};

        for (int r = 0; r < n; ++r)
            if ((int)grid[r].size() != n)
                return {};

        vector<unsigned int> columnAllowed(n, 0);

        bool vertical = true;
        bool horizontal = true;

        // Preprocessing
        for (int r = 0; r < n; ++r) {

            for (int c = 0; c < n; ++c) {

                bool hole = grid[r][c] == 'H';

                if (!hole) {
                    allowed[r] |= 1u << c;
                    columnAllowed[c] |= 1u << r;
                }

                // Deteksi simetri vertikal
                if (hole !=
                    (grid[r][n - 1 - c] == 'H'))
                    vertical = false;

                // Deteksi simetri horizontal
                if (hole !=
                    (grid[n - 1 - r][c] == 'H'))
                    horizontal = false;
            }
        }

        // Early termination
        for (int i = 0; i < n; ++i) {

            if (allowed[i] == 0 ||
                columnAllowed[i] == 0)
                return {};
        }

        // Jika hanya simetris horizontal,
        // transpose representasi papan
        if (!vertical && horizontal) {

            allowed.swap(columnAllowed);

            transposed = true;
        }

        // Tentukan strategi pencarian
        State initialState =
            (vertical || horizontal)
                ? State::PENDING
                : State::FULL;

        dfs(0, 0, 0, 0, initialState);

        return move(answers);
    }
};

vector<vector<int>> solveHoles(
    int n,
    const vector<string>& grid
) {

    if (n < 1 || n > 12)
        return {};

    return HolesOptimized(n).run(grid);
}