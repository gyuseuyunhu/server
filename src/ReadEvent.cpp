#include "ReadEvent.hpp"
#include <unistd.h>

ReadEvent::ReadEvent(const Server &server, int clientSocket)
    : AEvent(server), mClientSocket(clientSocket), mRequestLine(E_START_LINE)
{
}

ReadEvent::~ReadEvent()
{
}

int ReadEvent::handle()
{
    int n = read(mClientSocket, mBuffer, BUFFER_SIZE);
    if (n < 0) // 읽기를 실패 -> client에 500번대를 응답
    {
        return EVENT_FINISH;
    }
    else if (n == 0)
    {
        if (true) // content-length가 다 들어온 경우
        {
            // response를 여기서 파싱
            return EVENT_FINISH;
        }
        else // 타이머 걸어서 요청이 더 있는지 검증이 필요한지?
        {
            return EVENT_CONTINUE;
        }
    }
    else
    {
        mStringBuffer.append(mBuffer, n);
        try
        {
            mRequest.parse(mStringBuffer, mRequestLine);
        }
        catch (int status)
        {
            // response도 여기서 파싱
            // kevent에서 read event를 제외하고 write 이벤트를 넣어줌
            return EVENT_FINISH;
        }
    }
    return EVENT_CONTINUE;
}
