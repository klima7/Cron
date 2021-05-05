#include "interp.h"
#include "time.h"
#include "util.h"
#include <vector>
#include <sstream>

using namespace std;

Interpreter::Interpreter(Cron &cron): cron(cron) { }

string Interpreter::interpret(std::string command) {

    vector<string> tokens = tokenize(command, " ");

    string cmd = tokens[0];
    vector<string> args = vector<string>(tokens.begin()+1, tokens.end());

    stringstream stream;

    if(tokens[0] == "add") {
        add_command(args, stream);
        return stream.str();
    }
    else if(tokens[0] == "rm") {
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

void Interpreter::add_command(vector<string> arguments, stringstream &out) {
    try {
        auto iter = arguments.begin();

        bool relative = false;

        if(arguments.empty())
            throw ArgumentsException();

        // Check if relative
        if (*iter == "-r") {
            relative = true;
            iter++;
        }

        if(iter == arguments.end())
            throw ArgumentsException();

        // Get base time
        Time base_time = Time(relative, *iter++);

        if(iter == arguments.end())
            throw ArgumentsException();

        // Get repeat time if present
        Time repeat_time;
        if (*iter == "-c") {
            iter++;
            if(iter == arguments.end())
                throw ArgumentsException();
            repeat_time = Time(true, *iter++);
        }

        if(iter == arguments.end())
            throw ArgumentsException();

        // Get command and arguments
        string cmd = *iter++;
        vector<string> cmd_arguments = vector<string>(iter, arguments.end());

        // Call cron
        cron.add_task(cmd, cmd_arguments, base_time, repeat_time);
        out << "Task added" << endl;
    }
    catch(ArgumentsException &e) {
        out << "Invalid arguments" << endl;
        out << "Proper usage: cron add [-r] <s.m.h.d.m.y> [-c s.m.h.d.m.y] <command> [arguments...]" << endl;
    }
    catch(InvalidTimeException &e) {
        out << "Error: Invalid time format" << endl;
    }
}

void Interpreter::remove_command(vector<string> arguments, stringstream &out) {
    try {
        if(arguments.size() != 1)
            throw ArgumentsException();

        int id = 0;
        try {
            id = stoi(arguments[0]);
        }
        catch(invalid_argument&) {
            out << "Error: Provided id is not a number" << endl;
            return;
        }

        bool success = cron.remove_task(id);
        if(success)
            out << "Task with id " << id << " removed" << endl;
        else
            out << "Error: Unable to find task with given id " << endl;
    }
    catch(ArgumentsException &e) {
        out << "Invalid arguments" << endl;
        out << "Proper usage: cron rm <id>" << endl;
    }
}

void Interpreter::list_command(stringstream &out) {
    list<Task> tasks = cron.get_tasks();

    if(tasks.size() == 0) {
        out << "No scheduled tasks" << endl;
        return;
    }

    for(Task task : tasks) {
        print_task(out, &task);
    }
}

void Interpreter::exit_command(stringstream &out) {
    int count = cron.exit();
    out << "Cron exited. " << count << " tasks canceled" << endl;
}

void Interpreter::print_task(ostream &os, Task *task) {
    os << task->get_id() << " ";
    if(task->get_base_time().is_relative())
        os << "-r ";
    os << task->get_base_time() << " ";
    if(task->get_repeat_time().get_seconds() != 0)
        os << "-c " << task->get_repeat_time() << " ";
    os << task->get_command() << " ";
    for (string arg : task->get_arguments()) {
        os << arg << " ";
    }
    os << endl;
}