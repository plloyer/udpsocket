#include <udpsocket/Connection.h>

#include <udpsocket/Address.h>
#include <udpsocket/BitStream.h>
#include <udpsocket/NetInterface.h>

Connection::Connection(NetInterface& net, const Address& address)
	: m_netInterface(net)
	, m_address(new Address(address))
	, m_state(ConnectionState::NotConnected)
{

}

Connection::~Connection()
{

}

const Address* Connection::GetAddress() const
{
	return m_address.get();
}
