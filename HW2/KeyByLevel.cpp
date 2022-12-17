//
// Created by tom on 12/3/2021.
//

#include "KeyByLevel.h"


using namespace game;
using namespace std;

KeyByLevel::KeyByLevel(const int id, const int level):
Key(id),level(level)
{}

KeyByLevel::KeyByLevel(const KeyByLevel &other):
Key(other.id),level(other.level)
{}

int KeyByLevel::getLevel() const {
    return this->level;
}

int KeyByLevel::getId() const {
    return this->id;
}

bool KeyByLevel::operator<(const KeyByLevel & other) const
{
    if(this->level<other.getLevel()||(this->level==other.getLevel()&&this->id<other.getId()))
    {
        return true;
    }
    return false;
}

bool KeyByLevel::operator==(const KeyByLevel & other) const
{
    return this->id==other.id;
}
KeyByLevel& KeyByLevel::operator=(const KeyByLevel &other)
{
    if(this== &other)
    {
        return *this;
    }
    id = other.id;
    level = other.level;
    return *this;
}

KeyByLevel::KeyByLevel(const KeyByLevel *other) :
Key(other->id),level(other->level){}

