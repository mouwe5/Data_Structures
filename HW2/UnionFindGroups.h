//
// Created by nirit on 06/01/2022.
//

#ifndef GAME_UNIONFINDGROUPS_H
#define GAME_UNIONFINDGROUPS_H
#include "library2.h"
#include "AvlTree.h"
#include "Player.h"
#include "KeyByLevel.h"


class Group {
public:
    int size;
    int parent;
    int zero_level_players_counter;
    int *num_of_players_in_score;
    int scale;
    avl::AVL<game::KeyByLevel, std::shared_ptr<game::Player>>* players;

    Group(int id,int scale) : scale(scale),size(0), parent(id), zero_level_players_counter(0) {
       players = new avl::AVL<game::KeyByLevel, std::shared_ptr<game::Player>>();
       num_of_players_in_score=new int[scale+1] {0};

    }
    ~Group() {
        delete players;
        delete []num_of_players_in_score;
    }
    Group &operator=(const Group &other)
    {
        this->size=other.size;
        this->parent=other.parent;
        this->scale=other.scale;
        this->zero_level_players_counter=other.zero_level_players_counter;
        for(int i=0;i<scale+1;i++)
            this->num_of_players_in_score[i]=other.num_of_players_in_score[i];
        this->players=other.players;
    }

};

class UnionFindGroups {
public:
    Group** groups;
    int scale;
    int num_of_groups;
    UnionFindGroups(int size,int scale);
    ~UnionFindGroups();
    UnionFindGroups(const UnionFindGroups &other);

    Group *FindGroup(int group_id);
    StatusType UniteGroups(int first_id, int second_id);
    bool GroupIdIsValid(int id);
    void IncreaseScoreCounter(int group_id, int score);
    void DecreaseScoreCounter(int group_id, int score);
    void IncreaseZeroCounter(int group_id);
    void DecreaseZeroCounter(int group_id);
    void RemovePlayer(int group_id, game::KeyByLevel key);
    avl::Node<game::KeyByLevel, std::shared_ptr<game::Player>>* GetNonZeroPlayerInRange(int group_id, int min_level, int max_level, bool getMin, bool* found);
    int GetNumOfZeroLevelPlayersWithScore(int group_id, int score);
    UnionFindGroups &operator=(const UnionFindGroups &);
};


#endif //GAME_UNIONFINDGROUPS_H
