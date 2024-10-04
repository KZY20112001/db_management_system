#include "Disk_Storage.h"
#include "b-plus-tree.h"
#include "Record.h"

#include "Disk_Storage.cpp"
#include "b-plus-tree.cpp"

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
        unsigned int team_id = 0; // Default value
        int pts_home = 0; // Default value
        float fg_pct_home = 0.0f; // Default value
        float ft_pct_home = 0.0f; // Default value
        float fg3_pct_home = 0.0f; // Default value
        unsigned int ast_home = 0; // Default value
        unsigned int reb_home = 0; // Default value
        bool home_team_wins = false; // Default value

        iss >> team_id;
        iss >> pts_home;
        iss >> fg_pct_home;
        iss >> ft_pct_home;
        iss >> fg3_pct_home;
        iss >> ast_home;
        iss >> reb_home;
        iss >> home_team_wins;

        record.TEAM_ID_home = static_cast<uint16_t>(team_id % 1000);;
        record.PTS_home = static_cast<uint8_t>(pts_home);
        record.FG_PCT_home = fg_pct_home;
        record.FT_PCT_home = ft_pct_home;
        record.FG3_PCT_home = fg3_pct_home;
        record.AST_home = static_cast<uint8_t>(ast_home);
        record.REB_home = static_cast<uint8_t>(reb_home);
        record.HOME_TEAM_WINS = home_team_wins;

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
    Disk_Storage diskStorage(sizeof(Record), 500, BLOCK_SIZE); // Initialize disk storage
    string filePath = "C:\\Users\\apiec\\Desktop\\database\\project 1 test 2\\data\\games.txt"; // Full file path

    // Step 2: Load records directly into Disk_Storage and insert into B+ Tree
    BPlusTree bPlusTree; // Create B+ Tree object
    loadAndStoreRecords(filePath, diskStorage, bPlusTree); // Load records and write to disk storage

    cout << "Loaded records and written to disk storage.\n";
    cout << "Loaded " << diskStorage.numrecords << " records from the file.\n";
    cout << endl;
    cout << "Blocks Used " << diskStorage.blocksused << endl;
    cout << "Blocks Available " << diskStorage.availblocks << endl;
    cout << "Number of Records " << diskStorage.numrecords << endl;
    cout << "Memory Used " << diskStorage.memoryused << endl;
    cout << "Record Size " << diskStorage.recordsize << endl;
    cout << endl;
    //diskStorage.listBlocks();
    //diskStorage.listSpecificBlock(1);

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
