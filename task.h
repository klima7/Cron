#ifndef CRON_TASK_H
#define CRON_TASK_H

#include <string>
#include <vector>
#include "time.h"

class Task {
public:
    Task(std::string cmd, std::vector<std::string> args, TimeInterval base_time);
    Task(std::string cmd, std::vector<std::string> args, TimeInterval base_time, TimeInterval repeat_time);
    void run();
    void schedule();
private:
    static int next_id;
    int id;
    bool cyclic;

    std::string command;
    std::vector<std::string> args;
    TimeInterval base_time;
    TimeInterval repeat_time;
};

#endif
