#include <udpsocket/NetInterface.h>

#include <udpsocket/Address.h>
#include <udpsocket/BitStream.h>
#include <udpsocket/Connection.h>
#include <udpsocket/UDPSocket.h>


void NetInterface::Startup()
{
	UDPSocket::Startup();
}

void NetInterface::Cleanup()
{
	UDPSocket::Cleanup();
}

NetInterface::NetInterface()
	: m_socket(new UDPSocket())
{
}

NetInterface::~NetInterface()
{

}

bool NetInterface::Open(int bindPort)
{
	return m_socket->Open(bindPort);
}

void NetInterface::Connect(const Address& destination)
{
	if (GetConnection(destination) == nullptr && GetPendingConnection(destination) == nullptr)
	{
		m_connectionPendingList.push_back(std::unique_ptr<Connection>(new Connection(*this, destination)));

		BitStream stream;
		stream.WriteByte(Packet_ConnectRequest);
		Send(destination, stream);
	}
	else
	{
		printf("Warning: NetInterface::Connect() is called with an adress which is already connected or is already trying to connect: %s\n", destination.ToString().c_str());
	}
}

void NetInterface::Send(const Address& destination, const BitStream& data)
{
	m_socket->Send(destination, reinterpret_cast<const char*>(data.GetStream()), data.GetStreamLength());

#ifdef NET_DEBUG
	const_cast<BitStream&>(data).SetCurrentPosition(0);
	uint8_t type = data.ReadByte();
	printf("Sent packet to %s, type: %s, Data: %s\n", 
		destination.ToString().c_str(), 
		GetPacketTypeName(static_cast<PacketType>(type)), 
		reinterpret_cast<const char*>(data.GetStream()));
#endif
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

#ifdef NET_DEBUG
		printf("Received packet from %s, Type: %s, Data: %s\n", 
			senderAddress.ToString().c_str(), 
			GetPacketTypeName(static_cast<PacketType>(type)),
			reinterpret_cast<const char*>(stream.GetStream()));
#endif

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
		printf("WARNING: Unhandled protocol packet type : %s (%d)\n", GetPacketTypeName(static_cast<PacketType>(packetType)), packetType);
		break;
	}
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

	printf("Connected to: %s\n", sender.ToString().c_str());
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
		printf("Connected to: %s\n", sender.ToString().c_str());
		connection->SetState(ConnectionState::Connected);
	}
	else
	{
		printf("Unable to find connection for connection response from: %s\n", sender.ToString().c_str());
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

const char* NetInterface::GetPacketTypeName(PacketType type)
{
	//@LAME: Yeah I would need a proper enum to string which is also extensible in child class.
	switch (type)
	{
	case Packet_None:				return "Packet_None";
	case Packet_ConnectRequest:		return "Packet_ConnectRequest";
	case Packet_ConnectResponse:	return "Packet_ConnectResponse";
	default:						break;
	}

	static std::string out;
	out += "Unknown Type: ";
	out += std::to_string(type);
	out += ", add it in GetPacketTypeName()";
	return out.c_str();
}

int NetInterface::GetBoundPort() const
{
	return m_socket->GetBoundPort();
}