#pragma once

#include <memory>
#include <stdint.h>
#include <vector>

class Address;
class BitStream;
class Connection;
class UDPSocket;

// Extend this enum in the application layer
enum PacketType
{
	Packet_None,
	Packet_ConnectRequest,
	Packet_ConnectResponse,
	Packet_Ping,
	Packet_Pong,

	Packet_ProtocolLast
};

enum class Reliability
{
	Reliable,
	NotReliable
};

class NetInterface
{
public:
	static void Startup();
	static void Cleanup();

	NetInterface();
	virtual ~NetInterface();

	virtual void Tick(float seconds);

	bool Open(int bindPort);
	void Connect(const Address& destination);

	void Send(const Address& destination, const BitStream& data, Reliability reliability);
	PacketType Receive(Address& senderAddress, BitStream& stream);

	bool HasConnection() const { return !m_connectionList.empty(); }
	bool HasPendingConnection() const { return !m_connectionPendingList.empty(); }
	
	int GetBoundPort() const;

protected:
	void PingConnections(float frequence);

	virtual const char* GetPacketTypeName(PacketType type);

	void HandlePacket(uint8_t packetType, const Address& sender, const BitStream& stream);
	virtual void HandleConnectionRequest(const Address& sender, const BitStream& stream);
	virtual void HandleConnectionResponse(const Address& sender, const BitStream& stream);
	virtual void HandlePing(const Address& sender, const BitStream& stream);
	virtual void HandlePong(const Address& sender, const BitStream& stream);

	void UpdateConnectionList(std::vector<std::unique_ptr<Connection>>& connections, float seconds);

	Connection* GetConnection(const Address& address);
	Connection* GetPendingConnection(const Address& address);

private:
	std::unique_ptr<UDPSocket> m_socket;
	std::vector<std::unique_ptr<Connection>> m_connectionList;
	std::vector<std::unique_ptr<Connection>> m_connectionPendingList;
};