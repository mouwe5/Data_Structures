//
// Created by nirit on 09/01/2022.
//

#include "GM.h"
using namespace dynamic;
using namespace std;
using namespace game;
using namespace avl;
#define INIT -1

bool PlayerIdIsValid(int id)
{
    return id > 0;
}

bool GM::GroupIdIsValid(int id)
{
    return (id > 0 && id <= groups.num_of_groups);
}

GM::GM(int num_of_groups, int scale): groups(num_of_groups,scale){
    // init groups UF
    this->scale = scale;
    // TODO: dont want it on heap, want it in lifespan of GM. didn't "new". is this correct?
    // init players HT
    all_players= new DynamicArray<shared_ptr<Player>>();
}

StatusType GM::MergeGroups(int GroupID1, int GroupID2) {
    return groups.UniteGroups(GroupID1, GroupID2);
}

StatusType GM::AddPlayer(int PlayerID, int GroupID, int score) {
//
//        //todo:: counetr zero players in group and allplayers group
//
//        return HTToGMStatusTranslator(all_players->add(PlayerID,new_player));
    // ensure input is valid
    if(GroupID<=0||PlayerID<=0||score<=0)
    {
        return INVALID_INPUT;
    }
    if(groups.num_of_groups<GroupID||score>scale)
    {
        return INVALID_INPUT;
    }
    // ensure player doesn't exist
    if(PlayerExists(PlayerID))
    {
        return FAILURE;
    }
    // add player to UF by increasing zero-level counter
    groups.IncreaseZeroCounter(GroupID);
    groups.IncreaseZeroCounter(0);
    /*
    players_group->zero_level_players_counter+=1;
    groups.groups[0]->zero_level_players_counter+=1;//all players group
     */
    groups.IncreaseScoreCounter(GroupID,score);
    groups.IncreaseScoreCounter(0,score);
    /*
    groups.IncreaseZeroCounter(0);
    players_group->num_of_players_in_score[score]+=1;
    groups.groups[0]->num_of_players_in_score[score]+=1;
     */
    auto players_group = groups.FindGroup(GroupID);
    players_group->size+=1;
    groups.groups[0]->size+=1;
    // add player to HT
    shared_ptr<Player> new_player (new Player(PlayerID,score,GroupID));//create player
    return all_players->add(PlayerID,new_player);
    // add player to rank tree
}

bool GM::PlayerExists(const int id)const {
    return (all_players->find(id) != nullptr);
}

StatusType GM::RemovePlayer(int PlayerID) {

    // ensure id is valid
    if (!PlayerIdIsValid(PlayerID))
    {
        return INVALID_INPUT;
    }

    // save player data for UF
    int level = INIT, group_id = INIT, score = INIT;
    auto player = (shared_ptr<Player>)(all_players->find(PlayerID));

    // if player doesn't exist in GM, return FAILURE
    if ( player == nullptr)
    {
        return FAILURE;
    }

    level = player->getLevel();
    group_id = player->getGroup();
    score = player->getScore();

    // remove player from HT
    all_players->remove(PlayerID);

    // remove player from UF
    // if player is zero-level update zero-level counter
    if (level == 0)
    {
        groups.DecreaseZeroCounter(group_id);
        groups.DecreaseZeroCounter(0);
    }
    // else remove from tree
    else
    {
        KeyByLevel key = KeyByLevel(PlayerID, level);
        groups.RemovePlayer(group_id, key);
        groups.RemovePlayer(0,key);
    }
    // update group's score array and size
    groups.DecreaseScoreCounter(group_id, score);
    groups.DecreaseScoreCounter(0, score);
    groups.FindGroup(group_id)->size -= 1;
    groups.FindGroup(0)->size -= 1;

    // remove player from rank tree
    all_players->remove(PlayerID);
    return SUCCESS;
}


