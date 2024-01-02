#include "AEvent.hpp"

AEvent::AEvent(const Server &server) : mServer(server)
{
}

AEvent::AEvent(const Server &server, int clientSocket) : mServer(server), mClientSocket(clientSocket)
{
}

AEvent::~AEvent()
{
}