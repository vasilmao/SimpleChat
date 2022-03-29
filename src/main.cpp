#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "NetJobs.hpp"
#include "Conversation.hpp"

int main(int argc, const char* argv[]) {
    in_port_t port = htons(1234);
    in_addr_t ip = inet_addr("127.0.0.1");
    bool is_master = false;
    for (int i = 1; i < argc; ++i) {
        if (strncmp(argv[i], "-h", 2) == 0) {
            i++;
            ip = inet_addr(argv[i]);
            continue;
        }
        if (strncmp(argv[i], "-p", 2) == 0) {
            i++;
            port = htons(atoi(argv[i]));
            continue;
        }
        if (strcmp(argv[i], "--master") == 0) {
            is_master = true;
        }
    }


    Conversation* conversation = nullptr;

    if (is_master) {
        conversation = new MasterConversation(port);
    } else {
        conversation = new ClientConversation(ip, port);
    }

    ConversationCycle(conversation);

}