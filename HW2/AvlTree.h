#ifndef AVLTREE_AVLTREE_H
#define AVLTREE_AVLTREE_H

#include <cassert>
#include <cstddef>
#include <new>
#include <iostream>

namespace avl {
    // helper functions
    template <class T>
    void swapp (T& a, T&b)
    {
        T c(a);
        a=b;
        b=c;
    }

    enum AVL_STATUS {
        AVL_SUCCESS, AVL_KEY_ALREADY_EXISTS, AVL_NOT_FOUND, AVL_ALLOCATION_ERROR
    };

    // AVL tree node
    template<typename Key, typename Data>
    class Node{
    public:
        Key key;
        Data data;
        Node *left;
        Node *right;
        int height;
        int under_me=1;
        int scale=0;
        int score=0;
        int subtree_level_sum=0;
        int *score_array= nullptr;
        int level=0;
        ~Node()
        {
            delete []score_array;
        }
        Node() : key(), data(), left(), right(), height() {}

        Node(Key key, Data data,int level,int score,int scale) : key(key),scale(scale),score(score), data(data),level(level) ,left(), right(), height(1) {
            score_array = new int [scale+1];
            subtree_level_sum=level;
            for(int i=0;i<scale+1;i++)
            {
                score_array[i]=0;
            }
            score_array[score]+=1;

        }

        Node(const Node &other) : key(other.key), data(other.data), height(other.height),under_me(other.under_me),
                                  scale(other.scale),score(other.score),level(other.level),subtree_level_sum(other.subtree_level_sum){
            score_array = new int [other.scale+1];
            for(int i=0;i<scale;i++)
            {
                score_array[i]=other.score_array[i];
            }
            if (other.left) {
                left = new Node(*other);
            }
            try{
                if (other.right) {
                    right = new Node(*other);
                }
            } catch(const std::bad_alloc&)
            {
                delete left;
            }

        }
        void swapNodes(Node<Key,Data> &first, Node<Key,Data> &second) noexcept{
            swapp(first.key, second.key);
            swapp(first.data, second.data);
            swapp(first.height, second.height);
            swapp(first.left, second.left);
            swapp(first.right, second.right);
            swapp(first.under_me, second.right->under_me);
            swapp(first.scale, second.right->scale);
            swapp(first.score, second.right->score);
            swapp(first.level, second.right->level);
            swapp(first.subtree_level_sum, second.subtree_level_sum);
            ////
           //int backup_fist[first.scale+1];
          //  for(int i=0;i<first.scale+1;i++)
         ///   {
         //       backup_fist[i]=first.score_array[i];
         //   }
            ///
            delete[]first.score_array;
            first.score_array= new int [first.scale+1];
            for(int i=0;i<first.scale+1;i++)
            {
                first.score_array[i]=second.score_array[i];
            }
           // delete[]second.score_array;
          //  second.score_array= new int [second.scale+1];
         //   for(int i=0;i<second.scale+1;i++)
          //  {
         //       second.score_array[i]=backup_fist[i];
         //   }
        }



        // operators
        Node& operator=(Node other) {
            swapNodes(other, *this);
            return *this;
        }

    };

    // AVL tree

    template<typename Key, typename Data>
    class AVL {
    private:
        //todo: tom added
        Node<Key, Data> *locateNode(Key key, Node<Key, Data> * root, Node<Key, Data> *parent)const ;
        int max(int a, int b);
        Node<Key, Data> *leftRotate(Node<Key, Data> *node);
        Node<Key, Data> *rightRotate(Node<Key, Data> *node);
    public:
        // ctors
        AVL() : root() {}

        AVL(const AVL &other) { root = other.root;
            size = other.size;}

        ~AVL(){ destroyTree(root); }


        void swapAvlTrees(AVL<Key,Data> &first, AVL<Key,Data> &second) noexcept{
            swapp(first.root, second.root);
        }

        // operators
        AVL &operator=(const AVL &other) {
            AVL copy{other};
            swapAvlTrees(*this, copy);
            size=other.size;
            return *this;
        }

