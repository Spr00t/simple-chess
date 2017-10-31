#pragma once
#include "chessboard.h"
class Tests
{
public:
    Tests();
    bool RunAll();
    bool TestEvaluation();
    bool TestCheckMateIn3HalfMoves();
    ChessBoard board;
};
