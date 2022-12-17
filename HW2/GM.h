//
// Created by nirit on 09/01/2022.
//

#ifndef GAME_GM_H
#define GAME_GM_H

#include "library2.h"
#include "DynamicArray.h"
#include "UnionFindGroups.h"
#include "AvlTree.h"
#include "Player.h"
#include <memory>

class GM {
public:
    dynamic::DynamicArray<std::shared_ptr<game::Player>> *all_players;
    UnionFindGroups groups;
    int scale;

//    GM()=default;
    GM(int num_of_groups, int scale);
    ~GM(){
        delete all_players;
    }

    StatusType MergeGroups(int GroupID1, int GroupID2);
    StatusType AddPlayer(int PlayerID, int GroupID, int score);
    StatusType RemovePlayer(int PlayerID);
    StatusType IncreasePlayerIDLevel(int PlayerID, int LevelIncrease);
    StatusType ChangePlayerIDScore(int PlayerID, int NewScore);
    StatusType GetPercentOfPlayersWithScoreInBounds(int GroupID, int score, int lowerLevel, int higherLevel,
                                                    double * players);
    StatusType AverageHighestPlayerLevelByGroup(int GroupID, int m, double * level);

    bool GroupIdIsValid(int id);
    bool PlayerExists(const int id)const;

};


#endif //GAME_GM_H
