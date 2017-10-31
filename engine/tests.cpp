#include "tests.h"
#include "aiplayer.h"
#include <functional>
#include <string>
#include <utility>
#include <iostream>

#include <limits.h>

#include "gtest/gtest.h"
using namespace std;
Tests::Tests()
{

}

bool Tests::RunAll()
{
    typedef pair<function<bool()>, string > Handler ;
    Handler tests[] = {
        {bind(&Tests::TestEvaluation, *this), "Test evalutation"},
        {bind(&Tests::TestEvaluation, *this), "Test checkmate in 3 halfmoves"}
    };
    for (auto test: tests) {
        if (! test.first()) {
            cout << test.second << " FAILED!!!" << endl;
            return false;
        }
    }

    return true;
}

bool Tests::TestEvaluation()
{
    // same positions but opposite colors
    const char * positions[] = {
        "8/8/3K4/8/3k4/5r2/8/8 b - -",
        "8/8/3k4/8/3K4/5R2/8/8 w - -",
    };
    for (const char * position: positions) {
        board.loadFEN(position);
        AIPlayer player(board.next_move_color, 3);
        Move move;
        bool found = player.getMove(board, move);
        if (NOT found) return false;
        if (NOT (move.to == F6 && move.from == F3)) {
            return false;
        }
    }
    return true;

}
bool Tests::TestCheckMateIn3HalfMoves()
{
    // same positions but opposite colors
    const char * positions[] = {
        "6k1/8/6K1/8/1B6/8/8/3B4 w - -",
        "6K1/8/6k1/8/1b6/8/8/3b4 b - -",
    };
    for (const char * position: positions) {
        board.loadFEN(position);

        AIPlayer player(board.next_move_color, 4);
        Move move = EMPTY_MOVE;
        bool found = player.getMove(board, move);
        if (NOT found) return false;
        if (NOT (move.to == B3 && move.from == D1)) {
            board.print(move);
            return false;
        }
    }
    return true;

}

TEST(TestCheckMateIn3HalfMoves, Positive)
{
    Tests tests;
    EXPECT_TRUE(tests.TestCheckMateIn3HalfMoves());
}
TEST(TestEvaluation, Positive)
{
    Tests tests;
    EXPECT_TRUE(tests.TestEvaluation());
}
