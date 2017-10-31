#include <cstdlib>
#include <ctime>
#include <list>
#include <vector>
#include <algorithm>
#include "aiplayer.h"
#include "chessboard.h"

using namespace std;

#ifdef TRACE
static const int QUISCENT_DEPTH = 4;
#else
static const int QUISCENT_DEPTH = 6;
#endif
AIPlayer::AIPlayer(int color, int search_depth)
 : ChessPlayer(color),
   search_depth(search_depth)
{
	srand(time(NULL));
}

AIPlayer::~AIPlayer()
{}

bool AIPlayer::getMove(ChessBoard & board, Move & move) const
{
	vector<Move> candidates;
    list<Move> regulars, simple, captures, nulls;

#ifdef TRACE
    vector<list<Move>> best_chain_candidates;
    list<Move> chain, moved;
#endif
    bool quiescent = false;
    int best_value, tmp;

    int target_depth = this->search_depth - 1;

    if (board.get_all_figures_count() < 10) {
        //target_depth++;
    }
    if (board.get_all_figures_count() <= 5) {
        //target_depth++;
    }

	// first assume we are loosing
    best_value = -KING_VALUE;

	// get all moves
    MoveGenerator<false>::getMoves(board, board.next_move_color, simple, captures, nulls);
    regulars.swap(captures);
    //instead of copying
    copy(simple.begin(), simple.end(), back_inserter(regulars));

	// execute maintenance moves
	for(list<Move>::iterator it = nulls.begin(); it != nulls.end(); ++it)
		board.move(*it);

	// loop over all moves
	for(list<Move>::iterator it = regulars.begin(); it != regulars.end(); ++it)
	{
		// execute move
        board.move(*it);

#ifdef TRACE
        moved.push_back(*it);
#endif

        bool current_king_vulnerable   = board.isVulnerable((board.next_move_color ? board.black_king_pos : board.white_king_pos), board.next_move_color);
        bool previous_king_vulnerable = board.isVulnerable((board.next_move_color ? board.white_king_pos : board.black_king_pos), TOGGLE_COLOR(board.next_move_color));

		// check if own king is vulnerable now
        if(NOT previous_king_vulnerable) {

            if((*it).capture != EMPTY || previous_king_vulnerable || current_king_vulnerable)
                quiescent = true;
            else
                quiescent = false;

#ifdef TRACE
            chain.clear();
            Global::instance().log(string("Try move: ") + it->toString());

#endif
			// recursion
            tmp = -evalAlphaBeta(board, target_depth, -WIN_VALUE, -best_value, quiescent
#ifdef TRACE
                         ,&moved
                         ,&chain
#endif
                                 );
#ifdef TRACE
            stringstream sstr;
            Global::instance().log("=============================================");
            sstr << "Figures count: " << board.get_all_figures_count() << endl;
            sstr << "Depth: "         << target_depth << endl;

            sstr << "Fifty moves left: " << board.fifty_moves << endl;
            sstr << "Available move (" << tmp << ")" << it->toString()
                                    << " because of next chain: ";
            for (Move & m: chain) {
                sstr << m.toString() << "; ";
            }
            Global::instance().log(sstr.str());
            Global::instance().log("=============================================");
#endif
            if(tmp > best_value) {
                best_value = tmp;
#ifdef TRACE
                best_chain_candidates.clear();
                best_chain_candidates.push_back(chain);
#endif
				candidates.clear();
				candidates.push_back(*it);
			}
            else if(tmp == best_value) {
				candidates.push_back(*it);
#ifdef TRACE

                best_chain_candidates.push_back(chain);
#endif
			}
		}

		// undo move and inc iterator
		board.undoMove(*it);
#ifdef TRACE
        moved.pop_back();
#endif
	}

	// undo maintenance moves
	for(list<Move>::iterator it = nulls.begin(); it != nulls.end(); ++it)
		board.undoMove(*it);

	// loosing the game?
    if(best_value < -WIN_VALUE) {
		return false;
	}
	else {
		// select random move from candidate moves
        int select = rand() % candidates.size();
        move = candidates[select];
#ifdef TRACE
        stringstream tmp;
        Global::instance().log("=============================================");
        tmp << "Figures count: " << board.get_all_figures_count() << endl;
        tmp << "Depth: "         << target_depth << endl;

        tmp << "Fifty moves left: " << board.fifty_moves << endl;
        tmp << "Selected move (" << best_value << ")" << move.toString()
                                << " because of next chain: ";
        for (Move & move: best_chain_candidates[select]) {
            tmp << move.toString() << "; ";
        }
        Global::instance().log(tmp.str());
        Global::instance().log("=============================================");
#endif
		return true;
	}
}

