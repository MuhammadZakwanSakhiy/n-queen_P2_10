#include <algorithm>
#include <cstdlib>
#include <bitset>
#include <string>
#include <utility>
#include <vector>
#include "nqueens.h"

using namespace std;

class WalledClique {
public:
    static constexpr int LIMIT = 144; // max 12x12
    using Bits = bitset<LIMIT>;
    using Queen = pair<int,int>;
    using Board = vector<Queen>;

    int n;
    vector<string> grid;
    vector<Queen> cells;
    vector<Bits> compatible;
    vector<int> selected;
    vector<Board> answers;
    int best = -1;

    WalledClique(int size, const vector<string>& g) : n(size), grid(g) {}

    bool attack(Queen a, Queen b) const {
        int dr = b.first - a.first;
        int dc = b.second - a.second;
        if (dr != 0 && dc != 0 && abs(dr) != abs(dc)) return false;
        if (dr == 0 && dc == 0) return false;
        int stepR = (dr > 0) - (dr < 0);
        int stepC = (dc > 0) - (dc < 0);
        for (int r = a.first + stepR, c = a.second + stepC;
             r != b.first || c != b.second;
             r += stepR, c += stepC) {
            if (grid[r][c] == 'W') return false;
        }
        return true;
    }

    void record() {
        int count = (int)selected.size();
        if (count < best) return;
        if (count > best) {
            best = count;
            answers.clear();
        }
        Board board;
        for (int v : selected) board.push_back(cells[v]);
        sort(board.begin(), board.end());
        answers.push_back(move(board));
    }

    // Greedy coloring gives an upper bound on clique size for pruning.
    void colorSort(Bits P, vector<int>& order, vector<int>& bounds) const {
        int color = 0;
        while (P.any()) {
            ++color;
            Bits available = P;
            for (int v = 0; v < (int)cells.size(); ++v) {
                if (!available[v]) continue;
                order.push_back(v);
                bounds.push_back(color);
                P.reset(v);
                available.reset(v);
                available &= ~compatible[v];
            }
        }
    }

    void dfs(Bits P) {
        if (P.none()) {
            record();
            return;
        }
        vector<int> order, bounds;
        colorSort(P, order, bounds);
        for (int i = (int)order.size() - 1; i >= 0; --i) {
            if ((int)selected.size() + bounds[i] < best) break;
            int v = order[i];
            selected.push_back(v);
            Bits next = P & compatible[v];
            if (next.any()) dfs(next);
            else record();
            selected.pop_back();
            P.reset(v);
        }
    }

    vector<Board> run() {
        if ((int)grid.size() != n) return {};
        for (const auto& row : grid)
            if ((int)row.size() != n) return {};
        for (int r = 0; r < n; ++r)
            for (int c = 0; c < n; ++c)
                if (grid[r][c] != 'W') cells.push_back({r,c});

        compatible.resize(cells.size());
        for (int i = 0; i < (int)cells.size(); ++i)
            for (int j = i + 1; j < (int)cells.size(); ++j)
                if (!attack(cells[i], cells[j])) {
                    compatible[i].set(j);
                    compatible[j].set(i);
                }
        Bits all;
        for (int i = 0; i < (int)cells.size(); ++i) all.set(i);
        dfs(all);
        return move(answers);
    }
};

vector<vector<pair<int,int>>> solveWalled(int n, const vector<string>& grid) {
    if (n < 1 || n > 12) return {};
    return WalledClique(n, grid).run();
}
