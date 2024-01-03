#ifndef READFILEEVENT_HPP
#define READFILEEVENT_HPP

#include "AEvent.hpp"

class ReadFileEvent : public AEvent
{
  private:
    const int mFileFd;
    static const int BUFFER_SIZE = 1024; // todo 테스트를 통해 적절한 값 찾기
    char mBuffer[BUFFER_SIZE];
    int mFileSize;
    int mReadSize;
    int mHttpStatusCode;
    std::string mBody;

  public:
    ReadFileEvent(const Server &server, int clientSocket, int fileFd, int fileSize, int mHttpStatusCode);
    virtual ~ReadFileEvent();
    virtual int handle();
};

#endif