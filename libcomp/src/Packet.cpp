/**
 * @file libcomp/src/Packet.cpp
 * @ingroup libcomp
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Packet parsing and creation class implementation.
 *
 * This file is part of the COMP_hack Library (libcomp).
 *
 * Copyright (C) 2012-2016 COMP_hack Team <compomega@tutanota.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Packet.h"

#include "Compress.h"
#include "Endian.h"
#include "Log.h"
#include "PacketException.h"

#include <zlib.h>

#ifdef _WIN32
#include <windows.h>

#ifdef _MSC_VER
/// Macro to add data to the working buffer for the current hex dump line being
/// generated. Use the safe sprintf to avoid warnings with MSVC.
#define dump_print(...) sprintf_s(bufferp, 75, __VA_ARGS__)
#else // _MSC_VER
/// Macro to add data to the working buffer for the current hex dump line being
/// generated.
#define dump_print(...) sprintf(bufferp, __VA_ARGS__)
#endif // _MSC_VER

#else // _WIN32
#include <sys/socket.h>

/// Macro to add data to the working buffer for the current hex dump line being
/// generated.
#define dump_print(...) sprintf(bufferp, __VA_ARGS__)
#endif // _WIN32

#include <cstring>
#include <cstdio>

using namespace libcomp;

Packet::Packet() : mPosition(0), mSize(0)
{
    // The max packet size should be evenly divisible by 4 bytes.
    if(0 != (MAX_PACKET_SIZE % 4))
    {
        PACKET_EXCEPTION("MAX_PACKET_SIZE not a multiple of 4", this);
    }

    // Allocate the packet data
    mData = new uint8_t[MAX_PACKET_SIZE];

    // Ensure the packet is clear and the variables are set.
    Clear();
}

Packet::Packet(const std::vector<char>& data)
{
    // The max packet size should be evenly divisible by 4 bytes.
    if(0 != (MAX_PACKET_SIZE % 4))
    {
        PACKET_EXCEPTION("MAX_PACKET_SIZE not a multiple of 4", this);
    }

    // Allocate the packet data
    mData = new uint8_t[MAX_PACKET_SIZE];

    // Ensure the packet is clear and the variables are set.
    Clear();

    // If there is data to be written, use writeArray() to write it.
    if(!data.empty())
    {
        WriteArray(data);
    }

    // Rewind the packet back to the beginning.
    Rewind();
}

Packet::Packet(const void *pData, uint32_t sz)
{
    // The max packet size should be evenly divisible by 4 bytes.
    if(0 != (MAX_PACKET_SIZE % 4))
    {
        PACKET_EXCEPTION("MAX_PACKET_SIZE not a multiple of 4", this);
    }

    // Allocate the packet data
    mData = new uint8_t[MAX_PACKET_SIZE];

    // Ensure the packet is clear and the variables are set.
    Clear();

    // If there is data to be written, use writeArray() to write it.
    if(0 < sz)
    {
        WriteArray(pData, sz);
    }

    // Rewind the packet back to the beginning.
    Rewind();
}

Packet::~Packet()
{
    // Free the packet data.
    delete[] mData;
}

void Packet::GrowPacket(uint32_t sz)
{
    // Make sure the packet is growing.
    if(0 == sz)
    {
        PACKET_EXCEPTION("Attempted to grow the packet by 0 bytes", this);
    }

    // Calculate how big the packet will be if it can grow.
    uint32_t newSize = mPosition + sz;

    /* If the new size of the packet is less than the current size, keep the
     * packet at it's current size. If the new packet size is too big, throw a
     * packet exception. If the size is within the max packet size, set the new
     * size of the packet.
     */
    if(newSize < mSize)
    {
        // Keep the current size, do nothing.
    }
    else if(MAX_PACKET_SIZE < newSize || MAX_PACKET_SIZE < (mPosition + sz))
    {
        // The new size is too big, throw an exception.
        PACKET_EXCEPTION(String("Attempted to grow packet by %1 bytes; "
            "however, doing so would exceed MAX_PACKET_SIZE").Arg(sz), this);
    }
    else
    {
        // The new packet size is valid, set it.
        mSize = newSize;
    }
}

