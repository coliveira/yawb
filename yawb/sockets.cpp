// (c) 2009 by Carlos Oliveira

#include "StdAfx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>


class Socket {
public:
    Socket();
    ~Socket();
    bool Connect(const char *serverName, const char *hostPort);
    int Receive(char *buf, int size);
    bool Send(const char *sendbuf);

private:
    static bool Initialize();
    static bool initialized;
    bool TryConnection(addrinfo *addr);
    bool CreateSocket(addrinfo *addr);
    bool GetAddressInfo(const char *serverName, const char *hostPort, addrinfo* &result);
    SOCKET socket_;
    char *errorCause;
};

bool Socket::initialized = false;

Socket::Socket() 
    : socket_(INVALID_SOCKET)
    , errorCause(0) 
{
    Initialize();
}


Socket::~Socket() 
{
    if (INVALID_SOCKET != socket_) closesocket(socket_); 
}

bool Socket::Initialize()
{
    if (!initialized) {
        initialized = true;
        WSADATA wsaData;
        if (0 != WSAStartup(MAKEWORD(2,2), &wsaData)) return false;
    }
    return true;
}

#define SET_CAUSE(x)  case x: errorCause = #x; break;

int Socket::Receive(char *buf, int size) 
{
    int res = recv(socket_, buf, size, 0);
    if (res == SOCKET_ERROR) {
        switch (res) {
            SET_CAUSE(WSANOTINITIALISED)
            SET_CAUSE(WSAENETDOWN)
            SET_CAUSE(WSAEFAULT)
            SET_CAUSE(WSAENOTCONN)
            SET_CAUSE(WSAEINTR)
            SET_CAUSE(WSAEINPROGRESS)
            SET_CAUSE(WSAENETRESET)
            SET_CAUSE(WSAENOTSOCK)
            SET_CAUSE(WSAEOPNOTSUPP)
            SET_CAUSE(WSAESHUTDOWN)
            SET_CAUSE(WSAEWOULDBLOCK)
            SET_CAUSE(WSAEMSGSIZE)
            SET_CAUSE(WSAEINVAL)
            SET_CAUSE(WSAECONNABORTED)
            SET_CAUSE(WSAETIMEDOUT)
            SET_CAUSE(WSAECONNRESET)
            default: errorCause = "unknown";
        }
    }
    return res;
}

bool Socket::Send(const char *sendbuf)
{
    // Send an initial buffer
    int iResult = send(socket_, sendbuf, (int)strlen(sendbuf), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(socket_);
        WSACleanup();
        return false;
    }

    printf("Bytes Sent: %ld\n", iResult);
    return true;
}

bool Socket::TryConnection(addrinfo *addr)
{
    if (SOCKET_ERROR == connect(socket_, addr->ai_addr, (int)addr->ai_addrlen)) {
        closesocket(socket_);
        socket_ = INVALID_SOCKET;
        return false;
    }
    return true;
}

bool Socket::CreateSocket(addrinfo *addr)
{
    socket_ = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (socket_ == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        return false;
    }
    return true;
}

bool Socket::GetAddressInfo(const char *serverName, const char *hostPort, addrinfo* &result)
{
    addrinfo hints;
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    int iResult = getaddrinfo(serverName, hostPort, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed: %d\n", iResult);
        return false;
    }
    return true;
}

bool Socket::Connect(const char *serverName, const char *hostPort) 
{
    addrinfo *result;
    if (!GetAddressInfo(serverName, hostPort, result))
        return false;

    // Attempt to connect to an address until one succeeds
    for(addrinfo *ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
        if (!CreateSocket(ptr)) break;
        if (TryConnection(ptr)) break;
    }
    freeaddrinfo(result);

    if (socket_ == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        return false;
    }
    return true;
}

#define DEFAULT_BUFLEN 512

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                       LPTSTR lpCmdLine, int nCmdShow)
{
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != NO_ERROR) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    Socket s;
    bool res = s.Connect("www.google.com", "80");
    if (!res) return 1;

    char *sendbuf = "GET /index.html HTTP/1.0\n\rFrom: someuser@jmarshall.com\n\rUser-Agent: HTTPTool/1.0\n\r\n\r";
    s.Send(sendbuf);

    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN-1;

    FILE *f;
    errno_t err = fopen_s(&f, "c:\\co\\g.html", "w");
    if (err != 0) return -1;


    int bytesRead = 0;
    do {
        bytesRead = s.Receive(recvbuf, recvbuflen);
        if ( bytesRead > 0 ) {
            printf("\nBytes received: %d\n", bytesRead);
            recvbuf[bytesRead] = '\0';
            fprintf(f, "%s", recvbuf);
        } else if ( bytesRead == 0 )
            printf("\nConnection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());

    } while( bytesRead > 0 );

    WSACleanup();

    return 0;
}
