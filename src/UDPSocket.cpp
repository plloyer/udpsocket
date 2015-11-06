#include "UDPSocket.h"
#include "Address.h"
#include "SocketIncludes.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void UDPSocket::Startup()
{
#ifdef _WIN32
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed to initialize Winsock. Error Code : %d", socketerrno);
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
	: m_socket(INVALID_SOCKET)
{
}

UDPSocket::~UDPSocket()
{
#ifdef _WIN32
	closesocket(m_socket);
#else
	close(m_socket);
#endif
}

bool UDPSocket::Open(int port)
{
	if ((m_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		printf("socket created\n");
		
	// TODO: Having multiple NIC, verify which is available externally and use this one as the source sin_addr

	struct sockaddr_in myaddr;
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(port);

	if (bind(m_socket, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		printf("bind socket failed with error code: %d\n", socketerrno);
		return false;
	}

	// Set socket to non blocking when calling recvfrom
	u_long iMode=1;
#ifdef _WIN32
	ioctlsocket(m_socket, FIONBIO, &iMode);
#else
	ioctl(m_socket, FIONBIO, &iMode);
#endif

	return true;
}

bool UDPSocket::Send(const Address& address, const char* data, int length)
{
	struct sockaddr_in si_other;
	address.GetSocketAddress(si_other);

	//now reply the client with the same data
	if (sendto(m_socket, data, length, 0, (struct sockaddr*) &si_other, sizeof(si_other)) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", socketerrno);
		return false;
	}

	return true;
}

int UDPSocket::Receive(Address& senderAddress, char* data, int length)
{
	struct sockaddr_in si_other;
	socktlen slen = sizeof(si_other);

	//try to Receive some data, this is a blocking call
	int recv_len = 0;
	if ((recv_len = recvfrom(m_socket, data, length, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
	{
		if (socketerrno != SOCKET_EWOULDBLOCK)
		{
			printf("recvfrom() failed with error code : %d\n", socketerrno);
		}

		return 0;
	}

	if (recv_len > 0)
	{
		senderAddress.FromSocketAddress(si_other);
		printf("Received packet from %s, Data: %s\n", senderAddress.ToString().c_str(), data);
	}

	return recv_len;
}