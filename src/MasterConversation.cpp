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

// extern int errno;

MasterConversation::MasterConversation(in_port_t port) {
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ == -1) {
        perror("Can't create socket!!\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_port = port,
    };

    address.sin_addr.s_addr = inet_addr("127.0.0.1");

    socklen_t address_len = sizeof(struct sockaddr_in);

    if (bind(socket_fd_, (sockaddr*) &address, sizeof(struct sockaddr_in)) != 0) {
        perror("Can't bind!!");
        exit(EXIT_FAILURE);
    }

    if (listen(socket_fd_, SOMAXCONN) != 0) {
        perror("Can't listen!!");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for client...\n");

    companion_fd_ = accept(socket_fd_, (sockaddr*) &address, &address_len); // locks until client is connected
    printf("companion fd %d\n", companion_fd_);
    MakeNonBlocking(companion_fd_);

    printf("Client found! Ip: %s\n", inet_ntoa(address.sin_addr));

}

void MasterConversation::CheckStdin() {

    char buffer[BUFFER_SIZE + 1];
    memset(buffer, 0, BUFFER_SIZE + 1);

    int read_cnt = read(STDIN_FILENO, buffer, BUFFER_SIZE);
    if (read_cnt <= 0) {
        return;
    }
    if (buffer[0] == '\\') {
        if (read_cnt == BUFFER_SIZE) {
            return;
        }
        ParseCommand(buffer, read_cnt);
        return;
    }
    write(companion_fd_, buffer, read_cnt);
    buffer[read_cnt] = '\0';
    printf("SENT:\n%s", buffer);
    FILE* dump_file = fopen("history.txt", "a"); // yeah, postgresql would be much better, but time...
    fprintf(dump_file, "SENT: %s", buffer);
    fclose(dump_file);
    if (buffer[read_cnt - 1] == '\n') {
        return;
    }
    while ((read_cnt = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
        write(companion_fd_, buffer, read_cnt);
        buffer[read_cnt] = '\0';
        printf("SENT:\n%s", buffer);
        FILE* dump_file = fopen("history.txt", "a"); // yeah, postgresql would be much better, but time...
        fprintf(dump_file, "SENT: %s", buffer);
        fclose(dump_file);
        if (buffer[read_cnt - 1] == '\n') {
            break;
        }
    }
}

void MasterConversation::CheckInput() {
    char buffer[BUFFER_SIZE + 1];
    memset(buffer, 0, BUFFER_SIZE + 1);
    int read_cnt = '\0';
    while ((read_cnt = read(companion_fd_, buffer, BUFFER_SIZE)) > 0) {
        buffer[read_cnt] = '\0';
        printf("RECEIVED:\n%s", buffer);
        FILE* dump_file = fopen("history.txt", "a"); // yeah, postgresql would be much better, but time...
        fprintf(dump_file, "RECEIVED: %s", buffer);
        fclose(dump_file);
        if (buffer[read_cnt - 1] == '\n') {
            break;
        }
    }
}

void MasterConversation::ParseCommand(char* cmd, size_t cmd_len) {
    // get history for example or exit
    if (strncmp(cmd, "\\exit", sizeof("\\exit") - 1) == 0) {
        shutdown(companion_fd_, SHUT_RDWR);
        is_alive_ = false;
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

int MasterConversation::GetFD() {
    return companion_fd_;
}

bool MasterConversation::IsEnded() {
    return !is_alive_;
}

MasterConversation::~MasterConversation() {
    close(companion_fd_);
    close(socket_fd_);
}