        // functions
        AVL_STATUS insertNode(Key key, Data data,int level,int score,int scale);
        Node<Key, Data>* insertNodeInternal(Node<Key, Data> *node, Key key, Data data,int level,int score,int scale, AVL_STATUS *status);
        AVL_STATUS deleteNode(Key key);
        Node<Key, Data>* deleteNodeInternal(Node<Key, Data> *root, Key key, AVL_STATUS *status);

        void destroyTree(Node<Key, Data> *leaf);
        Data getData(Key key)const;
        int getSize()
        {
            return size;
        }
        int getAVLSize()const;
        Data getMaxNode();

        void getNodeInRange(Node<Key, Data> *subtree_root, Key min, Key max, bool getMin, Node<Key,Data> **result, bool* found);




        //private:
        Node<Key, Data> *root;
        bool isInitialized();

        //new
        int SumScoreArray(int k,int score);
        void SumScoreArrayInternal(int k,Node<Key, Data> *root,int score,int *result);
        int GetSubtreeLevelSumByIndex(int k);//that select but return Subtree level
        void GetSubtreeLevelSumByIndexInternal(int k,Node<Key, Data> *root,int *result);//that select but return Subtree level

        int size=0;
        //todo:  delete ?
        int Rank(Key key);
        Data Select(int k);
        void SelectInternal(int k,Node<Key, Data> *root,Data*);

    };
    template<typename Key, typename Data>
    Node<Key, Data>* AVL<Key, Data>::deleteNodeInternal(Node<Key, Data> *root, Key key, AVL_STATUS *status)
    {
        try {
            // Perform BST deletion
            // Find node to delete
            if (root == nullptr) {
                *status = AVL_NOT_FOUND;
                return root;
            } else if (key < root->key) {
                root->left = deleteNodeInternal(root->left, key, status);
            } else if (root->key <key ) {
                root->right = deleteNodeInternal(root->right, key, status);
            }
                // If keys are equal, delete node
            else {
                // If node has 0 children, delete node
                if ((root->left == NULL) && (root->right == NULL)) {
                    auto temp = root;
                    root = NULL;
                    *status = AVL_SUCCESS;
                    delete temp;
                }
                    // If node has 1 child, connect parent to child
                else if ((root->left == NULL) ||
                         (root->right == NULL)) {
                    auto subtree = (root->left == NULL)
                                   ? root->right
                                   : root->left;

                    // Replace node's content with child's content
                    //*root = *subtree; //TODO this is what originally was in the G4G, may be bugs
                    auto temp =root; // todo: tom added, not delete target
                    root = subtree;

                    // Free node //TODO should i delete this? i didnt "new" it
                    delete temp;// todo: tom added, delete targert
                    //delete subtree;
                    *status=AVL_SUCCESS;


                }
                    // If node has 2 children, swapp node with inorder successor
                else {
                    // Get inorder successor
                    auto inorder_successor = getNextInorderNode(root);
                    // todo: tom added, not delete target
                    // Replace node's key and data with successor's key and data
                    root->key = inorder_successor->key;
                    root->data = inorder_successor->data;
                    root->subtree_level_sum = inorder_successor->subtree_level_sum;
                    root->level = inorder_successor->level;
                    root->score=inorder_successor->score;

                    //TODO: should delete root's key, data? were they "new"ed?
                    // delete temp;// todo: tom added, delete targer

                    // Remove inorder successor from tree
                    root->right = deleteNodeInternal(root->right, inorder_successor->key, status);
                    *status = AVL_SUCCESS;
                }
            }
            if (root == NULL)
            {
                return root;
            }

            // Update height of node
            root->height = 1 + max(getNodeHeight(root->left),
                                   getNodeHeight(root->right));

            // Check if node became unbalanced
            int balance = getNodeHeight(root->left) - getNodeHeight(root->right);
            int left_node_balance = root->left == NULL
                                    ? 0
                                    : getNodeHeight(root->left->left) - getNodeHeight(root->left->right);
            int right_node_balance = root->right == NULL
                                     ? 0
                                     : getNodeHeight(root->right->left) - getNodeHeight(root->right->right);

            //TODO: this is different than G4G code.. but balance should be done the same way, no?
            if (balance > 1) {
                if (left_node_balance < 0)
                {
                    root->left = leftRotate(root->left);
                }
                return rightRotate(root);
            }
            if (balance < -1) {
                if (right_node_balance > 0) {
                    root->right = rightRotate(root->right);
                }
                return leftRotate(root);
            }
            root->under_me = getUnderMe(root->left)+ getUnderMe(root->right)+1;
            updateUnderDetails(root);
            return root;
        } catch (std::bad_alloc &) {
            *status = AVL_ALLOCATION_ERROR;
            return nullptr;
        }
    }
    template<typename Key, typename Data>
    AVL_STATUS AVL<Key, Data>::deleteNode(Key key) {
        if (root == NULL)
        {
            return AVL_NOT_FOUND;
        }
        AVL_STATUS status;
        Data node_data = getData(key);

        /*if(!node_data)//node exists
        {
            updateRoute(key,node_data,false);
        }
         */
        root = deleteNodeInternal(root, key, &status);
        if(status==AVL_SUCCESS)
            size--;
        return status;
    }
    template<typename Key, typename Data>
    void AVL<Key, Data>::destroyTree(Node<Key, Data> *leaf) {
        if(leaf)
        {
            destroyTree(leaf->left);
            destroyTree(leaf->right);
            delete leaf;
        }
    }
//todo : changed
    template<typename Key, typename Data>
    int AVL<Key, Data>::getAVLSize() const
    {
        return getAvlSizeInternal(root);
    }

