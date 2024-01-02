#include "ReadRequestEvent.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

ReadRequestEvent::ReadRequestEvent(const Server &server, int clientSocket) : AEvent(server, mClientSocket)
{
}

ReadRequestEvent::~ReadRequestEvent()
{
}

void ReadRequestEvent::makeReadFileEvent()
{
    std::map<std::string, std::string> header = mRequest.getHeaders();
    const std::string &path = mRequest.getPath();
    bool isFolder;

    const std::vector<std::string> filePaths = mServer.getFilePath(header["Host"], path, isFolder);
    int size = filePaths.size();

    struct stat fileInfo;
    for (int i = 0; i < size; ++i)
    {
        if (stat(filePaths[i].c_str(), &fileInfo) == 0) // 파일이 존재 // 권한 체크 문제는 어떻게 처리할지 테스트 필요
        {
            if (S_ISREG(fileInfo.st_mode)) // 파일이면
            {
                int fd = open(filePaths[i].c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
            }
            else if (S_ISDIR(fileInfo.st_mode)) // 디렉토리이면
            {
                ; // 디렉토리 로직 추가
            }
        }
    }

    mRequest.clear();
}

int ReadRequestEvent::handle()
{
    char buffer[BUFFER_SIZE];
    int n = read(mClientSocket, buffer, BUFFER_SIZE);
    if (n < 0)
    {
        // 읽기를 실패 -> client에 500번대를 응답
        return EVENT_FINISH;
    }
    else if (n == 0)
    {
        // 디스커넥트 소켓 로직 필요
        return EVENT_FINISH;
    }
    mStringBuffer.append(buffer, n);
    mRequest.parse(mStringBuffer);
    int status = mRequest.getStatus();
    if (status >= 400)
    {
        return EVENT_FINISH;
    }
    else if (status >= 200)
    {
        makeReadFileEvent();
    }
    return EVENT_CONTINUE;
}
