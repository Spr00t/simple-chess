#pragma once
#include <boost/optional.hpp>

class ChessBoard;
struct Move;
struct AdvancedMoveData
{
    int board_evaluation = 0;
};
class Config;
class ChessPlayer
{
	public:

        enum Status { Normal, InCheck, Stalemate, Checkmate, Draw };

        ChessPlayer(Config * config, int color)
         : config(config),
           color(color)
		{};

		virtual ~ChessPlayer(){};
		


        virtual void prepare(const ChessBoard & board) = 0;
        virtual bool getMove(const ChessBoard & board, Move & move, AdvancedMoveData * move_data = nullptr) = 0;
        virtual void showMove(const ChessBoard & board, Move & move) = 0;

        int getColor() const
        {
            return color;
        }
        void setColor(int value)
        {
            color = value;
        }

protected:
        Config * config;
        /*
        * Guess what...
		*/
        int color;
};





