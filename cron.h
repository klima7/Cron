#ifndef CRON_CRON_H
#define CRON_CRON_H

#include <string>
#include <vector>
#include <list>
#include "task.h"

class Cron {
public:
    void add_task(std::string path, std::vector<std::string> args, Time base_time, Time repeat_time);
    void remove_task(int task_id);
    std::list<Task*> get_tasks();
    int exit();
    bool is_exited();
private:
    bool exited = false;
    std::list<Task*> tasks;
};


#endif
