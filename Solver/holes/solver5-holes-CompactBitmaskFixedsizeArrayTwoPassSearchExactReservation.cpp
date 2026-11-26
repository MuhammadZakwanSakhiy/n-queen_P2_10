#include <vector>
#include <string>
#include <cstdint>
#include <utility>
#include "nqueens.h"

using namespace std;

class HolesLightest {
private:
    using U = unsigned int;

    int n;

    // 12 x 2 byte = 24 byte
    uint16_t allowed[12] = {};

    // 12 x 4 byte = 48 byte
    int position[12];

    vector<vector<int>> answers;

    // PASS 1:
    // Menghitung solusi tanpa menyimpan posisi
    size_t countDFS(
        int row,
        U columns,
        U diagLeft,
        U diagRight
    ) {

        U available =
            allowed[row] &
            ~(columns | diagLeft | diagRight);

        // Baris terakhir maksimal punya satu kandidat
        if (row == n - 1)
            return available != 0;

        size_t total = 0;

        while (available) {

            U bit = available & -available;

            available &= available - 1;

            total += countDFS(
                row + 1,
                columns | bit,
                (diagLeft | bit) << 1,
                (diagRight | bit) >> 1
            );
        }

        return total;
    }

    // PASS 2:
    // Menyimpan solusi
    void collectDFS(
        int row,
        U columns,
        U diagLeft,
        U diagRight
    ) {

        U available =
            allowed[row] &
            ~(columns | diagLeft | diagRight);

        if (!available)
            return;

        // Last-row optimization
        if (row == n - 1) {

            position[row] =
                __builtin_ctz(available);

            answers.emplace_back(
                position,
                position + n
            );

            return;
        }

        while (available) {

            U bit = available & -available;

            available &= available - 1;

            position[row] =
                __builtin_ctz(bit);

            collectDFS(
                row + 1,
                columns | bit,
                (diagLeft | bit) << 1,
                (diagRight | bit) >> 1
            );
        }
    }

public:

    explicit HolesLightest(int size)
        : n(size) {}

    vector<vector<int>> run(
        const vector<string>& grid
    ) {

        if ((int)grid.size() != n)
            return {};

        U full = (1u << n) - 1u;

        U columnUnion = 0;

        // Konversi holes menjadi bitmask
        for (int r = 0; r < n; ++r) {

            if ((int)grid[r].size() != n)
                return {};

            U mask = 0;

            for (int c = 0; c < n; ++c) {

                if (grid[r][c] != 'H')
                    mask |= 1u << c;
            }

            // Baris penuh holes
            if (!mask)
                return {};

            allowed[r] =
                static_cast<uint16_t>(mask);

            columnUnion |= mask;
        }

        // Kolom penuh holes
        if (columnUnion != full)
            return {};

        // Pass 1
        size_t count = countDFS(0, 0, 0, 0);

        if (!count)
            return {};

        // Alokasikan kapasitas sesuai jumlah solusi
        answers.reserve(count);

        // Pass 2
        collectDFS(0, 0, 0, 0);

        return move(answers);
    }
};

vector<vector<int>> solveHoles(
    int n,
    const vector<string>& grid
) {

    if (n < 1 || n > 12)
        return {};

    return HolesLightest(n).run(grid);
}