#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include "consoleopponent.h"
#include "chessboard.h"
using namespace boost::process;
using namespace std;

ConsoleOpponent::ConsoleOpponent(const std::string &path, int color)
    : ChessPlayer(color),
      color(color),
      process(path, "--slave", color == WHITE ? "white" : "black", std_out > pipe_out, std_in < pipe_in)
{

}

bool ConsoleOpponent::getMove(ChessBoard &board, Move &move) const
{
    string line;
    while (pipe_out && std::getline(pipe_out, line) && line.empty())
    {
        // skip empty line
    }
    if (!line.empty()) {
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
    int i = 0, j, l, n;


    if(strncmp(&buf[i], "quit", 4) == 0)
        exit(0);

    // convert from sth. like "b1c3"
    for(j = 0; j < 2; j++) {

        l = buf[i++];
        n = buf[i++];
        if(l >= 'a' && l <= 'h') {
            l = l - 'a';
        }
        else if(l >= 'A' && l <= 'H') {
            l = l - 'A';
        }
        else {
            return false;
        }
        if(n >= '1' && n <= '8') {
            n = n - '1';
        }
        else {
            return false;
        }
        if(j == 0)
            move.from = n * 8 + l;
        else
            move.to = n * 8 + l;
    }

    return true;
}
