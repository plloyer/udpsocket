#pragma once

#include <stdint.h>

class BitStream
{
public:
	BitStream();
	~BitStream();

	void			Reset();

	void			WriteBit(bool value);
	bool			ReadBit() const;
	void			WriteByte(uint8_t value);
	uint8_t			ReadByte() const;
	void			WriteU16(uint16_t value);
	uint16_t		ReadU16() const;
	void			WriteU32(uint32_t value);
	uint32_t		ReadU32() const;

	void			WriteData(uint8_t* apData, uint32_t aLength);
	void			ReadData(uint8_t* apData, uint32_t aLength) const;
	void			WriteData(uint16_t* apData, uint32_t aLength);
	void			ReadData(uint16_t* apData, uint32_t aLength) const;
	void			WriteData(uint32_t* apData, uint32_t aLength);
	void			ReadData(uint32_t* apData, uint32_t aLength) const;

	void			WriteBits(uint32_t value, uint32_t aLength=32);
	void			ReadBits(uint32_t& value, uint32_t aLength=32) const;

	//void LoadStream(char* lpszFileName);
	//void SaveStream(char* lpszFileName);
	void			LoadStream(uint8_t* apStream, uint32_t aLength);
	void			SaveStream(uint8_t* apStream);

	const uint8_t*	GetStream() const			{ return m_stream; }
	uint32_t		GetStreamBitCount() const	{ return m_streamOffset; }
	uint32_t		GetStreamLength() const		{ return m_streamLength; }
	uint32_t		GetCurrentPosition() const	{ return m_currentPosition; }
	void			SetCurrentPosition(uint32_t dwCurrentPosition);

private:
	void	 _WriteBit(uint8_t value);
	void	 _ReadBit(uint8_t& value)  const;

private:
	uint8_t*		 m_stream;
	uint32_t		 m_streamLength;
	uint32_t		 m_streamOffset;
	uint8_t			 m_currentBit;
	mutable uint32_t m_currentPosition;
};
