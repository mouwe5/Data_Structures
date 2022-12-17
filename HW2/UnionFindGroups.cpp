//
// Created by nirit on 06/01/2022.
//

#include "UnionFindGroups.h"
#include "KeyByLevel.h"

UnionFindGroups::UnionFindGroups(int size,int scale) : scale(scale) ,num_of_groups(size) {
    // allocate UF array
    groups = new Group*[size + 1];
    // init array
    for (int i = 0; i < size + 1; i++)
    {
        groups[i] = new Group(i,scale);
    }
}
UnionFindGroups::UnionFindGroups(const UnionFindGroups &other) : scale(other.scale),num_of_groups(other.num_of_groups) {
    for (int i=0; i<num_of_groups+1;i++)
    {
        groups[i] = other.groups[i];
    }
}
UnionFindGroups::~UnionFindGroups() {
    for (int i = 0; i < num_of_groups + 1; i++)
    {
        if (groups[i] != nullptr)
            delete groups[i];
    }
    delete[] groups;
}

Group* UnionFindGroups::FindGroup(int group_id) {
    // validate input
    if (!GroupIdIsValid(group_id))
        return nullptr;

    // reach root of united groups
    int iterator= group_id;
    int to_be_compressed = group_id;
    while (iterator != groups[iterator]->parent)
    {
        iterator = groups[iterator]->parent;
    }

    // compress path
    groups[to_be_compressed]->parent = iterator;

    // TODO: am returning reference not value, right?
    return groups[iterator];
}

bool UnionFindGroups::GroupIdIsValid(int id){
    if (id >= 0 && id <= num_of_groups)
        return true;
    return false;
}


StatusType UnionFindGroups::UniteGroups(int first_id, int second_id) {
    // ensure ids are valid
    if (!(GroupIdIsValid(first_id) && GroupIdIsValid(second_id)))
    {
        return INVALID_INPUT;
    }

    // find groups
    auto first = FindGroup(first_id);
    auto second = FindGroup(second_id);
    if (first == nullptr || second == nullptr)
    {
        return INVALID_INPUT;
    }

    // if same group, return success
    if (first->parent == second->parent)
        return SUCCESS;

    // if different groups, push smaller group into larger group:
    // merge players avl trees into larger group's tree
    auto larger_group = first->size > second->size
                        ? first
                        : second;
    auto smaller_group = first->size > second->size
                         ? second
                         : first;

    assert(larger_group->parent != smaller_group->parent);

    auto new_player_tree = mergeTrees(larger_group->players, smaller_group->players);
    if (new_player_tree == nullptr)
        return ALLOCATION_ERROR;
    //TODO possible leak with avl deletion
    delete larger_group->players;
    larger_group->players = new_player_tree;

    // free smaller group's avl tree and update to nullptr
    delete smaller_group->players;
    smaller_group->players = nullptr;

    // update larger group's zero player counter and size
    larger_group->zero_level_players_counter += smaller_group->zero_level_players_counter;
    larger_group->size += smaller_group->size;

    // update larger group's score array
    for (int i=0; i<scale+1;i++)
    {
        larger_group->num_of_players_in_score[i] += smaller_group->num_of_players_in_score[i];
    }

    // update smaller group's parent
    smaller_group->parent = larger_group->parent;

    return SUCCESS;
}

void UnionFindGroups::IncreaseScoreCounter(int group_id, int score) {
    // ensure id and score are valid
    if (!GroupIdIsValid(group_id) || score < 0 || score > scale)
    {
        return;
    }

    // increase score
    auto group = FindGroup(group_id);
    group->num_of_players_in_score[score] += 1;
}

void UnionFindGroups::DecreaseScoreCounter(int group_id, int score) {
    // ensure id and score are valid
    if (!GroupIdIsValid(group_id) || score < 0 || score > scale)
        return;

    // decrease score
    auto group = FindGroup(group_id);
    group->num_of_players_in_score[score] -= 1;
}

void UnionFindGroups::IncreaseZeroCounter(int group_id) {
    // ensure id is valid
    if (!GroupIdIsValid(group_id))
        return;

    // increase counter
    auto group = FindGroup(group_id);
    group->zero_level_players_counter += 1;
}

