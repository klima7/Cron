#include "interp.h"
#include "time.h"
#include "siglog.h"
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
        siglog::standard("Invalid command received: %s", tokens[0].c_str());
        return "Invalid command\n";
    }
}

void Interpreter::add_command(vector<string> arguments, stringstream &out) {
    siglog::standard("Add command received");
    try {
        auto iter = arguments.begin();

        bool relative = false;

        if(arguments.empty())
            throw ArgumentsException();

        // Check if relative
        if (*iter == "-r") {
            siglog::min("Task to add is relative");
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
            siglog::min("Task to add is cyclic");
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
    catch(runtime_error& e) {
        out << "Error: " << e.what() << endl;
    }
}

void Interpreter::remove_command(vector<string> arguments, stringstream &out) {
    siglog::standard("Remove command received");
    try {
        if(arguments.size() != 1)
            throw ArgumentsException();

        int id = 0;
        try {
            id = stoi(arguments[0]);
        }
        catch(invalid_argument&) {
            siglog::min("Task id to remove is invalid: %s", arguments[0].c_str());
            out << "Error: Provided id is not a number" << endl;
            return;
        }

        cron.remove_task(id);
        out << "Task with id " << id << " removed" << endl;
    }
    catch(ArgumentsException &e) {
        siglog::min("Remove command have invalid arguments");
        out << "Invalid arguments" << endl << "Proper usage: cron rm <id>" << endl;
    }
    catch(runtime_error& e) {
        out << "Error: " << e.what() << endl;
    }
}

void Interpreter::list_command(stringstream &out) {
    siglog::standard("List command received");
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
    siglog::standard("Exit command received");
    int count = cron.exit();
    out << "Cron exited. " << count << " tasks canceled" << endl;
}

void Interpreter::print_task(ostream &os, Task *task) {
    os << task->get_id() << " ";
    if(task->get_base_time().is_relative())
        os << "-r ";
    os << task->get_base_time() << " ";
    if(task->get_repeat_time().get_relative_seconds() != 0)
        os << "-c " << task->get_repeat_time() << " ";
    os << task->get_command() << " ";
    for (string arg : task->get_arguments()) {
        os << arg << " ";
    }
    os << endl;
}