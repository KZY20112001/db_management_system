#include <iostream>
#include <string> 
#include <sstream> 
#include <fstream>
#include <climits>
#include <vector>

using std::vector; 



int MAX = 256; 

struct keys_struct{
    float key_value;
    vector <void*> add_vect;
};

class BPTree;
class Node {
    bool IS_LEAF;
    int size;
    vector<keys_struct> key;
    vector<Node*> ptr; 
    friend class BPTree;

public:
    Node() {
        key.resize(MAX);  // Resizes the vector to hold 'MAX' elements
        ptr.resize(MAX + 1);  // Resizing to hold MAX + 1 child pointers
        IS_LEAF = true;
    }
};



class BPTree{
    Node *root; 
    public: 
        BPTree(){
            root = NULL; 
        }

        Node *getRoot(){
            return root; 
        }

}; 
