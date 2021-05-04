#include "cron.h"
#include "task.h"
#include <list>
#include <vector>

using namespace std;

void Cron::add_task(string path, vector<string> args, Time base_time) {
    SchedTask *task = new AbsTask(path, args, &base_time, NULL);
    tasks.push_back(task);
    task->schedule();
}

void Cron::remove_task(int task_id) {

}

std::list<Task> Cron::get_tasks() {
    return list<Task>();
}

void Cron::exit() {

}
