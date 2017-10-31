//#include <mcheck.h>
#include <cstdlib>
#include <cstdio>
#include <list>
#include <string>
#include "chessboard.h"
#include "humanplayer.h"
#include "aiplayer.h"
#include "tests.h"
#include <boost/process.hpp>
#include <memory>
#include "playchamp.h"
#include <thread>
#include <iostream>
#include <string.h>
#include "global.h"
using namespace std;
#define NOT !

void test_fen() {

    ChessBoard board;
    //board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -");
    //board.loadFEN("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    //board.loadFEN("K7/6r1/8/8/8/8/8/k6r b - -");
    //board.loadFEN("8/3K4/8/3k4/6r1/8/8/8 b - -");
    //board.loadFEN("8/8/3k4/8/3K4/8/6R1/8 b - -");
//    board.loadFEN("6k1/R7/5K2/8/8/8/8/8 w - -");
//    board.loadFEN("8/8/5K1k/8/8/8/6R1/8 w - -");
//    board.loadFEN("8/7k/8/4K3/8/4B3/6B1/8 w - -");
    board.loadFEN("8/8/3K4/8/3k4/5r2/8/8 b - -");



    list<Move> nulls;
    Move mov;
    bool found;
    unique_ptr<ChessPlayer> black;
    unique_ptr<ChessPlayer> white;

    black = make_unique<AIPlayer>(BLACK, 3);
    white = make_unique<AIPlayer>(WHITE, 3);
    Global::instance().setColor(BLACK);

    board.print();
    while (true) {

        if(board.next_move_color == WHITE)
            found = white->getMove(board, mov);
        else
            found = black->getMove(board, mov);


        if(!found)
            break;

        // execute maintenance moves
        for(list<Move>::iterator it = nulls.begin(); it != nulls.end(); ++it)
            board.move(*it);

        // execute move
        board.move(mov);

        board.print(mov);
    }
    ChessPlayer::Status status = board.getPlayerStatus(board.next_move_color);

    switch(status)
    {
        case ChessPlayer::Checkmate:
            printf("Checkmate\n");
            break;
        case ChessPlayer::Stalemate:
            printf("Stalemate\n");
            break;
        default:
            break;
    }
}

int main(int argc, char *argv[]) {

    if (0) {
        test_fen();
        return 0;
    }

    const string ARG = "--master";
    const string SLAVE = "--slave";
    bool master_mode = false;
    bool slave_mode = false;
    int my_color = BLACK;
    if (argc > 2 && ARG == argv[1] ) {
        master_mode = true;
        string competitors[2];
        if (argc >= 4) {
            competitors[0] = argv[2];
            competitors[1] = argv[3];
            return PlayChamp::Run(argc, argv);

        }
    }
    for (int i = 1; i < argc; i++) {
        my_color = (string("black") == argv[i] ? BLACK : WHITE);
    }
    if (argc >= 3 && SLAVE == argv[1] ) {
        slave_mode = true;
    }

	ChessBoard board;
	list<Move> regulars, nulls;
    Move mov;
	bool found;
    unique_ptr<ChessPlayer> black;
    unique_ptr<ChessPlayer> white;

	// Initialize players

    if (my_color == BLACK) {
        black = make_unique<AIPlayer>(BLACK, 3);
        white = make_unique<HumanPlayer>(WHITE, slave_mode);
    } else {
        black = make_unique<HumanPlayer>(BLACK, slave_mode);
        white = make_unique<AIPlayer>(WHITE, 3);
    }


	// setup board
	board.initDefaultSetup();

    Global::instance().setColor(my_color);

	for(;;) {
        if (NOT slave_mode) {
            // show board
            board.print(mov);
        }


		// query player's choice
        if(board.next_move_color == WHITE)
            found = white->getMove(board, mov);
		else
            found = black->getMove(board, mov);


		if(!found)
			break;

		// if player has a move get all moves
		regulars.clear();
		nulls.clear();
        MoveGenerator<false>::getMoves(board, board.next_move_color, regulars, regulars, nulls);

		// execute maintenance moves
		for(list<Move>::iterator it = nulls.begin(); it != nulls.end(); ++it)
			board.move(*it);

        if (board.next_move_color == my_color) {
            // do not show our move
            // it would be missunderstood by
            cout << mov.toString() << endl;
        }

		// execute move
        board.move(mov);

        stringstream str;
        str << (board.next_move_color == my_color ? "my" : "opp") << " "
            << (board.next_move_color == WHITE ? "white" : "black");
        Global::instance().log(mov.toString() );

        if (NOT slave_mode) {
            mov.print();
        }

	}

    ChessPlayer::Status status = board.getPlayerStatus(board.next_move_color);

	switch(status)
	{
		case ChessPlayer::Checkmate:
			printf("Checkmate\n");
			break;
		case ChessPlayer::Stalemate:
			printf("Stalemate\n");
			break;
        default:
            break;
	}
}
