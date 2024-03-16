#ifndef READFILEEVENT_HPP
#define READFILEEVENT_HPP

#include "AEvent.hpp"

class ReadFileEvent : public AEvent
{
  private:
    const int mFileFd;
    char mBuffer[BUFFER_SIZE];
    int mFileSize;
    int mReadSize;
    std::string mBody;

  public:
    ReadFileEvent(const Server &server, const Response &response, int clientSocket, int fileFd, int fileSize);
    virtual ~ReadFileEvent();
    virtual void handle();
    virtual void timer();
    virtual int getFd() const;
};

#endif