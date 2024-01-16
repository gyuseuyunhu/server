#include "AEvent.hpp"
#include <unistd.h>

AEvent::AEvent(const Server &server) : mServer(server)
{
}

AEvent::AEvent(const Server &server, int clientSocket) : mServer(server), mClientSocket(clientSocket)
{
}

AEvent::AEvent(const Server &server, const Response &response, int clientSocket)
    : mServer(server), mResponse(response), mClientSocket(clientSocket)
{
}

AEvent::~AEvent()
{
}

void AEvent::timer()
{
}