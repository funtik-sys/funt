#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>

#define PORT 65432
#define BUFFER_SIZE 1024

#pragma comment(lib, "ws2_32.lib")

int main() {

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "Инициализаиция winsock прошла неудачна" << std::endl;
        return 1;
    }
    
    SOCKET server_sok = socket(AF_INET, SOCK_STREAM, 0);

    if (server_sok == INVALID_SOCKET) {
        std::cout << "Возникла ошибка при создании сокета" << std::endl;
        WSACleanup();
        return 1;
    }

    
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    

    if (bind(server_sok, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cout << "Ошибка привязки сокета" << std::endl;
        closesocket(server_sok);
        WSACleanup();
        return 1;
    }

    if (listen(server_sok, 3) == SOCKET_ERROR) {
        std::cout << "Ошибка при попытке начать прослушивание" << std::endl;
        closesocket(server_sok);
        WSACleanup();
        return 1;
    }

    struct sockaddr_in client_addr;
    int client_len = sizeof(client_addr);
    SOCKET client_socket = accept(server_sok, (struct sockaddr*)&client_addr, &client_len);
    if (client_socket == INVALID_SOCKET) {
        std::cout << "Ошибка при попытке подключиться" << std::endl;
        closesocket(server_sok);
        WSACleanup();
        return 1;
    }


    char buffer[BUFFER_SIZE];
    int total_received = 0;
    char client_ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    std::cout << "Клиент подключился" << std::endl;

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);


        int bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0);

        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            total_received += bytes_read;

            if (strcmp(buffer, "exite") == 0) {
                std::cout << "Выход..." << std::endl;
                break;
            }

            // тут же можно обработать сообщение клиента, к примеру, через функцию: process_message(buffer);


        }
        else if (bytes_read == 0) {
            std::cout << "Клиент вышел..." << std::endl;
            break;
        }
        else {
            std::cout << "Не удалось получить сообщение от клиента..." << std::endl;
            break;
        }
    }
    WSACleanup();
    return 0;
}