#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstring> 

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define SERVER_ADDRESS "192.168.14.45" // Replace with your server's IP address
// you can find the address in linux terminal by writing this command "ifconfig" and "ipaddr show"

using namespace std;

void sendCommand(const string &command) {
    WSADATA wsaData;
    SOCKET client_socket = INVALID_SOCKET;
    struct sockaddr_in server_address;

    // Initialize Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup failed: " << result << endl;
        return;
    }

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == INVALID_SOCKET) {
        cout << "Socket creation failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return;
    }
    cout << "Socket created successfully" << endl;

    // Configure server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_ADDRESS, &server_address.sin_addr);

    // Connect to server
    result = connect(client_socket, (sockaddr *)&server_address, sizeof(server_address));
    if (result == SOCKET_ERROR) {
        cout << "Connection failed: " << WSAGetLastError() << endl;
        closesocket(client_socket);
        WSACleanup();
        return;
    }
    cout << "Connected to Server" << endl;

    // Send command to server
    send(client_socket, command.c_str(), command.size(), 0);
    cout << "Command sent: " << command << endl;

    // Read server response
    char buffer[2048] = {0};
    int bytes_read = recv(client_socket, buffer, 2048, 0);
    if (bytes_read > 0) {
        cout << "Server response: " << string(buffer, bytes_read) << endl;
    }

    // Close the socket
    closesocket(client_socket);
    WSACleanup();
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
