#include "StdAfx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>


class Socket {
public:
    Socket() : errorCause(0) {}
    ~Socket() { closesocket(socket_); }
    bool Create(UINT Port, LPCSTR zDottedAddr);
    bool Connect(LPCSTR strAddr, UINT hostPort);
    int Receive(char *buf, int size);
private:
    SOCKET socket_;
    char *errorCause;
};

#define SET_CAUSE(x)  case x: errorCause = #x; break;

int Socket::Receive(char *buf, int size) {
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
        return -1;
    }
    return res;
}

bool Socket::Create(UINT portNum, LPCSTR strAdr){

   socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   return socket_ != INVALID_SOCKET;
}

bool Socket::Connect(LPCSTR strAddr, UINT hostPort) {
    DWORD addr = strAddr ? inet_addr(strAddr) : INADDR_ANY;

    SOCKADDR_IN hostAddr;
    hostAddr.sin_family      = (short)AF_INET;
    hostAddr.sin_port        = htons((short)hostPort);
    hostAddr.sin_addr.s_addr = addr;

    int res = connect(socket_, (SOCKADDR *) &addr, sizeof(hostAddr));
    if (res == SOCKET_ERROR) {
        printf("error found:  %d\n", WSAGetLastError());
    }
    return res != SOCKET_ERROR;
}

/// This is a simple sample code that downloads the Google page.
/// TODO: redo this to make it modular

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "80"
#define URL          "www.google.com"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPTSTR lpCmdLine, int nCmdShow)
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    char *sendbuf = "GET /index.html HTTP/1.0\n\rFrom: someuser@jmarshall.com\n\rUser-Agent: HTTPTool/1.0\n\r\n\r";
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN-1;
    
    // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(URL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("Error at socket(): %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Send an initial buffer
    iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

    FILE *f;
    errno_t err = fopen_s(&f, "c:\\co\\g.html", "w");
    if (err != 0) return -1;

    int bytesRead = 0;
    // Receive until the peer closes the connection
    do {
        bytesRead = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if ( bytesRead > 0 ) {
            printf("\nBytes received: %d\n", bytesRead);
            recvbuf[bytesRead] = '\0';
            fprintf(f, "%s", recvbuf);
        } else if ( bytesRead == 0 )
            printf("\nConnection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());

    } while( bytesRead > 0 );

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}


int oldMain()
{
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != NO_ERROR) {
      printf("WSAStartup failed: %d\n", iResult);
      return 1;
    }

    Socket s;
    s.Create(80, "64.233.169.103");
    bool res = s.Connect("64.233.169.103", 80);
    if (!res) return 1;

    char buf[100];
    s.Receive(buf, 100);
}

