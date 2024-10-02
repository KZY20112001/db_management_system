#include "b-plus-tree.h"
#include "Disk_Storage.h"
#include <iostream> 

using std::cout;
using std::endl;



int main(){

    vector<int> values = {3, 6, 10, 12, 20, 4, 9, 11, 13, 22};

    vector<KeyStruct> keys;

    //for testing all keys will have 1,1 default location values as only keys are important for building b+ tree
    Record_Location testLocation = {1, 1};
    for (int v: values){
        
        KeyStruct key = {float(v), {testLocation}};
        keys.push_back(key); 
    }   


    BPlusTree tree; 
    for (auto key: keys){
        cout << "Inserting: " << key.value << endl; 
        tree.insert(key); 
    }
    tree.displayAllKeys(); 
    return 0; 
}