void Packet::Seek(uint32_t pos)
{
    // If the position is past the max packet size, thrown an exception.
    if(MAX_PACKET_SIZE < pos)
    {
        PACKET_EXCEPTION("Attempted to seek past the MAX_PACKET_SIZE", this);
    }

    // Set the new position.
    mPosition = pos;
}

void Packet::Skip(uint32_t sz)
{
    // If we are skipping 0 bytes, do nothing.
    if(0 == sz)
    {
        return;
    }

    // If advancing the position by sz bytes would place the current position
    // past the max packet size, thrown an exception.
    if(MAX_PACKET_SIZE < (mPosition + sz))
    {
        PACKET_EXCEPTION(String("Attempted to skip %1 bytes in the packet; "
            "however, doing so would exceed MAX_PACKET_SIZE").Arg(sz), this);
    }

    // Advance the current position by sz bytes.
    mPosition += sz;
}

void Packet::Rewind()
{
    // Reset the current position to the beginning of the packet.
    mPosition = 0;
}

void Packet::Rewind(uint32_t bytes)
{
    // If we are rewinding 0 bytes, do nothing.
    if(bytes == 0)
    {
        return;
    }

    // If rewinding more bytes than the current position, thrown an exception.
    if(bytes > mPosition)
    {
        PACKET_EXCEPTION(String("Attempted to rewind %1 bytes in the packet; "
            "however, doing so would rewind past the beginning of the "
            "packet").Arg(bytes), this);
    }

    // Rewind the current position by bytes.
    mPosition -= bytes;
}

void Packet::WriteBlank(uint32_t count)
{
    // If we are writing 0 blank bytes, do nothing.
    if(0 == count)
    {
        return;
    }

    // Grow the packet by the number of bytes we intend to write, set the bytes
    // to 0, and advance the current position by that many bytes.
    GrowPacket(count);
    memset(mData + mPosition, 0, count);
    Skip(count);
}

void Packet::WriteArray(const std::vector<char>& data)
{
    // If we are writing an array of 0 bytes, do nothing.
    if(data.empty())
    {
        return;
    }

    // Grow the packet by the number of bytes in the array, copy the array into
    // the packet data at the current position, and advance the current
    // position by the number of bytes in the array.
    GrowPacket((uint32_t)data.size());
    memcpy(mData + mPosition, &data[0], (uint32_t)data.size());
    Skip((uint32_t)data.size());
}

void Packet::WriteArray(const void *pData, uint32_t sz)
{
    // If we are writing an array of 0 bytes, do nothing.
    if(0 == sz)
    {
        return;
    }

    // Grow the packet by the number of bytes in the array, copy the array into
    // the packet data at the current position, and advance the current
    // position by the number of bytes in the array.
    GrowPacket(sz);
    memcpy(mData + mPosition, pData, sz);
    Skip(sz);
}

void Packet::WriteString(Convert::Encoding_t encoding, const String& str,
    bool nullTerminate)
{
    // Convert the string to the requested encoding and write it.
    WriteArray(Convert::ToEncoding(encoding, str, nullTerminate));
}

void Packet::WriteString32(Convert::Encoding_t encoding, const String& str,
    bool nullTerminate)
{
    // Convert the string to the requested encoding.
    std::vector<char> strd = Convert::ToEncoding(encoding, str, nullTerminate);

    // Write the size of the string data and the string.
    WriteU32((uint32_t)strd.size());
    WriteArray(strd);
}

void Packet::WriteString32Big(Convert::Encoding_t encoding, const String& str,
    bool nullTerminate)
{
    // Convert the string to the requested encoding.
    std::vector<char> strd = Convert::ToEncoding(encoding, str, nullTerminate);

    // Write the size of the string data and the string.
    WriteU32Big((uint32_t)strd.size());
    WriteArray(strd);
}

void Packet::WriteString32Little(Convert::Encoding_t encoding,
    const String& str, bool nullTerminate)
{
    // Convert the string to the requested encoding.
    std::vector<char> strd = Convert::ToEncoding(encoding, str, nullTerminate);

    // Write the size of the string data and the string.
    WriteU32Little((uint32_t)strd.size());
    WriteArray(strd);
}

void Packet::WriteString16(Convert::Encoding_t encoding, const String& str,
    bool nullTerminate)
{
    // Convert the string to the requested encoding.
    std::vector<char> strd = Convert::ToEncoding(encoding, str, nullTerminate);

    // Write the size of the string data and the string.
    WriteU16((uint16_t)strd.size());
    WriteArray(strd);
}

