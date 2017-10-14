//#include <mcheck.h>
#include <cstdlib>
#include <cstdio>
#include <list>
#include <string>
#include "chessboard.h"
#include "humanplayer.h"
#include "aiplayer.h"
#include <boost/process.hpp>
#include <memory>
#include "playchamp.h"
#include <thread>
#include <iostream>
#include <string.h>
#include "global.h"
using namespace std;
#define NOT !
int main(int argc, char *argv[]) {

    const string ARG = "--master";
    const string SLAVE = "--slave";
    bool master_mode = false;
    bool slave_mode = false;
    int my_color = WHITE;
    if (argc > 2 && ARG == argv[1] ) {
        master_mode = true;
        string competitors[2];
        if (argc >= 4) {
            competitors[0] = argv[2];
            competitors[1] = argv[3];
            return PlayChamp::Run(argc, argv);

        }
    }

    if (argc >= 3 && SLAVE == argv[1] ) {
        slave_mode = true;
        my_color = (string("black") == argv[2] ? BLACK : WHITE);
    }

	ChessBoard board;
	list<Move> regulars, nulls;
	int turn = WHITE;
    Move mov;
	bool found;
    unique_ptr<ChessPlayer> black;
    unique_ptr<ChessPlayer> white;

	// Initialize players
    if (slave_mode) {
        if (my_color == BLACK) {
            black = make_unique<AIPlayer>(BLACK, 3);
            white = make_unique<HumanPlayer>(WHITE);
        } else {
            black = make_unique<HumanPlayer>(BLACK);
            white = make_unique<AIPlayer>(WHITE, 3);
        }
    } else {
        black = make_unique<AIPlayer>(BLACK, 3);
        white = make_unique<HumanPlayer>(WHITE);
    }

	// setup board
	board.initDefaultSetup();

    Global::instance().setSlaveMode(slave_mode);
    Global::instance().setColor(my_color);

	for(;;) {
        if (NOT slave_mode) {
            // show board
            board.print();
        }


		// query player's choice
        if(turn == WHITE)
            found = white->getMove(board, mov);
		else
            found = black->getMove(board, mov);





		if(!found)
			break;

		// if player has a move get all moves
		regulars.clear();
		nulls.clear();
		board.getMoves(turn, regulars, regulars, nulls);

		// execute maintenance moves
		for(list<Move>::iterator it = nulls.begin(); it != nulls.end(); ++it)
			board.move(*it);

		// execute move
        board.move(mov);
        if (slave_mode) {
            if (turn == my_color) {
                // do not show our move
                // it would be missunderstood by
                cout << mov.toString() << endl;
            }
            stringstream str;
            str << (turn == my_color ? "my" : "opp") << " "
                << (turn == WHITE ? "white" : "black");
            Global::instance().log(mov.toString() );
        } else {
            mov.print();
        }


		// opponents turn
		turn = TOGGLE_COLOR(turn);
	}

	ChessPlayer::Status status = board.getPlayerStatus(turn);

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
