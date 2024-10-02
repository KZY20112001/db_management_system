#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

struct Date {
    unsigned int day   : 5;  // 5 bits for day (1 to 31)
    unsigned int month : 4;  // 4 bits for month (1 to 12)
    unsigned int year  : 5;  // 5 bits for year offset (0 to 31, representing 2000-2031)
};

struct Record {
    Date date;
    unsigned int TEAM_ID_home;
    unsigned int PTS_home;
    float FG_PCT_home;
    float FT_PCT_home;
    float FG3_PCT_home;
    unsigned int AST_home;
    unsigned int REB_home;
    bool HOME_TEAM_WINS; 
    Record() : TEAM_ID_home(0), PTS_home(0), FG_PCT_home(0.0f), FT_PCT_home(0.0f), 
               FG3_PCT_home(0.0f), AST_home(0), REB_home(0), HOME_TEAM_WINS(0) {}
};

// Function to load records from a file
std::vector<Record> loadRecords(const std::string& filePath) {
    std::vector<Record> records;
    std::ifstream file(filePath);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return records;
    }

    // Skip the header line
    std::getline(file, line);

    // Read each line from the file
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Record record; // Initialize with default values
        
        // Parse the date as a string
        std::string dateString;
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

    file.close();
    return records;
}

// Function to print a record (for testing purposes)
void printRecord(const Record& record) {
    // Print the full year correctly
    std::cout << "Date: " << record.date.day << "/" 
              << record.date.month << "/" 
              << (record.date.year + 2000) << "," // Adjusted to print the full year
              << "TEAM_ID_home: " << record.TEAM_ID_home << ","
              << "PTS_home: " << record.PTS_home << ","
              << "FG_PCT_home: " << record.FG_PCT_home << ","
              << "FT_PCT_home: " << record.FT_PCT_home << ","
              << "FG3_PCT_home: " << record.FG3_PCT_home << ","
              << "AST_home: " << record.AST_home << ","
              << "REB_home: " << record.REB_home << ","
              << "HOME_TEAM_WINS: " << (record.HOME_TEAM_WINS ? "Win" : "Lose") << "\n";
}

// Test the loading of records
void testLoadRecords(const std::string& filePath) {
    std::vector<Record> records = loadRecords(filePath);
    std::cout << "Loaded " << records.size() << " records from the file.\n";
    for (size_t i = 0; i < records.size(); ++i) {
        if(records[i].PTS_home == 0){

        std::cout << "Record " << (i + 1) << ":\n";
        printRecord(records[i]);
        }
    }
}

int main() {
    std::string filePath = "D:/Users Folder/Documents/GitHub/db_management_system/data/games.txt";
    testLoadRecords(filePath);
    return 0;
}
