#include "message.h"
#include <map>
#include <ostream>
#include <boost/format.hpp>

using namespace boost;
using namespace std;

static const map<string, Message::Type> reversing(const map<Message::Type, string> typeMap) {
    map<string, Message::Type> res;
    for (pair<Message::Type, string> tm : typeMap) {
        res[tm.second] = tm.first;
    }
    return res;
}
static const std::map<Message::Type, string> typeMap = {
    {Message::NONE,        "unknown"},
    {Message::HELLO,       "name"},
    {Message::NEW_GAME,    "color"},
    {Message::MOVE,        "move"},
    {Message::RESULT,      "result"},
    {Message::MATCH_RESULT,"score"},

};

static std::map<string, Message::Type> typeMapReverse = reversing(typeMap);

optional<Message> Message::fromString(const std::string &text)
{
    Message message;
    size_t pos = text.find(": ");
    if (pos != string::npos) {
        string type_str = text.substr(0, pos);
        pos = text.find_first_not_of(' ', pos + 1);
        if (pos != string::npos) {
            string rest = text.substr(pos);
            auto it = typeMapReverse.find(type_str);
            if (it != typeMapReverse.end()) {
                message.type = it->second;
                message.data = rest;
                return TOptionalMessage(message);
            }
        }
    }
    return TOptionalMessage();
}

ostream &operator<<(ostream &os, const Message &m) {
    auto it = typeMap.find(m.type);
    if (it != typeMap.end()) {
        os << it->second << ": " << m.data << endl;
    } else {
        os << "unknown: " << m.data << endl;
    }
    return os;
}

Message Message::fromMove(const Move &move)
{
    Message m(MOVE, move.toString());
    return m;
}

Message Message::fromStatus(AsyncPlayer::EndStatus status)
{
    switch (status) {
    case AsyncPlayer::WHITE_WIN:
        return Message(Message::RESULT, "white_win");
    case AsyncPlayer::DRAW:
        return Message(Message::RESULT, "draw");
    case AsyncPlayer::WHITE_LOOSE:
        return Message(Message::RESULT, "white_loose");
    case AsyncPlayer::ERROR_WHITE:
        return Message(Message::RESULT, "error_white");
    case AsyncPlayer::ERROR_BLACK:
        return Message(Message::RESULT, "error_black");
    case AsyncPlayer::NONE:
        assert(false);
    }
    return Message();
}

Message Message::fromScore(Score score)
{
    Message message;
    message.type =  MATCH_RESULT;
    message.data = str(format("%1%/%2%") % score.score % score.maximum);
    return message;
}

Message::Message():
    type(NONE)
{
}

Message::Message(Message::Type type, const string &data):
    type(type),
    data(data)
{
}

TMoveOpt Message::toMove(const ChessBoard &board) const {
    TMoveOpt res;
    if (type == MOVE) {
        res = Move::fromString(board, data);
    }
    return res;
}

AsyncPlayer::EndStatus Message::toStatus() const {
    AsyncPlayer::EndStatus status = AsyncPlayer::NONE;
    if (type == RESULT) {
        if (data == "white_win") {
            status = AsyncPlayer::WHITE_WIN;
        } else if (data == "white_loose") {
            status = AsyncPlayer::WHITE_LOOSE;
        } else if (data == "draw") {
            status = AsyncPlayer::DRAW;
        } else if (data == "error_white") {
            status = AsyncPlayer::ERROR_WHITE;
        } else if (data == "error_black") {
            status = AsyncPlayer::ERROR_BLACK;
        }
    }
    return status;
}

Message::TScoreOpt Message::toScore() const {
    Score score;
    try {
        if (type == MATCH_RESULT) {
            int slash_pos = data.find('/');
            if (slash_pos != std::string::npos) {
                score.score   = lexical_cast<int>(data.substr(0, slash_pos));
                score.maximum = lexical_cast<int>(data.substr(slash_pos + 1));
                return TScoreOpt(score);
            }
        }
    } catch (const std::exception & e)  {
        cout << "E: parsing error";
    }
    return TScoreOpt();
}
