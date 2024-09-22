#include <stdlib.h> 
#include <iostream>
#include <vector>

using std::vector; 
using std::cout;
using std::endl;

int MAX = 256; 

struct Date {
    unsigned int day   : 5;  
    unsigned int month : 4;  
    unsigned int yearOffset  : 5; 
};
//to replace with actual addresses 

struct Data{
    float value;
    vector <void*> addresses; //allow different addresses with same key 
};

class BPTree;
class Node {
    int size; 
    vector<Data*> key;
    vector<Node*> ptr;

    bool isLeaf;
    //Node *nextLeaf; 
    friend class BPTree;

public:
    Node() {
        key.resize(MAX);  // Resizes the vector to hold 'MAX' elements
        ptr.resize(MAX + 1);  // Resizing to hold MAX + 1 child pointers
        size = 0;
        isLeaf = true;
        //nextLeaf = NULL; 
    }
};



class BPTree{
    Node *root; 

    //utility func to get the possible leaf node containing the value 
    Node *getLeafNode(float value){ 
        if (!root) return NULL;

        Node *cur = root; 
            
        while (!cur->isLeaf){
            for (int i = 0; i < cur->size; i++){
                if (value < cur->key[i]->value){
                    cur = cur->ptr[i];
                    break; 
                }
                    //last pointer case
                if (i == cur->size - 1)
                    cur = cur->ptr[i + 1]; 
            }
        }

        return cur; 
    }

    Node *findParent(Node *cur, Node *child){ 
        if (cur->isLeaf or cur->ptr[0]->isLeaf)
            return NULL;

        for (int i = 0; i <= cur->size; i++){
            if (cur->ptr[i] == child)
                return cur; 
            else {
                //dfs
                Node *p = findParent(cur->ptr[i], child); 
                if (p)
                    return p; 
            }
        }
        return NULL; 
    }

    void insertInternalNode(Node *parent, Node *child, Data *data){ 
        
    }
    public: 
        BPTree(){
            root = NULL; 
        }

        Node *getRoot(){
            return root; 
        }

        int height(Node *cur){ 
            if (!cur) return 0;
            if (cur->isLeaf)
                return 1;
            return height(cur->ptr[0]) + 1; 
        }

        Data* search(float value){

            Node *leafNode = getLeafNode(value); 
            //search in leaf node
            for (auto data: leafNode->key){ 
                if (value == data->value) 
                    return data; 
            }
            return NULL; 
        }

        vector<Data*> searchInterval(float lowerBound, float upperBound){
            vector<Data *> res;

            Node *leafNode = getLeafNode(lowerBound); 
            
            while (leafNode){
                for (auto data: leafNode->key){
                    if (data->value >= lowerBound and data->value <= upperBound)
                        res.push_back(data); 

                    if (data->value > upperBound)
                        return res; 
                }
                leafNode = leafNode->ptr[leafNode->size]; 
            }
          
            return res; 
        }


        void insert(Data *data){ 
            //empty tree
            if (!root){
                root = new Node();
                root->size++;
                root->key.push_back(data); 
                return; 
            }


            //get the correct leafnode 
            Node *leafNode = getLeafNode(data->value);
            if (!leafNode){
                cout << "Error in inserting new data: " << data->value << endl; 
                return; 
            }
          

            for (auto &d: leafNode->key){
                //check if key with same value already exists
                if (d->value == data->value){
                    d->addresses.push_back(data->addresses[0]);
                    return; 
                }
            }
            
            //insert new key into cur leaf node 
            //leafNode is not full yet
            if (leafNode->size < MAX){
                int i = 0; 
                while (data->value > leafNode->key[i]->value and i < leafNode->size) 
                    i++;

                //make space for insertion
                for (int j = leafNode->size; j > i; j--)
                    leafNode->key[j] = leafNode->key[j - 1];

                leafNode->key[i] = data;
                leafNode->size++;
                leafNode->ptr[leafNode->size] = leafNode->ptr[leafNode->size - 1];
                leafNode->ptr[leafNode->size - 1] == NULL; 
            } else { 
                //leafNode is full make a new node
                Node *newLeaf = new Node();

                vector<Data *> temp(MAX+1);
                int i = 0; 
                for (auto key: leafNode->key)
                    temp[i++] = key;
                
                i = 0; 
                while(data->value > temp[i]->value and i < MAX)
                    i++;
                for (int j = MAX; j > i; j--)
                    temp[j] = temp[j - 1]; 
                temp[i] = data; 


                //set up new leaf;
                leafNode->size = (MAX + 1) / 2;
                newLeaf->size = MAX + 1 - (MAX + 1) / 2;

                leafNode->ptr[leafNode->size] = newLeaf;
                newLeaf->ptr[newLeaf->size] = leafNode->ptr[MAX];

                leafNode->key.clear();
                for (int i = 0; i < leafNode->size; i++)
                    leafNode->key[i] = temp[i];

                for (int j = 0, i = leafNode->size; j < newLeaf->size; i++, j++)
                    newLeaf->key[j] = temp[i]; 


                //modify the parent node
                //cur leaf is root node
                if (leafNode == root){
                    Node *newRoot = new Node();
                    newRoot->key[0] = newLeaf->key[0];
                    newRoot->ptr[0] = leafNode;
                    newRoot->ptr[1] = newLeaf;
                    newRoot->size++; 
                    root = newRoot; 
                } else{ // cur leaf is not root

                    // get parent node
                    Node *parent = findParent(root, leafNode);

                    insertInternalNode(parent, newLeaf, newLeaf->key[0]); 
                }
            }
        }

}; 
