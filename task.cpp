#include <spawn.h>
#include <csignal>
#include <ctime>
#include <cassert>
#include "task.h"

using namespace std;

int Task::next_id = 1;

Task::Task(std::string command, std::vector<std::string> args, Time base_time, Time repeat_time) {
    this->command = command;
    this->args = args;
    this->id = next_id++;
    this->base_time = base_time;
    this->repeat_time = repeat_time;
}

int Task::get_id() {
    return id;
}

void Task::run() {
    pid_t child_pid;
    char* arg_list[args.size()+1];
    for(int i=0; i<args.size(); i++)
        arg_list[i] = (char*)args[i].c_str();
    arg_list[args.size()] = NULL;
    posix_spawn(&child_pid, command.c_str(), NULL, NULL, arg_list, NULL);
}

void Task::cancel() {
    timer_delete(timer);
}

void Task::schedule() {
    struct sigevent event = {0};
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = Task::callback;
    event.sigev_value.sival_ptr = this;

    int res = timer_create(CLOCK_REALTIME, &event, &timer);
    assert(res == 0);

    struct itimerspec timespec = {0};
    timespec.it_value.tv_sec = base_time.get_seconds();
    timespec.it_interval.tv_sec = repeat_time.get_seconds();

    long diff = timespec.it_value.tv_sec - time(NULL);
    cout << "Diff=" << diff << endl;

    if(base_time.is_relative()) {
        cout << "relative" << endl;
    }
    else {
        cout << "absolute" << endl;
    }
    res = timer_settime(timer, base_time.is_relative() ? 0 : TIMER_ABSTIME, &timespec, NULL);
    assert(res == 0);
}

void Task::callback(__sigval_t arg) {
    Task *task = (Task*)arg.sival_ptr;
    cout << "Starting task" << endl;
    task->run();
}
