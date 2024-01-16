#ifndef KQUEUE_HPP
#define KQUEUE_HPP

#include "AEvent.hpp"
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
    static void addEvent(const struct kevent &event);
    static void handleEvents();
    static void pushAcceptEvent(); // 뺄지 넣을지 고민중

    static bool checkSameIdent(int n, int idx);
    static void deleteEvent(int fd, int filter);
    static void closeKq();
};

#endif