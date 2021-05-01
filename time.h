#ifndef CRON_TIME_H
#define CRON_TIME_H


#include <iostream>
#include <string>
#include <list>


class TimeInterval {
public:
    static TimeInterval from_string(bool relative, std::string str_time);
    long to_seconds();
private:
    TimeInterval(long seconds): seconds(seconds) {};
    static TimeInterval from_relative(int second, int minute, int hour, int day);
    static TimeInterval from_absolute(int second, int minute, int hour, int day);
    static std::list<std::string> get_tokens(std::string text, std::string delimiter);
    long seconds;
};


#endif
