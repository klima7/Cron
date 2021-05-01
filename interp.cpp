#include "interp.h"

using namespace std;

Interpreter::Interpreter(Cron cron) {
    this->cron = cron;
}

string Interpreter::interpret(std::string command) {

    vector<string> tokens = get_tokens(command, " ");

    if(tokens[0] == "add") {
        return "Task added";
    }
    else if(tokens[0] == "remove") {
        return "Task removed";
    }
    else if(tokens[0] == "exit") {
        return "Cron exited";
    }
    else if(tokens[0] == "list") {
        return "Listing tasks";
    }
    else {
        return "Invalid command";
    }
}

vector<string> Interpreter::get_tokens(string text, string delimiter) {
    vector<string> tokens;
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