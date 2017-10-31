#include <cstdio>
#include <cstring>
#include <list>
#include <boost/format.hpp>
#include <sstream>
#include "chessboard.h"
#include "chessplayer.h"

#define COLORED
using namespace boost;
using boost::format;
using boost::io::group;

using namespace std;
template class MoveGenerator<true>;
template class MoveGenerator<false>;

// Redefinition in order for MoveGenerator to be available without namespace
static const char * field_name[] = {
    "A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1",
    "A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
    "A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
    "A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
    "A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
    "A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
    "A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
    "A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8"
};
string Move::toString(void) const
{
    stringstream str;
    str << format("%1%%2%") % field_name[(int)from] % field_name[(int)to];
    return str.str();
}

optional<Move> Move::fromString(const std::string & str)
{
    optional<Move> result;
    result.reset(Move());
    int i = 0, j, l, n;
    string lineIn = str, lineOut="    ";
    copy_if(lineIn.begin(), lineIn.end(), lineOut.begin(),
        [](char c ) {
            return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
        });

    if (lineIn != lineOut) {
        Global::instance().log("Gotcha");
    }

//    if(strncmp(&buf[i], "quit", 4) == 0)
//        exit(0);

    // convert from sth. like "b1c3"
    for(j = 0; j < 2; j++) {

        l = lineOut[i++];
        n = lineOut[i++];
        if(l >= 'a' && l <= 'h') {
            l = l - 'a';
        }
        else if(l >= 'A' && l <= 'H') {
            l = l - 'A';
        }
        else {
            result.reset();
            break;
        }
        if(n >= '1' && n <= '8') {
            n = n - '1';
        }
        else {
            result.reset();
            break;
        }

        if(j == 0)
            result->from = n * 8 + l;
        else
            result->to = n * 8 + l;
    }

    return result;
}

void Move::print(void) const {



	if(IS_BLACK(figure))
		printf("   Black ");
	else 
		printf("   White ");

	switch(FIGURE(figure)) {
		case PAWN:
			printf("pawn ");
			break;
		case ROOK:
			printf("rook ");
			break;
		case KNIGHT:
			printf("knight ");
			break;
		case BISHOP:
			printf("bishop ");
			break;
		case QUEEN:
			printf("queen ");
			break;
		case KING:
			printf("king ");
			break;
	}
	
	printf("from %s to %s:\n", field_name[(int)from], field_name[(int)to]);
}

bool Move::operator==(const Move & b) const
{
	if(from != b.from)
		return false;
	if(to != b.to)
		return false;
	if(capture != b.capture)
		return false;
	if(figure != b.figure)
		return false;
		
	return true;
}


ChessBoard::ChessBoard()
{
	memset((void*)square, EMPTY, sizeof(square));
}

void ChessBoard::print(Move move) const
{
	char figure;
	int repr, unmoved, passant, row, col;

    printf("   ___ ___ ___ ___ ___ ___ ___ ___ \n");

	for(row = 7; row >= 0; row--)
	{
        printf("%d ", row + 1);
		for(col = 0; col < 8; col++)
		{
			figure = this->square[row*8+col];
			repr = getASCIIrepr(figure);
			unmoved = (IS_MOVED(figure) || (figure == EMPTY)) ? ' ' : '.';
			passant = IS_PASSANT(figure) ? '`' : ' ';
#ifdef COLORED
            static const char RED_COLOR[]="\033[0;31m";
            static const char NO_COLOR[]="\033[0m";

            if (move.figure && row*8+col == move.to) {
                printf("|%c%s%c%s%c", unmoved, RED_COLOR, repr, NO_COLOR, passant);
            } else if (move.figure && row*8+col == move.from) {
                if (!figure) {
                    repr = '.';
                }
                printf("|%c%s%c%s%c",unmoved, RED_COLOR, repr, NO_COLOR, passant);
            } else
#endif
            printf("|%c%c%c", unmoved, repr, passant);
		}
        printf("|\n  |___|___|___|___|___|___|___|___|\n");
	}
    printf("    A   B   C   D   E   F   G   H  \n\n");
}

