#ifndef CHESS_PLAYER_H_INCLUDED
#define CHESS_PLAYER_H_INCLUDED

class ChessBoard;
struct Move;
struct AdvancedMoveData
{
    int board_evaluation = 0;
};
class ChessPlayer
{
	public:

        enum Status { Normal, InCheck, Stalemate, Checkmate, Draw };

		ChessPlayer(int color)
		 : color(color)
		{};

		virtual ~ChessPlayer(){};
		
		/*
		* Ask player what to do next
		*/
        virtual bool getMove(ChessBoard & board, Move & move, AdvancedMoveData * move_data) const = 0;
        virtual bool showMove(ChessBoard & board, Move & move) = 0;


	protected:

		/*
		* Guess what...
		*/
        int color;
};

#endif