    template<typename Key, typename Data>
    int getAvlSizeInternal(const Node<Key,Data>* head)
    {
        if (head != nullptr)
        {
            int counter=1;
            counter+=getAvlSizeInternal(head->left);
            counter+=getAvlSizeInternal(head->right);
            return counter;
        }
        return 0;
    }
    template<typename Key, typename Data>
    Data AVL<Key, Data>::getMaxNode()
    {
        if(root== nullptr)
        {
            return NULL;
        }
        Node<Key, Data> *iterator = root;
        while(iterator!= nullptr)
        {
            if(iterator->right== nullptr)
            {
                return iterator->data;
            }
            iterator=iterator->right;
        }
        return NULL;

    }
    template<typename Key, typename Data>
    Node<Key, Data> * AVL<Key, Data>::locateNode(Key key, Node<Key, Data> * root, Node<Key, Data> *parent)const {
        if(root== nullptr) //NO CHECK IF KEY IS NULL
        {
            return nullptr;
        }
        Node<Key, Data> *iterator = root;
        while(iterator!= nullptr)
        {

            if(iterator->key==key)
            {

                return iterator;
            }
            if(parent!= nullptr) //update parent
            {
                parent=iterator;
            }
            if(iterator->key<key)//go right
            {
                iterator=iterator->right;
            }
            else //go left
            {
                iterator=iterator->left;
            }
        }
        if(parent!= nullptr)
        {
            parent= nullptr;
        }

        return nullptr;

    }
    template<typename Key, typename Data>
    Data AVL<Key, Data>::getData(Key key)const
    {
        if( root== nullptr)
        {
            return NULL;
        }
        Node<Key, Data> *node =locateNode(key,root, nullptr);
        if(node!= nullptr)
        {
            return node->data;
        }

        return NULL;
    }


    template<typename Key, typename Data>
    int AVL<Key, Data>::max(int a, int b)
    {
        return (a > b)? a : b;
    }


    template<typename Key, typename Data>
    bool AVL<Key, Data>::isInitialized() {
        return (root != nullptr);
    }
    template<typename Key, typename Data>
    int getUnderMe(Node<Key, Data> *node)
    {
        if(node!= nullptr)
        {
            return node->under_me;
        }
        return 0;
    }
    template<typename Key, typename Data>

