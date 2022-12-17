#include "library2.h"
#include "GM.h"
using namespace game;
using namespace std;
using namespace avl;


void *Init(int k, int scale)
{
    auto game_manager = new GM(k,scale);
    return (void*) game_manager;
}

StatusType MergeGroups(void *DS, int GroupID1, int GroupID2) {
    if (DS == nullptr || GroupID1 <= 0 || GroupID2 <= 0) {
        return INVALID_INPUT;
    }
    auto game_manager=(GM*) DS;
    return (StatusType)game_manager->MergeGroups(GroupID1,GroupID2);
}

StatusType AddPlayer(void *DS, int PlayerID, int GroupID, int score)
{
    if(DS== nullptr||GroupID<=0||PlayerID<=0||score<=0)
    {
        return INVALID_INPUT;
    }
    auto game_manager=(GM*) DS;
    return (StatusType)game_manager->AddPlayer(PlayerID,GroupID,score);
}

StatusType RemovePlayer(void *DS, int PlayerID){

    if(DS== nullptr||PlayerID<=0)
    {
        return INVALID_INPUT;
    }
    auto game_manager=(GM*) DS;
    return (StatusType)game_manager->RemovePlayer(PlayerID);

}

StatusType IncreasePlayerIDLevel(void *DS, int PlayerID, int LevelIncrease){
    if(DS== nullptr||PlayerID<=0||LevelIncrease<=0)
    {
        return INVALID_INPUT;
    }
    auto game_manager=(GM*) DS;
    return (StatusType)game_manager->IncreasePlayerIDLevel(PlayerID,LevelIncrease);

}

StatusType ChangePlayerIDScore(void *DS, int PlayerID, int NewScore){
    if(DS== nullptr||PlayerID<=0||NewScore<=0)
    {
        return INVALID_INPUT;
    }
    auto game_manager=(GM*) DS;
    return game_manager->ChangePlayerIDScore(PlayerID,NewScore);

}

StatusType GetPercentOfPlayersWithScoreInBounds(void *DS, int GroupID, int score, int lowerLevel, int higherLevel,
                                                double * players){
    if (DS == nullptr || players == nullptr || GroupID < 0 || GroupID > ((GM*)DS)->groups.num_of_groups)
    {
        return INVALID_INPUT;
    }

    auto game_manager = (GM*)DS;
    return game_manager->GetPercentOfPlayersWithScoreInBounds(GroupID, score, lowerLevel, higherLevel, players);
}

StatusType AverageHighestPlayerLevelByGroup(void *DS, int GroupID, int m, double * level)
{
    if(DS== nullptr||level==nullptr||GroupID<0||m<=0)
    {
        return INVALID_INPUT;
    }
    auto game_manager=(GM*) DS;
    return game_manager->AverageHighestPlayerLevelByGroup(GroupID,m,level);

}

StatusType GetPlayersBound(void *DS, int GroupID, int score, int m,
                           int * LowerBoundPlayers, int * HigherBoundPlayers)
{
    // BONUS FUNCTION
    return SUCCESS;

}

void Quit(void** DS)
{
        if(DS==nullptr)
            return;
        auto game_manager=(GM*) *DS;
        delete(game_manager);
        *DS=NULL;
}