void ChessBoard::loadFEN(const string& position)
{
    //initial FEN board: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

    int len = position.size();

    memset(square, 0, sizeof(square));
    int pos = 0, figure, skip;

    int local_pos = 63;
    while (pos < 64 && local_pos >= 0) {
        char fig = position[pos];
        int col = 7 - local_pos % 8;
        int row = local_pos / 8;
        int target_destination = row * 8 + col;
        switch (fig) {
            case 'r':
            case 'R':
                square[target_destination] = (fig == 'r') ? SET_BLACK(ROOK) : ROOK;
                break;
            case 'n':
            case 'N':
                square[target_destination] = (fig == 'n') ? SET_BLACK(KNIGHT) : KNIGHT;
                break;
            case 'k':
            case 'K':
                square[target_destination] = (fig == 'k') ? SET_BLACK(KING) : KING;
                break;
            case 'q':
            case 'Q':
                square[target_destination] = (fig == 'q') ? SET_BLACK(QUEEN) : QUEEN;
                break;
            case 'b':
            case 'B':
                square[target_destination] = (fig == 'b') ? SET_BLACK(BISHOP) : BISHOP;
                break;
            case 'p':
            case 'P':
                square[target_destination] = (fig == 'p') ? SET_BLACK(PAWN) : PAWN;
                break;
            case '/':
                pos++;
                continue;
            case ' ':
                goto cycle_out;
            default:
                skip = fig - '0';
                local_pos -= (skip - 1); // one additional increment will be applied in the end of loop
                break;
        }
        if (square[target_destination]) square[target_destination] = SET_UNMOVED(square[target_destination]);

        pos++;
        local_pos--;
    }
    cycle_out:

    while (position[pos] == ' ' && pos < len) { pos++; }

    next_move_color = (position[pos] == 'w' ? WHITE : BLACK);
    pos++;

    while (position[pos] == ' ' && pos < len) { pos++; }


    if (square[A1]) square[A1] = SET_MOVED(square[A1]);
    if (square[A8]) square[A8] = SET_MOVED(square[A8]);
    if (square[H1]) square[H1] = SET_MOVED(square[H1]);
    if (square[H8]) square[H8] = SET_MOVED(square[H8]);

    //castlings
    while (position[pos] != ' ' && pos < len) {
        char fig = position[pos];
        switch (fig) {
            case 'k':
                if (square[E8]) square[E8] = SET_UNMOVED(square[E8]);
                if (square[H8]) square[H8] = SET_UNMOVED(square[H8]);
                break;
            case 'K':
                if (square[E1]) square[E1] = SET_UNMOVED(square[E1]);
                if (square[H1]) square[H1] = SET_UNMOVED(square[H1]);
                break;
            case 'q':
                if (square[E8]) square[E8] = SET_UNMOVED(square[E8]);
                if (square[A8]) square[A8] = SET_UNMOVED(square[A8]);
                break;
            case 'Q':
                if (square[E8]) square[E8] = SET_UNMOVED(square[E8]);
                if (square[A1]) square[A1] = SET_UNMOVED(square[A1]);
                break;
            default:
                goto cycle_out2;
        }

        pos++;
    }
    cycle_out2:

    while (position[pos] == ' ' && pos < len) { pos++; }

    // ignore passant
    while (position[pos] != ' ' && pos < len) {pos++;}
    while (position[pos] == ' ' && pos < len) { pos++; }

    int number = 0;
    while (position[pos] >= '0' && position[pos] <= '9' && pos < len) {number += (position[pos] - '0' + 10 * number); pos++; }

    fifty_moves = 51 -  number;

    refreshFigures();

    //if kings are not on their places, mark them moved
    if (black_king_pos != E8) {
        square[black_king_pos] = SET_MOVED(square[black_king_pos]);
    }

    if (white_king_pos != E1) {
        square[white_king_pos] = SET_MOVED(square[white_king_pos]);
    }

    //ignore next move number

}

char ChessBoard::getASCIIrepr(int figure) const
{
	switch(FIGURE(figure))
	{
		case PAWN:
			if(IS_BLACK(figure))
				return 'o';
			else
				return 'x';
		case ROOK:
			if(IS_BLACK(figure))
				return 'T';
			else
				return 't';
		case KNIGHT:
			if(IS_BLACK(figure))
				return 'H';
			else
				return'h';
		case BISHOP:
			if(IS_BLACK(figure))
				return 'B';
			else
				return 'b';
		case QUEEN:
			if(IS_BLACK(figure))
				return 'Q';
			else
				return 'q';
		case KING:
			if(IS_BLACK(figure))
				return 'K';
			else
				return 'k';
	}
	
	return ' ';
}

void ChessBoard::initDefaultSetup(void)
{
	// clear board
	memset((void*)square, EMPTY, sizeof(square));

	// setup white aristocracy
	square[A1] = ROOK; square[B1] = KNIGHT; square[C1] = BISHOP; square[D1] = QUEEN;
	square[E1] = KING; square[F1] = BISHOP; square[G1] = KNIGHT; square[H1] = ROOK;
	
	// setup black aristocracy
	square[A8] = SET_BLACK(ROOK); square[B8] = SET_BLACK(KNIGHT);
	square[C8] = SET_BLACK(BISHOP); square[D8] = SET_BLACK(QUEEN);
	square[E8] = SET_BLACK(KING); square[F8] = SET_BLACK(BISHOP);
	square[G8] = SET_BLACK(KNIGHT); square[H8] = SET_BLACK(ROOK);
	
	// setup white pawns
	square[A2] = square[B2] = square[C2] = square[D2] =
		square[E2] = square[F2] = square[G2] = square[H2] = PAWN;
		
	// setup black pawns
	square[A7] = square[B7] = square[C7] = square[D7] =
		square[E7] = square[F7] = square[G7] = square[H7] = SET_BLACK(PAWN);

	// register kings
	black_king_pos = E8;
    white_king_pos = E1;
    refreshFigures();
}

