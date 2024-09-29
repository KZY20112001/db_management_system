#ifndef RECORD_H
#define RECORD_H

#include <cstdint>

struct Date {
    unsigned int day   : 5;  // 5 bits for day (1 to 31)
    unsigned int month : 4;  // 4 bits for month (1 to 12)
    unsigned int year  : 5;  // 5 bits for offset year (1 to 32)
};

// Record structure
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
};

#endif
