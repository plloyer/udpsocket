#pragma once

#include <stdint.h>
#include <string>

class Address
{
public:
	Address();
	Address(int port);
	Address(const char* stringAddress);
	Address(const char* stringAddress, int port);
	Address(const Address& rhs);

	void SetAddress(const char* stringAddress, int port);

	void GetSocketAddress(struct sockaddr_in& sockAddr) const;
	void FromSocketAddress(const struct sockaddr_in& sockAddr);

	std::string ToString() const;

	bool operator==(const Address &theAddress) const;

private:
	uint16_t m_port;
	uint32_t m_ip;
};