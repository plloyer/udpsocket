#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <winsock2.h>
#include <windows.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define socketerrno WSAGetLastError()
#define SOCKET_EWOULDBLOCK WSAEWOULDBLOCK
typedef int socktlen;
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR   (-1)

#include <errno.h>
#define socketerrno errno
#define SOCKET_EWOULDBLOCK EWOULDBLOCK
typedef socklen_t socktlen;
#endif