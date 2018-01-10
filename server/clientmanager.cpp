#include "clientmanager.h"
#include "clientplayer.h"

#include <thread>

using namespace boost::asio::ip;
using namespace boost::asio;
using namespace boost;
using namespace std;

ClientManager::ClientManager(std::shared_ptr<io_service> io, int playersCount):
    managerStrand(*io),
    waitForPlayersCount(playersCount),
    io(io)
{

}

void ClientManager::reggister(TClientPtr client) {
    TClientRecord client_data = {
        client_counter++,
        client,
        false/*busy*/,
        false/*result_ready_*/
    };
    TClientRecordPtr ptr = make_shared<TClientRecord>(client_data);
    clients[ptr->id] = ptr;
    client->start(managerStrand.wrap(bind(&ClientManager::onClientReady, this, ptr)));
}

std::list<int> ClientManager::getReadyPlayerIds() const
{
    std::list<int> playerIds;
    for (std::pair<int, TClientRecordPtr> clnt : readyClients ) {
        playerIds.push_back(clnt.second->id);
    }
    return playerIds;
}

void ClientManager::onClientReady(ClientManager::TClientRecordPtr record){
    cout << "Client ready: " << record->client->getName() << " ready: " << readyClients.size() <<" \n";

    clients.erase(record->id);
    readyClients[record->id] = record;

    if (readyClients.size() == waitForPlayersCount) {
        if (allClientsReadyHandler)
            allClientsReadyHandler();
    }
}

void ClientManager::tryNextGame()
{
    cout << "tryNextGame\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::set<int> freePlayerIds;
    for (std::pair<int, TClientRecordPtr> clnt : readyClients ) {
        if (NOT clnt.second->busy)
            freePlayerIds.insert(clnt.second->id);
    }

    ResultsTable::TGameRecordOption gameOpt = resultsTable.nextGame(freePlayerIds);
    if (gameOpt) {
        cout << "available game found\n";
        int white_id = gameOpt->white_id;
        int black_id = gameOpt->black_id;
        TClientRecordPtr whitePlayerRecord, blackPlayerRecord;

        whitePlayerRecord = readyClients[white_id];
        blackPlayerRecord = readyClients[black_id];

        cout << "white:  " << whitePlayerRecord->client->getName() << "vs black: " <<  blackPlayerRecord->client->getName();

        whitePlayerRecord->busy = true;
        blackPlayerRecord->busy = true;
        TAsyncPlayerPtr whitePlayer = make_shared<AsyncClientPlayer>(whitePlayerRecord->client, WHITE);
        TAsyncPlayerPtr blackPlayer = make_shared<AsyncClientPlayer>(blackPlayerRecord->client, BLACK);
        TAsyncGamePtr game = make_shared<AsyncGame>(io, whitePlayer, blackPlayer);
        TGameRecord record = {
            white_id,
            black_id,
            game
        };
        gamesList.push_back(record);
        game->start(managerStrand.wrap(bind(&ClientManager::onGameEnded, this, game, std::placeholders::_1)));

        /// try until all available games are played
        managerStrand.post(bind(&ClientManager::tryNextGame, this));
    }
}

void ClientManager::onGameEnded(TAsyncGamePtr gamePtr, AsyncPlayer::EndStatus status)
{
    cout << "on game ended\n";

    for (TGameList::iterator it = gamesList.begin(); it != gamesList.end(); it++) {
        TGameRecord & record = *it;
        if (record.game == gamePtr) {
            int gamescore = 0;

            switch (status) {
            case AsyncPlayer::WHITE_WIN:
                gamescore = 2;
                break;
            case AsyncPlayer::DRAW:
                gamescore = 1;
                break;
            case AsyncPlayer::WHITE_LOOSE:
                gamescore = 0;
                break;
            case AsyncPlayer::NONE:
                throw runtime_error("Uncorrect end game status");
            }


            TClientRecordPtr whitePlayerRecord = readyClients[record.white_id];
            TClientRecordPtr blackPlayerRecord = readyClients[record.black_id];

            whitePlayerRecord->busy = false;
            blackPlayerRecord->busy = false;
            resultsTable.recordScore(record.white_id, record.black_id, gamescore);

            record.game->stopGame();

            gamesList.erase(it);
        }
    }
    if (NOT resultsTable.isFinished()) {
        managerStrand.post(bind(&ClientManager::tryNextGame, this));
    } else {
        for (pair<int, TClientRecordPtr> client_record_pair : readyClients) {
            TClientRecordPtr client_record = client_record_pair.second;
            Score score = resultsTable.getScore(client_record->id);
            client_record->client->asyncShowMatchResult(score, managerStrand.wrap(bind(&ClientManager::onShowResultReady, this, client_record->id)));
        }
    }
}

void ClientManager::stop()
{
    matchFinishedHandler = TMatchFinishedHanlder();
    clients.clear();
    readyClients.clear();
    gamesList.clear();
    io->stop();
}

void ClientManager::onShowResultReady(int id)
{
    readyClients[id]->result_ready_ = true;
    bool all_ready = true;
    for (pair<int, TClientRecordPtr> client_pair: readyClients) {
        TClientRecordPtr ptr = client_pair.second;
        if (NOT ptr->result_ready_) {
            all_ready = false;
            break;
        }
    }
    if (all_ready) {
        matchFinishedHandler(resultsTable);
    }
}

void ClientManager::subscribeOnAllClientsReady(ClientManager::TReadyHandler handler)
{
    allClientsReadyHandler = handler;
}

void ClientManager::startMatch(ClientManager::TMatchFinishedHanlder handler)
{
    matchFinishedHandler = handler;

    resultsTable.init(getReadyPlayerIds());
    managerStrand.post(bind(&ClientManager::tryNextGame, this));
}

ClientManager::TClientsMap ClientManager::getReadyClients() const
{
    return readyClients;
}






