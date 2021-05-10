#include "time.h"
#include "util.h"
#include <string>
#include <vector>
#include <ctime>

using namespace std;

Time::Time(bool relative, string str_time) {
    cout << "Here 0" << endl;
    int numbers[6] = {0};
    
    vector<string> tokens = tokenize(str_time, ".");
    if(tokens.size() > 6)
        throw runtime_error("Invalid parts if time expression");

    int index = 0;
    for(string token : tokens) {
        size_t read_chars;
        try {
            numbers[index] = stoi(token, &read_chars);
        } catch(invalid_argument&) {
            throw runtime_error("Invalid number is time expression");
        }
        if(read_chars != token.length())
            throw runtime_error("Invalid number is time expression");
        index++;
    }

    this->relative = relative;
    second = numbers[0];
    minute = numbers[1];
    hour = numbers[2];
    day = numbers[3];
    month = numbers[4];
    year = numbers[5];
}

Time::Time(bool relative, int second, int minute, int hour, int day, int month, int year) {
    this->relative = relative;
    this->second = second;
    this->minute = minute;
    this->hour = hour;
    this->day = day;
    this->month = month;
    this->year = year;
}

bool Time::is_relative() const {
    return relative;
}

long Time::get_relative_seconds() const {
    return second + minute*60 + hour*3600 + day*3600*24 + month*3600*24*30 + year*3600*24*30*365;
}

long Time::get_absolute_seconds() const {
    time_t t1 = time(nullptr);
    struct tm *t2 = localtime(&t1);
    int daylight_saving = t2->tm_isdst;

    struct tm cal = {0};
    cal.tm_sec = second;
    cal.tm_min = minute;
    cal.tm_hour = hour;
    cal.tm_mday = day;
    cal.tm_mon = month - 1;
    cal.tm_year = year - 1900;
    cal.tm_isdst = daylight_saving;
    return mktime(&cal);
}

std::ostream& operator<<(std::ostream &os, const Time &time) {
    os << time.second << "." << time.minute << "." << time.hour << "." << time.day << "." << time.month << "." << time.year;
    return os;
}