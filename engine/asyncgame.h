#pragma once
#include "asyncplayer.h"
#include "chessboard.h"
#include "timeout.h"
#include <memory>
#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/optional.hpp>

class AsyncGame;
typedef std::shared_ptr<AsyncGame> TAsyncGamePtr;
typedef std::weak_ptr<AsyncGame> TAsyncGameWeakPtr;
class AsyncGame : public std::enable_shared_from_this<AsyncGame>
{
public:

    struct TPlayerStruct {
        int id; //  1 or 2
        bool ready;
        bool end_ready;
    };

    typedef std::pair<std::shared_ptr<TPlayerStruct>, TAsyncPlayerPtr> TPlayer;
    typedef std::function<void (AsyncPlayer::EndStatus)> TGameResultHandler;
    typedef std::function<void (int color)> TGameShowedResultHandler;
    typedef std::function<void ()> TReadyHandler;

    AsyncGame(std::shared_ptr<boost::asio::io_service> io, TAsyncPlayerPtr player_white, TAsyncPlayerPtr player_black, int timer_mulitplier_);
    virtual ~AsyncGame();
    void start(TGameResultHandler result);
    void stopGame(AsyncPlayer::EndStatus status, bool show_white, bool show_black);

    static void staticStopGame(std::weak_ptr<AsyncGame> game, AsyncPlayer::EndStatus status, bool show_white, bool show_black) {
        TAsyncGamePtr gamePtr(game);
        if (gamePtr) gamePtr->stopGame(status, show_white, show_black);
    }

    const ChessBoard& getBoard() const;

private:
    enum WaitingStatus{
        FOR_WHITE,
        FOR_BLACK,
    };
    //id == 1/2
    static void staticOnPrepared(std::weak_ptr<AsyncGame> game, TPlayer pl) {
        TAsyncGamePtr gamePtr(game);
        if (gamePtr) gamePtr->onPrepared(pl);
    }

    static void staticOnMoveReady(std::weak_ptr<AsyncGame> game, TPlayer pl, const Move & move){
        TAsyncGamePtr gamePtr(game);
        if (gamePtr) gamePtr->onMoveReady(pl, move);
    }

    static void staticOnShowedReady(std::weak_ptr<AsyncGame> game, TPlayer pl) {
        TAsyncGamePtr gamePtr(game);
        if (gamePtr) gamePtr->onShowedReady(pl);
    }

    static void staticOnResultShowed(std::weak_ptr<AsyncGame> game, bool error, int color)
    {
        if (TAsyncGamePtr gamePtr = game.lock()) {
            if (gamePtr) gamePtr->onResultShowed(error, color);
        }
    }
    static void staticOnTimerExpired(std::weak_ptr<AsyncGame> game)
    {
        if (TAsyncGamePtr gamePtr = game.lock()) {
            if (gamePtr) gamePtr->expired_timer();
        }
    }

    void onPrepared(TPlayer pl);
    void onMoveReady(TPlayer pl, const Move & move);
    void onShowedReady(TPlayer );
    void onResultShowed(bool error, int color);
    void onErrorHappened();

    void showResults(AsyncPlayer::EndStatus, bool show_white, bool show_black);

    void expired_timer();
    bool stop = false;
    ChessBoard board;
    TPlayer players[2];
    TGameResultHandler resultHandler;

    std::shared_ptr<boost::asio::io_service> io;
    std::shared_ptr<boost::asio::io_service::strand> flow;

    Timeout timeout;

    boost::optional<AsyncPlayer::EndStatus> endStatus;
    WaitingStatus waiting_status_;

    int readiness = 0;
    int timer_mulitplier_ = 1;
};


