#include "network_common/protocol.h"
#include "global.h"
#include "serverplayer.h"
#include "engine/asyncaiplayer.h"

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <thread>
#include <boost/asio/io_service.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

using namespace boost::asio::ip;
using namespace boost;
using namespace boost::asio;
using namespace std;

void help(string name)
{
    cout << "Usage " << name << "client_name server_ip port"<< endl;
}

class GameManager : public boost::noncopyable {
public:
    typedef std::function<void ()> TReadyHandler;
    typedef std::function<void (int color)> TGameStartHandler;

    typedef std::function<void ()> TAllPlayedHandler;

    GameManager(string client_name, boost::asio::ip::tcp::socket socket, TIoPtr io_ptr):
        game_strand(socket.get_io_service()),
        protocolPtr(make_shared<Protocol>(std::move(socket))),
        clientName(client_name),
        io_ptr_(io_ptr)
    {

    }
    void start(TAllPlayedHandler handler)
    {
        cout << "start\n";
        TGameStartHandler gameStartedHandler = bind(&GameManager::onGameStarted, this, std::placeholders::_1);
        sendGreetings(bind(&GameManager::waitForGameStartOrResults, this, gameStartedHandler));
        all_played_handler_ = handler;
    }
    void stop()
    {
        io_ptr_->stop();
    }

    void onGameStarted(int color) {
        cout << "onGameStarted\n";
        TAsyncPlayerPtr white_player, black_player;
        if (color == WHITE) {
            white_player = make_shared<AsyncAiPlayer>(WHITE, 2);
            server_player_ = make_shared<ServerPlayer>(BLACK, protocolPtr);
            black_player = server_player_;

        } else {
            server_player_ = make_shared<ServerPlayer>(WHITE, protocolPtr);
            white_player = server_player_;
            black_player = make_shared<AsyncAiPlayer>(BLACK, 2);
        }

        gamePtr = make_shared<AsyncGame>(io_ptr_, white_player, black_player);

        server_player_->subscribeResultReady(game_strand.wrap(bind(&GameManager::onGameEnded, this, true, std::placeholders::_1)));

        gamePtr->start(game_strand.wrap(bind(&GameManager::onGameEnded, this, false, std::placeholders::_1)));
    }
    void onGameEnded(bool fromServer, AsyncPlayer::EndStatus status) {
        cout << "onGameEnded fromServer: "<< fromServer << " status: " << status << endl;
        int status_id = fromServer ? 0 : 1;
        last_game_end_status_[status_id] = status;


        if (NOT fromServer) {
            // force retrieving confirmation from server (TODO: somehow refactor)
            server_player_->asyncGetNext(gamePtr->getBoard(), [](const Move& ) {/*empty handler*/});
        }

        if (last_game_end_status_[0] && last_game_end_status_[1]) {
            assert(last_game_end_status_[0] == last_game_end_status_[1]);
            game_played_count_++;
            last_game_end_status_[0] = AsyncPlayer::NONE;
            last_game_end_status_[1] = AsyncPlayer::NONE;

            TGameStartHandler gameStartedHandler = bind(&GameManager::onGameStarted, this, std::placeholders::_1);
            GameManager::waitForGameStartOrResults(gameStartedHandler);
        }
    }

    void sendGreetings(TReadyHandler handler)
    {
        cout << "sendGreetings\n";
        Message message = Message(Message::HELLO, clientName);
        protocolPtr->AsyncSendMessage(message, handler);
    }

    void waitForGameStartOrResults(TGameStartHandler gameStartHandler) {
        cout << "waitForGameStartOrResults\n";

        protocolPtr->AsyncGetMessage(game_strand.wrap([gameStartHandler, this](const Message & m){
            cout << "AsyncGetMessage:  " << m << endl;
            if (m.type == Message::NEW_GAME) {
                int color = (m.data == "white" ? WHITE : BLACK);
                gameStartHandler(color);
            } else if (m.type == Message::MATCH_RESULT) {
                Message::TScoreOpt score_opt = m.toScore();
                all_played_handler_();
            }
        }));
    }


    boost::asio::io_service::strand game_strand;
    TReadyHandler readyHandler;
    ProtocolPtr protocolPtr;
    string clientName;
    TAsyncGamePtr gamePtr;
    TIoPtr io_ptr_;
    int game_played_count_ = 0;
    TAllPlayedHandler all_played_handler_;
    std::shared_ptr<ServerPlayer> server_player_;
    AsyncPlayer::EndStatus last_game_end_status_[2];
};

int main(int argc, char * argv[]) {
    try
     {
        if (argc != 4) {
            help(argv[0]);
            return 1;
        }
        string client_name = argv[1];
        string server_addr = argv[2];
        int port = lexical_cast<int>(argv[3]);
        stringstream sstream;
        sstream << client_name << ".log";
        Global::instance().setLoggingFileName(sstream.str());

        auto io_ptr = std::make_shared<io_service>();

        tcp::resolver resolver(*io_ptr);
        tcp::resolver::query query(server_addr, lexical_cast<string>(port));
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        tcp::resolver::iterator end;

       // Try each endpoint until we successfully establish a connection.
       tcp::socket socket(*io_ptr);
       boost::system::error_code error = boost::asio::error::host_not_found;
       while (error && endpoint_iterator != end)
       {
         socket.close();
         socket.connect(*endpoint_iterator++, error);
       }
       if (error)
         throw boost::system::system_error(error);

       GameManager gameManager(client_name, std::move(socket), io_ptr);
       gameManager.start([&gameManager](){
           cout << "All played " << endl;
           gameManager.stop();
       });


       thread threads[1];

       for (thread & thrd : threads) {
           thrd = thread([io_ptr]() {
               io_ptr->run();
           });
       }
       for (thread & thrd : threads) {
           thrd.join();
       }

     }
     catch (std::exception& e)
     {
       std::cout << "Exception: " << e.what() << "\n";
     }

     return 0;}
