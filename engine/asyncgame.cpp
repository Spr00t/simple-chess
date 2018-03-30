#include "chessboard.h"
#include "aiplayer.h"
#include "humanplayer.h"
#include "config.h"
#include "asyncgame.h"
#include <sstream>

using namespace boost::posix_time;
using namespace std;
using namespace boost::asio;

AsyncGame::AsyncGame(std::shared_ptr<boost::asio::io_service> io, std::shared_ptr<AsyncPlayer> player1, std::shared_ptr<AsyncPlayer> player2, int time_multiplier):
    io(io),
    timeout(*io),
    timer_mulitplier_(time_multiplier)
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

void AsyncGame::stopGame(AsyncPlayer::EndStatus status, bool show_white, bool show_black)
{
    stop = true;
    if (show_white) {
        int color = board.next_move_color;
        /// show results in case of error only for next player
        showResults(status, show_white, show_black);
    }
    //resultHandler = TGameResultHandler();
}

void AsyncGame::onPrepared(TPlayer pl)
{
    if (stop)
        return;
    readiness++;
    pl.first->ready = true;
    if (readiness == 2) {
        players[0].second->asyncGetNext(board, flow->wrap(bind(&AsyncGame::onMoveReady, this, players[0], placeholders::_1)));
        waiting_status_ = (board.next_move_color == WHITE ? FOR_WHITE: FOR_BLACK);
    }
    timeout.async_wait(flow->wrap(bind(&AsyncGame::staticOnTimerExpired, shared_from_this())), seconds(60 * 2 * timer_mulitplier_));
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
            showResults(endStatus, true, true);
            stop = true;
            break;
        case ChessPlayer::Stalemate:
        case ChessPlayer::Draw:
            showResults(AsyncPlayer::DRAW, true, true);
        default:
            sstr << "On showed ready: ";
            Global::instance().log(sstr.str());
            players[1].second->asyncGetNext(board, flow->wrap(bind(&AsyncGame::onMoveReady, this, players[1], placeholders::_1)));
            waiting_status_ = (board.next_move_color == WHITE ? FOR_WHITE: FOR_BLACK);
            std::swap(players[0], players[1]);
            break;
    }
    timeout.async_wait(flow->wrap(bind(&AsyncGame::staticOnTimerExpired, shared_from_this())), seconds(10 * timer_mulitplier_));

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
    waiting_status_ = (board.next_move_color == WHITE ? FOR_WHITE: FOR_BLACK);

    timeout.async_wait(flow->wrap(bind(&AsyncGame::staticOnTimerExpired, shared_from_this())), seconds(5 * timer_mulitplier_));
}

void AsyncGame::onResultShowed(bool error, int color)
{
    int number = (color == WHITE ? 0 : 1);
    players[number].first->end_ready = true;
    if (error || (players[0].first->end_ready && players[1].first->end_ready) ) {
        assert(endStatus);
        resultHandler(*endStatus);
        stop = true;
    }
}


void AsyncGame::showResults(AsyncPlayer::EndStatus status, bool show_white, bool show_black)
{
    stop = true;

    endStatus = status;
    bool is_error = (status == AsyncPlayer::ERROR_BLACK || status == AsyncPlayer::ERROR_WHITE);

    int pl1_color = players[0].second->getColor();
    int pl2_color = players[1].second->getColor();
    if ((show_white && pl1_color == WHITE) || (show_black && pl1_color == BLACK))
        players[0].second->asyncShowResult(board, status, bind(staticOnResultShowed, shared_from_this(), is_error, WHITE));

    if ((show_white && pl2_color == WHITE) || (show_black && pl2_color == BLACK))
        players[1].second->asyncShowResult(board, status, bind(staticOnResultShowed, shared_from_this(), is_error, BLACK));

    timeout.stop();
}

void AsyncGame::expired_timer()
{
    if (stop)
        return;

    if (players[0].first->ready && players[1].first->ready) {
        int id = (board.next_move_color == WHITE ? 0 : 1);
        stringstream sstr;
        sstr << "Timer expired player on players move: " << (id ? "BLACK" : "WHITE");
        Global::instance().log(sstr.str());
        AsyncPlayer::EndStatus status = (board.next_move_color == WHITE ? AsyncPlayer::ERROR_WHITE : AsyncPlayer::ERROR_BLACK);
        flow->post(bind(&AsyncGame::staticStopGame, shared_from_this(), status, true, true));
    } else if (!players[0].first->ready && !players[1].first->ready) {
        showResults(AsyncPlayer::DRAW, true, true);
    } else if (!players[0].first->ready) {
        showResults(AsyncPlayer::WHITE_LOOSE, true, true);
    } else {
        showResults(AsyncPlayer::WHITE_WIN, true, true);
    }
    timeout.stop();
    stop = true;
}

const ChessBoard &AsyncGame::getBoard() const
{
    return board;
}
