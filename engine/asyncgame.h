#pragma once
#include "asyncplayer.h"
#include "chessboard.h"
#include "timeout.h"
#include <memory>
#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/optional.hpp>

class AsyncGame
{
public:

    struct TPlayerStruct {
        int id; //  1 or 2
        bool ready;
        bool end_ready;
    };

    typedef std::pair<std::shared_ptr<TPlayerStruct>, TAsyncPlayerPtr> TPlayer;
    typedef std::function<void (AsyncPlayer::EndStatus)> TGameResultHandler;

    AsyncGame(std::shared_ptr<boost::asio::io_service> io, TAsyncPlayerPtr player_white, TAsyncPlayerPtr player_black);
    virtual ~AsyncGame();
    void start(TGameResultHandler result);
    void stopGame();

    const ChessBoard& getBoard() const;

private:
    //id == 1/2
    void onPrepared(TPlayer pl);
    void onMoveReady(TPlayer pl, const Move & move);
    void onShowedReady(TPlayer );
    void onResultShowed(TPlayer pl);

    void showResults(AsyncPlayer::EndStatus);

    void expired_timer();
    bool stop = false;
    ChessBoard board;
    TPlayer players[2];
    TGameResultHandler resultHandler;

    std::shared_ptr<boost::asio::io_service> io;
    std::shared_ptr<boost::asio::io_service::strand> flow;

    Timeout timeout;

    boost::optional<AsyncPlayer::EndStatus> endStatus;

    int readiness = 0;
};
typedef std::shared_ptr<AsyncGame> TAsyncGamePtr;

