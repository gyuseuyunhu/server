#include "AcceptEvent.hpp"
#include "Kqueue.hpp"
#include "ReadRequestEvent.hpp"
#include <fcntl.h>

AcceptEvent::AcceptEvent(const Server &server) : AEvent(server)
{
}

AcceptEvent::~AcceptEvent()
{
}

void AcceptEvent::handle()
{
    struct kevent newEvent;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    int clientSocket = accept(mServer.getSocket(), (struct sockaddr *)&clientAddr, &clientAddrSize);
    if (clientSocket == -1)
    {
        return;
    }

    fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
    EV_SET(&newEvent, clientSocket, EVFILT_READ, EV_ADD, 0, 0, new ReadRequestEvent(mServer, clientSocket));
    Kqueue::addEvent(newEvent);
}