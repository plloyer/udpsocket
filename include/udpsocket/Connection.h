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

	void Update(float seconds);

	const Address* GetAddress() const;

	ConnectionState GetState() const { return m_state; }
	void SetState(ConnectionState state) { m_state = state; }

	bool IsConnectionTimedout() const;
	void PingReceived() { m_lastPingReceivedTimer = 0.0f; }

	void PingSent() { m_lastPingSentTimer = 0.0f; }
	float GetLastPingSentTimer() const { return m_lastPingSentTimer; }

private:
	NetInterface& m_netInterface;
	std::unique_ptr<Address> m_address;
	ConnectionState m_state;

	float m_lastPingReceivedTimer;
	float m_lastPingSentTimer;
};