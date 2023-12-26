#include "Accept.hpp"
#include "Kqueue.hpp"

Accept::Accept(const Server &server) : AEvent(server)
{
}

Accept::~Accept()
{
}

void Accept::handle()
{
    struct kevent newEvent;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    int clientSocket = accept(mServer.getSocket(), (struct sockaddr *)&clientAddr, &clientAddrSize);

    // EV_SET(&newEvent, clientSocket, EVFILT_READ, EV_ADD, 0, 0, new Read());

    Kqueue::addEvent(newEvent);
}