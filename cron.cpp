#include "cron.h"
#include <list>

using namespace std;

void Cron::add_task(Task task) {
    cout << "Task added" << endl;
}

void Cron::remove_task(int task_id) {

}

std::list<Task> Cron::get_tasks() {
    return list<Task>();
}

void Cron::exit() {

}
