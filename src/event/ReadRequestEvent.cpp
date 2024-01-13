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

void ReadRequestEvent::addMimeTypeHeader(const std::string &path)
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
        mResponse.addHead("Content-type", HttpStatusInfos::getMimeType(fileExtension));
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
                addMimeTypeHeader(filePath);
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
    }

    return NOT_FOUND;
}

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
            addMimeTypeHeader(errorPagePath);
            return fd;
        }
    }
    return NOT_FOUND;
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
            if (fd != NOT_FOUND)
            {
                mFileSize = fileInfo.st_size;
                addMimeTypeHeader(filePath);
                return fd;
            }
            status = 403; // 권한없음
            return NOT_FOUND;
        }
        else if (S_ISDIR(fileInfo.st_mode))
        {
            status = 301;
            return NOT_FOUND;
        }
    }
    status = 404;
    return NOT_FOUND;
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
    std::string requestPath = mRequest.getPath();
    const LocationBlock &lb = mServer.getLocationBlockForRequest(mRequest.getHost(), requestPath);
    setFilePrefix(lb);

    int fd;
    if (status == 200)
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
    if (status != 200)
    {
        return getErrorPageFd(lb, status);
    }
    return fd;
}

void ReadRequestEvent::makeWriteEvent(int &status)
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

void ReadRequestEvent::makeResponse(int &status)
{
    int fd = getRequestFd(status);
    mResponse.setStartLine(status);
    if (status == 301)
    {
        mResponse.addHead("location", mRequest.getPath() + "/");
    }
    else if (status == 307)
    {
        mResponse.addHead("location", mRedirectionPath); // todo
    }

    if (fd == NOT_FOUND)
    {
        makeWriteEvent(status);
    }
    else
    {
        makeReadFileEvent(fd);
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

    std::string pathExtension;
    std::string path = mRequest.getPath();
    size_t lastSlashPos = path.find_last_of('/');

    std::string fileName;
    std::string fileExtension;
    if (lastSlashPos != std::string::npos)
    {
        fileName = path.substr(lastSlashPos + 1);
    }
    size_t lastDotPos = fileName.find_last_of('.');

    if (lastDotPos != std::string::npos)
    {
        pathExtension = fileName.substr(lastDotPos);
    }
    // cgi 처리 필요
    // if (!pathExtension.empty())
    //     std::cout << lb.getCgiExtension() << "$  " << pathExtension << "  " << status << std::endl;
    if (!lb.getCgiExtension().empty() && status == 200 && lb.getCgiExtension() == pathExtension)
    {
        int fd[4];
        pipe(fd);     // 0번(부모의 읽기), 1번(자식의 쓰기)
        pipe(fd + 2); // 2번(자식의 읽기), 3번(부모의 쓰기)

        int pid = fork();
        if (pid == 0)
        {
            close(fd[0]);
            close(fd[3]);
            fcntl(fd[2], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
            fcntl(fd[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC);

            std::string cgiFullPath = HttpStatusInfos::getWebservRoot() + lb.getCgiPath();
            char *cgiPath = new char[cgiFullPath.length() + 1];
            strcpy(cgiPath, cgiFullPath.c_str());
            char *cmd[2];
            cmd[0] = cgiPath;
            cmd[1] = NULL;
            if (dup2(fd[2], STDIN_FILENO) == -1) // 자식은 2번에서 읽는다
            {
                perror("dup2");
            }
            if (dup2(fd[1], STDOUT_FILENO) == -1) // 자식은 1번에 쓴다
            {
                perror("dup22");
            }
            close(fd[2]);
            close(fd[1]);
            if (execve(cgiPath, cmd, mRequest.getCgiEnvp()) == -1)
            {
                exit(EXIT_SUCCESS);
            }
            std::cout << "execve error" << std::endl;
            exit(EXIT_FAILURE);
        }
        else
        {
            // 부모 프로세스
            close(fd[1]);
            close(fd[2]);
            fcntl(fd[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
            fcntl(fd[3], F_SETFL, O_NONBLOCK, FD_CLOEXEC);

            struct kevent newEvent;

            EV_SET(&newEvent, fd[3], EVFILT_WRITE, EV_ADD, 0, 0,
                   new WriteCgiEvent(mServer, mClientSocket, fd[3], mRequest.getBody()));
            Kqueue::addEvent(newEvent);
            EV_SET(&newEvent, fd[0], EVFILT_READ, EV_ADD, 0, 0, new ReadCgiEvent(mServer, mClientSocket, fd[0]));
            Kqueue::addEvent(newEvent);
            Kqueue::deleteEvent(mClientSocket, EVFILT_READ);
            delete this;
            return;
        }
    }
    makeResponse(status);
    Kqueue::deleteEvent(mClientSocket, EVFILT_READ);
    delete this;
}

int ReadRequestEvent::checkRequestError(const LocationBlock &lb)
{
    int status;

    if ((status = checkRequestStartLine(lb)) != OK)
    {
        return status;
    }
    else if ((status = checkRequestHeader(lb)) != OK)
    {
        return status;
    }
    else if ((status = checkRequestBody(lb)) != OK)
    {
        return status;
    }
    return OK;
}

int ReadRequestEvent::checkRequestStartLine(const LocationBlock &lb)
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
    else if (method == "HEAD")
    {
        return NOT_ALLOWED;
    }
    else
    {
        return NOT_IMPLEMENT;
    }
}

int ReadRequestEvent::checkRequestHeader(const LocationBlock &lb)
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

int ReadRequestEvent::checkRequestBody(const LocationBlock &lb)
{
    // todo cmb0일때?
    if (mRequest.getBody().size() > lb.getClientMaxBodySize())
    {
        return CONTENT_TOO_LARGE;
    }
    return OK;
}
