#include "cron.h"
#include "server.h"
#include <iostream>
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

string connect_arguments(int argc, char **argv);
string send_to_server(string command);

int main(int argc, char **argv) {

    // Start server command
    if(argc == 1) {
        try {
            start_server();
        } catch(cron_running_exception &e) {
            cout << "Cron is already running" << endl;
            return 0;
        } catch(exception &e) {
            cout << "Error occured on starting cron, check logs" << endl;
            return 0;
        }
        cout << "Cron started" << endl;
    }

    // Operation command
    else {
        try {
            string command = connect_arguments(argc, argv);
            string response = send_to_server(command);
            cout << response << endl;
        } catch(cron_not_running_exception &e) {
            cout << "Cron is not running. Start cron first" << endl;
        } catch(exception &e) {
            cout << "Error occured while executing command, check logs" << endl;
        }
    }

    return 0;
}

string connect_arguments(int argc, char **argv) {
    stringstream ss;
    for(int i=1; i<argc; i++) {
        ss << argv[i];
        if(i != argc-1)
            ss << " ";
    }
    return ss.str();
}

string send_to_server(string command) {

    // Create socket
    int socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if(socket_desc == -1)
        throw exception();

    // Connect socket
    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");;
    server.sin_port = htons(PORT);

    int res = connect(socket_desc ,(struct sockaddr *)&server , sizeof(server));
    if (res < 0)
        throw cron_not_running_exception();

    // Send command
    res = send(socket_desc , command.c_str() , command.size() , 0);
    if(res < 0)
        throw exception();

    // Receive response
    char response_buff[2000];
    res = recv(socket_desc, response_buff , 2000 , 0);
    if(res < 0)
        throw exception();

    // Display response
    return string(response_buff, res);
}
