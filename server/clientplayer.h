#include "asyncplayer.h"
#include "asyncgame.h"
#include "aiplayer.h"
#include "client.h"

#include <memory>

struct AsyncClientPlayerInternal;

class AsyncClientPlayer : public AsyncPlayer
{
public:
    AsyncClientPlayer(TClientPtr ptr, int color);

    virtual void asyncPrepare(const ChessBoard & board, ReadyHandler handler) override;
    virtual void asyncGetNext(const ChessBoard & board, MoveReadyHandler handler) override;
    virtual void asyncShowMove(const ChessBoard & board, const Move & move, ReadyHandler handler) override;
    virtual void asyncShowResult(const ChessBoard & board, EndStatus status, ReadyHandler handler) override;

private:
    int color;
    TClientPtr clientPtr;
};
