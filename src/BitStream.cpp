#include "BitStream.h"

#include <algorithm>
#include <io.h>
#include <stdlib.h>

BitStream::BitStream(void)
	: m_stream(nullptr)
{
	Reset();
}

BitStream::~BitStream(void)
{
	Reset();
}

void BitStream::Reset()
{
	free(m_stream);
	m_stream = nullptr;
	m_streamLength = 0;
	m_streamOffset = 0;
	m_currentPosition = 0;
	m_currentBit = 0;
}

void BitStream::_WriteBit(uint8_t value)
{
	// Check for current bit offset
	if ((m_currentBit % 8) == 0)
	{
		// Check for valid stream
		if (m_stream == nullptr)
		{
			m_stream = (uint8_t*)malloc(sizeof(uint8_t));
			m_streamLength = 1;
			m_stream[m_streamLength - 1] = 0;
			m_currentBit = 0;
			m_streamOffset = 0;
		}
		else
		{
			m_currentBit = 0;
			m_streamLength++;
			m_stream = (uint8_t*)realloc(m_stream, m_streamLength*sizeof(uint8_t));
			m_stream[m_streamLength - 1] = 0;
		}
	}

	// Write single BIT
	m_stream[m_streamLength - 1] |= ((value & 0x80) >> m_currentBit);
	m_streamOffset++;
	m_currentPosition = m_streamOffset - 1;
	m_currentBit++;
}

void BitStream::_ReadBit(uint8_t& value)  const
{
	// Check for valid position
	value = 0x00;
	if (m_currentPosition < m_streamOffset)
	{
		// Read single BIT
		uint32_t currentByte = m_currentPosition >> 3;
		uint8_t currentBit = (uint8_t)(m_currentPosition % 8);
		value = ((uint8_t)(m_stream[currentByte] << currentBit) >> 7);
		m_currentPosition = std::max((uint32_t)0, std::min(m_streamOffset - 1, m_currentPosition + 1));
	}
}

void BitStream::WriteBit(bool value)
{
	uint8_t bitValue = (((value ? 1 : 0) & 0x01) << 7);
	_WriteBit(bitValue);
}

bool BitStream::ReadBit() const
{
	uint8_t value;
	_ReadBit(value);
	return value != 0;
}

void BitStream::WriteByte(uint8_t value)
{
	uint8_t currentOffset = 0;
	uint8_t mask = 0x80;
	uint8_t bitValue = 0;
	for (uint32_t i = 0; i < 8; i++)
	{
		bitValue = ((value & mask) << currentOffset);
		_WriteBit(bitValue);
		mask = mask >> 1;
		currentOffset++;
	}
}

uint8_t BitStream::ReadByte() const
{
	uint8_t value = 0x00;
	uint8_t currentOffset = 7;
	uint8_t bitValue;
	for (uint32_t i = 0; i < 8; i++)
	{
		_ReadBit(bitValue);
		value |= (bitValue << currentOffset);
		currentOffset--;
	}
	return value;
}

void BitStream::WriteU16(uint16_t value)
{
	// Write single uint16_t
	uint8_t currentOffset = 0;
	uint16_t mask = 0x8000;
	uint8_t bitValue = 0;
	for (uint32_t i = 0; i < 16; i++)
	{
		bitValue = (uint8_t)(((value & mask) << currentOffset) >> 8);
		_WriteBit(bitValue);
		mask = mask >> 1;
		currentOffset++;
	}
}

uint16_t BitStream::ReadU16() const
{
	// Read single uint16_t
	uint16_t value = 0x0000;
	uint8_t currentOffset = 15;
	uint8_t bitValue;
	for (uint32_t i = 0; i < 16; i++)
	{
		_ReadBit(bitValue);
		value |= (bitValue << currentOffset);
		currentOffset--;
	}
	return value;
}

void BitStream::WriteU32(uint32_t value)
{
	// Write single uint32_t
	uint8_t currentOffset = 0;
	uint32_t mask = 0x80000000;
	uint8_t bitValue = 0;
	for (uint32_t i = 0; i < 32; i++)
	{
		bitValue = (uint8_t)(((value & mask) << currentOffset) >> 24);
		_WriteBit(bitValue);
		mask = mask >> 1;
		currentOffset++;
	}
}

uint32_t BitStream::ReadU32() const
{
	// Read single uint32_t
	uint32_t value = 0x00000000;
	uint8_t currentOffset = 31;
	uint8_t bitValue;
	for (uint32_t i = 0; i < 32; i++)
	{
		_ReadBit(bitValue);
		value |= (bitValue << currentOffset);
		currentOffset--;
	}
	return value;
}

void BitStream::WriteData(uint8_t* apData, uint32_t aLength)
{
	// Check for valid data
	if (apData != nullptr)
	{
		// Write variable-length data
		for (uint32_t i = 0; i < aLength; i++)
		{
			// Write single uint8_t
			WriteByte(apData[i]);
		}
	}
}

