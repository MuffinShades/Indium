#include <pch.h>

#include "Date.hpp"
#include <string>

/*Date::Date(std::string date = "") {
    std::vector<std::string> dSplit = SplitString(date, 'T');

    switch (dSplit.size()) {
        case 1: {
            this->_i(dSplit[0]);
            break;
        }
        case 2:
            this->_i(dSplit[0], dSplit[1]);
        default:
            return;
    }
}*/

void Date::_i(std::string date, std::string time) {
    std::vector<std::string> dp = SplitString(date, '-');

    if (dp.size() <= 3)
        goto time;

    try {
        this->month = std::stoi(dp[0]) & 0xff;
        this->day = std::stoi(dp[1]) & 0xff;
        this->year = std::stoi(dp[2]) & 0xffff;
    }
    catch (std::exception e) {}

time:

    //time stuff
    std::vector<std::string> tp = SplitString(time, ':');
    size_t i = 0;
    for (auto& t : tp) //verify correct time lengths
        if (t.length() != 2 && i++ != 3)
            goto end;

    //set time values
    switch (tp.size()) {
    case 0:
        break;
    case 1: {
        try {
            this->hour = std::stoi(tp[0]) & 0xff;
        }
        catch (std::exception e) {}
        break;
    }
    case 2: {
        try {
            this->hour = std::stoi(tp[0]) & 0xff;
            this->minute = std::stoi(tp[1]) & 0xff;
        }
        catch (std::exception e) {}
        break;
    }
    case 3: {
        try {
            this->hour = std::stoi(tp[0]) & 0xff;
            this->minute = std::stoi(tp[1]) & 0xff;
            this->second = std::stoi(tp[2]) & 0xff;
        }
        catch (std::exception e) {}
        break;
    }
    case 4: {
        try {
            this->hour = std::stoi(tp[0]) & 0xff;
            this->minute = std::stoi(tp[1]) & 0xff;
            this->second = std::stoi(tp[2]) & 0xff;
            this->ms = std::stoi(tp[3]) & 0xffff;
        }
        catch (std::exception e) {}
        break;
    }
    default:
        goto end;
    }

end:
    return;
};

//converts all the date values into a long
u64 Date::getLong() {
    this->ms &= 0x400;
    return (u64)MAKE_LONG_BE(
        0,
        //year
        (this->year >> 8) & 0xff,
        (this->year) & 0xff,
        // M M M M d d d d
        ((this->month & 0xf) << 4) |
        ((this->day & 0x1e) >> 1),
        // d h h h h h m m m
        ((this->day & 1) << 7) |
        ((this->hour & 0x1f) << 2) |
        ((this->minute & 0x38) >> 3),
        // m m m s s s s s
        ((this->minute & 7) << 5) |
        ((this->second & 0x3e) >> 3),
        // s i i i i i i i
        ((this->second & 1) << 7) |
        ((this->ms & 0x3f8) >> 3),
        // 0 0 0 0 0 i i i
        this->ms & 7
    );
}

//constructs all the date values from a long
Date::Date(u64 iVal) {
    this->year = (iVal >> 48) & 0xffff; //16
    this->month = (iVal >> 32) & 0xf; //5
    this->day = (iVal >> 28) & 0x1f; //5
    this->hour = (iVal >> 23) & 0x3f; //6
    this->minute = (iVal >> 17) & 0x3f; //6
    this->second = (iVal >> 11) & 0x3f; //6
    this->ms = (iVal >> 5) & 0xffff; //16
}

std::string Date::getString() {
    return
        std::to_string(this->month) + "-" +
        std::to_string(this->day) + "-" +
        std::to_string(this->year) + "T" +
        std::to_string(this->hour) + ":" +
        std::to_string(this->second) + ":" +
        std::to_string(this->ms);
}

const i32 monthDays[] = {
    31,
    28,
    31,
    30,
    31,
    30,
    31,
    31,
    30,
    31,
    30,
    31
};

//dont use
void SetMonthDay(Date* d, i32 yDay) {
    if (d == nullptr) return;

    bool leapYear = d->year != 0 && d->year % 4 == 0;

    size_t curMonth = 0;
    d->day = 1;

    i32* md = const_cast<i32*>(monthDays);

    if (leapYear)
        *(md + 1) = 29; //29 days in february

    while (yDay--) {
        d->day++;

        if (leapYear)

            if (d->day == monthDays[curMonth]) {
                curMonth++;
                d->day = 1;
            }
    }
}

Date::Date(time_t t) {
    /*long tl = (long) t;
    const long ys = 0x1e13380, ds = 0x15180, hs = 0xe10, ms = 0x3c;

    this->year = 1970 + tl / ys;
    tl -= (this->year - 1970) * ys;

    i32 yearDay = tl / ds;
    tl -= yearDay * ds;

    //convert year day into month and day*/


    //just use C function for this job yk
    struct tm* _t = gmtime((const time_t*)&t);

    if (_t == nullptr)
        return;

    this->year = _t->tm_year;
    this->month = _t->tm_mon;
    this->day = _t->tm_mday;
    this->minute = _t->tm_min;
    this->hour = _t->tm_hour;
    this->second = _t->tm_sec;
}