void ChessBoard::refreshFigures()
{
    figures_count[WHITE] = 0;
    figures_count[BLACK >> 8] = 0;
    for (int pos = 0; pos < 64; pos++ ) {

        int figure = square[pos];
        if (figure) {
            figures_count[IS_BLACK(figure) >> 8] ++;
        }
        if (FIGURE(figure) == KING) {
            if (IS_BLACK(figure)) {
                black_king_pos = pos;
            } else {
                white_king_pos = pos;
            }
        }
    }
}
template <bool capture_only>
void MoveGenerator<capture_only>::getMoves(ChessBoard & board, int color, list<Move> & moves, list<Move> & captures, list<Move> & null_moves)
{
	int pos, figure;
	
	for(pos = 0; pos < 64; pos++)
	{
        if((figure = board.square[pos]) != EMPTY)
		{
			if(IS_BLACK(figure) == color)
			{
				switch(FIGURE(figure))
				{
					case PAWN:
                        MoveGenerator<capture_only>::getPawnMoves(board, figure, pos, moves, captures, null_moves);
						break;
					case ROOK:
                        MoveGenerator<capture_only>::getRookMoves(board, figure, pos, moves, captures);
						break;
					case KNIGHT:
                        MoveGenerator<capture_only>::getKnightMoves(board, figure, pos, moves, captures);
						break;
					case BISHOP:
                        MoveGenerator<capture_only>::getBishopMoves(board, figure, pos, moves, captures);
						break;
					case QUEEN:
                        MoveGenerator<capture_only>::getQueenMoves(board, figure, pos, moves, captures);
						break;
					case KING:
                        MoveGenerator<capture_only>::getKingMoves(board, figure, pos, moves, captures);
						break;
					default:
						break;
				}
			}
		}
	}
}
template<bool capture_only>
void MoveGenerator<capture_only>::getPawnMoves(ChessBoard & board, int figure, int pos, list<Move> & moves, list<Move> & captures, list<Move>  & null_moves)
{
	Move new_move;
	int target_pos, target_figure;

	// If pawn was previously en passant candidate victim, it isn't anymore.
	// This is a null move because it has to be executed no matter what.
	if(IS_PASSANT(figure))
	{
		new_move.figure = CLEAR_PASSANT(figure);
		new_move.from = pos;
		new_move.to = pos;
		new_move.capture = figure;
		null_moves.push_back(new_move);
		
		figure = CLEAR_PASSANT(figure);
	}

	// Of course, we only have to set this once
	new_move.figure = figure;
	new_move.from = pos;

	// 1. One step ahead
	target_pos = IS_BLACK(figure) ? pos - 8 : pos + 8;
    if(NOT capture_only && (target_pos >= 0) && (target_pos < 64))
	{
        if((target_figure = board.square[target_pos]) == EMPTY)
		{
			new_move.to = target_pos;
			new_move.capture = target_figure;
			moves.push_back(new_move);
			
			// 2. Two steps ahead if unmoved
			if(!IS_MOVED(figure))
			{
				target_pos = IS_BLACK(figure) ? pos - 16 : pos + 16;
				if((target_pos >= 0) && (target_pos < 64))
				{
                    if((target_figure = board.square[target_pos]) == EMPTY)
					{
						new_move.to = target_pos;
						new_move.capture = target_figure;

						// set passant attribute and clear it later
						new_move.figure = SET_PASSANT(figure);
						moves.push_back(new_move);
						new_move.figure = figure;
					}
				}
			} // END 2.
		}
	} // END 1.
	
	// 3. Forward capture (White left; Black right)
	if(pos % 8 != 0)
	{
		target_pos = IS_BLACK(figure) ? pos - 9 : pos + 7;
		if((target_pos >= 0) && (target_pos < 64))
		{
            if((target_figure = board.square[target_pos]) != EMPTY)
			{
				if(IS_BLACK(target_figure) != IS_BLACK(figure))
				{
					new_move.to = target_pos;
					new_move.capture = target_figure;
					captures.push_back(new_move);
				}
			}
			else
			{
				// En passant?
                target_figure = board.square[pos - 1];
				if(IS_PASSANT(target_figure))
				{
					if(IS_BLACK(target_figure) != IS_BLACK(figure))
					{
						new_move.to = target_pos;
						new_move.capture = target_figure;
						captures.push_back(new_move);
					}				
				}
			}
		}
	}
	
	// 4. Forward capture (White right; Black left)
	if(pos % 8 != 7)
	{
		target_pos = IS_BLACK(figure) ? pos - 7 : pos + 9;
		if((target_pos >= 0) && (target_pos < 64))
		{
            if((target_figure = board.square[target_pos]) != EMPTY)
			{
				if(IS_BLACK(target_figure) != IS_BLACK(figure))
				{
					new_move.to = target_pos;
					new_move.capture = target_figure;
					captures.push_back(new_move);
				}
			}
			else
			{
				// En passant?
                target_figure = board.square[pos + 1];
				if(IS_PASSANT(target_figure))
				{
					if(IS_BLACK(target_figure) != IS_BLACK(figure))
					{
						new_move.to = target_pos;
						new_move.capture = target_figure;
						captures.push_back(new_move);
					}				
				}
			}
		}
	}	
}
template<bool capture_only>
void MoveGenerator<capture_only>::getRookMoves(ChessBoard & board, int figure, int pos, list<Move> & moves, list<Move> & captures)
{
	Move new_move;
	int target_pos, target_figure, end;

	// Of course, we only have to set this once
	new_move.figure = figure;
	new_move.from = pos;

	// 1. Move up
	for(target_pos = pos + 8; target_pos < 64; target_pos += 8)
	{
        if((target_figure = board.square[target_pos]) != EMPTY)
		{
			if(IS_BLACK(figure) != IS_BLACK(target_figure))
			{
				new_move.to = target_pos;
				new_move.capture = target_figure;
				captures.push_back(new_move);
			}
			
			break;
		}
		else
		{
			new_move.to = target_pos;
			new_move.capture = target_figure;
			moves.push_back(new_move);
		}
	}

	// 2. Move down
	for(target_pos = pos - 8; target_pos >= 0; target_pos -= 8)
	{	
        if((target_figure = board.square[target_pos]) != EMPTY)
		{
			if(IS_BLACK(figure) != IS_BLACK(target_figure))
			{
				new_move.to = target_pos;
				new_move.capture = target_figure;
				captures.push_back(new_move);
			}
			
			break;
		}
		else
		{
			new_move.to = target_pos;
			new_move.capture = target_figure;
			moves.push_back(new_move);
		}
	}

	// 3. Move left
	for(target_pos = pos - 1, end = pos - (pos % 8); target_pos >= end; target_pos--)
	{
        if((target_figure = board.square[target_pos]) != EMPTY)
		{
			if(IS_BLACK(figure) != IS_BLACK(target_figure))
			{
				new_move.to = target_pos;
				new_move.capture = target_figure;
				captures.push_back(new_move);
			}
			
			break;
		}
		else
		{
			new_move.to = target_pos;
			new_move.capture = target_figure;
			moves.push_back(new_move);
		}
	}

	// 4. Move right
	for(target_pos = pos + 1, end = pos + (8 - pos % 8); target_pos < end; target_pos++)
	{
        if((target_figure = board.square[target_pos]) != EMPTY)
		{
			if(IS_BLACK(figure) != IS_BLACK(target_figure))
			{
				new_move.to = target_pos;
				new_move.capture = target_figure;
				captures.push_back(new_move);
			}
			
			break;
		}
		else
		{
			new_move.to = target_pos;
			new_move.capture = target_figure;
			moves.push_back(new_move);
		}
	}
}

