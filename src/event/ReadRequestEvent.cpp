#include "ReadRequestEvent.hpp"
#include "HttpStatusInfos.hpp"
#include "Kqueue.hpp"
#include "ReadCgiEvent.hpp"
#include "ReadFileEvent.hpp"
#include "WriteCgiEvent.hpp"
#include "WriteEvent.hpp"
#include "util.hpp"
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

ReadRequestEvent::ReadRequestEvent(const Server &server, int clientSocket)
    : AEvent(server, clientSocket), mRequest(), mFileSize(0)
{
}

ReadRequestEvent::~ReadRequestEvent()
{
}

std::string ReadRequestEvent::getFileExtension(const std::string &path)
{
    size_t lastSlashPos = path.find_last_of('/');

    std::string fileName;
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
        return fileName.substr(lastDotPos);
    }
    return "";
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
            if (fd != -1)
            {
                mFileSize = fileInfo.st_size;
                mResponse.addHead("Content-type", HttpStatusInfos::getMimeType(getFileExtension(filePath)));
                return fd;
            }
            else if (status == OK)
            {
                status = FORBIDDEN;
            }
        }
        else if (status == OK)
        {
            status = NOT_FOUND;
        }
    }
    // 반복문을 돌았는데 열리는 index 파일이 없을 시 autoindex 지시어가 있는 경우 파일 목록을 응답
    if (status == NOT_FOUND && lb.isAutoIndex() == true)
    {
        status = OK;
    }

    return -1;
}

int ReadRequestEvent::getErrorPageFd(const LocationBlock &lb, int status)
{
    struct stat fileInfo;
    const std::map<int, std::string> &errorPages = lb.getErrorPages();
    std::map<int, std::string>::const_iterator it = errorPages.find(status);
    if (it == errorPages.end())
    {
        return -1;
    }

    std::string errorPagePath = HttpStatusInfos::getWebservRoot() + lb.getRoot() + it->second;

    if (stat(errorPagePath.c_str(), &fileInfo) == 0)
    {
        if (S_ISREG(fileInfo.st_mode))
        {
            int fd = nonBlockOpen(errorPagePath.c_str(), O_RDONLY);
            if (fd == -1)
            {
                return -1;
            }
            mFileSize = fileInfo.st_size;
            mResponse.addHead("Content-type", HttpStatusInfos::getMimeType(getFileExtension(errorPagePath)));
            return fd;
        }
    }
    return -1;
}

int ReadRequestEvent::getFileFd(int &status)
{
    struct stat fileInfo;
    std::string filePath = mFilePrefix;

    if (stat(filePath.c_str(), &fileInfo) == 0)
    {
        if (S_ISREG(fileInfo.st_mode))
        {
            int fd = nonBlockOpen(filePath.c_str(), O_RDONLY);
            if (fd != -1)
            {
                mFileSize = fileInfo.st_size;
                mResponse.addHead("Content-type", HttpStatusInfos::getMimeType(getFileExtension(filePath)));
                return fd;
            }
            status = FORBIDDEN;
            return -1;
        }
        else if (S_ISDIR(fileInfo.st_mode))
        {
            status = MOVED_PERMANENTLY;
            return -1;
        }
    }
    status = NOT_FOUND;
    return -1;
}

void ReadRequestEvent::setFilePrefix(const LocationBlock &lb)
{
    std::string requestPath = mRequest.getPath();
    requestPath.replace(0, lb.getLocationPath().length(), lb.getRoot());

    // // 환경변수로 저장된 웹서브 root를 요청된 path 앞에 삽입
    mFilePrefix = HttpStatusInfos::getWebservRoot() + requestPath;
}

int ReadRequestEvent::getRequestFd(const LocationBlock &lb, int &status)
{
    const std::string &requestPath = mRequest.getPath();
    setFilePrefix(lb);

    int fd;
    if (status == OK)
    {
        // 요청이 폴더로 들어온 경우
        if (requestPath[requestPath.size() - 1] == '/')
        {
            fd = getIndexFd(lb, status);
        }
        // 요청이 파일로 들어온 경우
        else
        {
            fd = getFileFd(status);
        }
    }
    // 위에서 status가 바뀔수 있음
    if (status != OK)
    {
        return getErrorPageFd(lb, status);
    }
    return fd;
}

void ReadRequestEvent::makeWriteEvent(int &status)
{
    struct kevent newEvent;
    std::string responseBody;
    if (status != OK)
    {
        responseBody = HttpStatusInfos::getHttpErrorPage(status);
    }
    else
    {
        responseBody = HttpStatusInfos::makeAutoIndexPage(mFilePrefix);
    }

    mResponse.addHead("Content-length", responseBody.size());
    if (mRequest.getConnectionStatus() == CONNECTION_CLOSE)
    {
        mResponse.setConnectionClose();
    }
    assert(responseBody.size() != 0);
    mResponse.setBody(responseBody);
    EV_SET(&newEvent, mClientSocket, EVFILT_WRITE, EV_ADD, 0, 0, new WriteEvent(mServer, mResponse, mClientSocket));
    Kqueue::addEvent(newEvent);
}

void ReadRequestEvent::makeReadFileEvent(int fd)
{
    struct kevent newEvent;

    mResponse.addHead("Content-length", mFileSize);
    if (mRequest.getConnectionStatus() == CONNECTION_CLOSE)
    {
        mResponse.setConnectionClose();
    }
    EV_SET(&newEvent, mClientSocket, EVFILT_WRITE, EV_ADD, 0, 0,
           new ReadFileEvent(mServer, mResponse, mClientSocket, fd, mFileSize));
    Kqueue::addEvent(newEvent);
}

