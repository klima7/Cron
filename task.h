#ifndef CRON_TASK_H
#define CRON_TASK_H


class task {
public:
    /*
     * execute()
     * __str__
     * metody do przeszukiwania listy
     */
private:
    int id;
    char *path;
    char **arguments;
};


#endif
