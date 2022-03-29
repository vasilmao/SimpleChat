#include "Conversation.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cassert>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>

const int BUFFER_SIZE = 1024;

ClientConversation::ClientConversation(in_addr_t ip, in_port_t port) {
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd_ == -1) {
        perror("Can't create socket!!");
        exit(EXIT_FAILURE);
    }


    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_port = port
    };

    address.sin_addr.s_addr = ip;

    printf("Connecting to master...\n");

    if (connect(socket_fd_, (sockaddr*) &address, sizeof(struct sockaddr_in)) != 0) {
        perror("Can't connect!!");
        exit(EXIT_FAILURE);
    }

    printf("Connected!\n");

    // success
}

void ClientConversation::CheckStdin() {

    char buffer[BUFFER_SIZE + 1];
    memset(buffer, 0, BUFFER_SIZE + 1);

    int read_cnt = read(STDIN_FILENO, buffer, BUFFER_SIZE);
    if (read_cnt <= 0) {
        return;
    }
    if (buffer[0] == '\\') {
        if (read_cnt == BUFFER_SIZE) {
            return; // wrong command
        }
        ParseCommand(buffer, read_cnt);
        return;
    }
    write(socket_fd_, buffer, read_cnt);
    buffer[read_cnt] = '\0';
    printf("SENT: %s", buffer);


    FILE* dump_file = fopen("history.txt", "a"); // yeah, postgresql would be much better, but time...
    fprintf(dump_file, "SENT: %s", buffer);
    fclose(dump_file);


    if (buffer[read_cnt - 1] == '\n') {
        return;
    }
    while ((read_cnt = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
        write(socket_fd_, buffer, read_cnt);
        buffer[read_cnt] = '\0';
        printf("SENT: %s", buffer);
        FILE* dump_file = fopen("history.txt", "a"); // yeah, postgresql would be much better, but time...
        fprintf(dump_file, "SENT: %s", buffer);
        fclose(dump_file);
        if (buffer[read_cnt - 1] == '\n') {
            break;
        }
    }
}

void ClientConversation::CheckInput() {
    char buffer[BUFFER_SIZE + 1];
    memset(buffer, 0, BUFFER_SIZE + 1);
    int read_cnt = '\0';
    while ((read_cnt = read(socket_fd_, buffer, BUFFER_SIZE)) > 0) {
        buffer[read_cnt] = '\0';
        printf("RECEIVED: %s", buffer);
        FILE* dump_file = fopen("history.txt", "a"); // yeah, postgresql would be much better, but time...
        fprintf(dump_file, "RECEIVED: %s", buffer);
        fclose(dump_file);
        if (buffer[read_cnt - 1] == '\n') {
            break;
        }
    }
}

void ClientConversation::ParseCommand(char* cmd, size_t cmd_len) {
    if (strncmp(cmd, "\\exit", sizeof("\\exit") - 1) == 0) {
        shutdown(socket_fd_, SHUT_RDWR);
        is_alive_ = false;
        return;
    }
    if (strncmp(cmd, "\\history", sizeof("\\history") - 1) == 0) {
        FILE* dump_file = fopen("history.txt", "r"); // yeah, postgresql would be much better, but time...
        char buffer[BUFFER_SIZE + 1];
        memset(buffer, 0, BUFFER_SIZE);
        size_t read_cnt = 0;
        while ((read_cnt = fread(buffer, sizeof(char), BUFFER_SIZE, dump_file)) > 0) {
            write(STDOUT_FILENO, buffer, read_cnt);
        }
        fclose(dump_file);
    }
}

int ClientConversation::GetFD() {
    return socket_fd_;
}

bool ClientConversation::IsEnded() {
    return !is_alive_;
}

ClientConversation::~ClientConversation() {
    close(socket_fd_);
}