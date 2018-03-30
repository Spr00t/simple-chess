#include "client.h"

#include <exception>
#include <sstream>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

Client::Client(tcp::socket socket):
    protocol(make_shared<Protocol>(std::move(socket)))
{

}

void Client::start(Client::TReadyHandler handler)
{
    readyHandler = handler;
    Protocol::TMessageHandler messageHandler = std::bind(&Client::onGreetingCompleted, this, std::placeholders::_1);
    protocol->AsyncGetMessage(messageHandler);
}

void Client::startGameColor(int color, TReadyHandler handler)
{
    Message message(Message::NEW_GAME, color == WHITE ? "white" : "black");
    protocol->AsyncSendMessage(message, handler);
}

void Client::asyncGetNext(const ChessBoard &board, AsyncPlayer::MoveReadyHandler handler)
{
    protocol->AsyncGetMessage([board, handler, this](const Message & message){
        stringstream sstream;
        sstream << "Player " << name << " sent message: \"" << message << "\"" << endl;
        cout << sstream.str();
        Global::instance().log(sstream.str());

        TMoveOpt move_opt = message.toMove(board);
        if (NOT move_opt) {
            throw std::runtime_error("Invalid move came");
        }
        handler(*move_opt);
    });
}

void Client::asyncShowMove(const ChessBoard &board, const Move &move, AsyncPlayer::ReadyHandler handler)
{
    Message message = Message::fromMove(move);
    stringstream sstream;
    sstream << "Showing to " << name << " move: \"" << move.toString() << "\"" << endl;
    Global::instance().log(sstream.str());
    protocol->AsyncSendMessage(message, [handler](){
        handler();
    });
}

void Client::asyncShowResult(const ChessBoard &board, AsyncPlayer::EndStatus status, AsyncPlayer::ReadyHandler handler)
{
    Message message = Message::fromStatus(status);
    protocol->AsyncSendMessage(message, [handler](){
        handler();
    });
}

void Client::asyncShowMatchResult(Score score, AsyncPlayer::ReadyHandler handler)
{
    Message message = Message ::fromScore(score);
    protocol->AsyncSendMessage(message, [handler]() {
        handler();
    });

}

std::string Client::getName() const
{
    return name;
}

void Client::subscribeErrorHappened(Client::TErrorHandler handler)
{
    protocol->subscribeErrorHappened([this, handler](){
        has_error_ = true;
        handler();
    });
}

void Client::onGreetingCompleted(const Message & message)
{
    if (message.type != Message::HELLO) {
        throw runtime_error("Got wrong message");
    }
    name = message.data;
    readyHandler();
}

bool Client::hasError() const
{
    return has_error_;
}

