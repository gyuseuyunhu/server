#include "Kqueue.hpp"

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
    AEvent *event;
    int n = kevent(mKq, &mNewEvents[0], mNewEvents.size(), mHandleEvents, 8, NULL);
    for (int i = 0; i < n; ++i)
    {
        event = reinterpret_cast<AEvent *>(mHandleEvents[i].udata);
        if (event->handle() == EVENT_FINISH)
        {
            EV_SET(&mHandleEvents[i], mHandleEvents[i].ident, mHandleEvents[i].filter, EV_DELETE, 0, 0, NULL);
            addEvent(mHandleEvents[i]);
            delete event;
        }
    }
    if (!mNewEvents.empty())
    {
        mNewEvents.clear();
    }
}

void Kqueue::pushAcceptEvent()
{
    int n = kevent(mKq, &mNewEvents[0], mNewEvents.size(), NULL, 0, NULL);
    if (n == -1)
    {
        perror("에러");
        throw std::runtime_error("Error Kqueue::pushAcceptEvent(): kqueue에 accept 이벤트 넣기 실패");
    }
    mNewEvents.clear();
}