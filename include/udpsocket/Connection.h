#pragma once

#include <memory>

class Address;
class BitStream;
class NetInterface;

enum class ConnectionState
{
	NotConnected,
	WaitingForConnectionResponse,
	Connected
};

class Connection
{
public:

	Connection(NetInterface& net, const Address& address);
	~Connection();

	const Address* GetAddress() const;

	ConnectionState GetState() const { return m_state; }
	void SetState(ConnectionState state) { m_state = state; }

private:
	NetInterface& m_netInterface;
	std::unique_ptr<Address> m_address;
	ConnectionState m_state;
};