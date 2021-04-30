#ifndef CRON_CRON_H
#define CRON_CRON_H

enum command_type {
    ADD,
    DELETE,
    LIST,
    EXIT
};

struct command {
    enum command_type type;
};

#endif