template<bool capture_only>
void MoveGenerator<capture_only>::getKnightMoves(ChessBoard & board, int figure, int pos, list<Move> & moves, list<Move> & captures)
{
	Move new_move;
	int target_pos, target_figure, row, col;

	// Of course, we only have to set this once
	new_move.figure = figure;
	new_move.from = pos;

	// Determine row and column
	row = pos / 8;
	col = pos % 8;

	// 1. Upper positions
	if(row < 6)
	{
		// right
		if(col < 7)
		{
			target_pos = pos + 17;
		
            if((target_figure = board.square[target_pos]) != EMPTY)
			{
				if(IS_BLACK(figure) != IS_BLACK(target_figure))
				{
					new_move.capture = target_figure;
					new_move.to = target_pos;
					captures.push_back(new_move);
				}
			}
			else
			{
				new_move.capture = target_figure;
				new_move.to = target_pos;
				moves.push_back(new_move);
			}
		}
		
		// left
		if(col > 0)
		{
			target_pos = pos + 15;
		
            if((target_figure = board.square[target_pos]) != EMPTY)
			{
				if(IS_BLACK(figure) != IS_BLACK(target_figure))
				{
					new_move.capture = target_figure;
					new_move.to = target_pos;
					captures.push_back(new_move);
				}
			}
			else
			{
				new_move.capture = target_figure;
				new_move.to = target_pos;
				moves.push_back(new_move);
			}		
		}
	}
	
	// 2. Lower positions
	if(row > 1)
	{
		// right
		if(col < 7)
		{
			target_pos = pos - 15;
		
            if((target_figure = board.square[target_pos]) != EMPTY)
			{
				if(IS_BLACK(figure) != IS_BLACK(target_figure))
				{
					new_move.capture = target_figure;
					new_move.to = target_pos;
					captures.push_back(new_move);
				}
			}
			else
			{
				new_move.capture = target_figure;
				new_move.to = target_pos;
				moves.push_back(new_move);
			}
		}
		
		// left
		if(col > 0)
		{
			target_pos = pos - 17;
		
            if((target_figure = board.square[target_pos]) != EMPTY)
			{
				if(IS_BLACK(figure) != IS_BLACK(target_figure))
				{
					new_move.capture = target_figure;
					new_move.to = target_pos;
					captures.push_back(new_move);
				}
			}
			else
			{
				new_move.capture = target_figure;
				new_move.to = target_pos;
				moves.push_back(new_move);
			}		
		}
	}

	// 3. Right positions
	if(col < 6)
	{
		// up
		if(row < 7)
		{
			target_pos = pos + 10;
		
            if((target_figure = board.square[target_pos]) != EMPTY)
			{
				if(IS_BLACK(figure) != IS_BLACK(target_figure))
				{
					new_move.capture = target_figure;
					new_move.to = target_pos;
					captures.push_back(new_move);
				}
			}
			else
			{
				new_move.capture = target_figure;
				new_move.to = target_pos;
				moves.push_back(new_move);
			}
		}
		
		// down
		if(row > 0)
		{
			target_pos = pos - 6;
		
            if((target_figure = board.square[target_pos]) != EMPTY)
			{
				if(IS_BLACK(figure) != IS_BLACK(target_figure))
				{
					new_move.capture = target_figure;
					new_move.to = target_pos;
					captures.push_back(new_move);
				}
			}
			else
			{
				new_move.capture = target_figure;
				new_move.to = target_pos;
				moves.push_back(new_move);
			}		
		}
	}

	// 4. Left positions
	if(col > 1)
	{
		// up
		if(row < 7)
		{
			target_pos = pos + 6;
		
            if((target_figure = board.square[target_pos]) != EMPTY)
			{
				if(IS_BLACK(figure) != IS_BLACK(target_figure))
				{
					new_move.capture = target_figure;
					new_move.to = target_pos;
					captures.push_back(new_move);
				}
			}
			else
			{
				new_move.capture = target_figure;
				new_move.to = target_pos;
				moves.push_back(new_move);
			}
		}
		
		// down
		if(row > 0)
		{
			target_pos = pos - 10;
		
            if((target_figure = board.square[target_pos]) != EMPTY)
			{
				if(IS_BLACK(figure) != IS_BLACK(target_figure))
				{
					new_move.capture = target_figure;
					new_move.to = target_pos;
					captures.push_back(new_move);
				}
			}
			else
			{
				new_move.capture = target_figure;
				new_move.to = target_pos;
				moves.push_back(new_move);
			}		
		}
	}	
}

template<bool capture_only>
void MoveGenerator<capture_only>::getBishopMoves(ChessBoard & board, int figure, int pos, list<Move> & moves, list<Move> & captures)
{
	Move new_move;
	int target_pos, target_figure, row, col, i, j;

	// Of course, we only have to set this once
	new_move.figure = figure;
	new_move.from = pos;

	// Determine row and column
	row = pos / 8;
	col = pos % 8;

	// 1. Go north-east
	for(i = row + 1, j = col + 1; (i < 8) && (j < 8); i++, j++)
	{
		target_pos = i * 8 + j;
        if((target_figure = board.square[target_pos]) != EMPTY)
		{
			if(IS_BLACK(figure) != IS_BLACK(target_figure))
			{
				new_move.to = target_pos;
				new_move.capture = target_figure;
				captures.push_back(new_move);
			}

			break;
		}
		else
		{
			new_move.to = target_pos;
			new_move.capture = target_figure;
			moves.push_back(new_move);
		}
	}
	
	// 2. Go south-east
	for(i = row - 1, j = col + 1; (i >= 0) && (j < 8); i--, j++)
	{
		target_pos = i * 8 + j;
        if((target_figure = board.square[target_pos]) != EMPTY)
		{
			if(IS_BLACK(figure) != IS_BLACK(target_figure))
			{
				new_move.to = target_pos;
				new_move.capture = target_figure;
				captures.push_back(new_move);
			}
			
			break;
		}
		else
		{
			new_move.to = target_pos;
			new_move.capture = target_figure;
			moves.push_back(new_move);
		}
	}

	// 3. Go south-west
	for(i = row - 1, j = col - 1; (i >= 0) && (j >= 0); i--, j--)
	{
		target_pos = i * 8 + j;
        if((target_figure = board.square[target_pos]) != EMPTY)
		{
			if(IS_BLACK(figure) != IS_BLACK(target_figure))
			{
				new_move.to = target_pos;
				new_move.capture = target_figure;
				captures.push_back(new_move);
			}
			
			break;
		}
		else
		{
			new_move.to = target_pos;
			new_move.capture = target_figure;
			moves.push_back(new_move);
		}
	}

	// 4. Go north-west
	for(i = row + 1, j = col - 1; (i < 8) && (j >= 0); i++, j--)
	{
		target_pos = i * 8 + j;
        if((target_figure = board.square[target_pos]) != EMPTY)
		{
			if(IS_BLACK(figure) != IS_BLACK(target_figure))
			{
				new_move.to = target_pos;
				new_move.capture = target_figure;
				captures.push_back(new_move);
			}
			
			break;
		}
		else
		{
			new_move.to = target_pos;
			new_move.capture = target_figure;
			moves.push_back(new_move);
		}
	}
}