void BitStream::ReadData(uint8_t* apData, uint32_t aLength) const
{
	// Check for valid data
	if (apData != nullptr)
	{
		// Read variable-length data
		for (uint32_t i = 0; i < aLength; i++)
		{
			// Read single uint8_t
			apData[i] = ReadByte();
		}
	}
}

void BitStream::WriteData(uint16_t* apData, uint32_t aLength)
{
	// Check for valid data
	if (apData != nullptr)
	{
		// Write variable-length data
		for (uint32_t i = 0; i < aLength; i++)
		{
			// Write single uint16_t
			WriteU16(apData[i]);
		}
	}
}

void BitStream::ReadData(uint16_t* apData, uint32_t aLength) const
{
	// Check for valid data
	if (apData != nullptr)
	{
		// Read variable-length data
		for (uint32_t i = 0; i < aLength; i++)
		{
			// Read single uint16_t
			apData[i] = ReadU16();
		}
	}
}

void BitStream::WriteData(uint32_t* apData, uint32_t aLength)
{
	// Check for valid data
	if (apData != nullptr)
	{
		// Write variable-length data
		for (uint32_t i = 0; i < aLength; i++)
		{
			// Write single uint32_t
			WriteU32(apData[i]);
		}
	}
}

void BitStream::ReadData(uint32_t* apData, uint32_t aLength) const
{
	// Check for valid data
	if (apData != nullptr)
	{
		// Read variable-length data
		for (uint32_t i = 0; i < aLength; i++)
		{
			// Read single uint32_t
			apData[i] = ReadU32();
		}
	}
}

void BitStream::WriteBits(uint32_t value, uint32_t aLength)
{
	// Write single BITs
	uint32_t _nLen = std::max((uint32_t)0, std::min((uint32_t)32, aLength));
	uint8_t currentOffset = (uint8_t)(_nLen - 1);
	uint32_t mask = (0x00000001 << (_nLen - 1));
	uint8_t bitValue = 0;
	for (uint32_t i = 0; i < _nLen; i++)
	{
		bitValue = ((uint8_t)((value & mask) >> currentOffset) << 7);
		_WriteBit(bitValue);
		mask = mask >> 1;
		currentOffset--;
	}
}

void BitStream::ReadBits(uint32_t& value, uint32_t aLength) const
{
	// Read single BITs
	value = 0x00000000;
	uint32_t _nLen = std::max((uint32_t)0, std::min((uint32_t)32, aLength));
	uint8_t currentOffset = 31;
	uint8_t bitValue;
	for (uint32_t i = 0; i < _nLen; i++)
	{
		_ReadBit(bitValue);
		value |= (bitValue << currentOffset);
		currentOffset--;
	}
}
/*
void BitStream::LoadStream(char* lpszFileName)
{
	// Check for valid file name
	if (lpszFileName != nullptr)
	{
		// Free stream
		if (m_stream != nullptr)
		{
			free(m_stream);
			m_stream = nullptr;
			m_streamLength = 0;
		}

		// Read stream from file
		FILE* file = _tfopen(lpszFileName, _T("rb"));
		if (file != nullptr)
		{
			m_streamLength = filelength(file->_file);
			m_stream = (uint8_t*)malloc(m_streamLength*sizeof(uint8_t));
			fread(m_stream, sizeof(uint8_t), m_streamLength, file);
			fclose(file);
			m_streamOffset = m_streamLength << 3;
			m_currentPosition = m_streamOffset - 1;
			m_currentBit = 0;
		}
	}
}

void BitStream::SaveStream(char* lpszFileName)
{
	// Check for valid file name
	if (lpszFileName != nullptr)
	{
		// Check for valid stream
		if (m_stream != nullptr)
		{
			// Write stream to file
			FILE* file = _tfopen(lpszFileName, _T("wb"));
			if (file != nullptr)
			{
				fwrite(m_stream, sizeof(uint8_t), m_streamLength, file);
				fclose(file);
			}
		}
	}
}*/

void BitStream::LoadStream(uint8_t* apStream, uint32_t aLength)
{
	// Check for valid memory buffer
	if (apStream != nullptr)
	{
		// Free stream
		if (m_stream != nullptr)
		{
			free(m_stream);
			m_stream = nullptr;
			m_streamLength = 0;
		}

		// Read stream from memory buffer
		m_streamLength = aLength;
		m_stream = (uint8_t*)malloc(m_streamLength*sizeof(uint8_t));
		memcpy(m_stream, apStream, m_streamLength*sizeof(uint8_t));
		m_streamOffset = m_streamLength << 3;
		m_currentPosition = m_streamOffset - 1;
		m_currentBit = 0;
	}
}

void BitStream::SaveStream(uint8_t* apStream)
{
	// Check for valid memory buffer
	if (apStream != nullptr)
	{
		// Check for valid stream
		if (m_stream != nullptr)
		{
			// Write stream to memory buffer
			memcpy(apStream, m_stream, m_streamLength*sizeof(uint8_t));
		}
	}
}

void BitStream::SetCurrentPosition(uint32_t currentPosition)
{
	m_currentPosition = std::max((uint32_t)0, std::min(m_streamOffset - 1, currentPosition));
}