StatusType GM::IncreasePlayerIDLevel(const int PlayerID,const int LevelIncrease) {
    // ensure player id is valid
    // ensure player doesn't exist
    if(!PlayerExists(PlayerID))
    {
        return FAILURE;
    }
    shared_ptr<Player> temp_player = all_players->find(PlayerID);
    // increase level in HT
    bool level_was_zero = (temp_player->getLevel()==0);
    // save player level
    KeyByLevel player_key(temp_player->getId(),temp_player->getLevel());
    auto players_group = groups.FindGroup(temp_player->getGroup());
    if(level_was_zero)
    {
        groups.DecreaseZeroCounter(temp_player->getGroup());
        groups.DecreaseZeroCounter(0);
    }
    else// remove player from rank tree
    {
        AVL_STATUS status1 = groups.groups[0]->players->deleteNode(player_key);
        AVL_STATUS status2 = players_group->players->deleteNode(player_key);
        if(status1!=AVL_SUCCESS || status2!=AVL_SUCCESS)
        {
            return FAILURE; //not found player,somethings wrong
        }
    }

    // increase player level
    temp_player->setLevel(LevelIncrease);//add the level to currect level
    KeyByLevel new_player_key(temp_player->getId(),temp_player->getLevel());
// readd player to rank tree with new level
    AVL_STATUS status3 =groups.groups[0]->players->insertNode(new_player_key,temp_player,temp_player->getLevel(),temp_player->getScore(),scale);
    AVL_STATUS status4 =players_group->players->insertNode(new_player_key,temp_player,temp_player->getLevel(),temp_player->getScore(),scale);// if player level was 0, add player to UF and decrease zero-level counter
    if(status3!=AVL_SUCCESS || status4!=AVL_SUCCESS)
        return ALLOCATION_ERROR;//not success to add player
    return SUCCESS;
}

StatusType GM::GetPercentOfPlayersWithScoreInBounds(int GroupID, int score, int lowerLevel, int higherLevel, double *players) {
    // check if input valid
    if (GroupID < 0 || GroupID > groups.num_of_groups || players == nullptr)
        return INVALID_INPUT;
    if( lowerLevel > higherLevel)
        return FAILURE;
    // if no players in range, return failure
    bool player_found=false;
    auto min_player_node = groups.GetNonZeroPlayerInRange(GroupID, lowerLevel, higherLevel, true, &player_found);
    auto max_player_node = groups.GetNonZeroPlayerInRange(GroupID, lowerLevel, higherLevel, false, &player_found);

    // get zero players if level zero is in range
    int zero_level_players_with_score = 0;
    if (lowerLevel <= 0 && higherLevel >= 0) {
        // score validation comes after so need to perform it here as well
        zero_level_players_with_score = (score > 0 && score <= scale)
                                        ? groups.GetNumOfZeroLevelPlayersWithScore(GroupID, score)
                                        : 0;

        // if range contains 0, need to check if any zero level players exist
        if (!player_found && groups.FindGroup(GroupID)->zero_level_players_counter == 0) {
            *players = 0;
            return FAILURE;
        }

    }else {
        // if range doesnt contain zero, only check if range contains player
        if (!player_found) {
            *players = 0;
            return FAILURE;
        }
    }

    // if only zero players found
    if (!player_found)
    {
        int total_zero_level_players = groups.FindGroup(GroupID)->zero_level_players_counter;
        // calculate percent
        *players = ((double)zero_level_players_with_score / (double)total_zero_level_players) * 100;
        return SUCCESS;
    }

    //GET RANK
    auto temp_group = groups.FindGroup(GroupID);
    int rank_max_player =temp_group->players->Rank(max_player_node->key);
    int rank_min_player =temp_group->players->Rank(min_player_node->key);
    //NEW FUNCCTION
    int max_under_score = temp_group->players->SumScoreArray(rank_max_player,score);
    // Sum score returns without self, add self if necessary
    if (max_player_node->score == score)
    {
        max_under_score += 1;
    }
    int min_under_score = temp_group->players->SumScoreArray(rank_min_player,score);
    // min shouldn't include self so no need to check like with max

    if (score < 0 || score > scale) {
        *players = 0;
        return SUCCESS;
    }

    // max - min + zero if in range
    int num_of_players_with_score = max_under_score - min_under_score;
    if (lowerLevel <= 0 && higherLevel >= 0) {
        num_of_players_with_score += zero_level_players_with_score;
    }

    // get num of players in range
    // get rank of highest player, lowest player, and subtract
    // add zero level if in range of levels

    int total_zero_level_players = groups.FindGroup(GroupID)->zero_level_players_counter;
    int total_players = (lowerLevel <= 0 && higherLevel >= 0)
                        ? (rank_max_player - (rank_min_player -1)) + total_zero_level_players
                        : rank_max_player - (rank_min_player -1);

    // calculate percent
    *players = ((double)num_of_players_with_score / (double)total_players) * 100;
    return SUCCESS;
}