template<bool capture_only>
void MoveGenerator<capture_only>::getQueenMoves(ChessBoard & board, int figure, int pos, list<Move> & moves, list<Move> & captures)
{
	// Queen is just the "cartesian product" of Rook and Bishop
    MoveGenerator<capture_only>::getRookMoves(board, figure, pos, moves, captures);
    MoveGenerator<capture_only>::getBishopMoves(board, figure, pos, moves, captures);
}

template<bool capture_only>
void MoveGenerator<capture_only>::getKingMoves(ChessBoard & board, int figure, int pos, list<Move> & moves, list<Move> & captures)
{
	Move new_move;
	int target_pos, target_figure, row, col;

	// Of course, we only have to set this once
	new_move.figure = figure;
	new_move.from = pos;

	// Determine row and column
	row = pos / 8;
	col = pos % 8;

	// 1. Move left
	if(col > 0)
	{
		// 1.1 up
		if(row < 7)
		{
			target_pos = pos + 7;
            if((target_figure = board.square[target_pos]) != EMPTY)
			{
				if(IS_BLACK(target_figure) != IS_BLACK(figure))
				{
					new_move.capture = target_figure;
					new_move.to = target_pos;
					captures.push_back(new_move);
				}
			}
			else
			{
				new_move.to = target_pos;
				new_move.capture = target_figure;
				moves.push_back(new_move);
			}
		}
		
		// 1.2 middle
		target_pos = pos - 1;
        if((target_figure = board.square[target_pos]) != EMPTY)
		{
			if(IS_BLACK(target_figure) != IS_BLACK(figure))
			{
				new_move.capture = target_figure;
				new_move.to = target_pos;
				captures.push_back(new_move);
			}
		}
		else
		{
			new_move.to = target_pos;
			new_move.capture = target_figure;
			moves.push_back(new_move);
		}
		
		// 1.3 down
		if(row > 0)
		{
			target_pos = pos - 9;
            if((target_figure = board.square[target_pos]) != EMPTY)
			{
				if(IS_BLACK(target_figure) != IS_BLACK(figure))
				{
					new_move.capture = target_figure;
					new_move.to = target_pos;
					captures.push_back(new_move);
				}
			}
			else
			{
				new_move.to = target_pos;
				new_move.capture = target_figure;
				moves.push_back(new_move);
			}
		}
	}
	
	// 2. Move right
	if(col < 7)
	{
		// 2.1 up
		if(row < 7)
		{
			target_pos = pos + 9;
            if((target_figure = board.square[target_pos]) != EMPTY)
			{
				if(IS_BLACK(target_figure) != IS_BLACK(figure))
				{
					new_move.capture = target_figure;
					new_move.to = target_pos;
					captures.push_back(new_move);
				}
			}
			else
			{
				new_move.to = target_pos;
				new_move.capture = target_figure;
				moves.push_back(new_move);
			}
		}
		
		// 2.2 middle
		target_pos = pos + 1;
        if((target_figure = board.square[target_pos]) != EMPTY)
		{
			if(IS_BLACK(target_figure) != IS_BLACK(figure))
			{
				new_move.capture = target_figure;
				new_move.to = target_pos;
				captures.push_back(new_move);
			}
		}
		else
		{
			new_move.to = target_pos;
			new_move.capture = target_figure;
			moves.push_back(new_move);
		}
		
		// 2.3 down
		if(row > 0)
		{
			target_pos = pos - 7;
            if((target_figure = board.square[target_pos]) != EMPTY)
			{
				if(IS_BLACK(target_figure) != IS_BLACK(figure))
				{
					new_move.capture = target_figure;
					new_move.to = target_pos;
					captures.push_back(new_move);
				}
			}
			else
			{
				new_move.to = target_pos;
				new_move.capture = target_figure;
				moves.push_back(new_move);
			}
		}
	}
	
	// 3. straight up
	if(row < 7)
	{
		// 2.2 middle
		target_pos = pos + 8;
        if((target_figure = board.square[target_pos]) != EMPTY)
		{
			if(IS_BLACK(target_figure) != IS_BLACK(figure))
			{
				new_move.capture = target_figure;
				new_move.to = target_pos;
				captures.push_back(new_move);
			}
		}
		else
		{
			new_move.to = target_pos;
			new_move.capture = target_figure;
			moves.push_back(new_move);
		}	
	}
	
	// 4. straight down
	if(row > 0)
	{
		// 2.2 middle
		target_pos = pos - 8;
        if((target_figure = board.square[target_pos]) != EMPTY)
		{
			if(IS_BLACK(target_figure) != IS_BLACK(figure))
			{
				new_move.capture = target_figure;
				new_move.to = target_pos;
				captures.push_back(new_move);
			}
		}
		else
		{
			new_move.to = target_pos;
			new_move.capture = target_figure;
			moves.push_back(new_move);
		}	
	}

	// 5. Castling
    if(!IS_MOVED(figure) && !board.isVulnerable(pos, figure))
	{
		// short
		target_pos = IS_BLACK(figure) ? F8 : F1;
        if((board.square[target_pos] == EMPTY) && !board.isVulnerable(target_pos, figure))
		{
			target_pos = IS_BLACK(figure) ? G8 : G1;
            if((board.square[target_pos] == EMPTY) && !board.isVulnerable(target_pos, figure))
			{
				target_pos = IS_BLACK(figure) ? H8 : H1;
                target_figure = board.square[target_pos];
                if(!IS_MOVED(target_figure) && (FIGURE(target_figure) == ROOK) && !board.isVulnerable(target_pos, figure))
				{
					if(IS_BLACK(target_figure) == IS_BLACK(figure))
					{
						new_move.capture = EMPTY;
						new_move.to = IS_BLACK(figure) ? G8 : G1;
						moves.push_back(new_move);
					}
				}
			}
		}
		
		// long
		target_pos = IS_BLACK(figure) ? B8 : B1;
        if((board.square[target_pos] == EMPTY) && !board.isVulnerable(target_pos, figure))
		{
			target_pos = IS_BLACK(figure) ? C8 : C1;
            if((board.square[target_pos] == EMPTY) && !board.isVulnerable(target_pos, figure))
			{
				target_pos = IS_BLACK(figure) ? D8 : D1;
                if((board.square[target_pos] == EMPTY) && !board.isVulnerable(target_pos, figure))
				{
					target_pos = IS_BLACK(figure) ? A8 : A1;
                    target_figure = board.square[target_pos];
                    if(!IS_MOVED(target_figure) && (FIGURE(target_figure) == ROOK) && !board.isVulnerable(target_pos, figure))
					{
						if(IS_BLACK(target_figure) == IS_BLACK(figure))
						{
							new_move.capture = EMPTY;
							new_move.to = IS_BLACK(figure) ? C8 : C1;
							moves.push_back(new_move);
						}
					}
				}
			}
		}
	}
}

