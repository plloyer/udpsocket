#pragma once

#ifdef _WIN32
#include <winsock2.h>
typedef SOCKET Socket;
#else
typedef int Socket;
#endif

class Address;

class UDPSocket
{
public:
	static void Startup();
	static void Cleanup();

	UDPSocket();
	~UDPSocket();

	bool Open(int port);
	bool Send(const Address& address, const char* data, int length);
	int Receive(Address& address, char* data, int length);

	int GetBoundPort() const { return m_port; }

private:
	Socket m_socket;
	int m_port;
};