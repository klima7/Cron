#include <spawn.h>
#include <csignal>
#include <ctime>
#include <cassert>
#include "task.h"

using namespace std;

int Task::next_id = 1;

Task::Task(std::string command, std::vector<std::string> args) {
    this->command = command;
    this->args = args;
    this->id = next_id++;
}

void Task::run() {
    pid_t child_pid;
    char* arg_list[args.size()+1];
    for(int i=0; i<args.size(); i++)
        arg_list[i] = (char*)args[i].c_str();
    arg_list[args.size()] = NULL;
    posix_spawn(&child_pid, command.c_str(), NULL, NULL, arg_list, NULL);
}

SchedTask::SchedTask(std::string command, std::vector<std::string> args, Time *base_time, Time *repeat_time): Task(command, args) {
    this->base_time = base_time;
    this->repeat_time = repeat_time;
}

void SchedTask::cancel() {
    timer_delete(timer);
}

void AbsTask::schedule() {
    struct sigevent event = {0};
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = AbsTask::callback;
    event.sigev_value.sival_ptr = this;

    int res = timer_create(CLOCK_REALTIME, &event, &timer);
    assert(res == 0);

    struct itimerspec timespec = {0};
    timespec.it_value.tv_sec = base_time->get_seconds_since_1970();

    res = timer_settime(timer, TIMER_ABSTIME, &timespec, NULL);
    assert(res == 0);
}

void AbsTask::callback(__sigval_t arg) {
    AbsTask *task = (AbsTask*)arg.sival_ptr;
    cout << "Starting task" << endl;
    task->run();
}

void RelTask::schedule() {
    struct sigevent event = {0};
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = RelTask::callback;
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

void RelTask::callback(__sigval_t arg) {
    RelTask *task = (RelTask*)arg.sival_ptr;
    cout << "Starting task" << endl;
    task->run();
}