int AIPlayer::evalAlphaBeta(ChessBoard & board, int search_depth, int alpha, int beta, bool quiescent_search
#ifdef TRACE
                          , std::list<Move> * moved
                          , std::list<Move> * best_chain
#endif
) const
{
#ifdef TRACE
    list<Move> chain;
    Move best_move = EMPTY_MOVE;
#endif
    list<Move> regulars, simple, ignored, captures, nulls;
    int best_value, tmp;

    bool long_depth = false, checkmate;
    if(search_depth <= 0 && !quiescent_search) {

        return +evaluateBoard(board);
    } else if (quiescent_search && search_depth <= -QUISCENT_DEPTH) {
        //limit maximum recursion
        return +evaluateBoard(board);
    } else if (quiescent_search && search_depth <= 0) {
        long_depth = true;
    }

	// first assume we are loosing
    best_value = -WIN_VALUE + 50 - board.fifty_moves; // in case we are winning lets win less moves

    if (long_depth && !quiescent_search) {
        // get only captures
        MoveGenerator<true>::getMoves(board, board.next_move_color, ignored, regulars, nulls);
    } else {
        MoveGenerator<false>::getMoves(board, board.next_move_color, simple, captures, nulls);
        //instead of copying
        regulars.swap(captures);
        copy(simple.begin(), simple.end(), back_inserter(regulars));
    }

	
	// execute maintenance moves
	for(list<Move>::iterator it = nulls.begin(); it != nulls.end(); ++it)
		board.move(*it);


    // assume we have a state_mate
    bool stalemate = true;

    bool our_king_vulnerable_before = board.isVulnerable(board.next_move_color ? board.black_king_pos : board.white_king_pos, board.next_move_color);

    if (our_king_vulnerable_before) {
        checkmate = true;
    } else {
        checkmate = false;
    }

	// loop over all moves
    for(list<Move>::iterator it = regulars.begin();
		alpha <= beta && it != regulars.end(); ++it)
    {
		// execute move
        board.move(*it);
#ifdef TRACE
        moved->push_back(*it);

        chain.clear();
        {
            static int break_counter = 0;
            stringstream trace;
            break_counter++;
            trace << break_counter << ": Try submove:";
            for (Move & move : *moved) {
                trace << move.toString() + "->";
            }
            Global::instance().log(trace.str());
            if (break_counter == 25) {
                board.print(moved->back());
                Global::instance().log("");
            }
        }
#endif
        int current_king_pos = board.next_move_color ? board.black_king_pos : board.white_king_pos;
        int previous_king_pos = board.next_move_color ? board.white_king_pos : board.black_king_pos;

        bool is_invalid_move = false;

        //check if king is near the other king which is invalid
        if (abs(current_king_pos - previous_king_pos) == 1
                || (abs(current_king_pos - previous_king_pos) >= 7 && abs(current_king_pos - previous_king_pos) <= 9)
                ) {
            is_invalid_move = true;
        }

        bool current_king_vulnerable  = board.isVulnerable(current_king_pos, board.next_move_color);
        bool previous_king_vulnerable = board.isVulnerable(previous_king_pos, TOGGLE_COLOR(board.next_move_color));

        //make a move and making a king vulnerable is illegal
        if (previous_king_vulnerable) {
            is_invalid_move = true;
        }

        if (NOT is_invalid_move && NOT previous_king_vulnerable) {
            checkmate = false;
            stalemate = false;
        }

		// check if own king is vulnerable now
        if(NOT is_invalid_move) {
            bool quiescent = false;
            if((*it).capture != EMPTY || current_king_vulnerable || previous_king_vulnerable)
                quiescent = true;
            else
                quiescent = false;

            if (board.fifty_moves <= 0) {
                tmp = 0;
            } else {
                // recursion 'n' pruning
                tmp = -evalAlphaBeta(board, search_depth - 1, -beta, -alpha, quiescent
#ifdef TRACE
                                     , moved, &chain
#endif
                                     );
            }
#ifdef TRACE
            {
                stringstream trace;
                for (Move & move : *moved) {
                    trace << move.toString() + "->";
                }
                trace << tmp;
                Global::instance().log(trace.str());
            }
#endif
            if(tmp > best_value) {
                best_value = tmp;
#ifdef TRACE

                *best_chain = chain;
                best_move = *it;
#endif
				if(tmp > alpha) {
					alpha = tmp;
				}
			}
		}

		// undo move and inc iterator
		board.undoMove(*it);
#ifdef TRACE
        moved->pop_back();
#endif
	}
	
	// undo maintenance moves
	for(list<Move>::iterator it = nulls.begin(); it != nulls.end(); ++it)
		board.undoMove(*it);
#ifdef TRACE
    if (best_move.figure) {
        best_chain->push_front(best_move);
    }
#endif
    if (checkmate) {
        // it is not stalemate :), this is checkmate
        stalemate = false;
    }
    //stalemate is not so bad :)
    return stalemate == true ? 0 : best_value;
}

