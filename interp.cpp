#include "interp.h"
#include <vector>

using namespace std;

Interpreter::Interpreter(Cron cron) {
    this->cron = cron;
}

string Interpreter::interpret(std::string command) {

    vector<string> tokens = get_tokens(command, " ");

    string cmd = tokens[0];
    vector<string> args = vector<string>(tokens.begin()+1, tokens.end());

    if(tokens[0] == "add") {
        return add_command(args);
    }
    else if(tokens[0] == "remove") {
        return remove_command(args);
    }
    else if(tokens[0] == "exit") {
        cron.exit();
        return "Cron exited";
    }
    else if(tokens[0] == "list") {
        return list_command();
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

string Interpreter::add_command(vector<string> arguments) {
    int next = 0;
    bool relative = false;

    if(arguments[next] == "-r") {
        relative = true;
        next++;
    }



    return "add command";
}

string Interpreter::remove_command(vector<string> arguments) {
    return "remove command";
}

string Interpreter::list_command() {
    list<Task> tasks = cron.get_tasks();
    return "list command";
}
