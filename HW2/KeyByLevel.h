//
// Created by tom on 12/3/2021.
//

#ifndef HW1_KEYBYLEVEL_H
#define HW1_KEYBYLEVEL_H
#include "Key.h"
namespace game {
    class KeyByLevel : public Key {
    private:
        int level;
    public:
        KeyByLevel()=default;
        KeyByLevel(const KeyByLevel &other);
        KeyByLevel(const KeyByLevel *other);
        KeyByLevel(const int id, const int level);
        ~KeyByLevel() override = default;
        int getLevel()const;
        int getId()const;
        bool operator<(const KeyByLevel &) const;
        bool operator==(const KeyByLevel &) const; //compare between 2
        KeyByLevel &operator=(const KeyByLevel &);



    };
};

#endif //HW1_KEYBYLEVEL_H
