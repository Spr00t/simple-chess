//#include <mcheck.h>
#include <cstdlib>
#include <cstdio>
#include <list>
#include <string>
#include <boost/process.hpp>
#include <memory>
#include <thread>
#include <iostream>
#include <string.h>
#include "global.h"
#include "playchamp.h"
#include "chessboard.h"
#include "humanplayer.h"
#include "aiplayer.h"
#include "consoleopponent.h"
#include "tests.h"
#include "config.h"
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
    board.loadFEN("5k2/5p2/8/8/8/8/1PPPPPP1/4K3 w - -");


    AdvancedMoveData advanced;

    Move mov;
    bool found;
    unique_ptr<ChessPlayer> black;
    unique_ptr<ChessPlayer> white;

    Config config;

    black = make_unique<AIPlayer>(&config, BLACK, 2);
    white = make_unique<AIPlayer>(&config, WHITE, 2);
    Global::instance().set_logging_color(BLACK);

    board.print();

    while (true) {

        if(board.next_move_color == WHITE)
            found = white->getMove(board, mov, &advanced);
        else
            found = black->getMove(board, mov, &advanced);


        if(!found)
            break;

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
    //sleep(10);
    ChessBoard board;

    string competitors[2];
    list<Move> regulars;
    Move mov;
    bool found;
    unique_ptr<ChessPlayer> black;
    unique_ptr<ChessPlayer> white;
    AdvancedMoveData advanced;
    bool ai_black = true;

    if (0) {
        test_fen();
        return 0;
    }

    Config config = Config::from_args(argc, argv);

    if (config.modeMaster()) {
        if (argc >= 4) {
            competitors[0] = argv[2];
            competitors[1] = argv[3];
        }
    }
	// Initialize players


    if (config.modeMaster()) {
        black = make_unique<ConsoleOpponent>(competitors[0], &config, BLACK);
        white = make_unique<ConsoleOpponent>(competitors[1], &config, WHITE);
    } else {
        if (config.isAiBlack()) {
            ai_black = true;
            black = make_unique<AIPlayer>(&config, BLACK, 4);
            white = make_unique<HumanPlayer>(&config, WHITE);
        } else {
            black = make_unique<HumanPlayer>(&config, BLACK);
            white = make_unique<AIPlayer>(&config, WHITE, 4);
        }
    }


	// setup board
    board.initDefaultSetup();
    //board.loadFEN("k7/4p1p1/8/4Pp2/8/8/5PP1/K7 b - - 0 1");
    board.loadFEN("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");

    Global::instance().set_logging_color(board.next_move_color);

    black->prepare(board);
    white->prepare(board);

	for(;;) {

        if(board.next_move_color == WHITE)
            found = white->getMove(board, mov, &advanced);
		else
            found = black->getMove(board, mov, &advanced);

		if(!found)
			break;

        if (NOT board.isValidMove(board.next_move_color, mov)) {
            cout << "Invalid move " << mov.toString() << endl;
            return 0;
        }

        if (board.next_move_color == config.ai_color && config.modeSlave()) {
            // do not show our move
            // it would be missunderstood by
            cout << mov.toString() << endl;
        }

		// execute move
        board.move(mov);

        // show opponents move to other player
        if(board.next_move_color == WHITE)
            white->showMove(board, mov);
        else
            black->showMove(board, mov);

        stringstream str;
        str << (board.next_move_color == config.ai_color ? "my" : "opp") << " "
            << (board.next_move_color == WHITE ? "white" : "black");
        Global::instance().log(mov.toString() );

#ifdef SHOW_DETAILS
        if (config.modeHuman()) {
            if (board.next_move_color == WHITE && ai_black) {
                cout << "Move evaluation: " << advanced.board_evaluation << endl;
            } else if (board.next_move_color == BLACK && NOT ai_black) {
                cout << "Move evaluation: " << advanced.board_evaluation << endl;
            }
            cout << board.toFEN() << endl;
            cout << "WHITE count: " << board.figures_count[WHITE] << endl;
            cout << "BLACK count: " << board.figures_count[BLACK >> 4] << endl;
        }
#endif

        if (NOT config.modeSlave()) {
            ChessPlayer::Status status = board.getPlayerStatus(board.next_move_color);

            switch(status)
            {
                case ChessPlayer::Checkmate:
                    board.print(mov);
                    cout << "Checkmate: " << (board.next_move_color == WHITE ? "white" : "black") << " are defeated" << endl;
                    return 0;
                case ChessPlayer::Stalemate:
                    board.print(mov);
                    cout << "Stalemate: on " << (board.next_move_color == WHITE ? "white" : "black") << " turn" << endl;
                    return 0;
                case ChessPlayer::Draw:
                    board.print(mov);
                    cout << "50 moves end game: on " << (board.next_move_color == WHITE ? "white" : "black") << " turn" << endl;
                    return 0;

                default:
                    continue;
            }
        }
	}
}
