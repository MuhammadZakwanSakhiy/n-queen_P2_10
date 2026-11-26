#include <vector>
#include <string>
#include <utility>
#include "nqueens.h"

using namespace std;

class HolesFastest {
private:
    using U = unsigned int;

    int n;
    U full, half, middle;

    // Tidak membutuhkan alokasi heap
    U allowed[12] = {};
    int position[12];

    bool transposed = false;

    vector<vector<int>> answers;

    // Mode:
    // 0 = Full search
    // 1 = Symmetry pending
    // 2 = Generate mirrored solution

    template<bool Mirror>
    inline void save() {

        if (!transposed && !Mirror) {
            answers.emplace_back(
                position, position + n
            );
            return;
        }

        answers.emplace_back(n);

        auto& result = answers.back();

        for (int r = 0; r < n; ++r) {

            int c = position[r];

            if constexpr (Mirror)
                c = n - 1 - c;

            if (transposed)
                result[c] = r;
            else
                result[r] = c;
        }
    }

    template<int Mode>
    void dfs(
        U rows,
        U columns,
        U diag1,
        U diag2
    ) {

        int chosen = -1;
        int minimum = n + 1;

        U candidates = 0;
        U scan = rows;

        // Dynamic Minimum Remaining Values
        while (scan) {

            int r = __builtin_ctz(scan);

            scan &= scan - 1;

            U available =
                allowed[r] &
                ~(columns |
                  (diag1 >> r) |
                  (diag2 >> (n - 1 - r)));

            // Forward checking
            if (!available)
                return;

            // Forced move
            if (!(available & (available - 1))) {

                chosen = r;
                candidates = available;
                break;
            }

            int count =
                __builtin_popcount(available);

            if (count < minimum) {

                minimum = count;
                chosen = r;
                candidates = available;
            }
        }

        // Symmetry reduction
        if constexpr (Mode == 1)
            candidates &= half;

        U rest = rows & ~(1u << chosen);

        // Iterasi hanya kandidat yang valid
        while (candidates) {

            U bit = candidates & -candidates;

            candidates &= candidates - 1;

            position[chosen] =
                __builtin_ctz(bit);

            // Last-row optimization:
            // Tidak perlu membuat pemanggilan DFS baru
            if (!rest) {

                save<false>();

                if constexpr (Mode == 2)
                    save<true>();

                if constexpr (Mode == 1) {
                    if (bit != middle)
                        save<true>();
                }

                return;
            }

            U nextColumns = columns | bit;

            U nextDiag1 =
                diag1 | (bit << chosen);

            U nextDiag2 =
                diag2 |
                (bit << (n - 1 - chosen));

            // Compile-time branching
            if constexpr (Mode == 1) {

                if (bit == middle) {

                    dfs<1>(
                        rest,
                        nextColumns,
                        nextDiag1,
                        nextDiag2
                    );

                } else {

                    dfs<2>(
                        rest,
                        nextColumns,
                        nextDiag1,
                        nextDiag2
                    );
                }

            } else {

                dfs<Mode>(
                    rest,
                    nextColumns,
                    nextDiag1,
                    nextDiag2
                );
            }
        }
    }

public:

    explicit HolesFastest(int size)
        : n(size),
          full((1u << size) - 1u),
          half((1u << ((size + 1) / 2)) - 1u),
          middle(size % 2
                 ? 1u << (size / 2)
                 : 0u) {}

    vector<vector<int>> run(
        const vector<string>& grid
    ) {

        if ((int)grid.size() != n)
            return {};

        for (int r = 0; r < n; ++r)
            if ((int)grid[r].size() != n)
                return {};

        U columnAllowed[12] = {};

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

                if (hole !=
                    (grid[r][n - 1 - c] == 'H'))
                    vertical = false;

                if (hole !=
                    (grid[n - 1 - r][c] == 'H'))
                    horizontal = false;
            }
        }

        int rowMin = n + 1;
        int colMin = n + 1;

        for (int i = 0; i < n; ++i) {

            if (!allowed[i] ||
                !columnAllowed[i])
                return {};

            int a =
                __builtin_popcount(allowed[i]);

            int b =
                __builtin_popcount(columnAllowed[i]);

            if (a < rowMin) rowMin = a;
            if (b < colMin) colMin = b;
        }

        // Pilih orientasi papan
        transposed =
            (!vertical && horizontal) ||
            (vertical == horizontal &&
             colMin < rowMin);

        if (transposed) {

            for (int i = 0; i < n; ++i)
                allowed[i] = columnAllowed[i];
        }

        // Pilih versi DFS saat preprocessing
        if (vertical || horizontal)
            dfs<1>(full, 0, 0, 0);
        else
            dfs<0>(full, 0, 0, 0);

        return move(answers);
    }
};

vector<vector<int>> solveHoles(
    int n,
    const vector<string>& grid
) {

    if (n < 1 || n > 12)
        return {};

    return HolesFastest(n).run(grid);
}