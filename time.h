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
    Time(bool relative=true, std::string str_time="0.0.0.0.0.0");
    Time(bool relative, int second, int minute, int hour, int day, int month, int year);
    bool is_relative();
    long get_seconds();
    long get_relative_seconds();
    long get_absolute_seconds();
private:
    static std::list<std::string> get_tokens(std::string text, std::string delimiter);
    bool relative;
    int second, minute, hour, day, month, year;
};


#endif
