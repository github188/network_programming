//
// Created by tw on 2018/2/28.
//
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "Practical.h"

int main(int argc, char *argv[])
{
    if (argc !=2)
        DieWithUserMessage("Parameter(s)", "<Server Port/Service>");
    char *service = argv[1];

    struct addrinfo addrCriteria;
    memset(&addrCriteria, 0, sizeof(addrCriteria));
    addrCriteria.ai_family = AF_UNSPEC;
    addrCriteria.ai_flags = AI_PASSIVE;
    addrCriteria.ai_socktype = SOCK_DGRAM;
    addrCriteria.ai_protocol = IPPROTO_UDP;

    struct addrinfo *servAddr;
    int rtnVal = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
    if (rtnVal != 0)
        DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));

    int sock = socket(servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol);
    if (sock < 0)
        DieWithSystemMessage("socket() failed");

    //Bind to the local address
    if (bind(sock, servAddr->ai_addr, servAddr->ai_addrlen) < 0)
        DieWithSystemMessage("bind() failed");

    //Free address list allocated by getaddrinfo()
    freeaddrinfo(servAddr);

    for (;;)
    {
        struct sockaddr_storage clntAddr;
        socklen_t clntAddrLen = sizeof(clntAddr);

        char buffer[1024];
        ssize_t numBytesRcvd = recvfrom(sock, buffer, 1024, 0, (
        struct sockaddr *)&clntAddr, &clntAddrLen);
        if (numBytesRcvd < 0)
            DieWithSystemMessage("recvfrom() failed");
        fputs("Handing client ",stdout);
        PrintSocketAddress((struct sockaddr *)&clntAddr, stdout);
        fputc('\n', stdout);

        ssize_t numBytesSent = sendto(sock, buffer, numBytesRcvd, 0, (
        struct sockaddr *)&clntAddr, sizeof(clntAddr));
        if (numBytesSent < 0)
            DieWithSystemMessage("sendto() failed");
        else if (numBytesSent != numBytesRcvd)
            DieWithUserMessage("sendto()", "sent unexpected number of bytes");

    }
}
