#ifndef KQUEUE_HPP
#define KQUEUE_HPP

#include "AEvent.hpp"
#define TIMEOUT_SECONDS 30

#ifdef __linux__
#include <sys/epoll.h>

#define MAX_EVENT_CNT 8
#define EVFILT_READ EPOLLIN
#define EVFILT_WRITE EPOLLOUT
#define EVFILT_TIMER EPOLLONESHOT

class Kqueue
{
  private:
    static int mEpollFd;
    static struct epoll_event mEvents[MAX_EVENT_CNT];
    static std::vector<struct epoll_event> mNewEvents;

  public:
    static void init();
    static void addEvent(AEvent *event, int filter);
    static void handleEvents();
    static void deleteEvent(AEvent *ptr, int filter);
    static void closeKq();
};

#elif defined(__APPLE__) || defined(__FreeBSD__)
#include <sys/event.h>
#include <vector>

class Kqueue
{
  private:
    static int mKq;
    static std::vector<struct kevent> mNewEvents; // kqueue에 등록시켜줄 이벤트
    static const int MAX_EVENT_CNT = 8;
    static struct kevent mHandleEvents[MAX_EVENT_CNT]; // kqueue에서 받아온 처리해줘야 하는 이벤트

  public:
    static void init();
    static void addEvent(AEvent *event, int filter);
    static void handleEvents();

    static bool checkSameIdent(int n, int idx);
    static void deleteEvent(AEvent *ptr, int filter);
    static void closeKq();
};

#endif

#endif