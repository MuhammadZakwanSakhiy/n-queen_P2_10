#include <algorithm>
#include <vector>
#include <string>
#include <utility>
#include "nqueens.h"

using namespace std;

using Queen = pair<int, int>;
using Board = vector<Queen>;

struct Cell {
    int row, col;
    int v, downRight, downLeft;
};

class WalledSolver {
public:
    int n;
    const vector<string>& grid;
    int countV = 0, countD1 = 0, countD2 = 0;
    int best = -1;

    vector<vector<Cell>> horizontal;
    vector<char> usedV, usedD1, usedD2;
    Board current;
    vector<Board> answers;

    using IndexGrid = vector<vector<int>>;

    WalledSolver(int n, const vector<string>& grid)
        : n(n), grid(grid) {}

    int numberSegments(int dr, int dc, IndexGrid& id) {
        int total = 0;

        for (int r = 0; r < n; r++) {
            for (int c = 0; c < n; c++) {
                if (grid[r][c] == 'W') continue;

                int prevR = r - dr;
                int prevC = c - dc;

                if (prevR >= 0 && prevR < n &&
                    prevC >= 0 && prevC < n &&
                    grid[prevR][prevC] != 'W') {
                    continue;
                }

                for (int rr = r, cc = c;
                     rr >= 0 && rr < n &&
                     cc >= 0 && cc < n &&
                     grid[rr][cc] != 'W';
                     rr += dr, cc += dc) {

                    id[rr][cc] = total;
                }

                total++;
            }
        }

        return total;
    }

    void buildSegments() {
        IndexGrid h(n, vector<int>(n, -1));
        IndexGrid v(n, vector<int>(n, -1));
        IndexGrid d1(n, vector<int>(n, -1));
        IndexGrid d2(n, vector<int>(n, -1));

        int hCount = numberSegments(0, 1, h);
        countV = numberSegments(1, 0, v);
        countD1 = numberSegments(1, 1, d1);
        countD2 = numberSegments(1, -1, d2);

        horizontal.assign(hCount, {});

        for (int r = 0; r < n; r++) {
            for (int c = 0; c < n; c++) {
                if (grid[r][c] != 'W') {
                    horizontal[h[r][c]].push_back({
                        r, c, v[r][c], d1[r][c], d2[r][c]
                    });
                }
            }
        }
    }

    void dfs(int index, int usedVCount,
             int usedD1Count, int usedD2Count) {

        int remainH = (int)horizontal.size() - index;

        int upper = (int)current.size() +
            min({remainH,
                 countV - usedVCount,
                 countD1 - usedD1Count,
                 countD2 - usedD2Count});

        if (upper < best) return;

        if (index == (int)horizontal.size()) {
            int count = (int)current.size();

            if (count > best) {
                best = count;
                answers.clear();
            }

            if (count == best) {
                answers.push_back(current);
            }
            return;
        }

        for (const Cell& cell : horizontal[index]) {
            if (usedV[cell.v] ||
                usedD1[cell.downRight] ||
                usedD2[cell.downLeft]) {
                continue;
            }

            usedV[cell.v] = 1;
            usedD1[cell.downRight] = 1;
            usedD2[cell.downLeft] = 1;

            current.push_back({cell.row, cell.col});

            dfs(index + 1,
                usedVCount + 1,
                usedD1Count + 1,
                usedD2Count + 1);

            current.pop_back();

            usedV[cell.v] = 0;
            usedD1[cell.downRight] = 0;
            usedD2[cell.downLeft] = 0;
        }

        dfs(index + 1, usedVCount,
            usedD1Count, usedD2Count);
    }

    vector<Board> run() {
        if ((int)grid.size() != n) return {};

        for (const string& row : grid) {
            if ((int)row.size() != n) return {};
        }

        buildSegments();

        usedV.assign(countV, 0);
        usedD1.assign(countD1, 0);
        usedD2.assign(countD2, 0);

        sort(horizontal.begin(), horizontal.end(),
            [](const vector<Cell>& a,
               const vector<Cell>& b) {
                return a.size() < b.size();
            });

        dfs(0, 0, 0, 0);
        return answers;
    }
};

vector<vector<pair<int, int>>> solveWalled(
    int n, const vector<string>& grid) {

    if (n < 1 || n > 12) return {};

    WalledSolver solver(n, grid);
    return solver.run();
}



