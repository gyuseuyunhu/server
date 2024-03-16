#ifndef READCGIEVENT_HPP
#define READCGIEVENT_HPP
#include "AEvent.hpp"

class ReadCgiEvent : public AEvent
{
  private:
    int mFd;
    bool mIsError;
    std::string mStringBuffer;
    char mBuffer[BUFFER_SIZE];

  public:
    ReadCgiEvent(const Server &server, int clientSocket, int fd);
    bool setResponse(const std::string &line);
    virtual void handle();
    virtual void timer();
    virtual int getFd() const;
};

#endif