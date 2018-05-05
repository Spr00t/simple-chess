#include "chessboard.h"
#include "aiplayer.h"
#include "humanplayer.h"
#include "config.h"
#include "asyncgame.h"
#include <sstream>

using namespace boost::posix_time;
using namespace std;
using namespace boost::asio;

//static const int time_multiplier = 1;

static const int time_multiplier = 2000; // for debug purposes

AsyncGame::AsyncGame(std::shared_ptr<boost::asio::io_service> io, std::shared_ptr<AsyncPlayer> player1, std::shared_ptr<AsyncPlayer> player2):
    io(io),
    timeout(*io)
{
    TPlayerStruct plStrct;
    flow = make_shared<io_service::strand>(*io);

    plStrct = {0, false/*ready*/, false/*end ready*/};
    players[0] = {make_shared<TPlayerStruct>(plStrct), player1};

    plStrct = {1, false/*ready*/, false/*end ready*/};
    players[1] = {make_shared<TPlayerStruct>(plStrct), player2};

    players[0].second->setStrand(flow);
    players[1].second->setStrand(flow);

}
AsyncGame::~AsyncGame()
{
    cout << "AsyncGame dtor!!!" << endl;
}

void AsyncGame::start(TGameResultHandler handler)
{
    // create board and default initialise
    board.initDefaultSetup();

    // Create game config
    Config config;
    config.mode = Mode::Human;

    players[0].second->setColor(WHITE);
    players[1].second->setColor(BLACK);
    for (TPlayer & pl : players ) {
        pl.second->asyncPrepare(board, flow->wrap(std::bind(&AsyncGame::onPrepared, this, pl)));
    }
    stop = false;
    resultHandler = handler;
}

void AsyncGame::stopGame()
{
    stop = true;
    //resultHandler = TGameResultHandler();
}

void AsyncGame::onPrepared(TPlayer /*pl*/)
{
    if (stop)
        return;
    readiness++;
    if (readiness == 2) {
        players[0].second->asyncGetNext(board, flow->wrap(bind(&AsyncGame::onMoveReady, this, players[0], placeholders::_1)));
    }
    timeout.async_wait(flow->wrap(bind(&AsyncGame::expired_timer, this)), seconds(60 * 2 * time_multiplier));
}

void AsyncGame::onShowedReady(AsyncGame::TPlayer /*pl*/)
{
    if (stop)
        return;

    ChessPlayer::Status status = board.getPlayerStatus(board.next_move_color);
    AsyncPlayer::EndStatus endStatus;
    stringstream log_status_str;
    log_status_str << "Chessplayer status:" << status;
    Global::instance().log(log_status_str.str());
    stringstream sstr;

    switch (status) {
        case ChessPlayer::Checkmate:
            endStatus = (players[1].second->getColor() == WHITE ? AsyncPlayer::WHITE_LOOSE : AsyncPlayer::WHITE_WIN);
            showResults(endStatus);
            stop = true;
            break;
        case ChessPlayer::Stalemate:
        case ChessPlayer::Draw:
            showResults(AsyncPlayer::DRAW);
            break;
        default:
            sstr << "On showed ready: ";
            Global::instance().log(sstr.str());
            players[1].second->asyncGetNext(board, flow->wrap(bind(&AsyncGame::onMoveReady, this, players[1], placeholders::_1)));
            std::swap(players[0], players[1]);
            break;
    }
    timeout.async_wait(flow->wrap(bind(&AsyncGame::expired_timer, this)), seconds(5 * time_multiplier));

}

void AsyncGame::onMoveReady(AsyncGame::TPlayer /*pl*/, const Move &move)
{
    if (stop)
        return;
    stringstream sstr;
    sstr << "On move ready: " << move.toString();
    Global::instance().log(sstr.str());

    board.move(move);
    board.print(move);
    cout << board.toFEN() << endl;

    players[1].second->asyncShowMove(board, move, flow->wrap(std::bind(&AsyncGame::onShowedReady, this, players[1])));

    timeout.async_wait(flow->wrap(bind(&AsyncGame::expired_timer, this)), seconds(5 * time_multiplier));
}

void AsyncGame::onResultShowed(AsyncGame::TPlayer pl)
{
    pl.first->end_ready = true;
    if (players[0].first->end_ready && players[1].first->end_ready ) {
        assert(endStatus);
        resultHandler(*endStatus);
        stop = true;
    }
}

void AsyncGame::showResults(AsyncPlayer::EndStatus status)
{
    stop = true;

    endStatus = status;

    players[0].second->asyncShowResult(board, status, bind(&AsyncGame::onResultShowed, this, players[0]));
    players[1].second->asyncShowResult(board, status, bind(&AsyncGame::onResultShowed, this, players[1]));

    timeout.stop();
}

void AsyncGame::expired_timer()
{
    if (stop)
        return;

    if (players[0].first->ready && players[1].first->ready) {

        int id = (board.next_move_color == WHITE ? 0 : 1);

        stringstream sstr;
        sstr << "Timer expired player on players move" << id;
        Global::instance().log(sstr.str());
        AsyncPlayer::EndStatus status = (board.next_move_color == WHITE ? AsyncPlayer::WHITE_WIN : AsyncPlayer::WHITE_LOOSE);
        showResults(status);

    } else if (!players[0].first->ready && !players[1].first->ready) {
        showResults(AsyncPlayer::DRAW);
    } else if (!players[0].first->ready) {
        showResults(AsyncPlayer::WHITE_LOOSE);
    } else {
        showResults(AsyncPlayer::WHITE_WIN);
    }
    timeout.stop();
    stop = true;
}

const ChessBoard &AsyncGame::getBoard() const
{
    return board;
}