StatusType  GM::ChangePlayerIDScore(int PlayerID, int NewScore){
    if(NewScore>scale)
    {
        return INVALID_INPUT;
    }
    if(!PlayerExists(PlayerID))
    {
        return FAILURE;
    }
    shared_ptr<Player> temp_player = all_players->find(PlayerID);
    auto players_group = groups.FindGroup(temp_player->getGroup());

    // increase level in HT
    // save player level
    groups.DecreaseScoreCounter(temp_player->getGroup(),temp_player->getScore());
    groups.DecreaseScoreCounter(0,temp_player->getScore());
/*
    players_group->num_of_players_in_score[temp_player->getScore()]--;
    groups.groups[0]->num_of_players_in_score[temp_player->getScore()]--;
*/
    if(temp_player->getLevel()!=0)//need to re-add player from trees
    {
        KeyByLevel player_key(temp_player->getId(),temp_player->getLevel());
        AVL_STATUS status1 = groups.groups[0]->players->deleteNode(player_key);
        AVL_STATUS status2 = players_group->players->deleteNode(player_key);
        if(status1!=AVL_SUCCESS || status2!=AVL_SUCCESS)
        {
            return FAILURE; //not found player,somethings wrong
        }
        temp_player->setScore(NewScore);
        status1 =groups.groups[0]->players->insertNode(player_key,temp_player,temp_player->getLevel(),temp_player->getScore(),scale);// if player level was 0, add player to UF and decrease zero-level counter
        status2 =players_group->players->insertNode(player_key,temp_player,temp_player->getLevel(),temp_player->getScore(),scale);// if player level was 0, add player to UF and decrease zero-level counter
        if(status1!=AVL_SUCCESS || status2!=AVL_SUCCESS)
            return ALLOCATION_ERROR;//not success to add player
    }
    else
        temp_player->setScore(NewScore);
    groups.IncreaseScoreCounter(temp_player->getGroup(),NewScore);
    groups.IncreaseScoreCounter(0,NewScore);
    /*
    players_group->num_of_players_in_score[NewScore]++;
    groups.groups[0]->num_of_players_in_score[NewScore]++;
     */
    return SUCCESS;
}

StatusType GM::AverageHighestPlayerLevelByGroup(int GroupID, int m, double *level)
{
    if(GroupID>groups.num_of_groups)
    {
        return INVALID_INPUT;
    }
    auto players_group = groups.FindGroup(GroupID);
    if(players_group->size<m)
    {
        return FAILURE;
    }
    if(players_group->zero_level_players_counter==players_group->size)
    {
        *level=0;
        return SUCCESS;
    }
    double  group_level_sum =(double) players_group->players->root->subtree_level_sum;
    //double max_player_level = (double) players_group->players->Select(players_group->players->size)->getLevel();
    double min_player_under_sum=0;
    if(m<=players_group->players->size) // not zero player include
    {
        min_player_under_sum=(double) players_group->players->GetSubtreeLevelSumByIndex((players_group->players->size) -m+1); //-1 because we dont want to remove last player level
    }
    *level=(group_level_sum-min_player_under_sum)/(double)m;
    return SUCCESS;

}

