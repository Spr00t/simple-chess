#include "chessboard.h"
#include "aiplayer.h"
#include "humanplayer.h"
#include "config.h"
#include "client.h"
#include "clientmanager.h"
#include "network_common/resultstable.h"

#include <thread>
#include <memory>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost::asio::ip;
using namespace boost::asio;
using namespace boost;
using namespace std;

typedef std::shared_ptr<io_service> TServicePtr;

class Server : public boost::noncopyable
{
public:
    Server(TServicePtr io_ptr,
           const tcp::endpoint& endpoint,
           int playersCount)
        : acceptor(*io_ptr, endpoint),
          socket(*io_ptr),
          manager(io_ptr, playersCount)
    {
    }
    void start() {
        accept();
        manager.subscribeOnAllClientsReady([this](){
            matchStart();
        });
    }

    void matchStart() {
        manager.startMatch([this](const ResultsTable & results){
            cout << "Championship has ended\n";

            std::map<int, std::string> player_id_name_map;
            ClientManager::TClientsMap clients = manager.getReadyClients();
            for (std::pair<int, ClientManager::TClientRecordPtr> clnt : clients ) {
                player_id_name_map[clnt.first] = clnt.second->client->getName();
            }
            results.show(player_id_name_map);

            manager.stop();
        });
    }

    void accept()
    {
        acceptor.async_accept(socket,
                              [this](boost::system::error_code ec)
        {
            if (!ec)
            {
                manager.reggister(std::make_shared<Client>(std::move(socket)));
            }

            accept();
        });
    }

private:
    ClientManager manager;
    tcp::acceptor acceptor;
    tcp::socket socket;
};



void help(string name)
{
    cout << "Usage: " << name << " port_number players_count"<< endl;
}

int main(int argc, char* argv[])
{
    try
    {
        Global::instance().setLoggingFileName("server.log");
        if (argc != 3) {
            help(argv[0]);
            return 1;
        }
        thread threads[5];

        auto io_ptr = std::make_shared<io_service>();

        int port_number = lexical_cast<int>(argv[1]);
        int players_count = lexical_cast<int>(argv[2]);

        Server server(io_ptr, tcp::endpoint(tcp::v4(), port_number), players_count);
        server.start();

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
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

