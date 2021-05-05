#ifndef CRON_TASK_H
#define CRON_TASK_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <csignal>
#include "time.h"


class Task {

public:
    Task(std::string command, std::vector<std::string> args, Time base_time, Time repeat_time);

    int get_id() const;
    Time get_base_time() const;
    Time get_repeat_time() const;
    std::string get_command() const;
    std::vector<std::string> get_arguments() const;
    bool is_active() const;

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
    bool active;

    static void callback(__sigval_t arg);
};


#endif