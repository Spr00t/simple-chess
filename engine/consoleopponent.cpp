#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include "consoleopponent.h"
#include "chessboard.h"

using namespace boost;
using namespace boost::process;
using namespace std;

ConsoleOpponent::ConsoleOpponent(const std::string &path, int color)
    : ChessPlayer(color),
      color(color)
{
    if (color == WHITE) {
        process = child(path, "--slave", color == WHITE ? "white" : "black", std_out > pipe_out, std_in < pipe_in);
    } else {
        process = child(path, "--slave", color == WHITE ? "white" : "black", std_out > pipe_out, std_in < pipe_in);
        //process = child("valgrind",  "--tool=callgrind", path, "--slave", color == WHITE ? "white" : "black", std_out > pipe_out, std_in < pipe_in);
    }
}

bool ConsoleOpponent::getMove(ChessBoard &board, Move &move) const
{
    string line;
    while (pipe_out && std::getline(pipe_out, line) && line.empty())
    {
        // skip empty line
    }
    if (!line.empty()) {
        string lineIn = line, lineOut="    ";
        copy_if(lineIn.begin(), lineIn.end(), lineOut.begin(),
            [](char c ) {
                return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
            });

        if (lineIn != lineOut) {
            Global::instance().log("Gotcha");
        }

        processInput(line, move);
        move.figure = board.square[move.from];
        return true;
    }
    return true;
}


bool ConsoleOpponent::showMove(ChessBoard &board, Move &move)
{
    pipe_in << move.toString() << endl;
    return true;
}

bool ConsoleOpponent::processInput(const string& buf, Move & move) const
{
    optional<Move> opt = Move::fromString(buf);
    if (opt) {
         move = *opt;
         return true;
    }

    return false;
}
