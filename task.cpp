#include <spawn.h>
#include <signal.h>
#include <time.h>
#include <assert.h>
#include "task.h"

using namespace std;

void fun(__sigval_t arg);

int Task::next_id = 1;

Task::Task(string command, vector<string> args, Time time): id(next_id++), base_time(time), repeat_time(time) {
    this->command = command;
    this->args = args;
    cyclic = false;
}

void Task::run() {
    pid_t child_pid;

    char* arg_list[args.size()+1];
    for(int i=0; i<args.size(); i++)
        arg_list[i] = (char*)args[i].c_str();
    arg_list[args.size()] = NULL;

    posix_spawn(&child_pid, command.c_str(), NULL, NULL, arg_list, NULL);
}

void Task::schedule() {
    struct sigevent event = {0};
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = fun;
    event.sigev_value.sival_ptr = this;

    timer_t timer;
    int res = timer_create(CLOCK_REALTIME, &event, &timer);
    assert(res == 0);

    struct itimerspec timespec = {0};
    timespec.it_value.tv_sec = time(NULL);
    timespec.it_value.tv_sec += 3;

    res = timer_settime(timer, TIMER_ABSTIME, &timespec, NULL);
    cout << "res: " << res << endl;
}

void fun(__sigval_t arg) {
    Task *task = (Task*)arg.sival_ptr;
    cout << "Starting task" << endl;
    task->run();
}