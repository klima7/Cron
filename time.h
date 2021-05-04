#ifndef CRON_TIME_H
#define CRON_TIME_H


#include <iostream>
#include <string>
#include <list>
#include <stdexcept>


class InvalidTimeException: public std::exception {};


class Time {
    friend std::ostream& operator<<(std::ostream &os, const Time &time);
public:
    Time(std::string str_time="0.0.0.0.0.0");
    Time add(const Time &other);
    long to_seconds();
private:
    static std::list<std::string> get_tokens(std::string text, std::string delimiter);
    int second, minute, hour, day, month, year;
};


#endif
