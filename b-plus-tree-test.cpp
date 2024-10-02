#include "b-plus-tree.h"
#include "Disk_Storage.h"
#include <iostream> 
#include <algorithm> 

using std::cout;
using std::endl;



int main(){

    //vector<int> values = {3, 4, 6, 9, 10, 11,12,13,20,22};
    //vector<int> values = {1, 4, 7, 10, 17, 21,31,25,19,20};
    vector<int> values = {10,20,30, 40,50,60, 70,80,90,100, 30,60, 10,10,10,10,10,20,20,20}; 
    vector<KeyStruct> keys;

    //for testing all keys will have 10,20 default location values as only keys are important for building b+ tree
    Record_Location testLocation = {10, 20};
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

    //search each key to test
    for (auto v: values){ 
        KeyStruct res = tree.search(v);
        if (res.value == -1)
            continue; 
        cout << "Number of results for key "<< v << " : " << res.addresses.size() << endl; 
        for (auto address: res.addresses){
            cout << "Block Num: " << address.blocknum << ", Offset: " << address.offset << endl; 
        }
        cout << endl; 
    }

    //search error key
    KeyStruct res = tree.search(0);
    cout << "Not Found for 0: " << res.value << endl;

    //interval search key
    int dummy = 0; 
    vector<KeyStruct> r = tree.searchInterval(10, 40, dummy); 
    cout << "Number of Keystruct: " << r.size() << endl; //should return 4 Keystructs (10,20,30,40)

    int h = tree.getHeight();
    cout << "Height: " << h << endl; 
    return 0; 
}   