void Packet::WriteString16Big(Convert::Encoding_t encoding, const String& str,
    bool nullTerminate)
{
    // Convert the string to the requested encoding.
    std::vector<char> strd = Convert::ToEncoding(encoding, str, nullTerminate);

    // Write the size of the string data and the string.
    WriteU16Big((uint16_t)strd.size());
    WriteArray(strd);
}

void Packet::WriteString16Little(Convert::Encoding_t encoding,
    const String& str, bool nullTerminate)
{
    // Convert the string to the requested encoding.
    std::vector<char> strd = Convert::ToEncoding(encoding, str, nullTerminate);

    // Write the size of the string data and the string.
    WriteU16Little((uint16_t)strd.size());
    WriteArray(strd);
}

void Packet::WriteU8(uint8_t value)
{
    // Grow the packet by the size of the value, copy the value into the packet
    // data, and advance the current packet position by the size of the value.
    GrowPacket(1);
    memcpy(mData + mPosition, &value, 1);
    Skip(1);
}

void Packet::WriteS8(int8_t value)
{
    // Grow the packet by the size of the value, copy the value into the packet
    // data, and advance the current packet position by the size of the value.
    GrowPacket(1);
    memcpy(mData + mPosition, &value, 1);
    Skip(1);
}

void Packet::WriteU16(uint16_t value)
{
    // Grow the packet by the size of the value, copy the value into the packet
    // data, and advance the current packet position by the size of the value.
    GrowPacket(2);
    memcpy(mData + mPosition, &value, 2);
    Skip(2);
}

void Packet::WriteU16Big(uint16_t value)
{
    // Convert the value to big endian byte order and then write it.
    WriteU16(htobe16(value));
}

void Packet::WriteU16Little(uint16_t value)
{
    // Convert the value to little endian byte order and then write it.
    WriteU16(htole16(value));
}

void Packet::WriteS16(int16_t value)
{
    // Grow the packet by the size of the value, copy the value into the packet
    // data, and advance the current packet position by the size of the value.
    GrowPacket(2);
    memcpy(mData + mPosition, &value, 2);
    Skip(2);
}

void Packet::WriteS16Big(int16_t value)
{
    // Convert the value to big endian byte order and then write it.
    WriteS16((int16_t)htobe16(value));
}

void Packet::WriteS16Little(int16_t value)
{
    // Convert the value to little endian byte order and then write it.
    WriteS16(htole16(value));
}

void Packet::WriteU32(uint32_t value)
{
    // Grow the packet by the size of the value, copy the value into the packet
    // data, and advance the current packet position by the size of the value.
    GrowPacket(4);
    memcpy(mData + mPosition, &value, 4);
    Skip(4);
}

void Packet::WriteU32Big(uint32_t value)
{
    // Convert the value to big endian byte order and then write it.
    WriteU32(htobe32(value));
}

void Packet::WriteU32Little(uint32_t value)
{
    // Convert the value to little endian byte order and then write it.
    WriteU32(htole32(value));
}

void Packet::WriteS32(int32_t value)
{
    // Grow the packet by the size of the value, copy the value into the packet
    // data, and advance the current packet position by the size of the value.
    GrowPacket(4);
    memcpy(mData + mPosition, &value, 4);
    Skip(4);
}

void Packet::WriteS32Big(int32_t value)
{
    // Convert the value to big endian byte order and then write it.
    WriteS32((int32_t)htobe32(value));
}

void Packet::WriteS32Little(int32_t value)
{
    // Convert the value to little endian byte order and then write it.
    WriteS32(htole32(value));
}

void Packet::WriteU64(uint64_t value)
{
    // Grow the packet by the size of the value, copy the value into the packet
    // data, and advance the current packet position by the size of the value.
    GrowPacket(8);
    memcpy(mData + mPosition, &value, 8);
    Skip(8);
}

void Packet::WriteU64Big(uint64_t value)
{
    // Convert the value to big endian byte order and then write it.
    WriteU64(htobe64(value));
}

void Packet::WriteU64Little(uint64_t value)
{
    // Convert the value to little endian byte order and then write it.
    WriteU64(htole64(value));
}

