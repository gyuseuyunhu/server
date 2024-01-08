#include "ReadRequestEvent.hpp"
#include "HttpStatusInfos.hpp"
#include "Kqueue.hpp"
#include "ReadFileEvent.hpp"
#include "WriteEvent.hpp"
#include "util.hpp"
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

ReadRequestEvent::ReadRequestEvent(const Server &server, int clientSocket)
    : AEvent(server, clientSocket), mFileSize(0), mMimeType("text/html")
{
}

ReadRequestEvent::~ReadRequestEvent()
{
}

void ReadRequestEvent::setMimeType(const std::string &path)
{
    size_t lastSlashPos = path.find_last_of('/');

    std::string fileName;
    std::string fileExtension;
    if (lastSlashPos != std::string::npos)
    {
        fileName = path.substr(lastSlashPos + 1);
    }
    else
    {
        assert(false);
    }
    size_t lastDotPos = fileName.find_last_of('.');

    if (lastDotPos != std::string::npos)
    {
        fileExtension = fileName.substr(lastDotPos + 1);
        mMimeType = HttpStatusInfos::getMimeType(fileExtension);
        return;
    }
}

int ReadRequestEvent::getIndexFd(const LocationBlock &lb, int &status)
{
    const std::vector<std::string> &indexs = lb.getIndexs();
    std::vector<std::string>::const_iterator indexIt = indexs.begin();
    std::string filePath;
    struct stat fileInfo;

    for (; indexIt != indexs.end(); ++indexIt)
    {
        filePath = mFilePrefix + *indexIt;
        if (stat(filePath.c_str(), &fileInfo) == 0)
        {
            int fd = nonBlockOpen(filePath.c_str(), O_RDONLY);
            if (fd != NOT_FOUND)
            {
                mFileSize = fileInfo.st_size;
                setMimeType(filePath);
                return fd;
            }
            else if (status == 200)
            {
                status = 403; // 권한없음
            }
        }
        else if (status == 200)
        {
            status = 404;
        }
    }
    // 반복문을 돌았는데 열리는 index 파일이 없을 시 autoindex 지시어가 있는 경우 파일 목록을 응답
    if (lb.isAutoIndex() == true)
    {
        status = 200;
        return NOT_FOUND;
    }

    return getErrorPageFd(lb, status);
}
// todo 고민중 매개변수를 (const std::map<int, std::string> &errorPages, const std::string &root, int status)
// 로 변경할지
int ReadRequestEvent::getErrorPageFd(const LocationBlock &lb, int status)
{
    struct stat fileInfo;
    const std::map<int, std::string> &errorPages = lb.getErrorPages();
    std::map<int, std::string>::const_iterator it = errorPages.find(status);
    if (it == errorPages.end())
    {
        return NOT_FOUND;
    }

    std::string errorPagePath = HttpStatusInfos::getWebservRoot() + lb.getRoot() + it->second;
    // todo debug 용
    std::cout << "errorPagePath : " << errorPagePath << std::endl;
    if (stat(errorPagePath.c_str(), &fileInfo) == 0)
    {
        if (S_ISREG(fileInfo.st_mode))
        {
            int fd = nonBlockOpen(errorPagePath.c_str(), O_RDONLY);
            if (fd == NOT_FOUND)
            {
                return NOT_FOUND;
            }
            mFileSize = fileInfo.st_size;
            setMimeType(errorPagePath);
            return fd;
        }
    }
    return NOT_FOUND;
}

int ReadRequestEvent::getFileFd(const LocationBlock &lb, int &status)
{
    struct stat fileInfo;
    std::string filePath = mFilePrefix;

    if (stat(filePath.c_str(), &fileInfo) == 0)
    {
        if (S_ISREG(fileInfo.st_mode))
        {
            int fd = nonBlockOpen(filePath.c_str(), O_RDONLY);
            if (fd != NOT_FOUND)
            {
                mFileSize = fileInfo.st_size;
                setMimeType(filePath);
                return fd;
            }
            status = 403; // 권한없음
            return getErrorPageFd(lb, status);
        }
        else if (S_ISDIR(fileInfo.st_mode))
        {
            status = 301;
            return NOT_FOUND;
        }
    }
    status = 404;
    return getErrorPageFd(lb, status);
}

void ReadRequestEvent::setFilePrefix(const LocationBlock &lb)
{
    std::string requestPath = mRequest.getPath();
    requestPath.replace(0, lb.getLocationPath().length(), lb.getRoot());

    // // 환경변수로 저장된 웹서브 root를 요청된 path 앞에 삽입
    mFilePrefix = HttpStatusInfos::getWebservRoot() + requestPath;
}

int ReadRequestEvent::getRequestFd(int &status)
{
    if (status >= 400)
    {
        return getErrorPageFd(LocationBlock(mServer.getServerInfos()[0].getServerBlock()), status);
    }

    assert(status == 200);
    std::string requestPath = mRequest.getPath();
    const LocationBlock lb = mServer.getLocationBlockForRequest(mRequest.getHost(), requestPath);
    setFilePrefix(lb);
    // 요청이 폴더로 들어온 경우
    if (requestPath[requestPath.size() - 1] == '/')
    {
        return getIndexFd(lb, status);
    }
    // 요정이 파일로 들어온 경우
    return getFileFd(lb, status);
}

void ReadRequestEvent::makeResponseEvent(int &status)
{
    struct kevent newEvent;
    std::string responseBody;
    if (status != 200)
    {
        responseBody = HttpStatusInfos::getHttpErrorPage(status);
    }
    else
    {
        responseBody = HttpStatusInfos::makeAutoIndexPage(mFilePrefix);
        // todo test
        std::cout << "auto : " << mFilePrefix << std::endl;
    }
    mResponse.init(status, responseBody.length());

    if (status == 301)
    {
        std::ostringstream oss;
        oss << "http://" << mRequest.getHost() << mRequest.getPath() << "/";
        mResponse.addHead("Location", oss.str());
    }
    assert(mMimeType.size() != 0);
    mResponse.addHead("Content-Type", mMimeType);
    assert(responseBody.size() != 0);
    mResponse.setBody(responseBody);
    EV_SET(&newEvent, mClientSocket, EVFILT_WRITE, EV_ADD, 0, 0, new WriteEvent(mServer, mResponse, mClientSocket));
    Kqueue::addEvent(newEvent);
}

void ReadRequestEvent::makeReadFileEvent(int fd, int &status)
{
    struct kevent newEvent;
    // todo filesize가 맞는지 확인 필요
    mResponse.init(status, mFileSize);
    mResponse.addHead("Content-Type", mMimeType);
    EV_SET(&newEvent, mClientSocket, EVFILT_WRITE, EV_ADD, 0, 0,
           new ReadFileEvent(mServer, mResponse, mClientSocket, fd, mFileSize));
    Kqueue::addEvent(newEvent);
}

void ReadRequestEvent::handle()
{
    char buffer[BUFFER_SIZE];
    int n = read(mClientSocket, buffer, BUFFER_SIZE);
    if (n <= 0)
    {
        close(mClientSocket);
        delete this;
        return;
    }

    mStringBuffer.append(buffer, n);
    mRequest.parse(mStringBuffer);
    int status = mRequest.getStatus();
    if (status >= 200)
    {
        int fd = getRequestFd(status);
        if (fd == NOT_FOUND)
        {
            makeResponseEvent(status);
        }
        else
        {
            makeReadFileEvent(fd, status);
        }
        Kqueue::deleteEvent(mClientSocket, EVFILT_READ);
        delete this;
    }
}
