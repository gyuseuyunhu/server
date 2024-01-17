#ifndef AEVENT_HPP
#define AEVENT_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"

class AEvent
{
  protected:
    enum
    {
        BUFFER_SIZE = 65536,
        TIMEOUT_SECONDS = 30,
    };
    const Server &mServer;
    Response mResponse;
    int mClientSocket;

  public:
    AEvent(const Server &server);
    AEvent(const Server &server, int clientSocket);
    AEvent(const Server &server, const Response &response, int clientSocket);
    virtual ~AEvent();
    virtual void handle() = 0;
    virtual void timer();
};

#endif