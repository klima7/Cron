#ifndef CRON_CRON_H
#define CRON_CRON_H

#include <string>
#include <list>
#include "task.h"

class Cron {
    void add_task(Task task);
    void remove_task(int task_id);
    void exit();
    std::list<Task> get_tasks();
};


#endif
