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
    static struct kevent mHandleEvents[8];        // kqueue에서 받아온 처리해줘야 하는 이벤트

  public:
    static void init();
    static void addEvent(const struct kevent &event);
    static void handleEvents();
    static void pushAcceptEvent(); // 뺄지 넣을지 고민중
};

#endif