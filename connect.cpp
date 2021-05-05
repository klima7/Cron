#include "connect.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;


int main(int argc, char **argv) {

    // Starting server
    if(argc == 1) {
        try {
            Server server;
            server.start_server();
        }
        catch(runtime_error& e) {
            cout << "Error: " << e.what() << endl;
            return EXIT_FAILURE;
        }
    }

    // Operation
    else {
        try {
            Client client;
            string response = client.execute_command(argc, argv);
            cout << response;
        }
        catch(runtime_error& e) {
            cout << "Error: " << e.what() << endl;
            return EXIT_FAILURE;
        }
    }

    return 0;
}

void Server::start_server() {

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
        throw runtime_error("Cron is already running");

    // Listen
    res = listen(server_sock, 10);
    if(res < 0)
        throw exception();

    while(true) {
        handle_connection(server_sock);
    }
}

void Server::handle_connection(int server_sock) {

    // Accept
    sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addrlen);
    if (client_sock<0)
        throw runtime_error("Error occurred while acceptation connection");

    // Read Command
    char response_buff[2000];
    int res = recv(client_sock, response_buff , 2000 , 0);
    if(res < 0) {
        close(client_sock);
        throw runtime_error("Error occurred on reading Command");
    }
    string command = string(response_buff, res);

    // Execute Command
    string reply;
    try {
        reply = interp.interpret(command);
    }
    catch(...) {
        close(client_sock);
        throw;
    }

    // Send reply
    res = send(client_sock , reply.c_str() , reply.size() , 0);
    if(res < 0) {
        close(client_sock);
        throw runtime_error("Error occurred on sending reply");
    }

    // Close connection
    close(client_sock);

    if(cron.is_exited()) {
        exit(0);
    }
}

string Client::execute_command(int argc, char **argv) {
    string command = connect_arguments(argc, argv);
    string response = send_to_server(command);
    return response;
}

string Client::connect_arguments(int argc, char **argv) {
    stringstream ss;
    for(int i=1; i<argc; i++) {
        ss << argv[i];
        if(i != argc-1)
            ss << " ";
    }
    return ss.str();
}

string Client::send_to_server(string command) {

    // Create socket
    int socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if(socket_desc == -1)
        throw exception();

    // Connect socket
    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORT);

    int res = connect(socket_desc ,(struct sockaddr *)&server , sizeof(server));
    if (res < 0) {
        close(socket_desc);
        throw runtime_error("Cron is not running. Start cron first");
    }

    // Send Command
    res = send(socket_desc , command.c_str() , command.size() , 0);
    if(res < 0) {
        close(socket_desc);
        throw runtime_error("Unable to send command to cron");
    }

    // Receive response
    char response_buff[2000];
    res = recv(socket_desc, response_buff , 2000 , 0);
    if(res < 0) {
        close(socket_desc);
        throw runtime_error("Unable to receive command from cron");
    }

    // Display response
    close(socket_desc);
    return string(response_buff, res);
}
