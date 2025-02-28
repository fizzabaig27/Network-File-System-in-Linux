#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring> 

#define PORT 8080
#define SERVER_ADDRESS "127.0.0.1"

using namespace std;

void sendCommand(const string &command) {
    int rnett = 0;
    int yup = 0;
    int client_socket;
    struct sockaddr_in server_address;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        cout << "Socket not opened" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Socket opened" << endl;

    // Configure server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    yup = inet_pton(AF_INET, SERVER_ADDRESS, &server_address.sin_addr);
    if (yup <= 0) {
        cout << "Invalid address" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Valid address" << endl;

    // Connect to server
    rnett = connect(client_socket, (sockaddr *)&server_address, sizeof(server_address));
    if (rnett < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    cout << "Connected to Server" << endl;

    // Send command to server
    send(client_socket, command.c_str(), command.size(), 0);
    cout << "Command sent: " << command << endl;

    // Read server response
    char buffer[2048] = {0};
    int bytes_read = read(client_socket, buffer, 2048);
    if (bytes_read > 0) {
        cout << "Server response: " << string(buffer, bytes_read) << endl;
    }

    // Close the socket
    close(client_socket);
    cout << "Connection closed" << endl;
}

int main() {
    int choice;
    string filename, content, command;

    while (true) {
        cout << "\nFile Management System" << endl;
        cout << "1. Create File" << endl;
        cout << "2. Read File" << endl;
        cout << "3. Update File" << endl;
        cout << "4. Delete File" << endl;
        cout << "5. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); // Clear the newline character from the input buffer

        switch (choice) {
            case 1:
                cout << "Enter filename: ";
                getline(cin, filename);
                cout << "Enter content: ";
                getline(cin, content);
                command = "CREATE " + filename + " " + content;
                sendCommand(command);
                break;

            case 2:
                cout << "Enter filename: ";
                getline(cin, filename);
                command = "READ " + filename;
                sendCommand(command);
                break;

            case 3:
                cout << "Enter filename: ";
                getline(cin, filename);
                cout << "Enter new content: ";
                getline(cin, content);
                command = "UPDATE " + filename + " " + content;
                sendCommand(command);
                break;

            case 4:
                cout << "Enter filename: ";
                getline(cin, filename);
                command = "DELETE " + filename;
                sendCommand(command);
                break;

            case 5:
                cout << "Exiting..." << endl;
                exit(0);

            default:
                cout << "Invalid choice, please try again." << endl;
        }
    }

    return 0;
}
