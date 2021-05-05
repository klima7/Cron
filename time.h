#ifndef CRON_TIME_H
#define CRON_TIME_H


#include <iostream>
#include <string>
#include <stdexcept>


class Time {
    friend std::ostream& operator<<(std::ostream &os, const Time &time);
public:
    Time(bool relative=true, std::string str_time="0.0.0.0.0.0");
    Time(bool relative, int second, int minute, int hour, int day, int month, int year);
    bool is_relative() const;
    long get_relative_seconds() const;
    long get_absolute_seconds() const;
private:
    bool relative;
    int second, minute, hour, day, month, year;
};


#endif
