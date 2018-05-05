#pragma once
#include "client.h"
#include "network_common/resultstable.h"
#include "asyncgame.h"

#include <boost/asio/io_service.hpp>
#include <functional>
#include <list>

class ClientManager {
public:
    struct TClientRecord {
        int id;
        TClientPtr client;
        bool busy;
        bool result_ready_ = false;
    };
    struct TGameRecord {
        int white_id;
        int black_id;
        TAsyncGamePtr game;
    };


    typedef std::function<void ()> TReadyHandler;
    typedef std::shared_ptr<TClientRecord> TClientRecordPtr;
    typedef std::map<int, TClientRecordPtr> TClientsMap;
    typedef std::list<TGameRecord> TGameList;
    typedef std::function<void(const ResultsTable & results)> TMatchFinishedHanlder;

    ClientManager(std::shared_ptr<boost::asio::io_service> io, int playersCount);

    void reggister(TClientPtr client);

    std::list<int> getReadyPlayerIds() const;
    void onClientReady(TClientRecordPtr record);

    void tryNextGame();
    void onGameEnded(TAsyncGamePtr gamePtr, AsyncPlayer::EndStatus status);

    void stop();

    void onShowResultReady(int id);

    void subscribeOnAllClientsReady(TReadyHandler handler);

    void startMatch(TMatchFinishedHanlder handler);
    TClientsMap getReadyClients() const;

private:
    boost::asio::io_service::strand managerStrand;
    TClientsMap clients;
    TClientsMap readyClients;

    std::string name;
    int client_counter = 0;
    size_t waitForPlayersCount;
    TMatchFinishedHanlder matchFinishedHandler;
    TReadyHandler allClientsReadyHandler;

    ResultsTable resultsTable;

    TGameList gamesList;
    std::shared_ptr<boost::asio::io_service> io;
};
