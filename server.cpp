#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "cron.h"
#include "server.h"

using namespace std;

void accept_connection(int server_sock);

int start_server() {

    // Create socket
    int server_sock = socket(AF_INET , SOCK_STREAM , 0);
    if(server_sock == -1)
        return SERVER_ERROR;

    // Bind
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    int res = bind(server_sock, (struct sockaddr *)&server_addr , sizeof(server_addr));
    if(res < 0)
        return SERVER_ALREADY_RUNNING_ERROR;

    // Listen
    res = listen(server_sock, 10);
    if(res < 0)
        return SERVER_ERROR;

    // Accept connections
    while(true) {
        accept_connection(server_sock);
    }

    return 0;
}

void accept_connection(int server_sock) {
    sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addrlen);
    if (client_sock<0)
        cout << "Error" << endl;
}