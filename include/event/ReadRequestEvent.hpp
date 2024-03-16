#ifndef READREQUESTEVENT_HPP
#define READREQUESTEVENT_HPP

#include "AEvent.hpp"

class ReadRequestEvent : public AEvent
{
  private:
    enum eCgiPipe
    {
        SERVER_READ = 0,
        SERVER_WRITE = 3,
        CGI_READ = 2,
        CGI_WRITE = 1,
    };
    Request mRequest;
    std::string mStringBuffer;
    std::string mFilePrefix;
    int mFileSize;
    std::string mMimeType;
    std::string mRedirectionPath;
    int getErrorPageFd(const LocationBlock &lb, int status);
    int getIndexFd(const LocationBlock &lb, int &stauts);
    int getFileFd(int &status);
    int getRequestFd(const LocationBlock &lb, int &status);
    void setFilePrefix(const LocationBlock &lb);
    std::string getFileExtension(const std::string &path);
    void makeWriteEvent(int &status);
    void makeReadFileEvent(int fd);

    void makeResponse(const LocationBlock &lb, int &status);
    int checkRequestError(const LocationBlock &lb);
    int checkStartLine(const LocationBlock &lb);
    int checkReturnDirective(const LocationBlock &lb);
    int checkBody(const LocationBlock &lb);

    bool checkCgiEvent(const LocationBlock &lb, int &status);

    void makeCgiEvent(const LocationBlock &lb);

  public:
    ReadRequestEvent(const Server &server, int clientSocket);
    virtual ~ReadRequestEvent();
    virtual void handle();
    virtual void timer();
    virtual int getFd() const;
};

#endif