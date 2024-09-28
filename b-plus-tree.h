#ifndef B_PLUS_TREE_H
#define B_PLUS_TREE_H

#include <iostream>
#include <vector>
#include <cmath>  
using std::vector; 


// Datablock structure representing the final datablock of a leaf node in the B+ Tree
class Datablock {
public:
    float value;                // The value of the key

    //vector<void*> stores the addresses of one relevant record
    //vector<vector<void*>> stores the addresses of multiple records having the same key
    vector<vector<void *>> addresses; 
};

// Node class definition
class Node {
public:    
    int size; 
    vector<Datablock*> key;
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
    void insertInternalNode(Datablock* data, Node* parent, Node* child);  // Insert an internal node

public:
    BPlusTree();  // Constructor
    Node* getRoot();  // Getter for root
    Datablock* search(float value);  // Search for a value in the tree
    vector<Datablock*> searchInterval(float lowerBound, float upperBound);  // Search within a range
    void insert(Datablock* data);  // Insert a new key into the tree
};

#endif // B_PLUS_TREE_H