    void updateUnderDetails(Node<Key, Data>* node)
    {
        int b;
        if(node!=nullptr)
        {
            node->under_me = getUnderMe(node->left)+ getUnderMe(node->right)+1;
            //reset player stats

            node->subtree_level_sum=node->level;
            for(int i=0;i<node->scale+1;i++)
                node->score_array[i]=0;

            //include itselft
            node->score_array[node->score]+=1;

            if(node->left!=nullptr)
            {
                node->subtree_level_sum+=node->left->subtree_level_sum;
                for(int i=0;i<node->scale+1;i++)
                    node->score_array[i]+= node->left->score_array[i];
            }
            if(node->right!=nullptr)
            {
                node->subtree_level_sum+=node->right->subtree_level_sum;
                for(int i=0;i<node->scale+1;i++)
                    node->score_array[i]+=node->right->score_array[i];
            }
            int a;
        }
    }

    template<typename Key, typename Data>
    AVL_STATUS AVL<Key, Data>::insertNode(Key key, Data data,int level,int score,int scale) //O(
    {
        if (root == NULL)//tree is empty
        {
            try{
                root = new Node<Key, Data>(key, data,level,score,scale);
                size++;
                return AVL_SUCCESS;
            } catch (std::bad_alloc&)
            {
                return AVL_ALLOCATION_ERROR;
            }
        }
        AVL_STATUS status;
        root = insertNodeInternal(root, key, data,level,score,scale, &status);
        if(status==AVL_SUCCESS)
        {
            size++;
            //  if(updateRoute(key,data,true)!=AVL_SUCCESS)
            //    return AVL_NOT_FOUND;
        }
        return status;
    }

    template<typename Key, typename Data>
    int getNodeHeight(Node<Key, Data>* node)
    {
        if (node == nullptr)
            return 0;
        return node->height;
    }
    template<typename Key, typename Data>
    int getNodeUnder(Node<Key, Data>* node)
    {
        if (node == nullptr)
            return 0;
        return node->under_me;
    }


    template<typename Key, typename Data>
    Node<Key, Data>* AVL<Key, Data>::insertNodeInternal(Node<Key, Data>* node, Key key, Data data,int level,int score,int scale, AVL_STATUS *status)
    {
        try {
            // BST insertion
            if (node == nullptr)
            {
                *status = AVL_SUCCESS;
                return (new Node<Key, Data>(key, data,level,score,scale));
            }
            else if (key == node->key)
            {
                *status = AVL_KEY_ALREADY_EXISTS;
                return node;
            }
            else if (key < node->key)
            {
                node->left = insertNodeInternal(node->left, key, data,level,score,scale, status);
            }
            else {
                node->right = insertNodeInternal(node->right, key, data,level,score,scale, status);

            }

            // Update height
            node->height = 1 + max(getNodeHeight(node->left), getNodeHeight(node->right));

            // Check if node became unbalanced
            int balance = getNodeHeight(node->left) - getNodeHeight(node->right);

            if (balance > 1) {
                //todo: changed only < exists
                if ( node->left->key< key) {
                    node->left = leftRotate(node->left);
                }
                return rightRotate(node);
            }
            if (balance < -1) {
                if (key < node->right->key) {
                    node->right = rightRotate(node->right);
                }
                return leftRotate(node);
            }
            //node->under_me = getUnderMe(node->left)+ getUnderMe(node->right)+1;
            updateUnderDetails(node);
            return node;
        } catch (std::bad_alloc &) {
            *status = AVL_ALLOCATION_ERROR;
            return nullptr;
        }
    }

    template<typename Key, typename Data>
    Node<Key, Data>* getNextInorderNode(Node<Key, Data>* node)
    {
        assert(node != nullptr);
        if (node == nullptr)
        {
            return node;
        }

        // single step right
        node = node->right;

        // find leftmost leaf
        while (node->left != nullptr)
        {
            node = node->left;
        }

        return node;
    }



    template<typename Key, typename Data>
    Node<Key, Data>* AVL<Key, Data>::rightRotate(Node<Key, Data>* node)
    {
        //todo can new_root be null? can this occur?
        Node<Key, Data>* new_root = node->left;
        Node<Key, Data>* subtree = (new_root != nullptr)
                                   ? new_root->right
                                   : nullptr;

        // rotate
        new_root->right = node;
        node->left = subtree;

        // update heights
        node->height = 1 + max(getNodeHeight(node->left),
                               getNodeHeight(node->right));
        new_root->height = 1 + max(getNodeHeight(new_root->left),
                                   getNodeHeight(new_root->right));

        // return new root
        updateUnderDetails(node);
        updateUnderDetails(new_root);
        //node->under_me = getUnderMe(node->left)+ getUnderMe(node->right)+1;
        //new_root->under_me = getUnderMe(new_root->left)+ getUnderMe(new_root->right)+1;
        return new_root;
    }