void Packet::WriteS64(int64_t value)
{
    // Grow the packet by the size of the value, copy the value into the packet
    // data, and advance the current packet position by the size of the value.
    GrowPacket(8);
    memcpy(mData + mPosition, &value, 8);
    Skip(8);
}

void Packet::WriteS64Big(int64_t value)
{
    // Convert the value to big endian byte order and then write it.
    WriteS64((int64_t)htobe64(value));
}

void Packet::WriteS64Little(int64_t value)
{
    // Convert the value to little endian byte order and then write it.
    WriteS64(htole64(value));
}

void Packet::WriteFloat(float value)
{
    // Grow the packet by the size of the value, copy the value into the packet
    // data, and advance the current packet position by the size of the value.
    GrowPacket(4);
    memcpy(mData + mPosition, &value, 4);
    Skip(4);
}

std::vector<char> Packet::ReadArray(uint32_t sz)
{
    std::vector<char> data;

    // If the number of bytes to read is 0, do nothing.
    if(0 == sz)
    {
        return data;
    }

    // If there are more bytes to read than bytes left in the packet data,
    // thrown an exception.
    if((mPosition + sz) > mSize)
    {
        PACKET_EXCEPTION(String("Attempted to read an array of %1 bytes; "
            "however, doing so would read more data then is remaining "
            "in the packet").Arg(sz), this);
    }

    // Copy the packet data into the array and advance the current position
    // past the array that was just read.
    data.insert(data.begin(), reinterpret_cast<char*>(mData + mPosition),
        reinterpret_cast<char*>(mData + mPosition + sz));
    Skip(sz);

    return data;
}

void Packet::ReadArray(void *buffer, uint32_t sz)
{
    // If the number of bytes to read is 0, do nothing.
    if(0 == sz)
    {
        return;
    }

    // If there are more bytes to read than bytes left in the packet data,
    // thrown an exception.
    if((mPosition + sz) > mSize)
    {
        PACKET_EXCEPTION(String("Attempted to read an array of %1 bytes; "
            "however, doing so would read more data then is remaining "
            "in the packet").Arg(sz), this);
    }

    // Copy the packet data into the array and advance the current position
    // past the array that was just read.
    memcpy(buffer, mData + mPosition, sz);
    Skip(sz);
}

String Packet::ReadString(Convert::Encoding_t encoding)
{
    uint32_t sz = 0;

    // Search for the null terminator and calculate the size of the string.
    while(Left() > 0 && ReadU8() != 0)
    {
        sz++;
    }

    // Return the current position to the beginning of the string.
    Rewind(++sz);

    // Read the string into the buffer.
    std::vector<char> buffer = ReadArray(sz);

    // Convert the string to the requested encoding.
    return Convert::FromEncoding(encoding, buffer);
}

String Packet::ReadString16(Convert::Encoding_t encoding)
{
    // Read the size of the string.
    uint16_t sz = ReadU16();

    // Read the string into the buffer.
    std::vector<char> buffer = ReadArray(sz);

    // Convert the string to the requested encoding.
    return Convert::FromEncoding(encoding, buffer);
}

String Packet::ReadString16Big(Convert::Encoding_t encoding)
{
    // Read the size of the string.
    uint16_t sz = ReadU16Big();

    // Read the string into the buffer.
    std::vector<char> buffer = ReadArray(sz);

    // Convert the string to the requested encoding.
    return Convert::FromEncoding(encoding, buffer);
}

String Packet::ReadString16Little(Convert::Encoding_t encoding)
{
    // Read the size of the string.
    uint16_t sz = ReadU16Little();

    // Read the string into the buffer.
    std::vector<char> buffer = ReadArray(sz);

    // Convert the string to the requested encoding.
    return Convert::FromEncoding(encoding, buffer);
}

String Packet::ReadString32(Convert::Encoding_t encoding)
{
    // Read the size of the string.
    uint32_t sz = ReadU32();

    // Read the string into the buffer.
    std::vector<char> buffer = ReadArray(sz);

    // Convert the string to the requested encoding.
    return Convert::FromEncoding(encoding, buffer);
}

String Packet::ReadString32Big(Convert::Encoding_t encoding)
{
    // Read the size of the string.
    uint32_t sz = ReadU32Big();

    // Read the string into the buffer.
    std::vector<char> buffer = ReadArray(sz);

    // Convert the string to the requested encoding.
    return Convert::FromEncoding(encoding, buffer);
}

