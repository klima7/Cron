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
        int res = start_server();
        if(res == SERVER_ALREADY_RUNNING_ERROR)
            cout << "Cron is already running" << endl;
        else if(res == SERVER_ERROR)
            cout << "Error occured, check logs" << endl;
        else
            cout << "Cron started" << endl;
    }

    // Operation command
    else {
        string command = connect_arguments(argc, argv);
        string response = send_to_server(command);
        cout << response << endl;
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
        return "Error occured";

    // Connect socket
    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");;
    server.sin_port = htons(PORT);

    int res = connect(socket_desc ,(struct sockaddr *)&server , sizeof(server));
    if (res < 0)
        return "Cron is not running. Start cron first";

    // Send command
    res = send(socket_desc , command.c_str() , command.size() , 0);
    if(res < 0)
        return "Cron is not running";

    // Receive response
    char response_buff[2000];
    res = recv(socket_desc, response_buff , 2000 , 0);
    if(res < 0)
        return "Error occured";

    // Display response
    return string(response_buff, res);
}
