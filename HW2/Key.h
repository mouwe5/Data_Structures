//
// Created by tom on 12/3/2021.
//

#ifndef HW1_KEY_H
#define HW1_KEY_H

namespace game {
    class Key
    {
    protected:
        int id;
        Key() = default;
        Key(const int id):id(id){};
        virtual ~Key() = default;
    public:

    };
};


#endif //HW1_KEY_H
