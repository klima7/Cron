#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "cron.h"
#include "server.h"

using namespace std;

string execute_command(string command);
void handle_connection(int server_sock);

void start_server() {

    // Create socket
    int server_sock = socket(AF_INET , SOCK_STREAM , 0);
    if(server_sock == -1)
        throw exception();

    int iSetOption = 1;
    int res = setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));
    if(res < 0)
        throw exception();

    // Bind
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    res = bind(server_sock, (struct sockaddr *)&server_addr , sizeof(server_addr));
    if(res < 0)
        throw cron_running_exception();

    // Listen
    res = listen(server_sock, 10);
    if(res < 0)
        throw exception();

    cout << "Cron is running" << endl;
    while(true) {
        handle_connection(server_sock);
    }
}

void handle_connection(int server_sock) {

    // Accept
    sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addrlen);
    if (client_sock<0) {
        cout << "Error occured on acception connection" << endl;
        return;
    }

    // Read command
    char response_buff[2000];
    int res = recv(client_sock, response_buff , 2000 , 0);
    if(res < 0) {
        cout << "Error occured on reading command" << endl;
        close(client_sock);
        return;
    }
    string command = string(response_buff, res);

    // Execute command
    string reply = execute_command(command);

    // Send reply
    res = send(client_sock , reply.c_str() , reply.size() , 0);
    if(res < 0) {
        cout << "Error occured on sending reply" << endl;
        close(client_sock);
        return;
    }

    // Close connection
    close(client_sock);
}

string execute_command(string command) {
    return "Command added\n";
}