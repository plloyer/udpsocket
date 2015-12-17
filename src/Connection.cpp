#include <udpsocket/Connection.h>

#include <udpsocket/Address.h>
#include <udpsocket/BitStream.h>
#include <udpsocket/NetInterface.h>

namespace
{
	const float gConnectionTimeout = 5.0f;
}

Connection::Connection(NetInterface& net, const Address& address)
	: m_netInterface(net)
	, m_address(new Address(address))
	, m_state(ConnectionState::NotConnected)
	, m_lastPingReceivedTimer(0.0f)
{

}

Connection::~Connection()
{

}

void Connection::Update(float seconds)
{
	m_lastPingReceivedTimer += seconds;
	m_lastPingSentTimer += seconds;
}

const Address* Connection::GetAddress() const
{
	return m_address.get();
}

bool Connection::IsConnectionTimedout() const
{
	return m_lastPingReceivedTimer > gConnectionTimeout;
}
