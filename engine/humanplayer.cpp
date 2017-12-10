#include <cstdlib>
#include <cstdio>
#include <list>
#include <cctype>
#include <cstring>
#include <sstream>
#include "humanplayer.h"
#include "chessboard.h"
#include "global.h"
#include "config.h"

using namespace std;
using namespace boost;

HumanPlayer::HumanPlayer(Config *config, int color)
 : ChessPlayer(config, color)
{}

HumanPlayer::~HumanPlayer()
{}

void HumanPlayer::prepare(const ChessBoard &board)
{
    if (NOT config->modeSlave()) {
        board.print();
    }
}

void HumanPlayer::showMove(const ChessBoard & orig_board, Move & move)
{
    ChessBoard board = orig_board;
    if (NOT config->modeSlave())
        board.print(move);

    board.move(move);
    ChessPlayer::Status status = board.getPlayerStatus(board.next_move_color);
    board.undoMove(move);

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


bool HumanPlayer::getMove(const ChessBoard & board, Move & move, AdvancedMoveData * move_data)
{
    string input;

	for(;;) {
        if (NOT config->modeSlave()) {
            printf(">> ");
        } else {
            stringstream str;
            str << "Waiting for input";
            Global::instance().log(str.str());
        }

        input = readInput();

		if(!processInput(input, move)) {
            stringstream str;
            str << "Error while parsing input:" << input;
            Global::instance().log(str.str());
			continue;
		}

		if(!board.isValidMove(color, move)) {

            stringstream str;
            str << "Invalid move " << move.toString();
            Global::instance().log(str.str());
			continue;
		}
        if (config->modeSlave()) {
            stringstream str;
            str << "Got move " << move.toString();
            Global::instance().log(str.str());
        }
        if (NOT config->modeSlave())
            printf("\n");
		break;
	}

	return true;
}

string HumanPlayer::readInput() const
{
    string line;
    char buffer[20];
    while (line == "") {
            cin >> line;
    }
    if (line == "") {
        line = buffer;
    }

    return line;
}

bool HumanPlayer::processInput(const string & buf, Move & move) const
{

    if (buf == "quit")
		exit(0);

    optional<Move> opt = Move::fromString(buf);
    if (opt) {
         move = *opt;
         return true;
    }

    return false;
}
