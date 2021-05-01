#ifndef CRON_CONNECT_H
#define CRON_CONNECT_H

#include <stdexcept>
#include "cron.h"

#define PORT 6438

class cron_running_exception: public std::exception {};

class cron_not_running_exception: public std::exception {};

class server {
public:
    void start_server();
private:
    void handle_connection(int server_sock);
    std::string execute_command(std::string command);

    Cron cron;
};

class client {
public:
    std::string execute_command(int argc, char **argv);
private:
    std::string connect_arguments(int argc, char **argv);
    std::string send_to_server(std::string command);
};

#endif
