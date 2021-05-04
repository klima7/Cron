#include "time.h"
#include <string>
#include <ctime>

using namespace std;

Time::Time(string str_time) {
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

    second = numbers[0];
    minute = numbers[1];
    hour = numbers[2];
    day = numbers[3];
    month = numbers[4];
    year = numbers[5];
}

list<string> Time::get_tokens(string text, string delimiter) {
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

long Time::to_seconds() {
    return second + minute*60 + hour*3600 + day*3600*24 + month*3600*24*30 + year*3600*24*30*365;
}

std::ostream& operator<<(std::ostream &os, const Time &time) {
    os << time.second << "." << time.minute << "." << time.hour << "." << time.day << "." << time.month << "." << time.year;
    return os;
}