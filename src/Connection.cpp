#include "Connection.h"

#include "Address.h"
#include "BitStream.h"
#include "NetInterface.h"

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
