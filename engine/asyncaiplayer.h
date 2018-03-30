#include "asyncplayer.h"
#include "asyncgame.h"
#include "aiplayer.h"
#include <memory>

struct AsyncAiPlayerInternal;

class AsyncAiPlayer : public AsyncPlayer
{
public:
    AsyncAiPlayer(int color, int depth);

    virtual void asyncPrepare(const ChessBoard & board, ReadyHandler handler) override;
    virtual void asyncGetNext(const ChessBoard & board, MoveReadyHandler handler) override;
    virtual void asyncShowMove(const ChessBoard & board, const Move & move, ReadyHandler handler) override;
    virtual void asyncShowResult(const ChessBoard & board, EndStatus status, ReadyHandler handler) override;

private:
    int color;
    int depth;
    std::shared_ptr<AsyncAiPlayerInternal> service;
};
