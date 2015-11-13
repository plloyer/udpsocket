#include <udpsocket/Address.h>
#include <udpsocket/SocketIncludes.h>

#include <stdlib.h>
#include <string.h>

namespace
{
	size_t strlcpy2(char *d, char const *s, size_t n)
	{
		return snprintf(d, n, "%s", s);
	}
}

Address::Address()
	: m_ip(0)
	, m_port(0)
{
}

Address::Address(int port)
	: m_ip(0)
	, m_port(port)
{
}

Address::Address(const char* stringAddress)
{
	char address[256];
	strlcpy2(address, stringAddress, 256);

	char *portString = strchr(address, ':');
	if (portString)
	{
		*portString = 0;
		++portString;
	}

	SetAddress(address, atoi(portString));
}

Address::Address(const char* stringAddress, int port)
{
	SetAddress(stringAddress, port);
}

Address::Address(const Address& rhs)
{
	*this = rhs;
}

void Address::SetAddress(const char* stringAddress, int port)
{
	sockaddr_in sockAddr;

	if (inet_pton(AF_INET, stringAddress, &sockAddr.sin_addr) == 0)
	{
		perror("Unable to convert address from string to binary.");
	}
	else
	{
		sockAddr.sin_port = htons(port);

		FromSocketAddress(sockAddr);
	}
}

void Address::GetSocketAddress(sockaddr_in& sockAddr) const
{
	memset(&sockAddr, 0, sizeof(sockaddr_in));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(m_port);
	sockAddr.sin_addr.s_addr = htonl(m_ip);
}

void Address::FromSocketAddress(const sockaddr_in& sockAddr)
{
	m_port = htons(sockAddr.sin_port);
	m_ip = htonl(sockAddr.sin_addr.s_addr);
}

std::string Address::ToString() const
{
	sockaddr_in sockAddr;
	GetSocketAddress(sockAddr);

	char address[128];
	inet_ntop(AF_INET, &sockAddr.sin_addr, address, 128);
	return std::string(address).append(":").append(std::to_string(m_port));
}

bool Address::operator==(const Address &rhs) const
{
	return m_port == rhs.m_port &&
		m_ip == rhs.m_ip;
}
