#ifndef HUMAN_PLAYER_H_INCLUDED
#define HUMAN_PLAYER_H_INCLUDED

#include <string>
#include <boost/optional.hpp>
#include "chessplayer.h"

class ChessBoard;
struct Move;

class HumanPlayer: public ChessPlayer {

	public:
	
        HumanPlayer(int color, bool slaveMode);
		
		~HumanPlayer();
		
		/*
		* Ask player what to do
		*/
        virtual bool getMove(ChessBoard & board, Move & move, AdvancedMoveData * move_data) const override;

        virtual bool showMove(ChessBoard & board, Move & move) override {return true;}

		
		/*
		* Read input from stdin
		*/
        std::string readInput() const;
		
		/*
		* Process input. Frees buffer allocated by readInput()
		*/
        bool processInput(const std::string &buf, Move & move) const;
private:
        bool slaveMode;
};

#endif
