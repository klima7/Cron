#ifndef CRON_INTERP_H
#define CRON_INTERP_H

#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include "cron.h"
#include "task.h"


class Interpreter {
public:
    Interpreter(Cron &cron);
    std::string interpret(std::string command);
private:
    class ArgumentsException: public std::exception {};
    void list_command(std::stringstream &out);
    void remove_command(std::vector<std::string> arguments, std::stringstream &out);
    void add_command(std::vector<std::string> arguments, std::stringstream &out);
    void exit_command(std::stringstream &out);
    void help_command(std::stringstream &out);
    Cron &cron;
};

#endif
