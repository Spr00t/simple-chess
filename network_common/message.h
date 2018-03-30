#pragma once
#include "engine/chessboard.h"
#include "engine/asyncgame.h"
#include "network_common/resultstable.h"

#include <string>
#include <boost/optional.hpp>
#include <string>

class Message;
typedef boost::optional<Message> TOptionalMessage;
class Message
{
public:
    enum Type {
        NONE,
        HELLO,
        NEW_GAME,
        MOVE,
        RESULT,
        MATCH_RESULT
    };

    typedef boost::optional<Score> TScoreOpt;

    static TOptionalMessage fromString(const std::string & text);
    static Message fromMove(const Move & move);
    static Message fromStatus(AsyncPlayer::EndStatus status);
    static Message fromScore(Score score);

    Message();

    Message (Type type, const std::string & data);

    TMoveOpt toMove(const ChessBoard & board) const;

    AsyncPlayer::EndStatus toStatus() const;
    TScoreOpt toScore() const;

    Type type = NONE;
    std::string data;
};
std::ostream  & operator<<(std::ostream & os, const Message & m);

