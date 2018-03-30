#pragma once
#include "asyncplayer.h"
#include "network_common/protocol.h"


#include <boost/asio.hpp>

class ServerPlayer : public AsyncPlayer
{
public:
    ServerPlayer(int color, ProtocolPtr protocolPtr);
    void subscribeResultReady(ResultReadyHandler);

    virtual void asyncPrepare(const ChessBoard & board, ReadyHandler handler) override;
    virtual void asyncGetNext(const ChessBoard & board, MoveReadyHandler handler) override;
    virtual void asyncShowMove(const ChessBoard & board, const Move & move, ReadyHandler handler) override;
    virtual void asyncShowResult(const ChessBoard & board, EndStatus status, ReadyHandler handler) override;

private:
    int color;
    ProtocolPtr protocol_ptr_;
    ResultReadyHandler result_ready_handler_;
};
