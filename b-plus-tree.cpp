#include "b-plus-tree.h"
#include <algorithm> 
#include <iostream> 
#include <cmath> 

using std::cout;
using std::endl;
using std::sort;
using std::reverse; 
int MAX = 256; 


Node::Node() {
    key.resize(MAX);  // Resizes the vector to hold 'MAX' elements
    ptr.resize(MAX + 1);  // Resizing to hold MAX + 1 child pointers
    size = 0;
    isLeaf = true;
}


BPlusTree::BPlusTree() {
    root = NULL;
}

Node* BPlusTree::getLeafNode(float value) {
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



Node* BPlusTree::findParent(Node* cur, Node* child) {
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



void BPlusTree::insertInternalNode(KeyStruct* data, Node* parent, Node* child) {
   //parent is not full we will just insert the new key 
    if (parent->size < MAX){
        int i = 0; 
        while (data->value > parent->key[i]->value and i < parent->size)
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
    vector<KeyStruct *> tempKeyStruct(MAX+1);
    vector<Node *> tempPointers(MAX + 2);
    for (int i = 0; i < MAX; i++)
        tempKeyStruct[i] = parent->key[i];

    for (int i = 0; i < MAX + 1; i++)
        tempPointers[i] = parent->ptr[i];
    
    int i = 0; 
    while (data->value > tempKeyStruct[i]->value and i < MAX)
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

    //smallest value from second node will be extracted at lifted as new root node
    KeyStruct *newRootKey = tempKeyStruct[parent->size];
    
    parent->size = ceil(MAX / 2.00);
    newInternal->size = floor(MAX / 2.00);

    
    
    parent->key.clear();
    parent->ptr.clear(); 
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

KeyStruct* BPlusTree::search(float value) {
    Node *leafNode = getLeafNode(value); 
    //search in leaf node
    for (auto data: leafNode->key){ 
        if (value == data->value) 
            return data; 
    }
}


vector<KeyStruct*> BPlusTree::searchInterval(float lowerBound, float upperBound, int &numNodesAccessed) {
    vector<KeyStruct *> res;
    Node *leafNode = getLeafNode(lowerBound); 
    numNodesAccessed++;
    
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


void BPlusTree::insert(KeyStruct* data) {
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

        vector<KeyStruct *> tempKeyStruct(MAX+1);
        int i = 0; 
        for (auto key: leafNode->key)
            tempKeyStruct[i++] = key;
        
        i = 0; 
        while(data->value > tempKeyStruct[i]->value and i < MAX)
            i++;
        for (int j = MAX; j > i; j--)
            tempKeyStruct[j] = tempKeyStruct[j - 1]; 
        tempKeyStruct[i] = data; 


        //set up new leaf;
        leafNode->size = ceil((MAX + 1) / 2.00); 
        newLeaf->size = floor((MAX + 1)/2.00);

        leafNode->ptr[leafNode->size] = newLeaf;
        newLeaf->ptr[newLeaf->size] = leafNode->ptr[MAX];

        leafNode->key.clear();
        for (int i = 0; i < leafNode->size; i++)
            leafNode->key[i] = tempKeyStruct[i];

        for (int j = 0, i = leafNode->size; j < newLeaf->size; i++, j++)
            newLeaf->key[j] = tempKeyStruct[i]; 


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

            //update parent nodes for the new leaf
            insertInternalNode(newLeaf->key[0], parent, newLeaf); 
        }
    }
}



//only use bulkInsert on empty tree i.e. root = null 
void BPlusTree::bulkInsert(vector<KeyStruct *> dataList){
    if (root)
        return; 

    for (auto cur: dataList)
        insert(cur); 
       
    return; 



    //build leaf level
    Node *cur, *prev = NULL;
    vector<Node*> leafNodes; 
    int i = 0;
    while (i < dataList.size()){
        cur = new Node();
        for (int j = 0; j < MAX and i < dataList.size(); i++,j++){
            cur->key[j] = dataList[i];
            cur->size++; 
        }

        if (prev)
            prev->ptr[prev->size] = cur;
        leafNodes.push_back(cur); 
        prev = cur;    
    }

    //check if last leaf node have min number of Keys (if not redistribute the key from perv last key)
    int MIN = (MAX + 1) / 2;
    if (!leafNodes.empty() && leafNodes.back()->size < MIN){
        Node *lastNode = leafNodes.back();
        Node *secondLastNode = leafNodes[leafNodes.size() - 2];
    
        int requiredKeys = MIN - lastNode->size;

        for (int i = 0; i < requiredKeys; i++){
            //move the last key of secondLastNode and insert it as the first key of last node
            lastNode->key.insert(lastNode->key.begin(), secondLastNode->key[secondLastNode->size - 1]);
            secondLastNode->key.pop_back();

            secondLastNode->size--;
            lastNode->size++; 
        }
    }

    //build internal nodes
    vector<Node *> curLevel = leafNodes;
    int order = MAX + 1;
    KeyStruct *rootKey; 

    while (curLevel.size() > 1){
        int numberOfNodes = ceil(curLevel.size() / float(order));
        vector<Node *> parentLevel(numberOfNodes);

        //fill from the back 
        for (int i = numberOfNodes - 1; i >= 0; i--){
            Node *cur = new Node();
            cur->isLeaf = false; 
            vector<Node *> children; //children for current parent node
            int j = 0;
            while (!curLevel.empty() and j < order){
                Node *n = curLevel.back();
                curLevel.pop_back();
                children.push_back(n); 
                j++;
            }
            reverse(children.begin(), children.end());

            for (int j = 1, keyIndex = 0; j < children.size(); j++, keyIndex++){
                cur->key[keyIndex] = children[j]->key[0];
                cur->ptr[keyIndex] = children[j - 1];  
            }
            cur->ptr[order] = children[children.size() - 1]; 
            parentLevel[i] = cur; 
        }
        curLevel = parentLevel; 
    }

    root = curLevel[0]; //final remaining node
    return; 
}
