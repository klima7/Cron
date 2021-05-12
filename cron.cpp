#include "cron.h"
#include "task.h"
#include "siglog.h"
#include <list>
#include <vector>
#include <sstream>

using namespace std;

Cron* Cron::dump_cron = nullptr;

void Cron::add_task(string path, vector<string> args, Time base_time, Time repeat_time) {
    Task *task = new Task(path, args, base_time, repeat_time);
    try {
        task->schedule();
        tasks.push_back(task);
        siglog::standard("New task added with id %d", task->get_id());
    } catch(runtime_error& e) {
        siglog::max((string("Error: ") + e.what()).c_str());
        delete task;
        throw;
    }
}

void Cron::remove_task(int task_id) {
    for(auto task : tasks) {
        if(task->is_active() && task->get_id() == task_id) {
            task->cancel();
            siglog::standard("Task with id %d removed", task_id);
            return;
        }
    }
    siglog::max("Unable to find task with id %d", task_id);
    throw runtime_error("Unable to find task with given id");
}

std::list<Task> Cron::get_tasks() const {
    siglog::min("Cron is listing tasks");
    list<Task> active_tasks;
    for(Task *task : tasks) {
        if(task->is_active()) {
            active_tasks.push_back(*task);
        }
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
    siglog::max("Cron exited");
    return count;
}

bool Cron::is_exited() const {
    return exited;
}

void Cron::dump(FILE *file) {
    siglog::standard("cron is performing dump");
    stringstream ss;
    std::list<Task> tasks = dump_cron->get_tasks();
    for(Task task : tasks)
        ss << &task << endl;
    fprintf(file, "%s", ss.str().c_str());
}

void Cron::init_dump() {
    siglog::min("Initializing dump");
    dump_cron = this;
    siglog::register_dump_function(dump);
}
