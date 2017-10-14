#include "playchamp.h"

//#include <mcheck.h>
#include <cstdlib>
#include <cstdio>
#include <list>
#include <boost/process.hpp>
#include <string>
#include "chessboard.h"
#include "humanplayer.h"
#include "aiplayer.h"
#include "consoleopponent.h"

using namespace std;
#include <thread>
#include <iostream>


int PlayChamp::Run(int argc, char *argv[])
{
    const string ARG = "--master";
    bool master_mode = false;
    if (argc > 2 && ARG == argv[1] ) {
        master_mode = true;
    }
    string competitors[2];
    if (argc >= 4) {
        competitors[0] = argv[2];
        competitors[1] = argv[3];


    }

    ChessBoard board;
    list<Move> regulars, nulls;
    int turn = WHITE;
    Move move;
    bool found;

    // Initialize players
    ConsoleOpponent white(competitors[0], WHITE);
    ConsoleOpponent black(competitors[1], BLACK);

    // setup board
    board.initDefaultSetup();

    for(;;) {
        // show board
        board.print();

        // query player's choice
        if(turn == WHITE) {
            found = white.getMove(board, move);
            black.showMove(board, move);
        }
        else {
            found = black.getMove(board, move);
            white.showMove(board, move);
        }

        if(!found)
            break;

        if (NOT board.isValidMove(turn, move)) {
            cout << "Invalid move " << move.toString() << endl;
            return 0;
        }

        // if player has a move get all moves
        regulars.clear();
        nulls.clear();
        board.getMoves(turn, regulars, regulars, nulls);

        // execute maintenance moves
        for(list<Move>::iterator it = nulls.begin(); it != nulls.end(); ++it)
            board.move(*it);

        // execute move
        board.move(move);
        move.print();

        // opponents turn
        turn = TOGGLE_COLOR(turn);

        ChessPlayer::Status status = board.getPlayerStatus(turn);

        switch(status)
        {
            case ChessPlayer::Checkmate:
                board.print();
                cout << "Checkmate: " << (turn == WHITE ? "white" : "black") << " are defeated" << endl;
                return 0;
            case ChessPlayer::Stalemate:
                board.print();
                cout << "Stalemate: " << (turn == WHITE ? "white" : "black") << " are defeated" << endl;

                return 0;
            default:
                continue;
        }

    }

    return 0;
}

