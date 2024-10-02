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



// Function to load records from txt file
vector<Record> loadRecords(const string& filePath) {
    vector<Record> records;
    string line;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << endl;
        return records;
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
            // Replace '/' with spaces to easily split date components
            std::replace(dateString.begin(), dateString.end(), '/', ' ');
            std::istringstream dateStream(dateString);

            // Temporary variables to store day, month, year
            unsigned int day, month, year;
            dateStream >> day >> month >> year;

            // Assign parsed values to the bit-field date struct
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

        // Add the record to the list even if some fields are missing
        records.push_back(record);
    }

    std::sort(records.begin(), records.end(), [](const Record& a, const Record& b) {
        return a.FG_PCT_home < b.FG_PCT_home; // Compare FG_PCT_home
    });

    file.close();
    return records;
}


int main() {
    // Step 1: Create Disk_Storage object
    Disk_Storage diskStorage(sizeof(Record), 144, BLOCK_SIZE); // Initialize disk storage
    string filePath = "D:\\Users Folder\\Documents\\GitHub\\db_management_system\\data\\games.txt"; // Full file path

    // Step 2: Load records from file into a vector
    vector<Record> records = loadRecords(filePath); // Load records
    cout << "Loaded " << records.size() << " records from the file.\n";

    // Step 3: Add blocks to Disk_Storage and write records
    for (const auto& record : records) {
        diskStorage.writeRecord(sizeof(Record), record); // Write each record to disk storage
    }

    // Step 4: Create B+ Tree and bulk insert records
    BPlusTree bPlusTree; // Create B+ Tree object

    // Step 5: Report statistics about the B+ Tree
    int n = MAX;            //placeholder                // Get the max number of keys in the B+ Tree
    int numNodes = 2;     //placeholder                // Get the number of nodes
    int numLevels = 2;    //placeholder                // Get the number of levels
    Node* rootKeys = bPlusTree.getRoot(); // Get keys in the root node

    // Print B+ Tree statistics
    cout << "B+ Tree Statistics:" << endl;
    cout << "N (number of keys): " << n << endl;
    cout << "Number of Nodes: " << numNodes << endl;
    cout << "Number of Levels: " << numLevels << endl;
    cout << "Root Keys: ";

    for (int i = 0; i < rootKeys->size; i++){
        cout << rootKeys->key[i]->value << " "; 
    }
     
    cout << endl;


    // Task 3: B+ Tree vs Linear Scan
    // Perform range query on B+ Tree
    auto startBPlus = std::chrono::high_resolution_clock::now();
    float lowerBound = 0.5;
    float upperBound = 0.8; 
    int numNodesAccessed = 0;
    auto rangeResults = bPlusTree.searchInterval(lowerBound, upperBound, numNodesAccessed);
    int sum = 0;
    int count = 0;
    for (const auto& record : rangeResults) {
        sum += record->value;
        count++;
    }
    float average = sum / count;
    auto endBPlus = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedBPlus = endBPlus - startBPlus;

    
    // Perform linear scan on records vector
    auto startLinear = std::chrono::high_resolution_clock::now();
    vector<Record> linearResults; // Vector to hold results from linear scan
    for (const auto& record : records) {
        if (record.FG_PCT_home >= lowerBound && record.FG_PCT_home <= upperBound) {
            linearResults.push_back(record); // Add to results if within range
        }
    }
    int sumLinear = 0;
    int countLinear = 0;
    for (const auto& record : linearResults) {
        sum += record.FG_PCT_home;
        count++;
    }
    float averageLinear = sum / count;
    auto endLinear = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedLinear = endLinear - startLinear;

    cout << "B+ Tree Statistics:" << endl;
    cout << "Index Nodes Accessed: " << numNodesAccessed << endl;
    cout << "Average FG3_PCT_home (B+ Tree): " << average << endl;
    cout << "Elapsed time for B Plus Tree: " << elapsedBPlus.count() << " seconds" << endl;

    cout << "Linear Scan Statistics:" << endl;
    cout << "Data Blocks Accessed: " << countLinear << endl;
    cout << "Average FG3_PCT_home (Linear Scan): " << averageLinear << endl;
    cout << "Elapsed time for Linear Scan: " << elapsedLinear.count() << " seconds" << endl;

    return 0; // Exit the program
}
