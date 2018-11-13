#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdio>
#include <cstdlib>

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char **argv)
{
    //-----------------------------------------
    // Declare and initialize variables
    WSADATA wsaData{ 0 };
    int iResult{ 0 };

    struct sockaddr_in saGNI;
    char hostname[NI_MAXHOST];
    char servInfo[NI_MAXSERV];


    // Validate the parameters
    if (argc != 3) {
        printf("usage: %s <IPv4 address> <port>\n", argv[0]);
        printf("example usage:\n");
        printf("\t%s 127.0.0.1 8443\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    //-----------------------------------------
    // Set up sockaddr_in structure which is passed
    // to the getnameinfo function
    saGNI.sin_family = AF_INET;
    u_short port = std::atoi(argv[2]);
    saGNI.sin_port = htons(port);

    if (inet_pton(AF_INET, argv[1], &(saGNI.sin_addr)) == 0)
    {
        printf("not a valid IP\n");
        return 1;
    }
    else if (inet_pton(AF_INET, argv[1], &(saGNI.sin_addr)) == -1)
    {
        printf("inet_pton failed with error # %ld\n", WSAGetLastError());
        return 1;
    }

    //-----------------------------------------
    // Call getnameinfo
    DWORD dwRetval = getnameinfo((struct sockaddr *) &saGNI,
        sizeof(struct sockaddr),
        hostname,
        NI_MAXHOST, servInfo, NI_MAXSERV, NI_NUMERICSERV);

    if (dwRetval != 0)
    {
        printf("getnameinfo failed with error # %ld\n", WSAGetLastError());
        return 1;
    }
    else
    {
        printf("getnameinfo returned hostname = %s\n", hostname);
        return 0;
    }
}
