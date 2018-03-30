#include "resultstable.h"

#include <iostream>

using namespace std;

ResultsTable::ResultsTable()
{
}

ResultsTable::TGameRecordMap ResultsTable::generateGames(const std::list<int> &player_ids){
    TGameRecordMap games;
    for (int id1 : player_ids) {
        for (int id2 : player_ids) {
            if (id1 != id2) {
                GameRecord record = {
                    id1,
                    id2,
                    -1,
                };
                games[id1][id2]  = record;
            }
        }
    }
    return games;
}

void ResultsTable::init(std::list<int> player_ids)
{
    allGames = generateGames(player_ids);

}

ResultsTable::TGameRecordOption ResultsTable::nextGame(std::set<int> freePlayerIds){
    for (int whitePlayerId: freePlayerIds) {
        for (auto & gameIt : allGames[whitePlayerId]) {
            GameRecord & record = gameIt.second;

            if (record.game_score == -1 && freePlayerIds.find(record.black_id) != freePlayerIds.end() ) {
                return TGameRecordOption(record);
            }
        }
    }
    return TGameRecordOption();
}

void ResultsTable::recordScore(int whiteId, int blackId, int gamescore){
    allGames[whiteId][blackId].game_score = gamescore;
}

void ResultsTable::recordLooseForAllUnplayed(int player_id)
{
    Score score;
    int p_count = allGames.size(); //it is a white player map games
    score.maximum = (p_count - 1) * 2 * 2;
    for (auto & whitePlayerGames : allGames) {
        for (auto & game : whitePlayerGames.second) {
           if (game.second.white_id == player_id) {
               if (game.second.game_score == -1) {
                   game.second.game_score = 0; // black win
               }
           } else if (game.second.black_id == player_id) {
               if (game.second.game_score== -1) {
                   game.second.game_score = 2; // white win
               }
           }
        }
    }
}

Score ResultsTable::getScore(int playerId) const
{
    Score score;
    int p_count = allGames.size(); //it is a white player map games
    score.maximum = (p_count - 1) * 2 * 2;
    for (auto & whitePlayerGames : allGames) {
        for (auto & game : whitePlayerGames.second) {
           if (game.second.white_id == playerId) {
               score.score += game.second.game_score;
           } else if (game.second.black_id == playerId) {
               score.score += (2 - game.second.game_score);
           }
        }
    }
    return score;
}

bool ResultsTable::isFinished() const
{
    for (auto & whitePlayerGames : allGames) {
        for (auto & game : whitePlayerGames.second) {
            /// means game is not finished yet
            if (game.second.game_score == -1) {
                return false;
            }
        }
    }
    return  true;
}

void ResultsTable::show(const std::map<int, string> &id_name_map) const
{
    std::map<int, string> id_name_map_copy = id_name_map;
    int game_id_counter = 0;
    std::map<int, int> players_score;

    for (auto & whitePlayerGames : allGames) {
        for (auto & game : whitePlayerGames.second) {
            const GameRecord & game_record = game.second;
            cout << "Game " << game_id_counter++
                     << "\n\twhite: " << id_name_map_copy[game_record.white_id]
                     << "\n\tblack: " << id_name_map_copy[game_record.black_id]
                     << "\n\tresult: " << game_record.game_score
                     << "\n\n";

            players_score[game_record.white_id] += game_record.game_score;
            players_score[game_record.black_id] += (2 - game_record.game_score);
        }
    }


    for (auto & score : players_score) {
        cout << "Player " << id_name_map_copy[score.first]
                 << "\tscore: " << score.second << "\n";
    }

}
