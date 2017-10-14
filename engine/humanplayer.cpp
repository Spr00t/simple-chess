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
#define NOT !

HumanPlayer::HumanPlayer(int color)
 : ChessPlayer(color)
{}

HumanPlayer::~HumanPlayer()
{}

bool HumanPlayer::getMove(ChessBoard & board, Move & move) const
{
	list<Move> regulars, nulls;
	char * input;

	for(;;) {
        if (NOT Global::instance().isSlaveMode()) {
            printf(">> ");
        } else {
            stringstream str;
            str << "Waiting for input";
            Global::instance().log(str.str());
        }


        if((input = readInput()) == NULL) {
            Global::instance().log("Could not read input.");
			continue;
		}

		if(!processInput(input, move)) {

            Global::instance().log("Error while parsing input.");
			continue;
		}

		if(!board.isValidMove(color, move)) {

            stringstream str;
            str << "Invalid move " << move.toString();
            Global::instance().log(str.str());
			continue;
		}
        if (Global::instance().isSlaveMode()) {
            stringstream str;
            str << "Got move " << move.toString();
            Global::instance().log(str.str());
        }
		printf("\n");
		break;
	}

	return true;
}

char * HumanPlayer::readInput(void) const
{
	int buffsize = 8, c, i;
	char * buffer, * tmp;
	
	// allocate buffer
	buffer = reinterpret_cast<char*>(malloc(buffsize));
	if(!buffer)	{
		return NULL;
	}
	
	// read one line
	for(i = 0; (c = fgetc(stdin)) != '\n'; i++)	{
		if(i >= buffsize - 1) {
			if((buffsize << 1) < buffsize) {
				fprintf(stderr, "HumanPlayer::readInput(): " \
					"Someone is trying a buffer overrun.\n");
				free(buffer);
				return NULL;
			}
			tmp = reinterpret_cast<char*>(realloc(buffer, buffsize<<1));
			if(!tmp) {
				free(buffer);
				return NULL;
			}
			buffer = tmp;
			buffsize <<= 1;
		}
		buffer[i] = c;
	}
	
	// terminate buffer
	buffer[i] = '\0';
	return buffer;	
}

bool HumanPlayer::processInput(char * buf, Move & move) const
{
	int i = 0, j, l, n;

	// skip whitespace
	while(true) {
		if(buf[i] == '\0') {
			free(buf);
			return false;
		}
		else if(!isspace(buf[i])) {
			break;
		}
		else {
			i++;
		}
	}

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
			free(buf);
			return false;
		}
		if(n >= '1' && n <= '8') {
			n = n - '1';
		}
		else {
			free(buf);
			return false;
		}
		if(j == 0)
			move.from = n * 8 + l;
		else 
			move.to = n * 8 + l;
	}

	free(buf);
	return true;
}
