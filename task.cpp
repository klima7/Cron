#include <spawn.h>
#include <csignal>
#include <ctime>
#include <cassert>
#include <iomanip>
#include "task.h"

using namespace std;

int Task::next_id = 1;

Task::Task(std::string command, std::vector<std::string> args, Time base_time, Time repeat_time) {
    this->command = command;
    this->args = args;
    this->id = next_id++;
    this->base_time = base_time;
    this->repeat_time = repeat_time;
    this->done = false;
}

int Task::get_id() const {
    return id;
}

Time Task::get_base_time() const {
    return base_time;
}

Time Task::get_repeat_time() const {
    return repeat_time;
}

std::string Task::get_command() const {
    return command;
}

std::vector<std::string> Task::get_arguments() const {
    return args;
}

bool Task::is_done() {
    return done;
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
    done = true;
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
    if(task->get_repeat_time().get_seconds() == 0)
        task->done = true;
    cout << "Starting task" << endl;
    task->run();
}

