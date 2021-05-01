#include "interp.h"

using namespace std;

Interpreter::Interpreter(Cron cron) {
    this->cron = cron;
}

string Interpreter::interpret(std::string command) {
    return "Command interpreted";
}
