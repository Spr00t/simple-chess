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
#include "config.h"

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

    AdvancedMoveData advaced;
    ChessBoard board;
    list<Move> regulars, nulls;
    Move move;
    bool found;

    Config config = Config::from_start_color(WHITE);

    // Initialize players
    ConsoleOpponent white(competitors[0], &config, WHITE);
    ConsoleOpponent black(competitors[1], &config, BLACK);

    // setup board
    board.initDefaultSetup();
    Move last_move = EMPTY_MOVE;
    for(;;) {
        // show board
        board.print(last_move);

        // query player's choice
        if(board.next_move_color == WHITE) {
            found = white.getMove(board, move, &advaced);
            black.showMove(board, move);
        }
        else {
            found = black.getMove(board, move, &advaced);
            white.showMove(board, move);
        }

        if(!found)
            break;

        if (NOT board.isValidMove(board.next_move_color, move)) {
            cout << "Invalid move " << move.toString() << endl;
            return 0;
        }

        // if player has a move get all moves
        regulars.clear();
        nulls.clear();
        MoveGenerator<false>::getMoves(board, board.next_move_color, regulars, regulars);

        // execute maintenance moves
        for(list<Move>::iterator it = nulls.begin(); it != nulls.end(); ++it)
            board.move(*it);

        // execute move
        board.move(move);
        last_move = move;
        move.print();

        ChessPlayer::Status status = board.getPlayerStatus(board.next_move_color);

        switch(status)
        {
            case ChessPlayer::Checkmate:
                board.print(last_move);
                cout << "Checkmate: " << (board.next_move_color == WHITE ? "white" : "black") << " are defeated" << endl;
                return 0;
            case ChessPlayer::Stalemate:
                board.print(last_move);
                cout << "Stalemate: on " << (board.next_move_color == WHITE ? "white" : "black") << " turn" << endl;
                return 0;
            case ChessPlayer::Draw:
                board.print(last_move);
                cout << "50 moves end game: on " << (board.next_move_color == WHITE ? "white" : "black") << " turn" << endl;
                return 0;

            default:
                continue;
        }

    }

    return 0;
}

