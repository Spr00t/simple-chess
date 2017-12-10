#pragma once
#include "chessboard.h"
class Tests
{
public:
    Tests();
    bool RunAll();
    bool TestEvaluation();
    bool TestCheckMateIn3HalfMoves();
    void TestFenNegative();
    void CheckPassant();
    void TestFen();
    void TestAdvanced();

    ChessBoard board;
};
