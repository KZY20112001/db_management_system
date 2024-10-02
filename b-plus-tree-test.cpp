#include "b-plus-tree.h"
#include "Disk_Storage.h"
#include <iostream> 
#include <algorithm> 

using std::cout;
using std::endl;



int main(){

    //vector<int> values = {3, 4, 6, 9, 10, 11,12,13,20,22};
    //vector<int> values = {1, 4, 7, 10, 17, 21,31,25,19,20};
    vector<int> values = {10,20,40,50,70,80,90,100, 30,60}; 
    vector<KeyStruct> keys;

    //for testing all keys will have 1,1 default location values as only keys are important for building b+ tree
    Record_Location testLocation = {10, 19};
    for (int v: values){
        
        KeyStruct key = {float(v), {testLocation}};
        keys.push_back(key); 
    }   


    BPlusTree tree; 
    for (auto key: keys){
        cout << "Inserting: " << key.value << endl; 
        tree.insert(key); 
    }
    cout << "Searching " << endl; 
    KeyStruct res = tree.search(10);
    cout << "result: " << res.value << endl; 
    tree.displayAllKeys();
    return 0; 
}   