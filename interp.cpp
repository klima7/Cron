#include "interp.h"
#include "time.h"
#include <vector>
#include <sstream>

using namespace std;

Interpreter::Interpreter(Cron cron) {
    this->cron = cron;
}

string Interpreter::interpret(std::string command) {

    vector<string> tokens = get_tokens(command, " ");

    string cmd = tokens[0];
    vector<string> args = vector<string>(tokens.begin()+1, tokens.end());

    stringstream stream;

    if(tokens[0] == "add") {
        add_command(args, stream);
        return stream.str();
    }
    else if(tokens[0] == "remove") {
        remove_command(args, stream);
        return stream.str();
    }
    else if(tokens[0] == "exit") {
        exit_command(stream);
        return stream.str();
    }
    else if(tokens[0] == "list") {
        list_command(stream);
        return stream.str();
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

void Interpreter::add_command(vector<string> arguments, stringstream &out) {
    try {
        int next = 0;
        bool relative = false;

        if(arguments.empty())
            throw ArgumentsException();

        if (arguments[next] == "-r") {
            relative = true;
            next++;
        }

        if(arguments.size() - next == 0)
            throw ArgumentsException();

        string base_time = arguments[next++];
        TimeInterval time_to_start = TimeInterval::from_string(relative, base_time);
        out << time_to_start.to_seconds() << endl;

        if(arguments.size() - next > 0) {
            string repeat_time_str = arguments[next++];
            TimeInterval repeat_time = TimeInterval::from_string(true, repeat_time_str);
            out << repeat_time.to_seconds() << endl;
        }
    }
    catch(ArgumentsException &e) {
        out << "Invalid arguments" << endl;
        out << "Proper usage: cron add [-r] s.m.h.d.m.y [s.m.h.d.m.y]" << endl;
    }
}

void Interpreter::remove_command(vector<string> arguments, stringstream &out) {
    out << "remove command";
}

void Interpreter::list_command(stringstream &out) {
    list<Task> tasks = cron.get_tasks();
    out << "list command";
}

void Interpreter::exit_command(stringstream &out) {
    cron.exit();
    out << "exit command";
}
