//
// Created by tom on 11/27/2021.
//

#ifndef HW1_PLAYER_H
#define HW1_PLAYER_H
#include "memory"
#include "AvlTree.h"
#define SCALE_SIZE 201
namespace game
{
    class Player
    {
    private:
        int id=0;
        int level=0;
        int group_id=0;
        int score=0;

    public:
        Player()=default;
        Player(const int id , const int score, int group_id);
        Player(const Player&);
        ~Player();
        Player& operator=(const Player &);
         bool operator==(const Player &) const;
         int getId() const;
         int getLevel()const;
         int getScore()const;
         void setLevel(const int);
         void setScore(const int);
         int getGroup()const;
        void setGroup(int);
         bool higherLevelThan(const std::shared_ptr<Player>)const;
    };


};


#endif //HW1_PLAYER_H
