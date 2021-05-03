#ifndef CRON_INTERP_H
#define CRON_INTERP_H

#include <string>
#include <vector>
#include "cron.h"
#include "task.h"


class Interpreter {
public:
    Interpreter(Cron cron);
    std::string interpret(std::string command);
private:
    static std::vector<std::string> get_tokens(std::string text, std::string delimiter);
    std::string list_command();
    std::string remove_command(std::vector<std::string> arguments);
    std::string add_command(std::vector<std::string> arguments);
    Cron cron;
};


#endif
