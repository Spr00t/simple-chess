#include "serverplayer.h"

#include <boost/format.hpp>

using namespace std;
using namespace boost;

ServerPlayer::ServerPlayer(int color, ProtocolPtr protocolPtr):
    AsyncPlayer (color),
    color(color),
    protocol_ptr_(protocolPtr)
{

}

void ServerPlayer::subscribeResultReady(AsyncPlayer::ResultReadyHandler handler)
{
    result_ready_handler_ = handler;
}

void ServerPlayer::asyncPrepare(const ChessBoard &board, AsyncPlayer::ReadyHandler handler)
{
    strand->post(handler);
}

void ServerPlayer::asyncGetNext(const ChessBoard &board, AsyncPlayer::MoveReadyHandler handler)
{
    protocol_ptr_->AsyncGetMessage(strand->wrap([board, handler, this](const Message & message){
        cout << "AsyncGetMessage:  " << message << endl;
        if (message.type == Message::MOVE) {
            TMoveOpt move_opt = message.toMove(board);
            if (NOT move_opt) {
                cout << "!!!!! invalid situation: invalid move came!!!!!\n";
                board.print();
                cout << board.toFEN();
                throw std::runtime_error(str(format("Invalid move came: %1%") % message));
            }
            handler(*move_opt);
        } else if (message.type == Message::RESULT) {
            AsyncPlayer::EndStatus status = message.toStatus();
            if (status) {
                result_ready_handler_(status);
            }
        }
    }));
}

void ServerPlayer::asyncShowMove(const ChessBoard &board, const Move &move, AsyncPlayer::ReadyHandler handler)
{
//    shared_ptr<io_service::strand> game_strand = strand;
//    TServicePtr sptr = service;
//    // call in service strand
//    service->strand.post([handler, board, game_strand, sptr]() {
//       Move move;
//       sptr->player.showMove(board, move);
//       // game strand
//       game_strand->post([handler](){
//            handler();
//       });
//    });

    Message message = Message::fromMove(move);
    cout << "asyncShowMove:  " << message << endl;

    protocol_ptr_->AsyncSendMessage(message, strand->wrap([handler](){
        handler();
    }));
}

void ServerPlayer::asyncShowResult(const ChessBoard &/*board*/, AsyncPlayer::EndStatus /*status*/, AsyncPlayer::ReadyHandler handler)
{
//    // nothing to be done here really
    strand->post(handler);
}