bool ChessBoard::isVulnerable(int pos, int color) const
{
	int target_pos, target_figure, row, col, i,j, end;

	// Determine row and column
	row = pos / 8;
	col = pos % 8;

	// 1. Look for Rooks, Queens and Kings above
	for(target_pos = pos + 8; target_pos < 64; target_pos += 8)
	{
		if((target_figure = this->square[target_pos]) != EMPTY)
		{
            if(IS_BLACK(target_figure) != IS_BLACK(color))
			{
				if((target_pos - pos) == 8)
				{
					if(FIGURE(target_figure) == KING)
						return true;
				}

				if((FIGURE(target_figure) == ROOK) || (FIGURE(target_figure) == QUEEN))
					return true;
			}

			break;
		}
	}

	// 2. Look for Rooks, Queens and Kings below
	for(target_pos = pos - 8; target_pos >= 0; target_pos -= 8)
	{
		if((target_figure = this->square[target_pos]) != EMPTY)
		{
            if(IS_BLACK(target_figure) != IS_BLACK(color))
			{
				if((pos - target_pos) == 8)
				{
					if(FIGURE(target_figure) == KING)
						return true;
				}

				if((FIGURE(target_figure) == ROOK) || (FIGURE(target_figure) == QUEEN))
					return true;
			}

			break;
		}
	}

	// 3. Look for Rooks, Queens and Kings left
	for(target_pos = pos - 1, end = pos - (pos % 8); target_pos >= end; target_pos--)
	{
		if((target_figure = this->square[target_pos]) != EMPTY)
		{
            if(IS_BLACK(color) != IS_BLACK(target_figure))
			{
				if((pos - target_pos) == 1)
				{
					if(FIGURE(target_figure) == KING)
						return true;
				}

				if((FIGURE(target_figure) == ROOK) || (FIGURE(target_figure) == QUEEN))
					return true;
			}

			break;
		}
	}

	// 4. Look for Rooks, Queens and Kings right
	for(target_pos = pos + 1, end = pos + (8 - pos % 8); target_pos < end; target_pos++)
	{
		if((target_figure = this->square[target_pos]) != EMPTY)
		{
            if(IS_BLACK(color) != IS_BLACK(target_figure))
			{
				if((target_pos - pos) == 1)
				{
					if(FIGURE(target_figure) == KING)
						return true;
				}

				if((FIGURE(target_figure) == ROOK) || (FIGURE(target_figure) == QUEEN))
					return true;
			}

			break;
		}
	}

	// 5. Look for Bishops, Queens, Kings and Pawns north-east
	for(i = row + 1, j = col + 1; (i < 8) && (j < 8); i++, j++)
	{
		target_pos = i * 8 + j;
		if((target_figure = this->square[target_pos]) != EMPTY)
		{
            if(IS_BLACK(color) != IS_BLACK(target_figure))
			{
				if((target_pos - pos) == 9)
				{
					if(FIGURE(target_figure) == KING)
						return true;
                    else if(!IS_BLACK(color) && (FIGURE(target_figure) == PAWN))
						return true;
				}

				if((FIGURE(target_figure) == BISHOP) || (FIGURE(target_figure) == QUEEN))
					return true;
			}

			break;
		}
	}
	
	// 6. Look for Bishops, Queens, Kings and Pawns south-east
	for(i = row - 1, j = col + 1; (i >= 0) && (j < 8); i--, j++)
	{
		target_pos = i * 8 + j;
		if((target_figure = this->square[target_pos]) != EMPTY)
		{
            if(IS_BLACK(color) != IS_BLACK(target_figure))
			{
				if((pos - target_pos) == 7)
				{
					if(FIGURE(target_figure) == KING)
						return true;
                    else if(IS_BLACK(color) && (FIGURE(target_figure) == PAWN))
						return true;
				}

				if((FIGURE(target_figure) == BISHOP) || (FIGURE(target_figure) == QUEEN))
					return true;
			}

			break;
		}
	}

	// 7. Look for Bishops, Queens, Kings and Pawns south-west
	for(i = row - 1, j = col - 1; (i >= 0) && (j >= 0); i--, j--)
	{
		target_pos = i * 8 + j;
		if((target_figure = this->square[target_pos]) != EMPTY)
		{
            if(IS_BLACK(color) != IS_BLACK(target_figure))
			{
				if((pos - target_pos) == 9)
				{
					if(FIGURE(target_figure) == KING)
						return true;
                    else if(IS_BLACK(color) && (FIGURE(target_figure) == PAWN))
						return true;
				}

				if((FIGURE(target_figure) == BISHOP) || (FIGURE(target_figure) == QUEEN))
					return true;
			}

			break;
		}
	}

	// 8. Look for Bishops, Queens, Kings and Pawns north-west
	for(i = row + 1, j = col - 1; (i < 8) && (j >= 0); i++, j--)
	{
		target_pos = i * 8 + j;
		if((target_figure = this->square[target_pos]) != EMPTY)
		{
            if(IS_BLACK(color) != IS_BLACK(target_figure))
			{
				if((target_pos - pos) == 7)
				{
					if(FIGURE(target_figure) == KING)
						return true;
                    else if(!IS_BLACK(color) && (FIGURE(target_figure) == PAWN))
						return true;
				}

				if((FIGURE(target_figure) == BISHOP) || (FIGURE(target_figure) == QUEEN))
					return true;
			}

			break;
		}
	}
	
	// 9. Look for Knights in upper positions
	if(row < 6)
	{
		// right
		if(col < 7)
		{
			target_pos = pos + 17;

			if((target_figure = this->square[target_pos]) != EMPTY)
			{
                if(IS_BLACK(color) != IS_BLACK(target_figure))
				{
					if(FIGURE(target_figure) == KNIGHT) return true;
				}
			}
		}
		
		// left
		if(col > 0)
		{
			target_pos = pos + 15;
		
			if((target_figure = this->square[target_pos]) != EMPTY)
			{
                if(IS_BLACK(color) != IS_BLACK(target_figure))
				{
					if(FIGURE(target_figure) == KNIGHT) return true;
				}
			}		
		}
	}
	
	// 10. Look for Knights in lower positions
	if(row > 1)
	{
		// right
		if(col < 7)
		{
			target_pos = pos - 15;
		
			if((target_figure = this->square[target_pos]) != EMPTY)
			{
                if(IS_BLACK(color) != IS_BLACK(target_figure))
				{
					if(FIGURE(target_figure) == KNIGHT) return true;
				}
			}
		}
		
		// left
		if(col > 0)
		{
			target_pos = pos - 17;
		
			if((target_figure = this->square[target_pos]) != EMPTY)
			{
                if(IS_BLACK(color) != IS_BLACK(target_figure))
				{
					if(FIGURE(target_figure) == KNIGHT) return true;
				}
			}		
		}
	}

	// 11. Look for Knights in right positions
	if(col < 6)
	{
		// up
		if(row < 7)
		{
			target_pos = pos + 10;
		
			if((target_figure = this->square[target_pos]) != EMPTY)
			{
                if(IS_BLACK(color) != IS_BLACK(target_figure))
				{
					if(FIGURE(target_figure) == KNIGHT) return true;
				}
			}
		}
		
		// down
		if(row > 0)
		{
			target_pos = pos - 6;
		
			if((target_figure = this->square[target_pos]) != EMPTY)
			{
                if(IS_BLACK(color) != IS_BLACK(target_figure))
				{
					if(FIGURE(target_figure) == KNIGHT) return true;
				}
			}		
		}
	}

	// 12. Look for Knights in left positions
	if(col > 1)
	{
		// up
		if(row < 7)
		{
			target_pos = pos + 6;
		
			if((target_figure = this->square[target_pos]) != EMPTY)
			{
                if(IS_BLACK(color) != IS_BLACK(target_figure))
				{
					if(FIGURE(target_figure) == KNIGHT) return true;
				}
			}
		}
		
		// down
		if(row > 0)
		{
			target_pos = pos - 10;
		
			if((target_figure = this->square[target_pos]) != EMPTY)
			{
                if(IS_BLACK(color) != IS_BLACK(target_figure))
				{
					if(FIGURE(target_figure) == KNIGHT) return true;
				}
			}		
		}
	}	

	return false;
}

