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

void Cron::remove_task(int task_id) {
    for(list<Task*>::iterator iter=tasks.begin(); iter != tasks.end(); iter++) {
        Task *task = *iter;
        if(task->get_id() == task_id) {
            task->cancel();
            return;
        }
    }
}

std::list<Task> Cron::get_tasks() {
    list<Task> active_tasks;
    for(Task *task : tasks) {
        if(!task->is_done())
            active_tasks.push_back(*task);
    }
    return active_tasks;
}

int Cron::exit() {
    int count = 0;
    for(Task *task : tasks) {
        if(!task->is_done()) {
            task->cancel();
            count++;
        }
        delete task;
    }
    tasks.clear();
    exited = true;
    return count;
}

bool Cron::is_exited() {
    return exited;
}