String Packet::ReadString32Little(Convert::Encoding_t encoding)
{
    // Read the size of the string.
    uint32_t sz = ReadU32Little();

    // Read the string into the buffer.
    std::vector<char> buffer = ReadArray(sz);

    // Convert the string to the requested encoding.
    return Convert::FromEncoding(encoding, buffer);
}

uint8_t Packet::PeekU8()
{
    // Check if there is enough data left in the packet to read the value; if
    // not, throw an exception.
    if((mPosition + 1) > mSize)
    {
        PACKET_EXCEPTION("Attempted to read 1 byte from the packet; however, "
            "this would read past the end of the packet.", this);
    }

    // Copy the value into a variable to return.
    uint8_t value;
    memcpy(&value, mData + mPosition, 1);

    return value;
}

uint16_t Packet::PeekU16()
{
    // Check if there is enough data left in the packet to read the value; if
    // not, throw an exception.
    if((mPosition + 2) > mSize)
    {
        PACKET_EXCEPTION("Attempted to read 2 bytes from the packet; however, "
            "this would read past the end of the packet.", this);
    }

    // Copy the value into a variable to return.
    uint16_t value;
    memcpy(&value, mData + mPosition, 2);

    return value;
}

uint16_t Packet::PeekU16Big()
{
    // Return the value converted from big endian byte order.
    return be16toh(PeekU16());
}

uint16_t Packet::PeekU16Little()
{
    // Return the value converted from little endian byte order.
    return le16toh(PeekU16());
}

uint32_t Packet::PeekU32()
{
    // Check if there is enough data left in the packet to read the value; if
    // not, throw an exception.
    if((mPosition + 4) > mSize)
    {
        PACKET_EXCEPTION("Attempted to read 4 bytes from the packet; however, "
            "this would read past the end of the packet.", this);
    }

    // Copy the value into a variable to return.
    uint32_t value;
    memcpy(&value, mData + mPosition, 4);

    return value;
}

uint32_t Packet::PeekU32Big()
{
    // Return the value converted from big endian byte order.
    return be32toh(PeekU32());
}

uint32_t Packet::PeekU32Little()
{
    // Return the value converted from little endian byte order.
    return le32toh(PeekU32());
}

uint8_t Packet::ReadU8()
{
    // Check if there is enough data left in the packet to read the value; if
    // not, throw an exception.
    if((mPosition + 1) > mSize)
    {
        PACKET_EXCEPTION("Attempted to read 1 byte from the packet; however, "
            "this would read past the end of the packet.", this);
    }

    // Copy the value into a variable to return and then advance the current
    // position by the size of the value.
    uint8_t value;
    memcpy(&value, mData + mPosition, 1);
    Skip(1);

    return value;
}

int8_t Packet::ReadS8()
{
    // Check if there is enough data left in the packet to read the value; if
    // not, throw an exception.
    if((mPosition + 1) > mSize)
    {
        PACKET_EXCEPTION("Attempted to read 1 byte from the packet; however, "
            "this would read past the end of the packet.", this);
    }

    // Copy the value into a variable to return and then advance the current
    // position by the size of the value.
    int8_t value;
    memcpy(&value, mData + mPosition, 1);
    Skip(1);

    return value;
}

uint16_t Packet::ReadU16()
{
    // Check if there is enough data left in the packet to read the value; if
    // not, throw an exception.
    if((mPosition + 2) > mSize)
    {
        PACKET_EXCEPTION("Attempted to read 2 bytes from the packet; however, "
            "this would read past the end of the packet.", this);
    }

    // Copy the value into a variable to return and then advance the current
    // position by the size of the value.
    uint16_t value;
    memcpy(&value, mData + mPosition, 2);
    Skip(2);

    return value;
}

uint16_t Packet::ReadU16Big()
{
    // Return the value converted from big endian byte order.
    return be16toh(ReadU16());
}

uint16_t Packet::ReadU16Little()
{
    // Return the value converted from little endian byte order.
    return le16toh(ReadU16());
}

int16_t Packet::ReadS16()
{
    // Check if there is enough data left in the packet to read the value; if
    // not, throw an exception.
    if((mPosition + 2) > mSize)
    {
        PACKET_EXCEPTION("Attempted to read 2 bytes from the packet; however, "
            "this would read past the end of the packet.", this);
    }

    // Copy the value into a variable to return and then advance the current
    // position by the size of the value.
    int16_t value;
    memcpy(&value, mData + mPosition, 2);
    Skip(2);

    return value;
}

