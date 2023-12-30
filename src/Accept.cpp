#include "Accept.hpp"
#include "Kqueue.hpp"
#include "Read.hpp"
#include <fcntl.h>

Accept::Accept(const Server &server) : AEvent(server)
{
}

Accept::~Accept()
{
}

int Accept::handle()
{
    // struct kevent newEvent;
    // struct sockaddr_in clientAddr;
    // socklen_t clientAddrSize = sizeof(clientAddr);
    //  int clientSocket = accept(mServer.getSocket(), (struct sockaddr *)&clientAddr, &clientAddrSize);

    fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
    EV_SET(&newEvent, clientSocket, EVFILT_READ, EV_ADD, 0, 0, new Read(mServer, clientSocket));
    Kqueue::addEvent(newEvent);

    return EVENT_CONTINUE;
}