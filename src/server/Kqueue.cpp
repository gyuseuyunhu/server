#include "Kqueue.hpp"
#include <unistd.h>

std::vector<struct kevent> Kqueue::mNewEvents;
int Kqueue::mKq = 0;
struct kevent Kqueue::mHandleEvents[8];

void Kqueue::init()
{
    if (mKq == 0)
    {
        mKq = kqueue();
        if (mKq == -1)
        {
            throw std::runtime_error("Error Kqueue::init(): ");
        }
    }
}

void Kqueue::addEvent(const struct kevent &event)
{
    mNewEvents.push_back(event);
}

void Kqueue::handleEvents()
{
    int n = kevent(mKq, &mNewEvents[0], mNewEvents.size(), mHandleEvents, 8, NULL);
    if (!mNewEvents.empty())
    {
        mNewEvents.clear();
    }
    for (int i = 0; i < n; ++i)
    {
        reinterpret_cast<AEvent *>(mHandleEvents[i].udata)->handle();
    }
}

void Kqueue::pushAcceptEvent()
{
    int n = kevent(mKq, &mNewEvents[0], mNewEvents.size(), NULL, 0, NULL);
    if (n == -1)
    {
        throw std::runtime_error("Error Kqueue::pushAcceptEvent(): kqueue에 accept 이벤트 넣기 실패");
    }
    mNewEvents.clear();
}

void Kqueue::deleteEvent(int fd, int filter)
{
    struct kevent event;
    EV_SET(&event, fd, filter, EV_DELETE, 0, 0, NULL);
    int n = kevent(mKq, &event, 1, NULL, 0, NULL);
    if (n == -1)
    {
        throw std::runtime_error("Error Kqueue::deleteEvent(): kqueue에 이벤트 삭제 실패");
    }
}

void Kqueue::closeKq()
{
    close(mKq);
}