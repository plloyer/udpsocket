#pragma once

#ifdef _WIN32
typedef __int64 Socket;
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

	bool open(int port);
	bool send(const Address& address, const char* data, int length);
	int receive(Address& address, char* data, int length);

private:
	Socket mSocket;
};