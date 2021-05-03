#include "task.h"

using namespace std;

int Task::next_id = 1;

Task::Task(string command, vector<string> args, TimeInterval time): id(next_id++), base_time(time), repeat_time(time) {
    this->command = command;
    this->args = args;
    cyclic = false;
}