    template<typename Key, typename Data>
    Node<Key, Data> *AVL<Key, Data>::leftRotate(Node<Key, Data> *node)
    {
        auto new_root = node->right;
        auto subtree = (new_root != nullptr)
                       ? new_root->left
                       : nullptr;

        // rotate
        new_root->left = node;
        node->right = subtree;

        // update heights
        node->height = 1 + max(getNodeHeight(node->left),
                               getNodeHeight(node->right));
        new_root->height = 1 + max(getNodeHeight(new_root->left),
                                   getNodeHeight(new_root->right));


        // return new root
        updateUnderDetails(node);
        updateUnderDetails(new_root);
        //node->under_me = getUnderMe(node->left)+ getUnderMe(node->right)+1;
        //new_root->under_me = getUnderMe(new_root->left)+ getUnderMe(new_root->right)+1;
        return new_root;
    }
/*
    template<typename Key, typename Data>
    AVL_STATUS AVL<Key, Data>::updateRoute(Key new_key,Data new_data,bool element_added) {
            Node<Key, Data> *iterator = root;
            while(iterator!= nullptr)
            {
                if(iterator->key==new_key)
                {
                    return AVL_SUCCESS;
                }
                if(iterator->key<new_key)//go right
                {
                    if(element_added)
                    {
                        iterator->data=iterator->data+new_data;
                        iterator->under_me+=1;
                    }
                    else
                    {
                        iterator->data=iterator->data-new_data;
                        iterator->under_me-=1;
                    }
                    iterator=iterator->right;
                }
                else //go left
                {
                    if(element_added)
                    {
                        iterator->data=iterator->data+new_data;
                        iterator->under_me+=1;
                    }
                    else {
                        iterator->data = iterator->data - new_data;
                        iterator->under_me-=1;
                    }
                    iterator=iterator->left;
                }
            }
        return AVL_NOT_FOUND;
    }
*/
    template<typename Key, typename Data>
    int AVL<Key, Data>::Rank(Key key) {
        int rank=0;
        auto iterator = root;
        while(iterator != nullptr)
        {
            if(iterator->key==key)//found node
            {

                return rank+ getUnderMe(iterator->left)+1;
            }
            if(iterator->key<key)//go right
            {

                rank+= getUnderMe(iterator->left)+1;
                iterator=iterator->right;
            }
            else //go left
            {
                iterator=iterator->left;
            }
        }
        return -1;

    }

    template<typename Key, typename Data>
    void AVL<Key, Data>::SelectInternal(int k, Node<Key, Data> *new_root,Data* data) {
        if(new_root != nullptr)
        {
            /*if(k==0)//found
            {
                *data=new_root->data;
                return;
            }*/
            if(getNodeUnder(new_root->left)==k-1) // there is left son
            {
                *data=new_root->data;
                return;
            }
            if(getNodeUnder(new_root->left)>k-1)//need to go left
                SelectInternal(k,new_root->left,data);
            if(getNodeUnder(new_root->left)<k-1)
                SelectInternal(k-getNodeUnder(new_root->left)-1,new_root->right,data);

        }
    }

    template<typename Key, typename Data>
    Data AVL<Key, Data>::Select(int k) {
        Data temp;
        if(root == nullptr||k>size)
        {
            return NULL;
        }
        SelectInternal(k,root,&temp);
        return temp;
    }

    template<typename Key, typename Data>
    int AVL<Key, Data>::GetSubtreeLevelSumByIndex(int k) {
        if(root == nullptr||k>size)
        {
            return -1;
        }
        int result=0;
        GetSubtreeLevelSumByIndexInternal(k,root,&result);
        return result;

    }

