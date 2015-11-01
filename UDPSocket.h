#pragma once

class UDPSocket
{
public:
	static void Startup();
	static void Cleanup();

	UDPSocket();
	~UDPSocket();

	bool open(int port);
	bool send(const char* address, int port, char* data, int length);
	bool receive();

private:
	int mSocket;
};