#ifndef READREQUESTEVENT_HPP
#define READREQUESTEVENT_HPP

#include "AEvent.hpp"

class ReadRequestEvent : public AEvent
{
  private:
    std::string mStringBuffer;
    std::string mFilePrefix;
    int mFileSize;
    int getErrorPageFd(const LocationBlock &lb, int status);
    int getIndexFd(const LocationBlock &lb, int &stauts);
    int getFileFd(const LocationBlock &lb, int &status);
    int getRequestFd(int &status);
    void setFilePrefix(const LocationBlock &lb);

  public:
    ReadRequestEvent(const Server &server, int clientSocket);
    virtual ~ReadRequestEvent();
    virtual int handle();
    void makeResponseEvent(int &status);
    void makeReadFileEvent(int fd, int &status);
};

#endif