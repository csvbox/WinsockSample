#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <cstdio>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

// set APPVER=5.02 for WinXP SP2 and later

int wmain(int argc, wchar_t **argv)
{
    //-----------------------------------------
    // Declare and initialize variables
    WSADATA wsaData;
    int iResult;
    INT iRetval;
    DWORD dwRetval;
    int i = 1;
    ADDRINFOW *result = NULL;
    ADDRINFOW *ptr = NULL;
    ADDRINFOW hints;
    LPSOCKADDR sockaddr_ip;
    wchar_t ipstringbuffer[256];
    DWORD ipbufferlength = 256;

    // Validate the parameters
    if (argc != 3) {
        wprintf(L"usage: %ws <hostname> <port/servicename>\n", argv[0]);
        wprintf(L"example usage:\n");
        wprintf(L"\t%ws www.example.com 0\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wprintf(L"WSAStartup failed: %d\n", iResult);
        return 1;
    }

    //--------------------------------
    // Setup the hints address info structure
    // which is passed to the getaddrinfo() function
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    wprintf(L"About to lookup (");
    wprintf(L"hostname = %ws, ", argv[1]);
    wprintf(L"servname (or port) = %ws\n\n", argv[2]);

    //--------------------------------
    // Call GetAddrinfoW(). If the call succeeds,
    // the result variable will hold a linked list
    // of addrinfow structures containing response
    // information
    dwRetval = GetAddrInfoW(argv[1], argv[2], &hints, &result);
    if (dwRetval != 0) {
        wprintf(L"GetAddrInfoW failed with error: %d\n", dwRetval);
        WSACleanup();
        return 1;
    }
    wprintf(L"GetAddrInfoW returned success\n");

    // Retrieve each address and print out the hex bytes
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        wprintf(L"GetAddrInfoW response %d\n", i++);
        wprintf(L"\tFlags: 0x%x\n", ptr->ai_flags);
        wprintf(L"\tFamily: ");
        switch (ptr->ai_family) {
        case AF_UNSPEC:
            wprintf(L"Unspecified\n");
            break;
        case AF_INET:
            wprintf(L"AF_INET (IPv4) address found\n");
            sockaddr_ip = (LPSOCKADDR)ptr->ai_addr;
            // The buffer length is changed by each call to WSAAddresstoString
            // So we need to set it for each iteration through the loop for safety
            ipbufferlength = 256;
            iRetval = WSAAddressToString(sockaddr_ip, (DWORD)ptr->ai_addrlen, NULL,
                ipstringbuffer, &ipbufferlength);
            if (iRetval)
                wprintf(L"WSAAddressToString failed with %u\n", WSAGetLastError());
            else
                wprintf(L"\tIPv4 address %ws\n", ipstringbuffer);
            break;
        case AF_INET6:
            wprintf(L"AF_INET6 (IPv6) address found\n");
            // the InetNtop function is available on Windows Vista and later
            // sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
            // printf("\tIPv6 address %s\n",
            //    InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 256) );

            // We use WSAAddressToString since it is supported on Windows XP and later
            sockaddr_ip = (LPSOCKADDR)ptr->ai_addr;
            // The buffer length is changed by each call to WSAAddresstoString
            // So we need to set it for each iteration through the loop for safety
            ipbufferlength = 256;
            iRetval = WSAAddressToString(sockaddr_ip, (DWORD)ptr->ai_addrlen, NULL,
                ipstringbuffer, &ipbufferlength);
            if (iRetval)
                wprintf(L"WSAAddressToString failed with %u\n", WSAGetLastError());
            else
                wprintf(L"\tIPv6 address %ws\n", ipstringbuffer);
            break;
        default:
            wprintf(L"Other %ld\n", ptr->ai_family);
            break;
        }
        wprintf(L"\tSocket type: ");
        switch (ptr->ai_socktype) {
        case 0:
            wprintf(L"Unspecified\n");
            break;
        case SOCK_STREAM:
            wprintf(L"SOCK_STREAM (stream)\n");
            break;
        case SOCK_DGRAM:
            wprintf(L"SOCK_DGRAM (datagram) \n");
            break;
        case SOCK_RAW:
            wprintf(L"SOCK_RAW (raw) \n");
            break;
        case SOCK_RDM:
            wprintf(L"SOCK_RDM (reliable message datagram)\n");
            break;
        case SOCK_SEQPACKET:
            wprintf(L"SOCK_SEQPACKET (pseudo-stream packet)\n");
            break;
        default:
            wprintf(L"Other %ld\n", ptr->ai_socktype);
            break;
        }
        wprintf(L"\tProtocol: ");
        switch (ptr->ai_protocol) {
        case 0:
            wprintf(L"Unspecified\n");
            break;
        case IPPROTO_TCP:
            wprintf(L"IPPROTO_TCP (TCP)\n");
            break;
        case IPPROTO_UDP:
            wprintf(L"IPPROTO_UDP (UDP) \n");
            break;
        default:
            wprintf(L"Other %ld\n", ptr->ai_protocol);
            break;
        }
        wprintf(L"\tLength of this sockaddr: %zd\n", ptr->ai_addrlen);
        wprintf(L"\tCanonical name: %s\n", ptr->ai_canonname);
    }

    FreeAddrInfoW(result);
    WSACleanup();

    return 0;
}
