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
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    int clientSocket = accept(mServer.getSocket(), (struct sockaddr *)&clientAddr, &clientAddrSize);
    if (clientSocket == -1)
    {
        return;
    }

    fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);

    AEvent *event = new ReadRequestEvent(mServer, clientSocket);
    Kqueue::addEvent(event, EVFILT_READ);
    Kqueue::addEvent(event, EVFILT_TIMER);
}

int AcceptEvent::getFd() const
{
    return mServer.getSocket();
}