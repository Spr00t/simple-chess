#pragma once
#include <list>
#include <map>
#include <set>
#include <boost/optional.hpp>

struct Score {
    int score = 0;
    int maximum = 0;
};
class ResultsTable
{
public:
    struct GameRecord {
        int white_id;
        int black_id;
        int game_score = -1;
    };

    typedef std::map<int, std::map<int, ResultsTable::GameRecord> > TGameRecordMap;
    typedef boost::optional<GameRecord> TGameRecordOption;

    ResultsTable();
    void init(std::list<int> player_ids);


    TGameRecordOption nextGame(std::set<int> freePlayerIds);

    void recordScore(int whiteId, int blackId, int gamescore);
    Score getScore(int playerId) const;

    bool isFinished() const;

    void show(const std::map<int, std::string> & id_name_map) const;
private:
    static TGameRecordMap generateGames(const std::list<int>& player_ids);
    TGameRecordMap allGames;
};
