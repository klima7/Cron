#include "time.h"
#include <string>
#include <ctime>

using namespace std;

TimeInterval TimeInterval::from_relative(int second, int minute, int hour, int day, int month, int year) {
    long seconds = second + minute*60 + hour*3600 + day*3600*24 + month * 3600 * 34 * 30 + year * 3600 * 34 * 30 * 365;
    return TimeInterval(seconds);
}

TimeInterval TimeInterval::from_absolute(int second, int minute, int hour, int day, int month, int year) {
    time_t curr_time = time(NULL);

    struct tm time = {};
    time.tm_sec = second;
    time.tm_min = minute;
    time.tm_hour = hour;
    time.tm_mday = day;
    time.tm_mon = month;
    time.tm_year = year;
    time_t that_time = mktime(&time);

    long diff = difftime(that_time, curr_time);
    return TimeInterval(diff);
}

TimeInterval TimeInterval::from_string(bool relative, string str_time) {
    int numbers[6];
    
    list<string> tokens = get_tokens(str_time, ".");
    if(tokens.size() != 6)
        throw InvalidTimeException();

    int index = 0;
    for(string token : tokens) {
        if(token.length() == 0)
            throw InvalidTimeException();
        size_t read_chars;
        numbers[index] = stoi(token, &read_chars);
        if(read_chars != token.length())
            throw InvalidTimeException();
        index++;
    }

    if(relative)
        return from_relative(numbers[0], numbers[1], numbers[2], numbers[3], numbers[4], numbers[5]);
    else
        return from_absolute(numbers[0], numbers[1], numbers[2], numbers[3], numbers[4], numbers[5]);
}

list<string> TimeInterval::get_tokens(string text, string delimiter) {
    list<string> tokens;
    size_t pos = 0;
    string token;

    while ((pos = text.find(delimiter)) != string::npos) {
        token = text.substr(0, pos);
        tokens.push_back(token);
        text.erase(0, pos + delimiter.length());
    }
    tokens.push_back(text);

    return tokens;
}

long TimeInterval::to_seconds() {
    return seconds;
}