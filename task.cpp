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
    this->active = true;
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

bool Task::is_active() const {
    return active;
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
    active = false;
}

void Task::schedule() {

    // Create event
    struct sigevent event = {0};
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = Task::callback;
    event.sigev_value.sival_ptr = this;

    // Create timer
    int res = timer_create(CLOCK_REALTIME, &event, &timer);
    if(res != 0)
        throw runtime_error("Unable to create clock");

    // Validate relative time
    long seconds;
    if(base_time.is_relative()) {
        seconds = base_time.get_relative_seconds();
        if(seconds < 0)
            throw runtime_error("Provided relative time must be positive");
    }

    // Validate absolute time
    else {
        seconds = base_time.get_absolute_seconds();
        long curr_seconds = time(NULL);
        if(curr_seconds > seconds) {
            throw runtime_error("Provided absolute time already elapsed");
        }
    }

    // Define delays
    struct itimerspec timespec = {0};
    timespec.it_interval.tv_sec = repeat_time.get_relative_seconds();
    timespec.it_value.tv_sec = seconds;

    // Start timer
    res = timer_settime(timer, base_time.is_relative() ? 0 : TIMER_ABSTIME, &timespec, NULL);
    if(res != 0)
        throw runtime_error("Unable to start timer");
}

void Task::callback(__sigval_t arg) {
    Task *task = (Task*)arg.sival_ptr;
    if(task->get_repeat_time().get_relative_seconds() == 0)
        task->active = false;
    task->run();
}

