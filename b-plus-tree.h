#ifndef B_PLUS_TREE_H
#define B_PLUS_TREE_H

#include <vector>
using std::vector; 


// KeyStruct structure representing the final KeyStruct of a leaf node in the B+ Tree
class KeyStruct {
public:
    float value; // The value of the key

    //vector<void*> stores the addresses of one relevant record
    //vector<vector<void*>> stores the addresses of multiple records having the same key
    vector<vector<void *>> addresses; 
};

// Node class definition
class Node {
public:    
    int size; 
    vector<KeyStruct*> key; //store keys 
    vector<Node*> ptr;
    bool isLeaf;

    Node();  // Constructor

    friend class BPlusTree;
};

// BPlusTree class definition
class BPlusTree {
private:
    Node* root;

    Node* getLeafNode(float value);            // Utility function to get the leaf node of a given value
    Node* findParent(Node* cur, Node* child);  // Utility function to find the parent of a node
    void insertInternalNode(KeyStruct* data, Node* parent, Node* child);  // Insert an internal node

public:
    BPlusTree();  // Constructor
    Node* getRoot();  // Getter for root
    KeyStruct* search(float value);  // Search for a value in the tree
    vector<KeyStruct*> searchInterval(float lowerBound, float upperBound, int &numNodesAccessed);  // Search within a range
    void insert(KeyStruct* data);  // Insert a new key into the tree
    void bulkInsert(vector<KeyStruct *> dataList);
};

#endif // B_PLUS_TREE_H
