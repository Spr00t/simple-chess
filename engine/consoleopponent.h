#pragma once
#include <string>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/process/pipe.hpp>
#include <boost/process.hpp>

#include "chessplayer.h"

class ChessBoard;
struct Move;

class ConsoleOpponent : public ChessPlayer
{
    public:

        enum Status { Normal, InCheck, Stalemate, Checkmate };

        ConsoleOpponent(const std::string & path, int color);

        virtual ~ConsoleOpponent(){};

        /*
        * Ask player what to do next
        */
        virtual bool getMove(ChessBoard & board, Move & move, AdvancedMoveData * move_data) const override;

        virtual bool showMove(ChessBoard & board, Move & move) override;

        std::string readInput(void) const;

        /*
        * Process input. Frees buffer allocated by readInput()
        */
        bool processInput(const std::string& buf, Move & move) const;



    protected:

        /*
        * Guess what...
        */
        int color;

        mutable boost::process::ipstream pipe_out;
        boost::process::opstream pipe_in;
        boost::process::child process;

        std::string line;

};
