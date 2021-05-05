#include "cron.h"
#include "task.h"
#include <list>
#include <vector>

using namespace std;

void Cron::add_task(string path, vector<string> args, Time base_time, Time repeat_time) {
    Task *task = new Task(path, args, base_time, repeat_time);
    tasks.push_back(task);
    task->schedule();
}

bool Cron::remove_task(int task_id) {
    for(auto iter=tasks.begin(); iter != tasks.end(); iter++) {
        Task *task = *iter;
        if(task->is_active() && task->get_id() == task_id) {
            task->cancel();
            return true;
        }
    }
    return false;
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

