#ifndef CRON_CRON_H
#define CRON_CRON_H

#include <string>
#include <vector>
#include <list>
#include "task.h"

class Cron {
public:
    void add_task(std::string path, std::vector<std::string> args, Time base_time);
//    void add_cyclic_task(std::string path, std::list<std::string> args, Time base_time, Time repeat_time);
    void remove_task(int task_id);
    std::list<Task> get_tasks();
    void exit();
private:
    std::list<SchedTask*> tasks;
};


#endif