void UnionFindGroups::DecreaseZeroCounter(int group_id) {
    // ensure id is valid
    if (!GroupIdIsValid(group_id))
        return;

    // decrease counter
    auto group = FindGroup(group_id);
    group->zero_level_players_counter -= 1;
}

UnionFindGroups &UnionFindGroups::operator=(const UnionFindGroups &other) {
    if(this==&other)
    {
        return *this;
    }
    int old_num_of_groups =this->num_of_groups;
    this->num_of_groups=other.num_of_groups;
    auto old_group =this->groups;
    this->scale=other.scale;
    this->groups= nullptr;
    for (int i = 0; i < old_num_of_groups + 1; i++)
        {
            if (old_group[i] != nullptr)
                delete old_group[i];
        }
        delete[] old_group;

    this->groups = new Group*[num_of_groups + 1];
    // init array
    for (int i = 0; i < num_of_groups + 1; i++)
    {
        groups[i] = new Group(i,scale);
        *groups[i]=*other.groups[i];
    }
    return *this;
}
void UnionFindGroups::RemovePlayer(int group_id, game::KeyByLevel key) {
    // ensure ids are valid
    if (!GroupIdIsValid(group_id))
        return;

    // remove player from players avl tree
    auto group = FindGroup(group_id);
    if (group->players != NULL)
    {
        group->players->deleteNode(key);
    }
}


avl::Node<game::KeyByLevel, std::shared_ptr<game::Player>>* UnionFindGroups::GetNonZeroPlayerInRange(int group_id, int min_level, int max_level, bool getMin, bool* found)
{
    if (group_id < 0 || group_id > num_of_groups || found == nullptr)
    {
        *found = false;
        return nullptr;
    }


    auto players = FindGroup(group_id)->players;
    if (players == nullptr || players->root == nullptr)
    {
        *found = false;
        return nullptr;
    }

    // create key range
    auto min_key = game::KeyByLevel(0,min_level);
    auto max_key = game::KeyByLevel(0, max_level+1);

    // create result
    auto result = players->root;
    if (min_key < (result)->key && (result)->key < max_key )
    {
        *found=true;
    }
    players->getNodeInRange(players->root, min_key, max_key, getMin, &result, found);
    return result;


/*    if (getMin)
    {
        while (iterator != nullptr)
        {
            // go left while possible
            if (min_key < iterator->key)
            {
                target = iterator;
                iterator = iterator->left;
            }
            // otherwise check if can go right
            else if (iterator->right != nullptr && min_key < iterator->right->key )
            {
                iterator = iterator->right;
                target = iterator->right;
            }
            else
            {
                break;
            }
        }

        // if out of range, return
        if (target == nullptr || target->key < min_key || max_key < target->key)
        {
            *found = false;
            return nullptr;
        }

        *found = true;
        return target;
    }
    else {
        while (iterator != nullptr)
        {
            // go right while possible
            if (iterator->key < max_key)
            {
                target = iterator;
                iterator = iterator->right;
            }
            // otherwise check if can go left
            else if (iterator->left != nullptr && iterator->left->key < max_key)
                {
                    iterator = iterator->left;
                    target = iterator->left;
                }
            else
            {
                break;
            }
        }

        // if out of range, return
        if (target == nullptr || target->key < min_key || max_key < target->key)
        {
            *found = false;
            return nullptr;
        }

        *found = true;
        return target;
    }*/
}


int UnionFindGroups::GetNumOfZeroLevelPlayersWithScore(int group_id, int score) {
    if (group_id < 0 || score <= 0 || score > scale)
    {
        return 0;
    }

    auto group = FindGroup(group_id);

    // calculate how many zero-level players in score:
    // get how many players at score from score array (which holds both zero and nonzero)
    auto total_player_with_score = group->num_of_players_in_score[score];

    // get how many players at score in entire tree (from root?) which are the non-zero
    if (group->players == nullptr || group->players->root == nullptr)
    {
        return total_player_with_score;
    }
    auto nonzero_players_with_score = group->players->root->score_array[score];

    // remove non-zero from all and get zero
    return total_player_with_score - nonzero_players_with_score;
}




