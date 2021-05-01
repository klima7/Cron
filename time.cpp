#include "time.h"
#include <string>
#include <ctime>

using namespace std;

TimeInterval TimeInterval::from_relative(int second, int minute, int hour, int day) {
    long seconds = second + minute*60 + hour*3600 + day*3600*24;
    return TimeInterval(seconds);
}

TimeInterval TimeInterval::from_absolute(int second, int minute, int hour, int day) {
    time_t curr_time = time(NULL);
    time_t that_time = second + minute * 60 + hour * 3600 + day * 3600 * 24;
    long diff = difftime(that_time, curr_time);
    return TimeInterval(diff);
}

TimeInterval TimeInterval::from_string(bool relative, string str_time) {
    int numbers[4];
    
    list<string> tokens = get_tokens(str_time, " ");
    if(tokens.size() != 4)
        throw exception();

    int index = 0;
    for(string token : tokens) {
        numbers[index] = stoi(token);
        index++;
    }

    if(relative)
        return from_relative(numbers[0], numbers[1], numbers[2], numbers[3]);
    else
        return from_absolute(numbers[0], numbers[1], numbers[2], numbers[3]);
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