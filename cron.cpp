#include "cron.h"
#include "task.h"
#include <list>
#include <vector>

using namespace std;

void Cron::add_task(string path, vector<string> args, Time base_time, Time repeat_time) {
    Task *task = new Task(path, args, base_time, repeat_time);
    try {
        task->schedule();
        tasks.push_back(task);
    } catch(runtime_error&) {
        delete task;
        throw;
    }
}

void Cron::remove_task(int task_id) {
    for(auto task : tasks) {
        if(task->is_active() && task->get_id() == task_id) {
            task->cancel();
            return;
        }
    }
    throw runtime_error("Unable to find task with given id");
}

std::list<Task> Cron::get_tasks() const {
    list<Task> active_tasks;
    for(Task *task : tasks) {
        if(task->is_active())
            active_tasks.push_back(*task);
    }
    return active_tasks;
}

int Cron::exit() {
    int count = 0;
    for(Task *task : tasks) {
        if(task->is_active()) {
            task->cancel();
            count++;
        }
        delete task;
    }
    tasks.clear();
    exited = true;
    return count;
}

bool Cron::is_exited() const {
    return exited;
}

