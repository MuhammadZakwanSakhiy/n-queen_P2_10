#include <cassert>
#include <iostream>
#include "nqueens.h"
using namespace std;
int main() {
    assert(solveNQueens(4).size() == 2);
    assert(solveNQueens(8).size() == 92);
    assert(solveNQueens(12).size() == 14200);
    assert(solvePreplaced(4, {{0,1}}).size() == 1);
    assert(solvePreplaced(4, {{0,1},{1,1}}).empty());
    assert(solveHoles(4, {"....","....","....","...."}).size() == 2);
    assert(solveHoles(4, {"HHHH","....","....","...."}).empty());
    assert(solveWalled(4, {"....","....","....","...."}).size() == 2);
    auto w = solveWalled(2, {"WW","WW"});
    assert(w.size() == 1 && w[0].empty());
    cout << "All local tests passed.\n";
}
