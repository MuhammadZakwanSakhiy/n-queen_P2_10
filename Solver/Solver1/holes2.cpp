#include <vector>
#include <string>
#include <utility>
#include "nqueens.h"

using namespace std;

class HolesDLX {
public:
    struct Node {
        int left, right, up, down, column, rowId;
    };
    int n, primary, total;
    vector<Node> nodes;
    vector<int> columnSize;
    vector<pair<int,int>> cell;
    vector<int> solution;
    vector<vector<int>> answers;

    explicit HolesDLX(int size) : n(size), primary(2 * size),
                                  total(6 * size - 2),
                                  columnSize(total + 1, 0), solution(size, -1) {
        nodes.resize(total + 1);
        nodes[0] = {primary, primary ? 1 : 0, 0, 0, 0, -1};
        for (int c = 1; c <= total; ++c) {
            nodes[c] = {c, c, c, c, c, -1};
            if (c <= primary) {
                nodes[c].left = c - 1;
                nodes[c].right = (c == primary ? 0 : c + 1);
            }
        }
    }

    void addRow(int r, int c) {
        int rowId = (int)cell.size();
        cell.push_back({r,c});
        int names[4] = {
            1 + r,
            1 + n + c,
            1 + 2*n + (r - c + n - 1),
            1 + 2*n + (2*n - 1) + r + c
        };
        int first = -1;
        int previous = -1;
        for (int col : names) {
            int k = (int)nodes.size();
            int above = nodes[col].up;
            nodes.push_back({k, k, above, col, col, rowId});
            nodes[above].down = k;
            nodes[col].up = k;
            ++columnSize[col];
            if (first == -1) first = k;
            else {
                nodes[k].left = previous;
                nodes[previous].right = k;
            }
            previous = k;
        }
        nodes[first].left = previous;
        nodes[previous].right = first;
    }

    void cover(int c) {
        nodes[nodes[c].right].left = nodes[c].left;
        nodes[nodes[c].left].right = nodes[c].right;
        for (int i = nodes[c].down; i != c; i = nodes[i].down) {
            for (int j = nodes[i].right; j != i; j = nodes[j].right) {
                nodes[nodes[j].down].up = nodes[j].up;
                nodes[nodes[j].up].down = nodes[j].down;
                --columnSize[nodes[j].column];
            }
        }
    }

    void uncover(int c) {
        for (int i = nodes[c].up; i != c; i = nodes[i].up) {
            for (int j = nodes[i].left; j != i; j = nodes[j].left) {
                ++columnSize[nodes[j].column];
                nodes[nodes[j].down].up = j;
                nodes[nodes[j].up].down = j;
            }
        }
        nodes[nodes[c].right].left = c;
        nodes[nodes[c].left].right = c;
    }

    void dfs() {
        if (nodes[0].right == 0) {
            answers.push_back(solution);
            return;
        }
        int chosen = -1, minimum = n + 1;
        for (int c = nodes[0].right; c != 0; c = nodes[c].right) {
            if (columnSize[c] < minimum) {
                chosen = c;
                minimum = columnSize[c];
            }
        }
        if (minimum == 0) return;
        cover(chosen);
        for (int i = nodes[chosen].down; i != chosen; i = nodes[i].down) {
            auto [r,c] = cell[nodes[i].rowId];
            solution[r] = c;
            for (int j = nodes[i].right; j != i; j = nodes[j].right)
                cover(nodes[j].column);
            dfs();
            for (int j = nodes[i].left; j != i; j = nodes[j].left)
                uncover(nodes[j].column);
            solution[r] = -1;
        }
        uncover(chosen);
    }

    vector<vector<int>> run(const vector<string>& grid) {
        if ((int)grid.size() != n) return {};
        for (int r = 0; r < n; ++r) {
            if ((int)grid[r].size() != n) return {};
            for (int c = 0; c < n; ++c)
                if (grid[r][c] != 'H') addRow(r,c);
        }
        dfs();
        return move(answers);
    }
};

vector<vector<int>> solveHoles(int n, const vector<string>& grid) {
    if (n < 1 || n > 12) return {};
    return HolesDLX(n).run(grid);
}
