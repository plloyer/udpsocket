#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "UDPSocket.h"

#define BUFLEN 2048
#define MSGS 5	/* number of messages to send */
#define SERVER_PORT 31000
#define CLIENT_PORT 31001

void runServer()
{
	UDPSocket udp;
	udp.open(SERVER_PORT);

	while (1) {
		udp.receive();
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
		printf("Need command line argument 'client' or 'server'");
	}

	if (strcmp("server", argv[1]) == 0)
	{
		runServer();
	}
	else
	{
		runClient();
	}
	
	UDPSocket::Cleanup();

	return 0;
}
