#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "db_handler.h"
#include "logger.h"

#define PORT 8080
#define BUFFER_SIZE 1024

void initialize_server();
int create_socket();
int bind_socket(int server_fd);
void start_listening(int server_fd);
void accept_clients(int server_fd);
void handle_client(int client_fd);
void process_data(int client_fd, char *buffer);
void send_response(int client_fd, const char *message);

int server_fd;

int main() {
    initialize_server();
    accept_clients(server_fd);
    close(server_fd);
    return 0;
}

void initialize_server() {
    init_logger("logs/server.log");

    server_fd = create_socket();
    if (server_fd == -1) exit(EXIT_FAILURE);

    if (bind_socket(server_fd) == -1) exit(EXIT_FAILURE);

    start_listening(server_fd);
}

int create_socket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        log_error("Socket creation failed");
        return -1;
    }
    log_info("Socket created successfully");
    return sockfd;
}

int bind_socket(int server_fd) {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        log_error("Binding socket failed");
        return -1;
    }
    log_info("Socket successfully bound");
    return 0;
}

void start_listening(int server_fd) {
    if (listen(server_fd, 5) < 0) {
        log_error("Failed to start listening");
        exit(EXIT_FAILURE);
    }
    log_info("Server is listening on port 8080...");
}

void accept_clients(int server_fd) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_fd;

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) {
            log_error("Failed to accept client connection");
            continue;
        }
        log_info("New client connected");

        handle_client(client_fd);
    }
}

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
        
        if (bytes_received <= 0) {
            log_info("Client disconnected.");
            break;
        }

        process_data(client_fd, buffer);
    }

    close(client_fd);
    log_info("Client connection closed.");
}

void process_data(int client_fd, char *buffer) {
    log_info("Received data: %s", buffer);

    if (store_user_data(buffer) == 0) {
        send_response(client_fd, "Data stored successfully\n");
    } else {
        send_response(client_fd, "Failed to store data\n");
    }
}

void send_response(int client_fd, const char *message) {
    send(client_fd, message, strlen(message), 0);
}