int16_t Packet::ReadS16Big()
{
    // Return the value converted from big endian byte order.
    return (int16_t)be16toh(ReadS16());
}

int16_t Packet::ReadS16Little()
{
    // Return the value converted from little endian byte order.
    return le16toh(ReadS16());
}

uint32_t Packet::ReadU32()
{
    // Check if there is enough data left in the packet to read the value; if
    // not, throw an exception.
    if((mPosition + 4) > mSize)
    {
        PACKET_EXCEPTION("Attempted to read 4 bytes from the packet; however, "
            "this would read past the end of the packet.", this);
    }

    // Copy the value into a variable to return and then advance the current
    // position by the size of the value.
    uint32_t value;
    memcpy(&value, mData + mPosition, 4);
    Skip(4);

    return value;
}

uint32_t Packet::ReadU32Big()
{
    // Return the value converted from big endian byte order.
    return be32toh(ReadU32());
}

uint32_t Packet::ReadU32Little()
{
    // Return the value converted from little endian byte order.
    return le32toh(ReadU32());
}

int32_t Packet::ReadS32()
{
    // Check if there is enough data left in the packet to read the value; if
    // not, throw an exception.
    if((mPosition + 4) > mSize)
    {
        PACKET_EXCEPTION("Attempted to read 4 bytes from the packet; however, "
            "this would read past the end of the packet.", this);
    }

    // Copy the value into a variable to return and then advance the current
    // position by the size of the value.
    int32_t value;
    memcpy(&value, mData + mPosition, 4);
    Skip(4);

    return value;
}

int32_t Packet::ReadS32Big()
{
    // Return the value converted from big endian byte order.
    return (int32_t)be32toh(ReadS32());
}

int32_t Packet::ReadS32Little()
{
    // Return the value converted from little endian byte order.
    return le32toh(ReadS32());
}

uint64_t Packet::ReadU64()
{
    // Check if there is enough data left in the packet to read the value; if
    // not, throw an exception.
    if((mPosition + 8) > mSize)
    {
        PACKET_EXCEPTION("Attempted to read 8 bytes from the packet; however, "
            "this would read past the end of the packet.", this);
    }

    // Copy the value into a variable to return and then advance the current
    // position by the size of the value.
    uint64_t value;
    memcpy(&value, mData + mPosition, 8);
    Skip(8);

    return value;
}

uint64_t Packet::ReadU64Big()
{
    // Return the value converted from big endian byte order.
    return be64toh(ReadU64());
}

uint64_t Packet::ReadU64Little()
{
    // Return the value converted from little endian byte order.
    return le64toh(ReadU64());
}

int64_t Packet::ReadS64()
{
    // Check if there is enough data left in the packet to read the value; if
    // not, throw an exception.
    if((mPosition + 8) > mSize)
    {
        PACKET_EXCEPTION("Attempted to read 8 bytes from the packet; however, "
            "this would read past the end of the packet.", this);
    }

    // Copy the value into a variable to return and then advance the current
    // position by the size of the value.
    int64_t value;
    memcpy(&value, mData + mPosition, 8);
    Skip(8);

    return value;
}

int64_t Packet::ReadS64Big()
{
    // Return the value converted from big endian byte order.
    return (int64_t)be64toh(ReadS64());
}

int64_t Packet::ReadS64Little()
{
    // Return the value converted from little endian byte order.
    return le64toh(ReadS64());
}

float Packet::ReadFloat()
{
    // Check if there is enough data left in the packet to read the value; if
    // not, throw an exception.
    if((mPosition + 4) > mSize)
    {
        PACKET_EXCEPTION("Attempted to read 4 bytes from the packet; however, "
            "this would read past the end of the packet.", this);
    }

    // Copy the value into a variable to return and then advance the current
    // position by the size of the value.
    float value;
    memcpy(&value, mData + mPosition, 4);
    Skip(4);

    return value;
}

