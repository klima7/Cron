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
    Time(int second, int minute, int hour, int day, int month, int year);
    Time *add(const Time &other);
    long get_seconds();
    long get_seconds_since_1970();
private:
    static std::list<std::string> get_tokens(std::string text, std::string delimiter);
    int second, minute, hour, day, month, year;
};


#endif
