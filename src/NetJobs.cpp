#include "NetJobs.hpp"

#include "Conversation.hpp"

#include <sys/socket.h>
#include <cstdint>
#include <sys/epoll.h>
#include <cassert>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdio>


const int EVENTS_MAX = 10;

const size_t BUFFER_SIZE = 1024;

void ConversationCycle(Conversation* conversation) {
    int waiting_fd = conversation->GetFD();

    int queue_fd = epoll_create(EVENTS_MAX); // EVENTS_MAX is ignored actually because in newer versions memory is allocated dynamically

    struct epoll_event ep_event_new_msg = {.events = EPOLLIN};
    ep_event_new_msg.data.fd = waiting_fd;
    assert(epoll_ctl(queue_fd, EPOLL_CTL_ADD, waiting_fd, &ep_event_new_msg) == 0);

    MakeNonBlocking(STDIN_FILENO);
    struct epoll_event ep_event_out_msg = {.events = EPOLLIN};
    ep_event_out_msg.data.fd = STDIN_FILENO;
    assert(epoll_ctl(queue_fd, EPOLL_CTL_ADD, STDIN_FILENO, &ep_event_out_msg) == 0);

    struct epoll_event events[EVENTS_MAX];
    memset(events, 0, EVENTS_MAX * sizeof(struct epoll_event));

    bool running = true;
    while (running) {
        printf("waiting for events...\n");
        int events_cnt = epoll_wait(queue_fd, events, EVENTS_MAX, -1);
        printf("got events: %d\n", events_cnt);
        for (int i = 0; i < events_cnt; ++i) {
            printf("event number %d\n", i);
            int cur_fd = events[i].data.fd;
            if (cur_fd == STDIN_FILENO) {
                conversation->CheckStdin();
            } else {
                printf("its input!\n");
                conversation->CheckInput();
            }
        }
        printf("events ended\n");
    }

}