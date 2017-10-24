#include <cstdlib>
#include <cstdio>
#include <list>
#include <cctype>
#include <cstring>
#include <sstream>
#include "humanplayer.h"
#include "chessboard.h"
#include "global.h"

using namespace std;
using namespace boost;

HumanPlayer::HumanPlayer(int color, bool slaveMode)
 : ChessPlayer(color),
   slaveMode(slaveMode)
{}

HumanPlayer::~HumanPlayer()
{}

bool HumanPlayer::getMove(ChessBoard & board, Move & move) const
{
	list<Move> regulars, nulls;
    string input;

	for(;;) {
        if (NOT slaveMode) {
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
        if (slaveMode) {
            stringstream str;
            str << "Got move " << move.toString();
            Global::instance().log(str.str());
        }
        if (NOT slaveMode)
            printf("\n");
		break;
	}

	return true;
}

string HumanPlayer::readInput() const
{
    string line;
    while (line == "") {
            cin >> line;
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
