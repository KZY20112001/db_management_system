#include <stdlib.h> 
#include <vector>

using std::vector; 



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
    Node *nextLeaf; 
    friend class BPTree;

public:
    Node() {
        key.resize(MAX);  // Resizes the vector to hold 'MAX' elements
        ptr.resize(MAX + 1);  // Resizing to hold MAX + 1 child pointers
        size = MAX;
        isLeaf = true;
        nextLeaf = NULL; 
    }
};



class BPTree{
    Node *root; 

    //utility func to get the leaf node containing the value 
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
                leafNode = leafNode->nextLeaf; 
            }
          
            return res; 
        }

}; 
