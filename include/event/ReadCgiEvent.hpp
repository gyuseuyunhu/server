#ifndef READCGIEVENT_HPP
#define READCGIEVENT_HPP
#include "AEvent.hpp"

class ReadCgiEvent : public AEvent
{
  private:
    int mSocket;
		bool mIsError;
    std::string mStringBuffer;
    char mBuffer[BUFFER_SIZE];

  public:
    ReadCgiEvent(const Server &server, int clientSocket, int socket);
    bool setReponse(const std::string &line);
    virtual void handle();
};

#endif