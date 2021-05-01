#ifndef CRON_CRON_H
#define CRON_CRON_H

#include <stdexcept>

#define PORT 6438

class cron_running_exception: public std::exception {};

class cron_not_running_exception: public std::exception {};

#endif
