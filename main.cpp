#include "Disk_Storage.h"
#include "b-plus-tree.h"
#include "Record.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <chrono>
#include <algorithm>

using std::cout;
using std::endl;
using std::string;
using std::vector;

const int MAX = 256; 

// Function to load records and write them directly to Disk_Storage
void loadAndStoreRecords(const string& filePath, Disk_Storage& diskStorage, BPlusTree& bPlusTree) {
    string line;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << endl;
        return;
    }

    // Skip the header line
    std::getline(file, line);

    // Read each line from the file
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Record record; // Initialize with default values
        
        // Parse the date as a string
        string dateString;
        iss >> dateString;

        if (!dateString.empty()) {
            std::replace(dateString.begin(), dateString.end(), '/', ' ');
            std::istringstream dateStream(dateString);

            unsigned int day, month, year;
            dateStream >> day >> month >> year;

            record.date.day = day;
            record.date.month = month;
            record.date.year = year % 100;  // Store last two digits of the year
        }

        // Try parsing each field one by one. If parsing fails, the default value will remain.
        iss >> record.TEAM_ID_home;
        iss >> record.PTS_home;
        iss >> record.FG_PCT_home;
        iss >> record.FT_PCT_home;
        iss >> record.FG3_PCT_home;
        iss >> record.AST_home;
        iss >> record.REB_home;
        iss >> record.HOME_TEAM_WINS;

        // Write the record to Disk_Storage and get the location and key
        auto [recordLocation, key] = diskStorage.writeRecord(sizeof(Record), record);
        
        // Create KeyStruct
        KeyStruct keyStruct = {key, {recordLocation}}; // Store location in a vector

        // Check if the key already exists in the B+ Tree
        // If key does not exist, insert the new KeyStruct
        bPlusTree.insert(keyStruct);
        
    }

    file.close();
}

int main() {
    // Step 1: Create Disk_Storage object
    Disk_Storage diskStorage(sizeof(Record), 144, BLOCK_SIZE); // Initialize disk storage
    string filePath = "D:\\Users Folder\\Documents\\GitHub\\db_management_system\\data\\games.txt"; // Full file path

    // Step 2: Load records directly into Disk_Storage and insert into B+ Tree
    BPlusTree bPlusTree; // Create B+ Tree object
    loadAndStoreRecords(filePath, diskStorage, bPlusTree); // Load records and write to disk storage

    cout << "Loaded records and written to disk storage.\n";

    // Step 5: Report statistics about the B+ Tree
    int n = MAX;            //placeholder                // Get the max number of keys in the B+ Tree
    int numNodes = 2;     //placeholder                // Get the number of nodes
    Node* rootKeys = bPlusTree.getRoot(); // Get keys in the root node
    int numLevels = bPlusTree.getHeight(); // Get the number of levels

    // Print B+ Tree statistics
    cout << "B+ Tree Statistics:" << endl;
    cout << "N (number of keys): " << n << endl;
    cout << "Number of Nodes: " << numNodes << endl;
    cout << "Number of Levels: " << numLevels << endl;
    cout << "Root Keys: ";

    for (int i = 0; i < rootKeys->size; i++){
        cout << rootKeys->key[i].value << " "; 
    }
     
    cout << endl;

    // Task 3: B+ Tree vs Disk Storage Linear Scan
    
    // Perform range query on B+ Tree
    float lowerBound = 0.5;
    float upperBound = 0.8; 
    int numNodesAccessed = 0;
    auto startBPlus = std::chrono::high_resolution_clock::now();
    vector<KeyStruct> bPlusResults = bPlusTree.searchInterval(lowerBound, upperBound, numNodesAccessed);
    auto endBPlus = std::chrono::high_resolution_clock::now();
    float sumBPlusFG3_PCT_home = 0;
    for (const auto& keyStruct : bPlusResults) {
        for (const auto& recordLocation : keyStruct.addresses) {
            Record record = diskStorage.retrieveRecord(recordLocation);
            sumBPlusFG3_PCT_home += record.FG3_PCT_home;
        }
    }
    float averageBPlusFG3_PCT_home = bPlusResults.size() > 0 ? sumBPlusFG3_PCT_home / bPlusResults.size() : 0.0f;
    std::chrono::duration<double> elapsedBPlus = endBPlus - startBPlus;

    cout << "B+ Tree Statistics:" << endl;
    cout << "Index Nodes Accessed: " << numNodesAccessed << endl;
    cout << "Average FG3_PCT_home (B+ Tree): " << averageBPlusFG3_PCT_home << endl;
    cout << "Elapsed time for B Plus Tree: " << elapsedBPlus.count() << " seconds" << endl;

    // Perform linear scan on Disk_Storage
    auto startLinear = std::chrono::high_resolution_clock::now();
    auto [linearIOCount, averageLinearFG3_PCT_home] = diskStorage.linearScan(lowerBound, upperBound);
    auto endLinear = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> elapsedLinear = endLinear - startLinear;

    cout << "Linear Scan Statistics:" << endl;
    cout << "Data Blocks Accessed: " << linearIOCount << endl;
    cout << "Average FG3_PCT_home (Linear Scan): " << averageLinearFG3_PCT_home << endl;
    cout << "Elapsed time for Linear Scan: " << elapsedLinear.count() << " seconds" << endl;


    return 0; // Exit the program
}