bool ChessBoard::isValidMove(int color, Move & move)
{
	bool valid = false;
	list<Move> regulars, nulls;

    MoveGenerator<false>::getMoves(*this, color, regulars, regulars, nulls);

	for(list<Move>::iterator it = regulars.begin(); it != regulars.end() && !valid; ++it)
	{
		if(move.from == (*it).from && move.to == (*it).to)
		{
			move = *it;

			this->move(move);
			if(!isVulnerable(color ? black_king_pos : white_king_pos, color))
				valid = true;
			undoMove(*it);
		}
	}

	return valid;
}

ChessPlayer::Status ChessBoard::getPlayerStatus(int color)
{
    if (fifty_moves <= 0) {
        return ChessPlayer::Draw;
    }
	bool king_vulnerable = false, can_move = false;
	list<Move> regulars, nulls;

    MoveGenerator<false>::getMoves(*this, color, regulars, regulars, nulls);

	if(isVulnerable(color ? black_king_pos : white_king_pos, color))
		king_vulnerable = true;

	for(list<Move>::iterator it = regulars.begin(); it != regulars.end() && !can_move; ++it)
	{
		this->move(*it);
		if(!isVulnerable(color ? black_king_pos : white_king_pos, color))
		{
			can_move = true;
		}
		undoMove(*it);
	}

	if(king_vulnerable && can_move)
		return ChessPlayer::InCheck;
	if(king_vulnerable && !can_move)
		return ChessPlayer::Checkmate;
    if((!king_vulnerable && !can_move) || fifty_moves <= 0)
		return ChessPlayer::Stalemate;

	return ChessPlayer::Normal;
}

