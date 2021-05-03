#include "cron.h"
#include <list>

using namespace std;

void Cron::add_task(Task *task) {
    tasks.push_back(task);
//    task.run();
    task->schedule();
}

void Cron::remove_task(int task_id) {

}

std::list<Task> Cron::get_tasks() {
    return list<Task>();
}

void Cron::exit() {

}
