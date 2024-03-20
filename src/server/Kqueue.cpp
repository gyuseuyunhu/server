#include "Kqueue.hpp"
#include <unistd.h>

#ifdef __linux__

int Kqueue::mEpollFd = 0;
struct epoll_event Kqueue::mEvents[MAX_EVENT_CNT];
std::vector<struct epoll_event> Kqueue::mNewEvents;

void Kqueue::init()
{
    if (mEpollFd == 0)
    {
        mEpollFd = epoll_create1(0);
        if (mEpollFd == -1)
        {
            throw std::runtime_error("Error Kqueue::init(): ");
        }
    }
}

void Kqueue::addEvent(AEvent *event, int filter)
{
    struct epoll_event newEvent;
    if (filter == EVFILT_TIMER)
    {
        return;
    }
    newEvent.events = filter;
    newEvent.data.ptr = event;
    if (epoll_ctl(mEpollFd, EPOLL_CTL_ADD, event->getFd(), &newEvent) < 0)
    {
        perror("epoll_ctl");
    }
}

void Kqueue::deleteEvent(AEvent *ptr, int filter)
{
    struct epoll_event event;
    if (filter == EVFILT_TIMER)
    {
        return;
    }
    event.events = filter;
    event.data.ptr = ptr;
    int n = epoll_ctl(mEpollFd, EPOLL_CTL_DEL, ptr->getFd(), &event);
    if (n == -1)
    {
        throw std::runtime_error("Error Kqueue::deleteEvent(): epoll에 이벤트 삭제 실패");
    }
}

void Kqueue::closeKq()
{
    close(mEpollFd);
}

void Kqueue::handleEvents()
{
    int n = epoll_wait(mEpollFd, mEvents, MAX_EVENT_CNT, -1);
    for (int i = 0; i < n; ++i)
    {
        reinterpret_cast<AEvent *>(mEvents[i].data.ptr)->handle();
    }
}

#elif defined(__APPLE__) || defined(__FreeBSD__)

std::vector<struct kevent> Kqueue::mNewEvents;
int Kqueue::mKq = 0;
struct kevent Kqueue::mHandleEvents[MAX_EVENT_CNT];

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

void Kqueue::addEvent(AEvent *event, int filter)
{
    struct kevent newEvent;
    if (filter == EVFILT_TIMER)
    {
        EV_SET(&newEvent, event->getFd(), filter, EV_ADD, NOTE_SECONDS, TIMEOUT_SECONDS, event);
    }
    else
    {
        EV_SET(&newEvent, event->getFd(), filter, EV_ADD, 0, 0, event);
    }
    mNewEvents.push_back(newEvent);
}

void Kqueue::deleteEvent(AEvent *ptr, int filter)
{
    struct kevent event;
    EV_SET(&event, ptr->getFd(), filter, EV_DELETE, 0, 0, NULL);
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

bool Kqueue::checkSameIdent(int n, int idx)
{
    for (int i = 0; i < n; ++i)
    {
        if (i == idx)
        {
            continue;
        }
        if (mHandleEvents[idx].ident == mHandleEvents[i].ident)
        {
            return true;
        }
    }
    return false;
}

void Kqueue::handleEvents()
{
    int n = kevent(mKq, &mNewEvents[0], mNewEvents.size(), mHandleEvents, MAX_EVENT_CNT, NULL);
    if (!mNewEvents.empty())
    {
        mNewEvents.clear();
    }
    for (int i = 0; i < n; ++i)
    {
        if (mHandleEvents[i].filter == EVFILT_TIMER)
        {
            if (checkSameIdent(n, i))
            {
                continue;
            }
            reinterpret_cast<AEvent *>(mHandleEvents[i].udata)->timer();
        }
        else
        {
            reinterpret_cast<AEvent *>(mHandleEvents[i].udata)->handle();
        }
    }
}
#endif