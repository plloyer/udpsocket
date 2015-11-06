#include "NetInterface.h"

#include "Address.h"
#include "BitStream.h"
#include "Connection.h"
#include "UDPSocket.h"


void NetInterface::Startup()
{
	UDPSocket::Startup();
}

void NetInterface::Cleanup()
{
	UDPSocket::Cleanup();
}

NetInterface::NetInterface(int bindPort)
	: m_socket(new UDPSocket())
{
	m_socket->Open(bindPort);
}

NetInterface::~NetInterface()
{

}

void NetInterface::Send(const Address& destination, const BitStream& data)
{
	m_socket->Send(destination, reinterpret_cast<const char*>(data.GetStream()), data.GetStreamLength());
}

PacketType NetInterface::Receive(Address& senderAddress, BitStream& stream)
{
	static char buffer[2048];
	int length;
	while ((length = m_socket->Receive(senderAddress, buffer, sizeof(buffer))) > 0)
	{
		stream.Reset();
		stream.WriteData(reinterpret_cast<uint8_t*>(buffer), length);
		stream.SetCurrentPosition(0);

		uint8_t type = stream.ReadByte();
		if (type < Packet_ProtocolLast)
		{
			HandlePacket(type, senderAddress, stream);
		}
		else
		{
			return (PacketType)type;
		}
	}

	return Packet_None;
}

void NetInterface::HandlePacket(uint8_t packetType, const Address& sender, const BitStream& stream)
{
	switch (packetType)
	{
	case Packet_ConnectRequest:
		HandleConnectionRequest(sender, stream);
		break;

	case Packet_ConnectResponse:
		HandleConnectionResponse(sender, stream);
		break;
		
	default:
		printf("WARNING: Unhandled protocol packet type : %d\n", packetType);
		break;
	}
}

void NetInterface::HandleUserPacket(uint8_t packetType, const Address& sender, const BitStream& stream)
{
	printf("WARNING: Unhandled user packet type : %d\n", packetType);
}

void NetInterface::HandleConnectionRequest(const Address& sender, const BitStream& stream)
{
	Connection* connection = new Connection(*this, sender);
	m_connectionList.push_back(std::unique_ptr<Connection>(connection));
	connection->SetState(ConnectionState::Connected);

	// Send the connection reponse
	BitStream response;
	response.WriteByte(Packet_ConnectResponse);
	Send(sender, response);
}

void NetInterface::HandleConnectionResponse(const Address& sender, const BitStream& stream)
{
	Connection* connection = nullptr;
	for (auto it = m_connectionPendingList.begin(); it != m_connectionPendingList.end(); ++it)
	{
		if (*(*it)->GetAddress() == sender)
		{
			connection = (*it).get();
			m_connectionList.push_back(std::move(*it));
			m_connectionPendingList.erase(it);
			break;
		}
	}

	if (connection != nullptr)
	{
		connection->SetState(ConnectionState::Connected);
	}
}

Connection* NetInterface::GetConnection(const Address& address)
{
	for (auto& connection : m_connectionList)
	{
		if (*connection->GetAddress() == address)
			return connection.get();
	}
	return nullptr;
}

Connection* NetInterface::GetPendingConnection(const Address& address)
{
	for (auto& connection : m_connectionPendingList)
	{
		if (*connection->GetAddress() == address)
			return connection.get();
	}
	return nullptr;
}