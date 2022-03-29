#include "Conversation.hpp"
#include <fcntl.h>
#include <cassert>
#include <cstdio>

Conversation::~Conversation() {}

void MakeNonBlocking(int fd) {
    int32_t flags = fcntl(fd, F_GETFD);
    assert(fcntl(fd, F_SETFD, flags | O_NONBLOCK) == 0);
    printf("%d now non-blocking!\n", fd);
}