#include "connect.h"
#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;


int main(int argc, char **argv) {

    // Start server command
    if(argc == 1) {
        try {
            server server;
            server.start_server();
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
            client client;
            string response = client.execute_command(argc, argv);
            cout << response << endl;
        } catch(cron_not_running_exception &e) {
            cout << "Cron is not running. Start cron first" << endl;
        } catch(exception &e) {
            cout << "Error occured while executing command, check logs" << endl;
        }
    }

    return 0;
}

void server::start_server() {

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

void server::handle_connection(int server_sock) {

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

string server::execute_command(string command) {
    return "Command added\n";
}

string client::execute_command(int argc, char **argv) {
    string command = connect_arguments(argc, argv);
    string response = send_to_server(command);
    return response;
}

string client::connect_arguments(int argc, char **argv) {
    stringstream ss;
    for(int i=1; i<argc; i++) {
        ss << argv[i];
        if(i != argc-1)
            ss << " ";
    }
    return ss.str();
}

string client::send_to_server(string command) {

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
