#pragma once

#ifdef _WIN32
typedef __int64 Socket;
#else
typedef int Socket;
#endif

class UDPSocket
{
public:
	static void Startup();
	static void Cleanup();

	UDPSocket();
	~UDPSocket();

	bool open(int port);
	bool send(const char* address, int port, const char* data, int length);
	int receive(char* data, int length);

private:
	Socket mSocket;
};