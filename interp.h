#ifndef CRON_INTERP_H
#define CRON_INTERP_H

#include <string>
#include "cron.h"


class Interpreter {
public:
    Interpreter(Cron cron);
    std::string interpret(std::string command);
private:
    Cron cron;
};


#endif
