#include "b-plus-tree.h"
#include <algorithm> 
#include <iostream> 
#include <cmath> 
#include <queue> 

using std::cout;
using std::endl;
using std::sort;
using std::reverse; 





Node::Node() {
    key = new KeyStruct[MAX]; 
    ptr = new Node*[MAX + 1]; 
    for (int i = 0; i <= MAX; ++i) {
        ptr[i] = nullptr;  // Initialize each pointer to nullptr
    }
    size = 0;
    isLeaf = true;
}


BPlusTree::BPlusTree() {
    root = nullptr;
}

Node* BPlusTree::getLeafNode(float value) {
    if (!root) return nullptr;

    Node *cur = root; 
        
    while (!cur->isLeaf){
        int size = cur->size; 
        for (int i = 0; i < size; i++){
            if (value < cur->key[i].value){
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



Node* BPlusTree::findParent(Node* cur, Node* child) {
    if (cur->isLeaf)
        return nullptr;

    for (int i = 0; i <= cur->size; i++)
        if (cur->ptr[i] == child)
            return cur;
    
    //dfs on each child node
    for (int i = 0; i <= cur->size; i++){
        Node *p = findParent(cur->ptr[i], child);
        if (p)
            return p;
    }
    return nullptr; 
}



void BPlusTree::insertInternalNode(KeyStruct data, Node* parent, Node* child) {
   //parent is not full we will just insert the new key 
    if (parent->size < MAX){
        int i = 0; 
        while (data.value > parent->key[i].value and i < parent->size)
            i++;

        //make space for new key 
        for (int j = parent->size; j > i; j--)
            parent->key[j] = parent->key[j - 1]; 

        //make space for new pointer
        for (int j = parent->size + 1; j > i + 1; j--)
            parent->ptr[j] = parent->ptr[j - 1];

        parent->key[i] = data;
        parent->ptr[i + 1] = child;
        parent->size++;

        return; 
    }

    //parent is full so we split the parent node
    vector<KeyStruct> tempKeyStruct(MAX+1);
    vector<Node *> tempPointers(MAX + 2);
    for (int i = 0; i < MAX; i++)
        tempKeyStruct[i] = parent->key[i];

    for (int i = 0; i < MAX + 1; i++)
        tempPointers[i] = parent->ptr[i];
    
    int i = 0; 
    while (data.value > tempKeyStruct[i].value and i < MAX)
        i++; 
        
    //make space for new key
    for (int j = MAX; j > i; j--)
        tempKeyStruct[j] = tempKeyStruct[j - 1];

    for (int j = MAX + 1; j > i + 1; j--)
        tempPointers[j] = tempPointers[j - 1];

    tempKeyStruct[i] =  data;
    tempPointers[i + 1] = child;

    
    Node *newInternal = new Node();
    newInternal->isLeaf = false;


    parent->size = ceil(MAX / 2.00);
    newInternal->size = floor(MAX / 2.00);



    //smallest value from second node will be extracted at lifted as new root node
    KeyStruct newRootKey = tempKeyStruct[parent->size];
    

    //insert keys and pointers back to parent and newInternal node
    for (i = 0; i < parent->size; i++)
        parent->key[i] = tempKeyStruct[i];
    
    for (int j = 0, i = parent->size+1; j < newInternal->size; i++, j++)
        newInternal->key[j] = tempKeyStruct[i];

    for (i = 0; i < parent->size + 1; i++)
        parent->ptr[i] = tempPointers[i];

    for (int j = 0, i = parent->size + 1; j < newInternal->size + 1; i++, j++)
        newInternal->ptr[j] = tempPointers[i]; 


    if (parent == root){
        Node *newRoot = new Node();
        newRoot->key[0] = newRootKey;
        newRoot->ptr[0] = parent;
        newRoot->ptr[1] = newInternal;
        newRoot->isLeaf = false;
        newRoot->size++;
        root = newRoot; 
    } else {
        Node *grandparent = findParent(root, parent);
        insertInternalNode(newRootKey, grandparent, newInternal); 
    }
}

Node * BPlusTree::getRoot(){
    return root; 
}

KeyStruct BPlusTree::search(float value) {
    Node *leafNode = getLeafNode(value);
    
    //search in leaf node
    for (int i = 0; i < leafNode->size; i++){
        if (value == leafNode->key[i].value) 
            return leafNode->key[i];
    }

    
    return {-1, {{-1, -1}}}; 
}


vector<KeyStruct> BPlusTree::searchInterval(float lowerBound, float upperBound, int &numNodesAccessed) {
    vector<KeyStruct> res;
    Node *leafNode = getLeafNode(lowerBound); 
    numNodesAccessed++;

    while (leafNode){
        for (int i = 0; i < leafNode->size; i++){
                auto data = leafNode->key[i]; 
                if (data.value >= lowerBound and data.value <= upperBound)
                    res.push_back(data);

                if (data.value > upperBound)
                    return res; 
        }
        leafNode = leafNode->ptr[leafNode->size]; 
        numNodesAccessed++;
    }
    
    return res; 

}


void BPlusTree::insert(KeyStruct data) {
    //empty tree
    if (!root){
        cout << "Tree is empty. Creating a root... " << endl; 
        root = new Node();
        root->size++;
        root->key[0] = data;
        return; 
    }


    //get the correct leafnode 
    Node *leafNode = getLeafNode(data.value);
    
    if (!leafNode){
        cout << "Error in inserting new data: " << data.value << endl; 
        return; 
    }
    for (int i = 0; i < leafNode->size; i++){
        //check if key with same value already exists
        if (leafNode->key[i].value == data.value){
            vector<Record_Location> cur = leafNode->key[i].addresses;
            cur.push_back(data.addresses[0]);
            leafNode->key[i] = {data.value, cur}; 
            return; 
        }
    }
    
    //insert new key into cur leaf node 
    //leafNode is not full yet
    if (leafNode->size < MAX){
    
        int i = 0; 
        while (data.value > leafNode->key[i].value and i < leafNode->size) 
            i++;

        //make space for insertion
        for (int j = leafNode->size; j > i; j--)
            leafNode->key[j] = leafNode->key[j - 1];

        leafNode->key[i] = data;
        leafNode->size++;
        leafNode->ptr[leafNode->size] = leafNode->ptr[leafNode->size - 1];
        leafNode->ptr[leafNode->size - 1] == nullptr; 
    } else { 

        //leafNode is full make a new node
        Node *newLeaf = new Node();

        vector<KeyStruct> tempKeyStruct(MAX+1);
        for (int i = 0; i < leafNode->size; i++)
            tempKeyStruct[i] = leafNode->key[i];
        
        int i = 0; 
        while(data.value > tempKeyStruct[i].value and i < MAX)
            i++;
        for (int j = MAX; j > i; j--)
            tempKeyStruct[j] = tempKeyStruct[j - 1]; 
        tempKeyStruct[i] = data;

     

        // set up new leaf
        leafNode->size =  ceil((MAX + 1) / 2.00);
        newLeaf->size =  floor((MAX + 1)/2.00);



        leafNode->ptr[leafNode->size] = newLeaf;
        newLeaf->ptr[newLeaf->size] = leafNode->ptr[MAX];


        for (int i = 0; i < leafNode->size; i++)
            leafNode->key[i] = tempKeyStruct[i];

        for (int j = 0, i = leafNode->size; j < newLeaf->size; i++, j++)
            newLeaf->key[j] = tempKeyStruct[i]; 



        //modify the parent node
        //cur leaf is root node
        if (leafNode == root){
            Node *newRoot = new Node();
            newRoot->isLeaf = false; 
            newRoot->key[0] = newLeaf->key[0];
            newRoot->ptr[0] = leafNode;
            newRoot->ptr[1] = newLeaf;
            newRoot->size++; 
            root = newRoot;


        } else{ // cur leaf is not root

            // get parent node
            Node *parent = findParent(root, leafNode);
            //update parent nodes for the new leaf
            insertInternalNode(newLeaf->key[0], parent, newLeaf); 
        }
    }
}



//only use bulkInsert on empty tree i.e. root = null 
void BPlusTree::bulkInsert(vector<KeyStruct> dataList){
    if (root)
        return; 

    for (auto cur: dataList)
        insert(cur); 
}


int getHeightUtil(Node *cur){ 
    if (!cur->isLeaf)
        return 1 + getHeightUtil(cur->ptr[0]);
    return 1; 
}

int BPlusTree::getHeight(){
    return getHeightUtil(root);
}

void BPlusTree::displayAllKeys(){ 
    if (!root){
        cout << "B+ tree is currently empty." << endl;
        return; 
    }

    std::queue<Node *> q;
    q.push(root);

    int level = 1; 
    while (!q.empty()){
        cout << "Level " << level << " : "; 
        int size = q.size();
        for (int i = 0; i < size; i++){ 
            Node *cur = q.front();
            q.pop();

            //print nodes of current keys
            cout << "[ "; 
            for (int i = 0; i < cur->size; i++) 
                cout << cur->key[i].value << " ";
            cout << "] "; 

            if (cur->isLeaf)
                continue;
            
            //enqueue child nodes if cur node is internal node
            for (int i = 0; i < cur->size + 1; i++)
                q.push(cur->ptr[i]);
        }
        level++; 
        cout << endl;
    }
}
