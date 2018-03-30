#include "clientplayer.h"
#include "config.h"

using namespace std;
using namespace boost::asio;


AsyncClientPlayer::AsyncClientPlayer(TClientPtr ptr, int color):
    AsyncPlayer (color),
    color(color),
    clientPtr(ptr)
{

}

void AsyncClientPlayer::asyncPrepare(const ChessBoard &board, AsyncPlayer::ReadyHandler handler)
{
    clientPtr->startGameColor(color, handler);
}

void AsyncClientPlayer::asyncGetNext(const ChessBoard &board, AsyncPlayer::MoveReadyHandler handler)
{
    clientPtr->asyncGetNext(board, handler);
}

void AsyncClientPlayer::asyncShowMove(const ChessBoard &board, const Move &move, AsyncPlayer::ReadyHandler handler)
{
    clientPtr->asyncShowMove(board, move, handler);
}

void AsyncClientPlayer::asyncShowResult(const ChessBoard &board, AsyncPlayer::EndStatus status, AsyncPlayer::ReadyHandler handler)
{
    clientPtr->asyncShowResult(board, status, handler);
}