    template<typename Key, typename Data>
    void AVL<Key, Data>::GetSubtreeLevelSumByIndexInternal(int k,Node<Key, Data> *new_root,int *result) {
        if(new_root != nullptr)
        {
            if(getNodeUnder(new_root->left)==k-1) // there is left son
            {
                if(new_root->left!=nullptr) // add left botton
                {
                    *result+=new_root->left->subtree_level_sum;
                }
                return;
            }
            if(getNodeUnder(new_root->left)>k-1)//need to go left
                GetSubtreeLevelSumByIndexInternal(k,new_root->left,result);
            if(getNodeUnder(new_root->left)<k-1)
            {
                if(new_root->right!=nullptr)
                {
                    *result+=new_root->subtree_level_sum-new_root->right->subtree_level_sum;
                }

                GetSubtreeLevelSumByIndexInternal(k-getNodeUnder(new_root->left)-1,new_root->right,result);
            }


        }
    }

    template<typename Key, typename Data>
    int AVL<Key, Data>::SumScoreArray(int k,int score) {
        if(root == nullptr||k>size) //index bigger than tree size
        {
            return -1;
        }
        int result=0;
        SumScoreArrayInternal(k,root,score,&result);
        return result;

    }

    template<typename Key, typename Data>
    void AVL<Key, Data>::SumScoreArrayInternal(int k,Node<Key, Data> *new_root,int score,int *result) {
        if(new_root != nullptr)
        {
            if(getNodeUnder(new_root->left)==k-1) // there is left son
            {
                if(new_root->left!=nullptr) // add left botton
                {
                    *result+=new_root->left->score_array[score];
                }
                return;
            }
            if(getNodeUnder(new_root->left)>k-1)//need to go left
                SumScoreArrayInternal(k,new_root->left,score,result);
            if(getNodeUnder(new_root->left)<k-1)
            {
                if(new_root->right!=nullptr)
                {
                    *result+=new_root->score_array[score]-new_root->right->score_array[score];
                }

                SumScoreArrayInternal(k-getNodeUnder(new_root->left)-1,new_root->right,score,result);
            }


        }
    }

    template<typename Key, typename Data>
    void AVL<Key, Data>::getNodeInRange(Node<Key, Data> *subtree_root, Key min, Key max, bool getMin, Node<Key,Data> **result, bool* found)
    {
        if (subtree_root == nullptr)
            return;

        // recurse left subtree
        if (min < subtree_root->key)
        {
            getNodeInRange(subtree_root->left, min, max, getMin, result, found);
        }

        // if key in range, update result
        // NOTE - dont check equality because in our case equality will never occur
        if (min < subtree_root->key && subtree_root->key < max)
        {
            if (getMin)
            {
                // ensure update is correct
                // if our result already in range, ensure we are updating to something smaller
                if (min < (*result)->key && (*result)->key < max)
                {
                    if (subtree_root->key < (*result)->key)
                    {
                        (*result)= subtree_root;
                        *found = true;
                    }

                }
                else {
                    (*result) = subtree_root;
                    *found = true;
                }

            }
            else{
                // ensure update is correct
                // if our result already in range, ensure we are updating to something bigger
                if (min < (*result)->key && (*result)->key < max)
                {
                    if ((*result)->key < subtree_root->key)
                    {
                        (*result) = subtree_root;
                        *found = true;
                    }

                }
                else {
                    (*result) = subtree_root;
                    *found = true;
                }
            }
        }

        // recurse right subtree
        getNodeInRange(subtree_root->right, min, max, getMin, result, found);
    }


    //todo: changed
    template<typename Key, typename Data>
    void getInOrderKeyArray(AVL<Key, Data>* tree,Key* keys,int num_of_nodes) //O(n) - n number of nodes in tree
    {
        int index = 0; // when adding something to the array we need an index
        getInOrderKeyArrayInternal(tree->root, keys,&index,num_of_nodes-1);
    }

