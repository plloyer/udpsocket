#include "UDPSocket.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <winsock2.h>
#include <windows.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <netdb.h>
#include <sys/socket.h>
#endif

void UDPSocket::Startup()
{
#ifdef _WIN32
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed to initialize Winsock. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
#endif
}

void UDPSocket::Cleanup()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

UDPSocket::UDPSocket()
	: mSocket(INVALID_SOCKET)
{
}

UDPSocket::~UDPSocket()
{
	closesocket(mSocket);
}

bool UDPSocket::open(int port)
{
	if ((mSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		printf("socket created\n");
		
	// TODO: Having multiple NIC, verify which is available externally and use this one as the source sin_addr

	struct sockaddr_in myaddr;
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(port);

	if (bind(mSocket, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return false;
	}

	// Set socket to non blocking when calling recvfrom
	u_long iMode=1;
	ioctlsocket(mSocket, FIONBIO, &iMode);

	return true;
}

bool UDPSocket::send(const char* address, int port, char* data, int length)
{
	struct sockaddr_in si_other;

	memset((char *)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(port);

	if (inet_pton(AF_INET, address, &si_other.sin_addr) == 0)
	{
		perror("Unable to convert address from string to binary.");
		return false;
	}

	//now reply the client with the same data
	if (sendto(mSocket, data, length, 0, (struct sockaddr*) &si_other, sizeof(si_other)) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		return false;
	}

	return true;
}

int UDPSocket::receive(char* data, int length)
{
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);

	//try to receive some data, this is a blocking call
	int recv_len = 0;
	if ((recv_len = recvfrom(mSocket, data, length, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			printf("recvfrom() failed with error code : %d\n", WSAGetLastError());
		}

		return 0;
	}

	if (recv_len > 0)
	{
		// Print info about the received data as a string and show from where it came
		char address[128];
		inet_ntop(AF_INET, &si_other.sin_addr, address, 128);
		printf("Received packet from %s:%d\n", address, ntohs(si_other.sin_port));
		printf("Data: %s\n", data);
	}

	return recv_len;
}