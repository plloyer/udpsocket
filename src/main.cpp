#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <thread>

#include "Address.h"
#include "BitStream.h"
#include "NetInterface.h"

#define BUFLEN 2048
#define MSGS 5	/* number of messages to Send */
#define SERVER_PORT 31000
#define CLIENT_PORT 31001

const char* serverIP = "127.0.0.1";
volatile int gServerPacketReceived = 0;
bool done = false;

enum UserPacketType
{
	PacketType_String = Packet_ProtocolLast,
};

void runServer()
{
	NetInterface netInterface;
	netInterface.Open(SERVER_PORT);

	const int sBufferSize = 2048;
	char data[sBufferSize];
	memset(data, '\0', sBufferSize);

	while (!done) 
	{
		Address senderAddress;
		BitStream stream;
		PacketType type = netInterface.Receive(senderAddress, stream);
		if (type != Packet_None)
		{
			gServerPacketReceived++;

			stream.ReadData((uint8_t*)data, sizeof(data));

			memset(data, '\0', sBufferSize);
		}
	}
}

void runClient()
{
	NetInterface netInterface;
	netInterface.Open(CLIENT_PORT);

	Address serverAddress(serverIP, SERVER_PORT);

	BitStream stream;
	stream.WriteByte(PacketType_String);
	stream.WriteData((uint8_t*)"Test1!", 7);
	netInterface.Send(serverAddress, stream);

	BitStream stream2;
	stream2.WriteByte(PacketType_String);
	stream2.WriteData((uint8_t*)"Test2!", 7);
	netInterface.Send(serverAddress, stream2);

	BitStream stream3;
	stream3.WriteByte(PacketType_String);
	stream3.WriteData((uint8_t*)"Test3!", 7);
	netInterface.Send(serverAddress, stream3);
}

int main(int argc, char* argv[])
{
	NetInterface::Startup();

	if (argc < 2)
	{
		printf("Need command line argument 'client', 'server' or 'unittest'");
		return 1;
	}

	if (strcmp("server", argv[1]) == 0)
	{
		runServer();
	}
	else if(strcmp("client", argv[1]) == 0)
	{
		if (argc == 3)
			serverIP = argv[2];
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
	
	NetInterface::Cleanup();

	return 0;
}
