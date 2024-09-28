#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include "Disk_Storage.h"
#include "b-plus-tree.h"
#include "Record.h"

// Function to read data from the text file and load it into Disk_Storage
std::vector<Record> loadRecords(const std::string& filePath) {
    std::vector<Record> records; // Vector to hold all the records
    std::ifstream file(filePath); // Open the file
    std::string line;

    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return records; // Return empty vector if file can't be opened
    }

    // Read each line from the file
    while (std::getline(file, line)) {
        std::istringstream iss(line); // Stream for parsing the line
        Record record;
        
        // Read each field from the line and populate the Record struct
        int day, month, year;
        iss >> day >> month >> year >> record.PTS_home >> record.FG_PCT_home
            >> record.FT_PCT_home >> record.FG3_PCT_home >> record.AST_home
            >> record.REB_home >> record.HOME_TEAM_WINS;

        // Assign parsed values to the Date struct
        record.date.day = day;
        record.date.month = month;
        record.date.year = year;

        // Add the populated record to the vector
        records.push_back(record);
    }

    file.close(); // Close the file
    return records; // Return the vector of records
}

int main() {
    // Step 1: Create Disk_Storage object
    Disk_Storage diskStorage(sizeof(Record), 144, BLOCK_SIZE); // Initialize disk storage
    std::string filePath = "D:\\Users Folder\\Documents\\GitHub\\db_management_system\\data\\games.txt"; // Full file path

    // Step 2: Load records from file into a vector
    std::vector<Record> records = loadRecords(filePath); // Load records
    std::cout << "Loaded " << records.size() << " records from the file." << std::endl;

    // Step 3: Add blocks to Disk_Storage and write records
    for (const auto& record : records) {
        diskStorage.writeRecord(sizeof(Record)); // Write each record to disk storage
    }

    // Step 4: Create B+ Tree and bulk insert records
    BPlusTree bPlusTree; // Create B+ Tree object
    for (const auto& record : records) {
        bPlusTree.bulkInsert(record.FG_PCT_home, record); // Insert into B+ Tree using FG_PCT_home as key
    }

    // Step 5: Report statistics about the B+ Tree
    int n  =  // Get the max number of keys in the B+ Tree
    int numNodes =  // Get the number of nodes
    int numLevels =  // Get the number of levels
    auto rootKeys = bPlusTree.getRoot(); // Get keys in the root node

    // Print B+ Tree statistics
    std::cout << "B+ Tree Statistics:" << std::endl;
    std::cout << "N (number of keys): " << n << std::endl;
    std::cout << "Number of Nodes: " << numNodes << std::endl;
    std::cout << "Number of Levels: " << numLevels << std::endl;
    std::cout << "Root Keys: ";
    for (const auto& key : rootKeys) {
        std::cout << key << " "; // Print each key in the root node
    }
    std::cout << std::endl;


    // Task 3: B+ Tree vs Linear Scan
    // Perform range query on B+ Tree
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

    // Perform linear scan on records vector
    std::vector<Record> linearResults; // Vector to hold results from linear scan
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

    std::cout << "B+ Tree Statistics:" << std::endl;
    std::cout << "Index Nodes Accessed: " << numNodesAccessed << std::endl;
    std::cout << "Average FG3_PCT_home (B+ Tree): " << average << std::endl;

    std::cout << "Linear Scan Statistics:" << std::endl;
    std::cout << "Data Blocks Accessed: " << countLinear << std::endl;
    std::cout << "Average FG3_PCT_home (Linear Scan): " << averageLinear << std::endl;

    return 0; // Exit the program
}
