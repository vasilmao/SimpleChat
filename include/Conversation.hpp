#ifndef CONVERSATION_INCLIDE
#define CONVERSATION_INCLIDE

#include <netinet/in.h>
#include <cstdint>

class Conversation {
  public:
    virtual void CheckStdin() = 0;
    virtual void CheckInput() = 0;
    virtual ~Conversation()   = 0;
    virtual bool IsEnded()    = 0;
    virtual int GetFD()       = 0;
};

void MakeNonBlocking(int fd);


class MasterConversation : public Conversation {
  public:
    MasterConversation(in_port_t port);
    void CheckStdin() override;
    void CheckInput() override;
    ~MasterConversation() override;
    bool IsEnded() override;
    int GetFD() override;

  private:
    void ParseCommand(char* buffer, size_t cmd_len);

  private:
    bool is_alive_ = true;
    int socket_fd_ = -1;
    int companion_fd_ = -1;
};

class ClientConversation : public Conversation {
  public:
    ClientConversation(in_addr_t ip, in_port_t port);
    void CheckStdin() override;
    void CheckInput() override;
    ~ClientConversation() override;
    bool IsEnded() override;
    int GetFD() override;

  private:
    void ParseCommand(char* buffer, size_t cmd_len);

  private:
    bool is_alive_ = true;
    int socket_fd_ = -1;
};

#endif // CONVERSATION_INCLIDE