int AIPlayer::evaluateBoard(const ChessBoard & board) const
{//A7G7->G8F8->F6E6->F8G7->E6D7
    int figure, pos, sum = 0, summand, row, col, edge_distance_row, edge_distance_col;
    static int br_counter = 0;
    br_counter ++;
    stringstream sstr;
    sstr << "BREAK EV: " << br_counter;
    if (false && br_counter == 9) {
        board.print();
        Global::instance().log("");
    }
    Global::instance().log(sstr.str());
    int black_sum = 0, white_sum = 0;
	for(pos = 0; pos < 64; pos++)
	{
		figure = board.square[pos];
		switch(FIGURE(figure))
		{
			case PAWN:
				summand = PAWN_VALUE;
                row = pos / 8;
                if (IS_BLACK(figure)) {
                    summand += 6 - row;
                } else {
                    summand += row - 1;
                }
				break;
			case ROOK:
				summand = ROOK_VALUE;
				break;
			case KNIGHT:
				summand = KNIGHT_VALUE;
				break;
			case BISHOP:
				summand = BISHOP_VALUE;
				break;
			case QUEEN:
				summand = QUEEN_VALUE;
				break;
			default:
				summand = 0;
				break;
		}
        if (IS_BLACK(figure)) {
            black_sum +=  summand;
        } else {
            white_sum +=  summand;
        }
	}
    struct {
        int pos;
        int opp_pos;
        int color;
        int figure;
        int * value;
    } kings[2] = {
        {board.white_king_pos, board.black_king_pos, WHITE, board.square[board.white_king_pos], &white_sum},
        {board.black_king_pos, board.white_king_pos, BLACK, board.square[board.black_king_pos], &black_sum}
    };
    for (int i = 0; i < 2; i++) {

        if (kings[i].figure) {
            if (*kings[i].value < ROOK_VALUE) {
                // in case we have small amount of figures it is better to have a king nearer to the center
                row = kings[i].pos / 8;
                col = kings[i].pos % 8;

                edge_distance_row = min(row, 7 - row);
                edge_distance_col = min(col, 7 - col);

                int additional_minimum = min(edge_distance_row, edge_distance_col);

                *kings[i].value += additional_minimum + edge_distance_row + edge_distance_col - 6;

                // lets got also a fee to be as far from the opponent king
                int opp_row = kings[i].opp_pos / 8;
                int opp_col = kings[i].opp_pos % 8;

                int king_distance = abs(row - opp_row) + abs(col - opp_col);
                *kings[i].value += king_distance;
            }
            *kings[i].value += KING_VALUE;
        }
    }
//    // it is
//    sum -= board.fifty_moves;

    if (IS_BLACK(this->color) )
        sum = black_sum - white_sum;
    else
        sum = white_sum - black_sum;

	
    return (board.next_move_color != this->color ? -sum : sum);
}