void ReadRequestEvent::makeResponse(const LocationBlock &lb, int &status)
{
    int fd = getRequestFd(lb, status);

    mResponse.setStartLine(status);
    if (status == MOVED_PERMANENTLY)
    {
        mResponse.addHead("location", mRequest.getPath() + "/");
    }
    else if (status == TEMPORARY_REDIRECT)
    {
        mResponse.addHead("location", mRedirectionPath);
    }

    if (fd == -1)
    {
        makeWriteEvent(status);
    }
    else
    {
        makeReadFileEvent(fd);
    }
}

int ReadRequestEvent::checkRequestError(const LocationBlock &lb)
{
    int status;

    if ((status = checkStartLine(lb)) != OK)
    {
        return status;
    }
    else if ((status = checkReturnDirective(lb)) != OK)
    {
        return status;
    }
    else if ((status = checkBody(lb)) != OK)
    {
        return status;
    }
    return OK;
}

int ReadRequestEvent::checkStartLine(const LocationBlock &lb)
{
    const std::string &method = mRequest.getMethod();
    if (method == "GET" || method == "POST" || method == "DELETE")
    {
        if ((method == "GET" && lb.isAllowedGet() == false) || (method == "POST" && lb.isAllowedPost() == false) ||
            (method == "DELETE" && lb.isAllowedDelete() == false))
        {
            return NOT_ALLOWED;
        }
        return OK;
    }
    else if (method == "HEAD" || method == "PUT")
    {
        return NOT_ALLOWED;
    }
    else
    {
        return NOT_IMPLEMENT;
    }
}

int ReadRequestEvent::checkReturnDirective(const LocationBlock &lb)
{
    if (!lb.getRedirectionPath().empty())
    {
        mRedirectionPath = lb.getRedirectionPath();
        if (mRedirectionPath[0] == '/')
        {
            mRedirectionPath = "http://" + mRequest.getHost() + mRedirectionPath;
        }
        return TEMPORARY_REDIRECT;
    }
    return OK;
}

int ReadRequestEvent::checkBody(const LocationBlock &lb)
{
    if (mRequest.getBody().size() > lb.getClientMaxBodySize())
    {
        return CONTENT_TOO_LARGE;
    }
    return OK;
}

bool ReadRequestEvent::checkCgiEvent(const LocationBlock &lb, int &status)
{
    const std::string &cgiExtension = lb.getCgiExtension();
    if (cgiExtension.empty())
    {
        return false;
    }

    const std::string &requestExtension = getFileExtension(mRequest.getPath());
    if (cgiExtension != requestExtension)
    {
        return false;
    }
    const std::string &cgiPath = lb.getCgiPath();
    if (cgiPath.empty())
    {
        status = NOT_FOUND;
        return false;
    }
    const std::string &cgiFullPath = HttpStatusInfos::getWebservRoot() + cgiPath;
    if (access(cgiFullPath.c_str(), F_OK) == -1)
    {
        status = NOT_FOUND;
        return false;
    }

    if (access(cgiFullPath.c_str(), X_OK) == -1)
    {
        status = FORBIDDEN;
        return false;
    }

    return true;
}

void ReadRequestEvent::makeCgiEvent(const std::string &lbCgiPath)
{
    int fd[4];
    pipe(fd);
    pipe(fd + 2);

    int pid = fork();
    if (pid == 0)
    {
        close(fd[SERVER_READ]);
        close(fd[SERVER_WRITE]);
        fcntl(fd[CGI_READ], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
        fcntl(fd[CGI_WRITE], F_SETFL, O_NONBLOCK, FD_CLOEXEC);

        if (dup2(fd[CGI_READ], STDIN_FILENO) == -1)
        {
            perror("dup2");
        }
        if (dup2(fd[CGI_WRITE], STDOUT_FILENO) == -1)
        {
            perror("dup22");
        }
        close(fd[2]);
        close(fd[1]);

        const std::string &cgiPath = HttpStatusInfos::getWebservRoot() + lbCgiPath;
        const char *cmd[2] = {cgiPath.c_str(), NULL};
        if (execve(cgiPath.c_str(), const_cast<char *const *>(cmd), mRequest.getCgiEnvp()) == -1)
        {
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        close(fd[CGI_READ]);
        close(fd[CGI_WRITE]);
        fcntl(fd[SERVER_READ], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
        fcntl(fd[SERVER_WRITE], F_SETFL, O_NONBLOCK, FD_CLOEXEC);

        struct kevent newEvent;
        EV_SET(&newEvent, fd[SERVER_WRITE], EVFILT_WRITE, EV_ADD, 0, 0,
               new WriteCgiEvent(mServer, mClientSocket, fd[SERVER_WRITE], mRequest.getBody()));
        Kqueue::addEvent(newEvent);
        EV_SET(&newEvent, fd[SERVER_READ], EVFILT_READ, EV_ADD, 0, 0,
               new ReadCgiEvent(mServer, mClientSocket, fd[SERVER_READ]));
        Kqueue::addEvent(newEvent);
    }
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
    if (mRequest.tryParse(mStringBuffer) == false)
    {
        return;
    }
    int status = mRequest.getStatus();
    const LocationBlock &lb = mServer.getLocationBlockForRequest(mRequest.getHost(), mRequest.getPath());
    if (status != BAD_REQUEST)
    {
        status = checkRequestError(lb);
    }

    if (status == OK && checkCgiEvent(lb, status))
    {
        makeCgiEvent(lb.getCgiPath());
        Kqueue::deleteEvent(mClientSocket, EVFILT_READ);
        delete this;
        return;
    }

    makeResponse(lb, status);
    Kqueue::deleteEvent(mClientSocket, EVFILT_READ);
    delete this;
}