void ChessBoard::move(const Move & move)
{
    if (move.capture || (FIGURE(move.figure) == PAWN)) {
        if (move.to != move.from) {
            fifty_moves_stack.push_back(fifty_moves);
            fifty_moves = 50;
        }
    } else {
        fifty_moves--;
    }
	// kings and pawns receive special treatment
	switch(FIGURE(move.figure))
	{
		case KING:
			moveKing(move);
			break;
		case PAWN:
            fifty_moves = 50;
			if(move.to != move.from) {
				movePawn(move);
				break;
			}
		default:
			this->square[(int)move.from] = EMPTY;
			this->square[(int)move.to] = SET_MOVED(move.figure);
			break;
	}
    if (move.capture) {
        figures_count[IS_BLACK(move.capture) >> 8]--;
    }
    if (move.to != move.from) {
        toogleColor();
    }
}

void ChessBoard::undoMove(const Move & move)
{
    if (move.capture || FIGURE(move.figure) == PAWN) {
        if (move.to != move.from) {
            fifty_moves = fifty_moves_stack.back();
            fifty_moves_stack.pop_back();
        }
    } else {
        fifty_moves++;
    }
	// kings and pawns receive special treatment
	switch(FIGURE(move.figure))
	{
		case KING:
			undoMoveKing(move);
			break;
		case PAWN:
			if(move.to != move.from) {
				undoMovePawn(move);
				break;
			}
		default:
			this->square[(int)move.from] = move.figure;
			this->square[(int)move.to] = move.capture;
			break;
	}
    if (move.capture) {
        figures_count[IS_BLACK(move.capture) >> 8]++;
    }
    if (move.to != move.from) {
        toogleColor();
    }}

void ChessBoard::movePawn(const Move & move)
{
	int capture_field;

	// check for en-passant capture
	if(IS_PASSANT(move.capture))
	{
		if(IS_BLACK(move.figure))
		{
			capture_field = move.to + 8;
			if((move.from / 8) == 3)
				this->square[capture_field] = EMPTY;
		}
		else
		{
			capture_field = move.to - 8;
			if((move.from / 8) == 4)
				this->square[capture_field] = EMPTY;
		}
	}

	this->square[(int)move.from] = EMPTY;

	// mind pawn promotion
	if(IS_BLACK(move.figure)) {
		if(move.to / 8 == 0)
			this->square[(int)move.to] = SET_MOVED(SET_BLACK(QUEEN));
		else
			this->square[(int)move.to] = SET_MOVED(move.figure);
	}
	else {
		if(move.to / 8 == 7)
			this->square[(int)move.to] = SET_MOVED(QUEEN);
		else
			this->square[(int)move.to] = SET_MOVED(move.figure);
	}
}

void ChessBoard::undoMovePawn(const Move & move)
{
	int capture_field;

	this->square[(int)move.from] = CLEAR_PASSANT(move.figure);

	// check for en-passant capture
	if(IS_PASSANT(move.capture))
	{
		if(IS_BLACK(move.figure))
		{
			capture_field = move.to + 8;
			if(move.from / 8 == 3) {
				this->square[capture_field] = move.capture;
				this->square[(int)move.to] = EMPTY;
			}
			else {
				this->square[(int)move.to] = move.capture;
			}
		}
		else
		{
			capture_field = move.to - 8;
			if(move.from / 8 == 4) {
				this->square[capture_field] = move.capture;
				this->square[(int)move.to] = EMPTY;
			}
			else {
				this->square[(int)move.to] = move.capture;
			}
		}
	}
	else
	{
		this->square[(int)move.to] = move.capture;
	}
}

void ChessBoard::moveKing(const Move & move)
{
	// check for castling
	if(!IS_MOVED(move.figure))
	{
		switch(move.to)
		{
			case G1:
				this->square[H1] = EMPTY;
				this->square[F1] = SET_MOVED(ROOK);
				break;
			case G8:
				this->square[H8] = EMPTY;
				this->square[F8] = SET_MOVED(SET_BLACK(ROOK));
				break;
			case C1:
				this->square[A1] = EMPTY;
				this->square[D1] = SET_MOVED(ROOK);
				break;
			case C8:
				this->square[A8] = EMPTY;
				this->square[D8] = SET_MOVED(SET_BLACK(ROOK));
				break;
			default:
				break;
		}
	}

	// regular move
	this->square[(int)move.from] = EMPTY;
	this->square[(int)move.to] = SET_MOVED(move.figure);
	
	// update king position variable
	if(IS_BLACK(move.figure)) {
		black_king_pos = move.to;
	}
	else {
		white_king_pos = move.to;
	}
}

void ChessBoard::undoMoveKing(const Move & move)
{
	// check for castling
	if(!IS_MOVED(move.figure))
	{
		// set rook depending on
		// king's target field
		switch(move.to)
		{
			case G1:
				this->square[H1] = ROOK;
				this->square[F1] = EMPTY;
				break;
			case G8:
				this->square[H8] = SET_BLACK(ROOK);
				this->square[F8] = EMPTY;
				break;
			case C1:
				this->square[A1] = ROOK;
				this->square[D1] = EMPTY;
				break;
			case C8:
				this->square[A8] = SET_BLACK(ROOK);
				this->square[D8] = EMPTY;
				break;
			default:
				break;
		}
	}

	// regular undo
	this->square[(int)move.from] = move.figure;
	this->square[(int)move.to] = move.capture;

	// update king position variable
	if(IS_BLACK(move.figure)) {
		black_king_pos = move.from;
	}
	else {
		white_king_pos = move.from;
	}
}
// Manually instantiate templates
void HelperFunction() {
    ChessBoard board;
    int color;
    list<Move> moves;

    MoveGenerator<false>::getMoves(board, color, moves, moves, moves);
    MoveGenerator<true>::getMoves(board, color, moves, moves, moves);

}
