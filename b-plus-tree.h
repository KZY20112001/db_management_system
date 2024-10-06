#ifndef B_PLUS_TREE_H
#define B_PLUS_TREE_H

#include "Disk_Storage.h"
#include <vector>
using std::vector; 

const int MAX = 32; 

// KeyStruct structure representing the final KeyStruct of a leaf node in the B+ Tree
struct KeyStruct {
    float value; // The value of the key
    vector<Record_Location> addresses; //store in a vector in case there are duplicate record locations
};


// Node class definition
class Node {
public:    
    int size; 
    KeyStruct* key; //store keys 
    Node** ptr;
    bool isLeaf;

    Node();  // Constructor
    friend class BPlusTree;
};

// BPlusTree class definition
class BPlusTree {
private:
    int nodeCount; 
    Node* root;
    Node* getLeafNode(float value, int &numNodesAccessed);            // Utility function to get the leaf node of a given value
    Node* findParent(Node* cur, Node* child);  // Utility function to find the parent of a node
    void insertInternalNode(KeyStruct data, Node* parent, Node* child);  // Insert an internal node

public:
    BPlusTree();  // Constructor
    Node* getRoot();  // Getter for root
    KeyStruct search(float value, int &numNodesAccessed);  // Search for a value in the tree
    vector<KeyStruct> searchInterval(float lowerBound, float upperBound, int &numNodesAccessed);  // Search within a range
    void insert(KeyStruct data);  // Insert a new key into the tree
    void bulkInsert(vector<KeyStruct> dataList); // Insert multiple KeyStruct into the tree
    int getHeight(); // Get height of the b+ tree
    int getNodeCount(); 
    void displayAllKeys(); 
};

#endif // B_PLUS_TREE_H
