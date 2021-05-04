#ifndef CRON_TASK_H
#define CRON_TASK_H

#include <string>
#include <vector>
#include <csignal>
#include "time.h"


class Task {
public:
    Task(std::string command, std::vector<std::string> args);
    void run();

private:
    static int next_id;
    int id;

    std::string command;
    std::vector<std::string> args;
};


class SchedTask: public Task {
public:
    SchedTask(std::string command, std::vector<std::string> args, Time *base_time, Time *repeat_time=NULL);
    virtual void schedule()=0;
    void cancel();
protected:
    timer_t timer;
    Time *base_time;
    Time *repeat_time;
};


class AbsTask: public SchedTask {
public:
    AbsTask(std::string command, std::vector<std::string> args, Time *base_time, Time *repeat_time=NULL): SchedTask(command, args, base_time, repeat_time) {};
    void schedule() override;
private:
    static void callback(__sigval_t arg);
};


class RelTask: public SchedTask {
public:
    void schedule() override;
private:
    static void callback(__sigval_t arg);
};


#endif
