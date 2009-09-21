#include "StdAfx.h"
#include <winsock.h>

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

   socket_ = socket(AF_INET, SOCK_STREAM, 0);
   if (socket_ != INVALID_SOCKET)
       return false;

   SOCKADDR_IN add;
   add.sin_family      = (short)AF_INET;
   add.sin_port        =  ::htons((short)portNum);
   add.sin_addr.s_addr = (strAdr ? ::inet_addr(strAdr) : INADDR_ANY);

   return bind(socket_, (SOCKADDR *) &add,sizeof(add)) != SOCKET_ERROR;
}

bool Socket::Connect(LPCSTR strAddr, UINT hostPort) {
    DWORD addr = strAddr ? inet_addr(strAddr) : INADDR_ANY;

    SOCKADDR_IN hostAddr;
    hostAddr.sin_family      = (short)AF_INET;
    hostAddr.sin_port        = htons((short)hostPort);
    hostAddr.sin_addr.s_addr = addr;

    return connect(socket_, (SOCKADDR *) &addr, sizeof(hostAddr)) != SOCKET_ERROR;
}
