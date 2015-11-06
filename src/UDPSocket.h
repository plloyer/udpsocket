#pragma once

#ifdef _WIN32
typedef unsigned __int64 Socket;
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

private:
	Socket m_socket;
};