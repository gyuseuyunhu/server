#ifndef READCGIEVENT_HPP
#define READCGIEVENT_HPP
#include "AEvent.hpp"

class ReadCgiEvent : public AEvent
{
  private:
    int mSocket;
    std::string mStringBuffer;
    char mBuffer[BUFFER_SIZE];

  public:
    ReadCgiEvent(const Server &server, int clientSocket, int socket);
    virtual void handle();
};

#endif