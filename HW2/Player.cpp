//
// Created by tom on 12/3/2021.
//

#include "Player.h"
//#include "Group.h"
using namespace game;
using namespace std;
using namespace avl;

Player::Player(const int id, const int score, const int group):
id(id),group_id(group), score(score)
{
}

Player::~Player(){
}

Player::Player(const Player &other): Player(other.getId(),other.score,other.group_id)
{
    this->level=other.level;
}

Player& Player::operator=(const Player &other)
{
    if(this==&other)
    {
        return *this;
    }
    id=other.id;
    level=other.level;
    score=other.score;
    group_id=other.group_id;
    return *this;
}
int Player::getLevel() const {
    return this->level;
}

int Player::getId() const {
    return this->id;
}

int Player::getScore() const {
    return this->score;
}

bool Player::operator==(const Player & other) const
{
    return this->id==other.getId();
}

int Player::getGroup() const
{
    return group_id;
}

void Player::setGroup(int new_group_id)
{
    group_id = new_group_id;

}

bool Player::higherLevelThan(const std::shared_ptr<Player> other) const {
    if(other == nullptr)
    {
        return true;
    }
    if(level>other->getLevel()||(this->level==other->getLevel()&&this->id<other->getId()))
    {
        return true;
    }
    return false;
}

void Player::setLevel(const int levels) {
    level+=levels;

}

void Player::setScore(const int new_score) {
    score = new_score;

}


