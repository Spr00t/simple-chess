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

        ConsoleOpponent(const std::string & path, Config * config, int color);

        virtual ~ConsoleOpponent(){};

        virtual void prepare(const ChessBoard & board) override;
        virtual bool getMove(const ChessBoard & board, Move & move, AdvancedMoveData * move_data) override;

        virtual void showMove(const ChessBoard & board, Move & move) override;

        std::string readInput(void) const;

        /*
        * Process input. Frees buffer allocated by readInput()
        */
        bool processInput(const std::string& buf, Move & move) const;



    protected:
        mutable boost::process::ipstream pipe_out;
        boost::process::opstream pipe_in;
        boost::process::child process;

        std::string line;

};
