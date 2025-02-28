#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring> 

#define PORT 8080

using namespace std;

void handleClient(int new_socket) {
    char buffer[2048] = {0}; // Increased buffer size to handle larger commands
    int bytes_read;

    // Read message from client
    bytes_read = read(new_socket, buffer, 2048);
    if (bytes_read > 0) {
        string command(buffer, bytes_read);
        istringstream iss(command);
        vector<string> tokens;
        string token;
        while (iss >> token) {
            tokens.push_back(token);
        }

        // Check the command type and perform the corresponding operation
        if (tokens[0] == "CREATE" && tokens.size() > 2) {
            string filename = tokens[1];
            string content = command.substr(command.find(filename) + filename.length() + 1);
            ofstream outfile(filename);
            if (outfile.is_open()) {
                outfile << content;
                outfile.close();
                send(new_socket, "File created", strlen("File created"), 0);
            } else {
                send(new_socket, "Failed to create file", strlen("Failed to create file"), 0);
            }
        } else if (tokens[0] == "READ" && tokens.size() > 1) {
            string filename = tokens[1];
            ifstream infile(filename);
            if (infile.is_open()) {
                stringstream buffer;
                buffer << infile.rdbuf();
                string content = buffer.str();
                send(new_socket, content.c_str(), content.size(), 0);
                infile.close();
            } else {
                send(new_socket, "File not found", strlen("File not found"), 0);
            }
        } else if (tokens[0] == "UPDATE" && tokens.size() > 2) {
            string filename = tokens[1];
            string content = command.substr(command.find(filename) + filename.length() + 1);
            ofstream outfile(filename);
            if (outfile.is_open()) {
                outfile << content;
                outfile.close();
                send(new_socket, "File updated", strlen("File updated"), 0);
            } else {
                send(new_socket, "Failed to update file", strlen("Failed to update file"), 0);
            }
        } else if (tokens[0] == "DELETE" && tokens.size() > 1) {
            string filename = tokens[1];
            if (remove(filename.c_str()) == 0) {
                send(new_socket, "File deleted", strlen("File deleted"), 0);
            } else {
                send(new_socket, "Failed to delete file", strlen("Failed to delete file"), 0);
            }
        } else {
            send(new_socket, "Invalid command", strlen("Invalid command"), 0);
        }
    }

    // Close the connection
    close(new_socket);
}

int main() {
    int rnet = 0;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Configure server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Create socket
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    cout << "Socket created successfully" << endl;

    // Bind socket to address and port
    rnet = bind(server_fd, (sockaddr *)&address, sizeof(address));
    if (rnet < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    cout << "Socket bound to address and port" << endl;

    // Listen for incoming connections
    rnet = listen(server_fd, 5);
    if (rnet < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    cout << "Server listening on port " << PORT << endl;

    // Accept incoming connections
    int new_socket;
    while (true) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        cout << "Accepted new connection" << endl;
        handleClient(new_socket);
    }

    return 0;
}
