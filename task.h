#ifndef CRON_TASK_H
#define CRON_TASK_H

#include <string>
#include <vector>
#include <csignal>
#include "time.h"


class Task {
public:
    Task(std::string command, std::vector<std::string> args, Time base_time, Time repeat_time);
    int get_id();
    void run();
    void schedule();
    void cancel();

private:
    static int next_id;
    int id;

    std::string command;
    std::vector<std::string> args;

    timer_t timer;
    Time base_time;
    Time repeat_time;

    static void callback(__sigval_t arg);
};


#endif