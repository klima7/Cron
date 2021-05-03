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
        auto iter = arguments.begin();

        bool relative = false;
        bool cyclic = false;

        if(arguments.empty())
            throw ArgumentsException();

        if (*iter == "-r") {
            relative = true;
            iter++;
        }

        if(iter == arguments.end())
            throw ArgumentsException();

        string base_time_str = *iter++;

        if(iter == arguments.end())
            throw ArgumentsException();

        string repeat_time_str;
        if (*iter == "-c") {
            iter++;
            if(iter == arguments.end())
                throw ArgumentsException();
            repeat_time_str = *iter++;
        }

        if(iter == arguments.end())
            throw ArgumentsException();

        string cmd = *iter++;
        vector<string> cmd_arguments = vector<string>(iter, arguments.end());

        // Create task
        TimeInterval base_time = TimeInterval::from_string(relative, base_time_str);
        if(repeat_time_str.length() == 0) {
            Task task(cmd, cmd_arguments, base_time);
            cron.add_task(task);
        }
        else {
            TimeInterval repeat_time = TimeInterval::from_string(relative, repeat_time_str);
            Task task(cmd, cmd_arguments, base_time);
            cron.add_task(task);
        }

        out << "Task added" << endl;
    }
    catch(ArgumentsException &e) {
        out << "Invalid arguments" << endl;
        out << "Proper usage: cron add [-r] s.m.h.d.m.y [s.m.h.d.m.y] command [arguments...]" << endl;
    }
    catch(InvalidTimeException &e) {
        out << "Invalid time format" << endl;
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