    template<typename Key, typename Data>
    void getInOrderKeyArrayInternal(Node<Key, Data> *node, Key* keys  ,int* index,int num_of_nodes) {
        if (node && (*index)<=num_of_nodes)
        {
            getInOrderKeyArrayInternal(node->left, keys, index,num_of_nodes);   // first do every left child tree
            if (*index > num_of_nodes)
                return;
            keys[*index] = node->key;// then write the data in the array
            (*index)+=1;
            getInOrderKeyArrayInternal(node->right, keys, index,num_of_nodes);  // do the same with the right child
        }

    }
    // Function stores inorder traversal of tree in array
    // O(n)
    template<typename Key, typename Data>
    void storeInorderTreeAsArray(Node<Key, Data>* node, Node<Key, Data>* inorder_array[], int* index)
    {
        if (node == nullptr)
        {
            return;
        }

        // Traverse inorder
        storeInorderTreeAsArray(node->left, inorder_array, index);
        inorder_array[*index] = node;
        (*index)++;
        storeInorderTreeAsArray(node->right, inorder_array, index);
    }

    // Function merges two sorted arrays
    // O(n+m)
    template<typename Key, typename Data>
    void mergeSortedArrays(Node<Key, Data>* first_array[], Node<Key, Data>* second_array[],
                           int first_array_size, int second_array_size,
                           Node<Key, Data>* merged_array[])
    {
        // create result array if null
        if (merged_array == nullptr) {
            return;
        }

        // merge arrays
        int i=0, j=0, k=0;
        while (i<first_array_size && j<second_array_size)
        {
            if (first_array[i]->key < second_array[j]->key)
            {
                merged_array[k] = first_array[i];
                i++;
                k++;
            }
            else
            {
                merged_array[k] = second_array[j];
                j++;
                k++;
            }
        }

        // add remaining elements if any
        while (i < first_array_size)
        {
            merged_array[k] = first_array[i];
            i++;
            k++;
        }

        while (j < second_array_size)
        {
            merged_array[k] = second_array[j];
            j++;
            k++;
        }
    }


    template<typename Key, typename Data>
    Node<Key,Data>*  storeSortedArrayAsAvlInternal(Node<Key, Data>* array[], int left, int right)
    {
        if (left > right)
            return NULL;

        // make middle root of subtree
        int middle = (left+right)/2;
        auto root = new Node<Key, Data>(array[middle]->key, array[middle]->data,array[middle]->level,array[middle]->score,array[middle]->scale);

        // create left subtree
        root->left = storeSortedArrayAsAvlInternal(array, left, middle - 1);

        // create right subtree
        root->right = storeSortedArrayAsAvlInternal(array, middle + 1, right);

        // fix height
        if ((root->left != nullptr) || (root->right != nullptr))
        {
            root->height = (root->left != nullptr)
                           ? root->left->height + 1
                           : root->right->height + 1;
        }

        // new
        updateUnderDetails(root);

        return root;
    }

    // Function creates an AVL tree from a sorted array
    template<typename Key, typename Data>
    AVL<Key,Data>*  storeSortedArrayAsAvl(Node<Key, Data>* array[], int array_size)
    {
        auto avl = new AVL<Key,Data>;
        avl->root = storeSortedArrayAsAvlInternal(array, 0, array_size - 1);
        return avl;
    }
    template<typename Key, typename Data>
    AVL<Key,Data>* mergeTrees(AVL<Key,Data>* first, AVL<Key,Data>* second)
    {
       // int first_size = first->getAVLSize();
       // int second_size = second->getAVLSize();
        int first_size= first->size;
        int second_size= second->size;
        // flatten trees to arrays
        Node<Key, Data>* first_array[first_size];
        int first_counter = 0;
        // fill first array
        storeInorderTreeAsArray(first->root, first_array, &first_counter);

        Node<Key, Data>* second_array[second_size];
        int second_counter = 0;
        // fill second array
        storeInorderTreeAsArray(second->root, second_array, &second_counter);

        // merge
        auto merged = new Node<Key,Data>*[first_size + second_size];
        mergeSortedArrays(first_array, second_array, first_size, second_size, merged);

        auto avl = storeSortedArrayAsAvl(merged, first_size + second_size);
        delete[] merged;
        avl->size=first_size+second_size;
        return avl;
    }

}


#endif //AVLTREE_AVLTREE_H
