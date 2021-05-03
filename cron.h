#ifndef CRON_CRON_H
#define CRON_CRON_H

#include <string>
#include <list>
#include "task.h"

class Cron {
public:
    void add_task(Task *task);
    void remove_task(int task_id);
    std::list<Task> get_tasks();
    void exit();
private:
    std::list<Task*> tasks;
};


#endif
