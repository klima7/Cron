#ifndef CRON_TASK_H
#define CRON_TASK_H

#include <string>
#include <list>

class Task {
public:
    void run();
private:
    int id;
    std::string path;
    std::list<std::string> args;
};

#endif
