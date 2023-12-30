#include "AcceptEvent.hpp"
#include "Kqueue.hpp"
#include "ReadEvent.hpp"
#include <fcntl.h>

AcceptEvent::AcceptEvent(const Server &server) : AEvent(server)
{
}

AcceptEvent::~AcceptEvent()
{
}

int AcceptEvent::handle()
{
    struct kevent newEvent;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    int clientSocket = accept(mServer.getSocket(), (struct sockaddr *)&clientAddr, &clientAddrSize);
    if (clientSocket == -1)
    {
        return EVENT_CONTINUE;
    }

    fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
    EV_SET(&newEvent, clientSocket, EVFILT_READ, EV_ADD, 0, 0, new ReadEvent(mServer, clientSocket));
    Kqueue::addEvent(newEvent);

    return EVENT_CONTINUE;
}