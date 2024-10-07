#ifndef RECORD_H
#define RECORD_H

#include <cstdint>

struct Date {
    unsigned int day   : 5;  // 5 bits for day (1 to 31)
    unsigned int month : 4;  // 4 bits for month (1 to 12)
    unsigned int year  : 5;  // 5 bits for offset year (1 to 32)
    // Total: 14 bits = 2 bytes (no change needed)
};

struct Record {
    Date date;                     // 2 bytes
    uint16_t TEAM_ID_home;     // 2 bytes (reduced from 4 bytes)
    uint8_t PTS_home;              // 1 byte (reduced from 4 bytes)
    float FG_PCT_home;             // 4 bytes
    float FT_PCT_home;             // 4 bytes
    float FG3_PCT_home;            // 4 bytes
    uint8_t AST_home;              // 1 byte (reduced from 4 bytes)
    uint8_t REB_home;              // 1 byte (reduced from 4 bytes)
    bool HOME_TEAM_WINS;           // 1 byte (unchanged)

    Record() : TEAM_ID_home(0), PTS_home(0), FG_PCT_home(0.0f), FT_PCT_home(0.0f), 
               FG3_PCT_home(0.0f), AST_home(0), REB_home(0), HOME_TEAM_WINS(0) {}
};

#endif
