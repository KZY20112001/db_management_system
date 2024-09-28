#ifndef RECORD_H
#define RECORD_H

#include <cstdint>

struct Date {
    unsigned int day   : 5;  // 5 bits for day (1 to 31)
    unsigned int month : 4;  // 4 bits for month (1 to 12)
    unsigned int year  : 14;  // 14 bits for year (1 to 16k)
};

struct Record {
    Date date;              // Date of the record
    uint8_t PTS_home;       // 1 byte
    float FG_PCT_home;      // 4 bytes, key
    float FT_PCT_home;      // 4 bytes
    float FG3_PCT_home;     // 4 bytes
    uint8_t AST_home;       // 1 byte
    uint8_t REB_home;       // 1 byte
    bool HOME_TEAM_WINS;    // 1 byte
};

#endif
