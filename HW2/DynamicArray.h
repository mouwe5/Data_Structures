//
// Created by tom on 1/4/2022.
//

#ifndef GAME_DYNAMICARRAY_H
#define GAME_DYNAMICARRAY_H
#include "library2.h"
#define SIZE_MULTIPLY 2
#define ARRAY_BASE_SIZE 10
#include "exception"
// AVL tree node
namespace dynamic {

    template<typename Data>
    class ArrayNode {
    public:
        int key;
        Data data;
        ArrayNode<Data> *next;

        ~ArrayNode() = default;

        ArrayNode() : key(0), data(), next(nullptr) {}

        ArrayNode(int key, Data data) : key(key), data(data), next(nullptr) {}

        ArrayNode(const ArrayNode &other) : key(other.key), data(other.data), next(other.next) {}

        // operators
        ArrayNode &operator=(ArrayNode &other) {
            if (this == other)
                return *this;
            this->key = other.key();
            this->data = other.data();
            this->next = other.next();
            return *this;
        }

        StatusType addNode(ArrayNode *other) {
            if (other->key == this->key) {
                return FAILURE;
            }
            if (this->next == nullptr) {
                this->next = other;
                return SUCCESS;
            }
            ArrayNode<Data> *iter = this->next;
            while (iter != nullptr)//continue until no more nodes in list
            {
                if (other->key == iter->key) {
                    return FAILURE;
                }
                if (iter->next == nullptr) {
                    iter->next = other;
                    return SUCCESS;
                }
                iter=iter->next;
            }
            return FAILURE;
        }
    };
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

    template<typename Data>
    class DynamicArray {
    private:


        StatusType expandArray();

        StatusType shrinkArray();

        int hashFunction(const int key);

        ArrayNode<Data> **createArray(int size);

        StatusType copyArrays(ArrayNode<Data> **, int const);


    public:
        ArrayNode<Data> **array;
        int elements = 0;
        int size = ARRAY_BASE_SIZE;

        DynamicArray();

        ~DynamicArray();

        DynamicArray(const DynamicArray &);

        DynamicArray &operator=(const DynamicArray &);

        StatusType add(const int,Data );
        Data find(const int key);

        StatusType remove(int key);
    };
    template<typename Data>
    void deleteArray(  ArrayNode<Data> **array,int size)
    {
        for(int i=0;i<size;i++)
        {
            if(array[i]!=nullptr)
            {
                ArrayNode<Data>* before = array[i];
                ArrayNode<Data>* after = before->next;
                while(after)
                {
                    delete before;
                    before=after;
                    after=after->next;
                }
                delete before;
            }
        }
        delete[] array;
    }
    template<typename Data>
    DynamicArray<Data>::DynamicArray() {
        try {
            array = createArray(ARRAY_BASE_SIZE);
        } catch (std::bad_alloc &) {
            throw ALLOCATION_ERROR;
        }
    }

    template<typename Data>
    ArrayNode<Data> **DynamicArray<Data>::createArray(int size) {
        try{
            auto array = new ArrayNode<Data> *[size];
            for (int i = 0; i < size; i++) {
                array[i] = nullptr;
            }
            return array;
        }
        catch (std::bad_alloc &) {
            throw std::bad_alloc();
        }
    }

    template<typename Data>
    DynamicArray<Data>::DynamicArray(const DynamicArray &other) {
        this->array=createArray(other.size);
        this->size=other.size;
        copyArrays(other.array,other.size);
    }

    template<typename Data>
    StatusType DynamicArray<Data>::copyArrays( ArrayNode<Data>** other,int const size_other) {
        int index=0;
        while(index<size_other)
        {
            ArrayNode<Data>* temp = other[index];
            while(temp!= nullptr)
            {
                this->add(temp->key,temp->data);
                temp=temp->next;
            }
            index++;
        }
        return SUCCESS;
    }

    template<typename Data>
    StatusType DynamicArray<Data>::add(const int key,Data data) {
        if ((elements + 1) >= size)//expand if elements==size
        {
            if(expandArray()==FAILURE)
                return FAILURE;
        }
        auto node= new ArrayNode<Data> (key,data);
        int index= hashFunction(key);
        if(this->array[index]== nullptr)
        {
            this->array[index]=node;
            elements++;
            return SUCCESS;
        }
        ArrayNode<Data>* temp=this->array[index];
        if(temp->addNode(node)!=SUCCESS) //if player already exists
        {
            delete node;
            return FAILURE;
        }
        elements++;
        return SUCCESS;
    }
    template<typename Data>
    int DynamicArray<Data>::hashFunction(const int key) {
        return key % size;
    }

    template<typename Data>
    DynamicArray<Data> &DynamicArray<Data>::operator=(const DynamicArray &other) {
        if(other==this) {
            return *this;
        }
        this->array=createArray(other.size);
        this->size=other.size;
        copyArrays(other.array,other.size);
        return *this;
    }

    template<typename Data>
    StatusType DynamicArray<Data>::expandArray() {
        try {
            auto temp_array = (this->array);
            this->array=nullptr;
            this->array = createArray(size * SIZE_MULTIPLY);
            this->elements=0;
            this->size = size * SIZE_MULTIPLY;
            this->copyArrays(temp_array, size/SIZE_MULTIPLY);
            deleteArray(temp_array,size/SIZE_MULTIPLY);
            return SUCCESS;
        } catch (std::bad_alloc &) {
            return FAILURE;
        }
    }

    template<typename Data>
    StatusType DynamicArray<Data>::remove(const int key) {
        int index= hashFunction(key);
        if(this->array[index]== nullptr)//player not exists
        {
            return FAILURE;
        }
        ArrayNode<Data>* temp=this->array[index];
        if(temp->key==key)//if first
        {
            array[index]=temp->next;
            elements--;
            delete temp;
        }
        else
        {
            bool found=false;
            ArrayNode<Data>* before_temp=this->array[index];
            temp=temp->next;
            while(temp&&!found)
            {
                if(temp->key==key)//if first
                {
                    before_temp->next=temp->next;
                    elements--;
                    found=true;
                    delete temp;
                    break;

                }
                temp=temp->next;
                before_temp=before_temp->next;
            }
            if(!found)//player not exists
            {
                return FAILURE;
            }
        }
        if ((elements<size/4)&&size>ARRAY_BASE_SIZE)//need to shrink
        {
            if (shrinkArray()!=SUCCESS)
            {
                return ALLOCATION_ERROR;
            }
        }
        return SUCCESS;
    }

    template<typename Data>
    StatusType DynamicArray<Data>::shrinkArray() {
        try {
            auto temp_array = (this->array);
            this->array=nullptr;
            this->array = createArray(size / SIZE_MULTIPLY);
            this->elements=0;
            this->size = size / SIZE_MULTIPLY;
            copyArrays(temp_array, size*SIZE_MULTIPLY);
            deleteArray(temp_array,size*SIZE_MULTIPLY);
            return SUCCESS;
        } catch (std::bad_alloc &) {
            return FAILURE;
        }
    }

    template<typename Data>
    Data DynamicArray<Data>::find(const int key) {
        int index = hashFunction(key);
        ArrayNode<Data> *temp = array[index];
        while(temp)
        {
            if(temp->key==key)
            {
                return temp->data;
            }
            temp=temp->next;
        }
        return nullptr;
    }

    template<typename Data>
    DynamicArray<Data>::~DynamicArray() {
        deleteArray(array,size);

    }
}

#endif //GAME_DYNAMICARRAY_H
