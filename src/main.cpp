#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <thread>

#include "UDPSocket.h"

#define BUFLEN 2048
#define MSGS 5	/* number of messages to send */
#define SERVER_PORT 31000
#define CLIENT_PORT 31001

volatile int gServerPacketReceived = 0;
bool done = false;

void runServer()
{
	UDPSocket udp;
	udp.open(SERVER_PORT);

	const int sBufferSize = 2048;
	char data[sBufferSize];
	memset(data, '\0', sBufferSize);

	while (!done) {
		int length = udp.receive(data, sBufferSize);
		if (length > 0)
		{
			gServerPacketReceived++;
			memset(data, '\0', sBufferSize);
		}
	}
}

void runClient()
{
	UDPSocket udp;
	udp.open(CLIENT_PORT);


	udp.send("127.0.0.1", SERVER_PORT, "Test1!", 7);
	udp.send("127.0.0.1", SERVER_PORT, "Test2!", 7);
	udp.send("127.0.0.1", SERVER_PORT, "Test3!", 7);
}

int main(int argc, char* argv[])
{
	UDPSocket::Startup();

	if (argc != 2)
	{
		printf("Need command line argument 'client', 'server' or 'unittest'");
	}

	if (strcmp("server", argv[1]) == 0)
	{
		runServer();
	}
	else if(strcmp("client", argv[1]) == 0)
	{
		runClient();
	}
	else if (strcmp("unittest", argv[1]) == 0)
	{
		printf("Starting server.\n");
		std::thread server(runServer);
		printf("Starting client.\n");
		std::thread client(runClient);

		printf("Waiting for client to be done.\n");
		client.join();
		printf("Client Done.\n");
		while (gServerPacketReceived < 3); //printf("Received: %i\n", gServerPacketReceived);
		printf("Received 3 packets on server.\n");
		done = true;
		server.join();

		printf("Server received all packets from client sucessfully.\n");
	}
	else
	{
		perror("Invalid command line argument");
		return -1;
	}
	
	UDPSocket::Cleanup();

	return 0;
}
