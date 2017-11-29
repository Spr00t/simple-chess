#ifndef AI_PLAYER_H_INCLUDED
#define AI_PLAYER_H_INCLUDED

#include "chessplayer.h"
#include <global.h>
#include <list>


// Pieces' values
#define WIN_VALUE  50000	// win the game
#define PAWN_VALUE    30	// 8x
#define ROOK_VALUE    90	// 2x
#define KNIGHT_VALUE  85	// 2x
#define BISHOP_VALUE  84	// 2x
#define QUEEN_VALUE  300	// 1x
#define KING_VALUE 	 ((PAWN_VALUE * 8) + (ROOK_VALUE * 2) \
						+ (KNIGHT_VALUE * 2) + (BISHOP_VALUE * 2) + QUEEN_VALUE + WIN_VALUE)

class ChessBoard;

struct EvaluationInformation {

};

class AIPlayer: public ChessPlayer {

	public:
	
		AIPlayer(int color, int search_depth);

		~AIPlayer();

		/*
		* Ask player what to do next
		*/
        bool getMove(ChessBoard & board, Move & move) const override;

        bool getMove(ChessBoard & board, Move & move) const override;

        bool showMove(ChessBoard & board, Move & move) override {return true;}

		/*
		* MinMax search for best possible outcome
		*/ 
        int evalAlphaBeta(ChessBoard & board, int depth, int alpha, int beta, bool quiescent
#ifdef TRACE
                          , std::list<Move> * moved
                          , std::list<Move> * best
#endif
                          ) const;

		/*
		* For now, this checks only material
		*/
		int evaluateBoard(const ChessBoard & board) const;
	
	protected:

		/*
		* how deep to min-max
		*/
		int search_depth;
};

#endif