void Packet::Clear()
{
    // Reset the position and size of the packet.
    mPosition = 0;
    mSize = 0;

#ifdef COMP_HACK_DEBUG
    uint32_t deadbeef = 0xEFBEADDE;

    // Fill the buffer with "dead beef" so you can see what is and isn't data.
    for(uint32_t i = 0; i < MAX_PACKET_SIZE; i += 4)
    {
        memcpy(mData + i, &deadbeef, 4);
    }
#endif // COMP_HACK_DEBUG
}

void Packet::End()
{
    // Set the current position to the size of the packet data.
    mPosition = mSize;
}

uint32_t Packet::Tell() const
{
    // Return the current position.
    return mPosition;
}

uint32_t Packet::Left() const
{
    // Calculate how much data is between the current position and the end of
    // the packet data.
    return mSize - mPosition;
}

uint32_t Packet::Size() const
{
    // Return the size of the data in the packet.
    return mSize;
}

uint32_t Packet::Length() const
{
    // Alias of size(); return the size of the data in the packet.
    return mSize;
}

void Packet::EraseRight()
{
    // Set the size of the packet to the current position removing anything
    // after the current position.
    mSize = mPosition;
}

char* Packet::Data() const
{
    // Return a pointer to the packet data.
    return (char*)mData;
}

void Packet::HexDump() const
{
    // Generate the hex dump string and log it.
    LOG_INFO("==========================================="
        "==============================\n");
    LOG_INFO(Dump() + "\n");
    LOG_INFO("==========================================="
        "==============================\n");
}

String Packet::Dump() const
{
    // Do not generate a hex dump if there is not data.
    if(0 == mSize)
    {
        return String();
    }

    uint32_t line = 0;
    char buffer[75];

    std::list<String> final;

    // Loop for each line of the hex dump.
    while(line < mSize)
    {
        // Reset the buffer pointer to the beginning of the working buffer.
        char *bufferp = buffer;

        // Print line number (add it to the working buffer). If the current
        // position is on the first byte of this line, print the left side of
        // the position marker.
        bufferp += dump_print(mPosition == line ? "%04X {" : "%04X  ", line);

        // Loop and print 8 bytes of the packet. If the current byte is past
        // the end of the packet, blank spaces will be written. If the current
        // position is the next byte, print the left side of the position
        // marker. If the current byte is the current position, print the byte
        // followed by the right size of the position marker. Also, place a
        // single space between each byte in the dump.
        for(uint32_t i = line; i < (line + 8); ++i)
        {
            if(i >= mSize)
            {
                bufferp += dump_print(mPosition == i ? "  }" : "   ");
            }
            else if(mPosition == i)
            {
                bufferp += dump_print("%02X}", mData[i]);
            }
            else if(mPosition == (i + 1))
            {
                bufferp += dump_print("%02X{", mData[i]);
            }
            else
            {
                bufferp += dump_print("%02X ", mData[i]);
            }
        }

        // Place a gap between the two sets of 8 bytes. Also place the right
        // size of the current position marker if the current position is the
        // last byte in this set of 8 bytes.
        bufferp += dump_print(mPosition == (line + 8) ? "}" : " ");

        // Loop and print 8 bytes of the packet. If the current byte is past
        // the end of the packet, blank spaces will be written. If the current
        // position is the next byte, print the left side of the position
        // marker. If the current byte is the current position, print the byte
        // followed by the right size of the position marker. Also, place a
        // single space between each byte in the dump.
        for(uint32_t i = (line + 8); i < (line + 16); ++i)
        {
            if(i >= mSize)
                bufferp += dump_print(mPosition == i ? "  }" : "   ");
            else if(mPosition == i)
                bufferp += dump_print("%02X}", mData[i]);
            else if(mPosition == (i + 1))
                bufferp += dump_print("%02X{", mData[i]);
            else
                bufferp += dump_print("%02X ", mData[i]);
        }

        // Print a space between the hex dump and the ASCII representation.
        bufferp += dump_print(" ");

        // Print the ASCII character the the byte represents. Only printable
        // ASCII is included; all other characters are printed as a dot.
        for(uint32_t i = line; i < (line + 8) && i < mSize; ++i)
        {
            uint8_t val = mData[i];
            bufferp += dump_print("%c",
                (val >= 0x20 && val < 0x7f) ? val : '.');
        }

        // Print a space between the two sets of 8 ASCII characters.
        bufferp += dump_print(" ");

        // Print the ASCII character the the byte represents. Only printable
        // ASCII is included; all other characters are printed as a dot.
        for(uint32_t i = (line + 8); i < (line + 16) && i < mSize; ++i)
        {
            uint8_t val = mData[i];
            bufferp += dump_print("%c",
                (val >= 0x20 && val < 0x7f) ? val : '.');
        }

        // Advance the line by 16 bytes
        line += 16;

        // Append the generated line to the output string.
        final.push_back(String(buffer, (size_t)(bufferp - buffer)));
    }

    // Join all the lines of the hex dump together and return the final string.
    return String::Join(final, "\n");
}

