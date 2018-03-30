#include "asyncaiplayer.h"
#include "config.h"

using namespace std;
using namespace boost::asio;
typedef std::shared_ptr<AsyncAiPlayerInternal> TServicePtr;
struct AsyncAiPlayerInternal {
    AsyncAiPlayerInternal(boost::asio::io_service * io, int color, int depth):
        player(color, depth),
        strand(*io)
    {

    }
    void start();
    AIPlayer player;
    // different strand than game strand
    boost::asio::io_service::strand strand;
};

AsyncAiPlayer::AsyncAiPlayer(int color, int depth):
    AsyncPlayer (color),
    color(color),
    depth(depth)
{

}

void AsyncAiPlayer::asyncPrepare(const ChessBoard &board, AsyncPlayer::ReadyHandler handler)
{
    service = make_shared<AsyncAiPlayerInternal>(&strand->get_io_service(), color, depth);

    strand->post(handler);
}

void AsyncAiPlayer::asyncGetNext(const ChessBoard &board, AsyncPlayer::MoveReadyHandler handler)
{
    shared_ptr<io_service::strand> game_strand = strand;
    TServicePtr sptr = service;
    // call in service strand
    service->strand.post([handler, board, game_strand, sptr]() {
       Move move;
       sptr->player.getMove(board, move);
       // game strand
       game_strand->post([move, handler](){
            handler(move);
       });
    });
}

void AsyncAiPlayer::asyncShowMove(const ChessBoard &board, const Move &move, AsyncPlayer::ReadyHandler handler)
{
    shared_ptr<io_service::strand> game_strand = strand;
    TServicePtr sptr = service;
    // call in service strand
    service->strand.post([handler, board, game_strand, sptr]() {
       Move move;
       sptr->player.showMove(board, move);
       // game strand
       game_strand->post([handler](){
            handler();
       });
    });
}

void AsyncAiPlayer::asyncShowResult(const ChessBoard &board, AsyncPlayer::EndStatus status, AsyncPlayer::ReadyHandler handler)
{
    // nothing to be done here really
    strand->post(handler);
}

