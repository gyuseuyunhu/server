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
    int n = kevent(mKq, &mNewEvents[0], mNewEvents.size(), mHandleEvents, 8, NULL);
    for (int i = 0; i < n; ++i)
    {
        reinterpret_cast<AEvent *>(mHandleEvents[i].udata)->handle();
    }
}