char* Packet::Direct(uint32_t sz)
{
    // If there is no size specified simply return the pointer to the data.
    if(0 == sz)
    {
        return (char*)mData;
    }

    // Make sure the requested size is not bigger than the max packet size
    if(MAX_PACKET_SIZE < sz)
    {
        PACKET_EXCEPTION(String("Attempted to directly access the first %1 "
            "bytes of the packet; however, this size exceeds the "
            "MAX_PACKET_SIZE").Arg(sz), this);
    }

    // Make sure that the current position will not be past the new size of the
    // packet. If so, thrown an exception.
    if(mPosition > sz)
    {
        PACKET_EXCEPTION(String("Attempted to directly access the first %1 "
            "bytes of the packet; however, this size exceeds the "
            "size of the packet").Arg(sz), this);
    }

    // Set the new size of the packet.
    mSize = sz;

    // Return the pointer to the packet data.
    return (char*)mData;
}

void Packet::Split(Packet& other, uint32_t sz)
{
    // If there is no data to split simply clear the other packet and return.
    if(0 == sz)
    {
        other.Clear();

        return;
    }

    // If the request would read more data than contained in the packet, throw
    // an exception.
    if((mPosition + sz) > mSize)
    {
        PACKET_EXCEPTION(String("Attempted to split %1 bytes from the packet; "
            "however, this size exceeds the size of the packet").Arg(sz), this);
    }

    // Clear the other packet, write the data into it, and reset the current
    // position to the beginning of the packet.
    other.Clear();
    other.ReadArray(mData + mPosition, sz);
    other.Rewind();
}

int32_t Packet::Decompress(int32_t sz)
{
    // If there is no data to decompress, do nothing.
    if(0 >= sz)
    {
        return 0;
    }

    // Make sure that the data we wish to decompress is contained inside the
    // remaining packet data. If not, throw an exception.
    if((mPosition + (uint32_t)sz) > mSize)
    {
        PACKET_EXCEPTION(String("Attempted to decompress %1 bytes from the "
            "packet; however, this size exceeds the size of the "
            "packet").Arg(sz), this);
    }

    // Allocate the compressed copy.
    uint8_t *pData = new uint8_t[sz];

    if(nullptr == pData)
    {
        return 0;
    }

    // Copy the data to decompress.
    memcpy(pData, mData + mPosition, (size_t)sz);

    // Update the size of the packet.
    mSize = mPosition;

    // Decompress the data
    int32_t written = Compress::Decompress(pData, mData + mPosition,
        sz, (int32_t)(MAX_PACKET_SIZE - mSize));

    // Update the size.
    mSize += (uint32_t)written;

    // Free the compressed copy.
    delete[] pData;

    return written;
}

int32_t Packet::Compress(int32_t sz)
{
    // If there is no data to compress, do nothing.
    if(0 == sz)
    {
        return 0;
    }

    // Make sure that the data we wish to compress is contained inside the
    // remaining packet data. If not, throw an exception.
    if((mPosition + (uint32_t)sz) > mSize)
    {
        PACKET_EXCEPTION(String("Attempted to compress %1 bytes from the "
            "packet; however, this size exceeds the size of the "
            "packet").Arg(sz), this);
    }

    // Allocate the compressed copy.
    uint8_t *pData = new uint8_t[sz];

    if(nullptr == pData)
    {
        return 0;
    }

    // Copy the data to compress.
    memcpy(pData, mData + mPosition, (size_t)sz);

    // Update the size.
    mSize = mPosition;

    // Compress the data
    int32_t written = Compress::Compress(pData, mData + mPosition,
        sz, (int32_t)(MAX_PACKET_SIZE - mSize));

    // Update the size.
    mSize += (uint32_t)written;

    // Free the uncompressed copy.
    delete[] pData;

    return written;
}
