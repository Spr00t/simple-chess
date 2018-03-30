#pragma once
#include "chessboard.h"
#include "aiplayer.h"
#include "humanplayer.h"
#include "config.h"
#include "network_common/message.h"
#include "network_common/protocol.h"
#include "asyncplayer.h"
#include "asyncgame.h"


#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>
#include <functional>

class Client : public boost::noncopyable {
public:

    typedef std::function<void ()> TReadyHandler;
    typedef std::function<void ()> TErrorHandler;

    Client(boost::asio::ip::tcp::socket socket);

    void start(TReadyHandler handler);
    void startGameColor(int color, TReadyHandler handler);

    void asyncGetNext(const ChessBoard & board, AsyncPlayer::MoveReadyHandler handler);
    void asyncShowMove(const ChessBoard & board, const Move & move, AsyncPlayer::ReadyHandler handler);
    void asyncShowResult(const ChessBoard & board, AsyncPlayer::EndStatus status, AsyncPlayer::ReadyHandler handler);

    void asyncShowMatchResult(Score score, AsyncPlayer::ReadyHandler handler);

    std::string getName() const;

    void subscribeErrorHappened(TErrorHandler handler);

    bool hasError() const;

private:

    void onGreetingCompleted(const Message& message);

    ProtocolPtr protocol;

    TReadyHandler readyHandler;
    TErrorHandler errorHandler;
    std::string name;
    bool has_error_ = false;

};

typedef std::shared_ptr<Client> TClientPtr;
