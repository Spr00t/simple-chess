#include <functional>
#include <string>
#include <utility>
#include <iostream>
#include <limits.h>
#include <boost/format.hpp>
#include <string>
#include <exception>

#include "gtest/gtest.h"

#include "tests.h"
#include "aiplayer.h"
#include "config.h"

using namespace std;
using namespace boost;
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
    Config config = Config::from_start_color(WHITE);
    for (const char * position: positions) {
        board.loadFEN(position);
        AIPlayer player(&config, board.next_move_color, 3);
        Move move;
        AdvancedMoveData advanced;
        bool found = player.getMove(board, move, &advanced);
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
    Config config = Config::from_start_color(WHITE);

    for (const char * position: positions) {
        board.loadFEN(position);

        AIPlayer player(&config, board.next_move_color, 4);
        Move move = EMPTY_MOVE;
        AdvancedMoveData advanced;
        bool found = player.getMove(board, move, &advanced);
        if (NOT found) return false;
        if (NOT (move.to == B3 && move.from == D1)) {
            board.print(move);
            return false;
        }
    }
    return true;

}
void Tests::TestAdvanced()
{
    board.loadFEN("5k2/5p2/8/8/8/8/1PPPPPP1/4K3 w - -");
    Config config = Config::from_start_color(WHITE);
    AdvancedMoveData advanced;
    
    AIPlayer ai(&config, WHITE, 3);
    Move move;
    ai.getMove(board, move, &advanced);
    
    EXPECT_GT(advanced.board_evaluation, 0);
    
}
void Tests::TestFenNegative()
{
    ChessBoard board;
    string fen = "rnbqkbnr/pppppppp/8/8/P7/8/1PPPPPPP/RNBQKBNR w KQkq 9 -";

    //string expected = str(format("FEN: %1% Unexpected character at pos %2%") % fen % fen.find_last_of('9') );
    //EXPECT_THROW(board.loadFEN(fen), std::runtime_error(expected));

}
void Tests::CheckPassant()
{
    string fen;

    fen = "5k2/8/8/8/5p2/8/4P3/4K3 w - -";
    board.loadFEN(fen);

    Move move = EMPTY_MOVE;
    move.from = E2;
    move.to = E4;

    move.figure = board.square[move.from];

    board.move(move);

    EXPECT_EQ(board.passant_pos, E4);
    EXPECT_TRUE(IS_PASSANT(board.square[E4]));

}
void Tests::TestFen()
{
    CheckPassant();
    string fen;

    fen = "5k2/8/8/5p2/4P3/8/8/4K3 w f6 -";
    board.loadFEN(fen);
    EXPECT_EQ(board.passant_pos, F5);

    fen = "5k2/8/8/8/4Pp2/8/8/4K3 b e3 -";
    board.loadFEN(fen);
    EXPECT_TRUE(IS_PASSANT(board.square[E4]));
    EXPECT_EQ(board.passant_pos, E4);

    Move move = EMPTY_MOVE;
    move.from = F4;
    move.to = F3;
    move.passant_pos_opponent = E4;
    move.figure = board.square[F4];

    board.move(move);

    EXPECT_EQ(board.next_move_color, WHITE);

    EXPECT_EQ(board.passant_pos, -1);
    EXPECT_FALSE(IS_PASSANT(board.square[E4]));

}

TEST(TestFen, Negative)
{
    Tests tests;
    tests.TestFenNegative();
}
TEST(TestFen, Positive)
{
    Tests tests;
    tests.TestFen();
}
TEST(TestAdvanced, Positive)
{
    Tests tests;
    tests.TestAdvanced();
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
