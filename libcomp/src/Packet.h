/**
 * @file libcomp/src/Packet.h
 * @ingroup libcomp
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Packet parsing and creation class definition.
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

#ifndef LIBCOMP_SRC_PACKET_H
#define LIBCOMP_SRC_PACKET_H

#include "Constants.h"
#include "Convert.h"
#include "String.h"

#include <vector>

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef Q_OS_WIN32
#include <unistd.h>
#endif // Q_OS_WIN32

namespace libcomp
{

// This class is thrown when an error occurs in the Packet class.
class PacketException;

/**
 * Convenience class to read and write packet data. This class is designed to
 * make it easy to read and write packet data. Strings can be converted between
 * the internal Unicode format and Windows code pages. All integer numbers can
 * be written in host byte order, big endian, or little endian. The packet data
 * is usually read or written in sequantial order but you can use random access
 * as well. Note that data is not inserted into the middle of the packet, it
 * is instead overwritten with the new data. Here is some example code:
 * @code
 * Packet p;
 * p.WriteU16Little(0x1234); // Code for this packet.
 * p.WriteBlank(4); // Number of items (blank for now).
 *
 * // Initialize the item count to 0.
 * int count = 0;
 *
 * // Loop through and write each item to the packet.
 * while(HaveItems())
 * {
 *     count++;
 *     p.WriteU32Little(NextItem());
 * }
 *
 * // Return to where the item count should be, write the count, and return
 * // the current position back to the end of the packet.
 * p.Seek(2);
 * p.WriteU32Little(count);
 * p.End();
 * @endcode
 */
class Packet
{
public:
    /// This class needs to directly access data in the Packet class.
    friend class PacketException;

    /**
     * Initialize a blank Packet object without any data.
     */
    explicit Packet();

    /**
     * @brief Move the packet data from another Packet object into this one.
     * @param other Packet object to move the data from.
     */
    explicit Packet(Packet&& other);

    /**
     * Initialize a Packet object with @em data.
     * @param data Buffer of packet data to initalize the object with.
     */
    explicit Packet(const std::vector<char>& data);

    /**
     * Initialize a Packet object with @em pBuffer of @em size bytes.
     * @param pData Buffer of packet data to initalize the object with.
     * @param sz Number of bytes in the data buffer.
     */
    explicit Packet(const void *pData, uint32_t sz);

    /**
     * Destructor to clean-up the object (free the data).
     */
    ~Packet();

    /**
     * Write @em count blank bytes of value 0 into the packet.
     * @param count Number of blank bytes to write.
     */
    void WriteBlank(uint32_t count);

    /**
     * Copy the desired buffer into the packet.
     * @param data Data to write into the packet.
     */
    void WriteArray(const std::vector<char>& data);

    /**
     * Copy the desired buffer into the packet.
     * @param pData Data to write into the packet.
     * @param sz Size (in bytes) of the data to write.
     */
    void WriteArray(const void *pData, uint32_t sz);

    /**
     * Write the string @em str into the packet encoded with @em encoding.
     * There is no size written before the string so using this function
     * without a null terminator only makes sense if the string is of fixed
     * size.
     * @param encoding String encoding to use. Can be one of:
     * - ENCODING_UTF8 (Unicode)
     * - ENCODING_CP932 (Japanese)
     * - ENCODING_CP1252 (US English)
     * @param str String to write into the packet.
     * @param nullTerminate If true, the string will end with a null
     * terminator; otherwise, the string will not contain a null byte at
     * the end.
     * @sa ReadString
     * @sa WriteString16
     * @sa WriteString32
     */
    void WriteString(Convert::Encoding_t encoding, const String& str,
        bool nullTerminate = false);

    /**
     * Write the string @em str into the packet encoded with @em encoding. A
     * 16-bit unsigned integer (host byte order) is written before the string
     * to indicate the size of the string (including any null terminator).
     * @param encoding String encoding to use. Can be one of:
     * - ENCODING_UTF8 (Unicode)
     * - ENCODING_CP932 (Japanese)
     * - ENCODING_CP1252 (US English)
     * @param str String to write into the packet.
     * @param nullTerminate If true, the string will end with a null
     * terminator; otherwise, the string will not contain a null byte
     * at the end.
     * @sa WriteString
     * @sa WriteString16Big
     * @sa WriteString16Little
     */
    void WriteString16(Convert::Encoding_t encoding, const String& str,
        bool nullTerminate = false);

    /**
     * Write the string @em str into the packet encoded with @em encoding. A
     * 16-bit unsigned integer (big endian byte order) is written before the
     * string to indicate the size of the string (including any null
     * terminator).
     * @param encoding String encoding to use. Can be one of:
     * - ENCODING_UTF8 (Unicode)
     * - ENCODING_CP932 (Japanese)
     * - ENCODING_CP1252 (US English)
     * @param str String to write into the packet.
     * @param nullTerminate If true, the string will end with a null
     * terminator; otherwise, the string will not contain a null byte
     * at the end.
     * @sa WriteString
     * @sa WriteString16
     * @sa WriteString16Little
     */
    void WriteString16Big(Convert::Encoding_t encoding, const String& str,
        bool nullTerminate = false);

    /**
     * Write the string @em str into the packet encoded with @em encoding. A
     * 16-bit unsigned integer (little endian byte order) is written before the
     * string to indicate the size of the string (including any null
     * terminator).
     * @param encoding String encoding to use. Can be one of:
     * - ENCODING_UTF8 (Unicode)
     * - ENCODING_CP932 (Japanese)
     * - ENCODING_CP1252 (US English)
     * @param str String to write into the packet.
     * @param nullTerminate If true, the string will end with a null
     * terminator; otherwise, the string will not contain a null byte
     * at the end.
     * @sa WriteString
     * @sa WriteString16
     * @sa WriteString16Big
     */
    void WriteString16Little(Convert::Encoding_t encoding, const String& str,
        bool nullTerminate = false);

    /**
     * Write the string @em str into the packet encoded with @em encoding. A
     * 32-bit unsigned integer (host byte order) is written before the string
     * to indicate the size of the string (including any null terminator).
     * @param encoding String encoding to use. Can be one of:
     * - ENCODING_UTF8 (Unicode)
     * - ENCODING_CP932 (Japanese)
     * - ENCODING_CP1252 (US English)
     * @param str String to write into the packet.
     * @param nullTerminate If true, the string will end with a null
     * terminator; otherwise, the string will not contain a null byte
     * at the end.
     * @sa WriteString
     * @sa WriteString32Big
     * @sa WriteString32Little
     */
    void WriteString32(Convert::Encoding_t encoding, const String& str,
        bool nullTerminate = false);

    /**
     * Write the string @em str into the packet encoded with @em encoding. A
     * 32-bit unsigned integer (big endian byte order) is written before the
     * string to indicate the size of the string (including any null
     * terminator).
     * @param encoding String encoding to use. Can be one of:
     * - ENCODING_UTF8 (Unicode)
     * - ENCODING_CP932 (Japanese)
     * - ENCODING_CP1252 (US English)
     * @param str String to write into the packet.
     * @param nullTerminate If true, the string will end with a null
     * terminator; otherwise, the string will not contain a null byte
     * at the end.
     * @sa WriteString
     * @sa WriteString32
     * @sa WriteString32Little
     */
    void WriteString32Big(Convert::Encoding_t encoding, const String& str,
        bool nullTerminate = false);

    /**
     * Write the string @em str into the packet encoded with @em encoding. A
     * 32-bit unsigned integer (little endian byte order) is written before the
     * string to indicate the size of the string (including any null
     * terminator).
     * @param encoding String encoding to use. Can be one of:
     * - ENCODING_UTF8 (Unicode)
     * - ENCODING_CP932 (Japanese)
     * - ENCODING_CP1252 (US English)
     * @param str String to write into the packet.
     * @param nullTerminate If true, the string will end with a null
     * terminator; otherwise, the string will not contain a null byte
     * at the end.
     * @sa WriteString
     * @sa WriteString32
     * @sa WriteString32Big
     */
    void WriteString32Little(Convert::Encoding_t encoding, const String& str,
        bool nullTerminate = false);

    /**
     * Write an unsigned 8-bit integer @em value to the packet.
     * @param value 8-bit unsigned integer to write.
     */
    void WriteU8(uint8_t value);

    /**
     * Write a signed 8-bit integer @em value to the packet.
     * @param value 8-bit signed integer to write.
     */
    void WriteS8(int8_t value);

    /**
     * Write an unsigned 16-bit integer @em value to the packet in host byte
     * order.
     * @param value 16-bit unsigned integer to write.
     * @sa WriteU16Big
     * @sa WriteU16Little
     */
    void WriteU16(uint16_t value);

    /**
     * Write an unsigned 16-bit integer @em value to the packet in big endian
     * byte order.
     * @param value 16-bit unsigned integer to write.
     * @sa WriteU16
     * @sa WriteU16Little
     */
    void WriteU16Big(uint16_t value);

    /**
     * Write an unsigned 16-bit integer @em value to the packet in little
     * endian byte order.
     * @param value 16-bit unsigned integer to write.
     * @sa WriteU16
     * @sa WriteU16Big
     */
    void WriteU16Little(uint16_t value);

    /**
     * Write a signed 16-bit integer @em value to the packet in host byte
     * order.
     * @param value 16-bit signed integer to write.
     * @sa WriteS16Big
     * @sa WriteS16Little
     */
    void WriteS16(int16_t value);

    /**
     * Write a signed 16-bit integer @em value to the packet in big endian byte
     * order.
     * @param value 16-bit signed integer to write.
     * @sa WriteS16
     * @sa WriteS16Little
     */
    void WriteS16Big(int16_t value);

    /**
     * Write a signed 16-bit integer @em value to the packet in little endian
     * byte order.
     * @param value 16-bit signed integer to write.
     * @sa WriteS16
     * @sa WriteS16Big
     */
    void WriteS16Little(int16_t value);

    /**
     * Write an unsigned 32-bit integer @em value to the packet in host byte
     * order.
     * @param value 32-bit unsigned integer to write.
     * @sa WriteU32Big
     * @sa WriteU32Little
     */
    void WriteU32(uint32_t value);

    /**
     * Write an unsigned 32-bit integer @em value to the packet in big endian
     * byte order.
     * @param value 32-bit unsigned integer to write.
     * @sa WriteU32
     * @sa WriteU32Little
     */
    void WriteU32Big(uint32_t value);

    /**
     * Write an unsigned 32-bit integer @em value to the packet in little
     * endian byte order.
     * @param value 32-bit unsigned integer to write.
     * @sa WriteU32
     * @sa WriteU32Big
     */
    void WriteU32Little(uint32_t value);

    /**
     * Write a signed 32-bit integer @em value to the packet in host byte
     * order.
     * @param value 32-bit signed integer to write.
     * @sa WriteS32Big
     * @sa WriteS32Little
     */
    void WriteS32(int32_t value);

    /**
     * Write a signed 32-bit integer @em value to the packet in big endian
     * byte order.
     * @param value 32-bit signed integer to write.
     * @sa WriteS32
     * @sa WriteS32Little
     */
    void WriteS32Big(int32_t value);

    /**
     * Write a signed 32-bit integer @em value to the packet in little endian
     * byte order.
     * @param value 32-bit signed integer to write.
     * @sa WriteS32
     * @sa WriteS32Big
     */
    void WriteS32Little(int32_t value);

    /**
     * Write an unsigned 64-bit integer @em value to the packet in host byte
     * order.
     * @param value 64-bit unsigned integer to write.
     * @sa WriteU64Big
     * @sa WriteU64Little
     */
    void WriteU64(uint64_t value);

    /**
     * Write an unsigned 64-bit integer @em value to the packet in big endian
     * byte order.
     * @param value 64-bit unsigned integer to write.
     * @sa WriteU64
     * @sa WriteU64Little
     */
    void WriteU64Big(uint64_t value);

    /**
     * Write an unsigned 64-bit integer @em value to the packet in little
     * endian byte order.
     * @param value 64-bit unsigned integer to write.
     * @sa WriteU64
     * @sa WriteU64Big
     */
    void WriteU64Little(uint64_t value);

    /**
     * Write a signed 64-bit integer @em value to the packet in host byte
     * order.
     * @param value 64-bit signed integer to write.
     * @sa WriteS64Big
     * @sa WriteS64Little
     */
    void WriteS64(int64_t value);

    /**
     * Write a signed 64-bit integer @em value to the packet in big endian byte
     * order.
     * @param value 64-bit signed integer to write.
     * @sa WriteS64
     * @sa WriteS64Little
     */
    void WriteS64Big(int64_t value);

    /**
     * Write a signed 64-bit integer @em value to the packet in little endian
     * byte order.
     * @param value 64-bit signed integer to write.
     * @sa WriteS64
     * @sa WriteS64Big
     */
    void WriteS64Little(int64_t value);

    /**
     * Write a 32-bit floating point @em value to the packet.
     * @param value 32-bit floating point value to write.
     */
    void WriteFloat(float value);

#ifdef COMP_HACK_PACKETEXT

    /**
     * Write a 32-bit integer @em value that represents the current game ticks.
     * This is a packet extension that calls Server::InternalToGameTicks()
     * to convert the internal time of the server to the on-the-wire time
     * format the client expects. You can call this method by hand and pass the
     * value to @ref WriteU32Little instead; this method is for convenience.
     * Since this method requires access to Server class, this extension
     * only works from within the channel server project.
     * @param value 32-bit floating point value to write.
     * @param value Internal server time to convert and write into the packet.
     */
    void WriteGameTicks(uint64_t value);

#endif // COMP_HACK_PACKETEXT

    /**
     * Read @em sz bytes of data from the packet. After reading the data,
     * the current position in the packet will advance by @em sz bytes.
     * @param sz Number of bytes to read from the packet into the buffer.
     * @returns Buffer the data was read into.
     */
    std::vector<char> ReadArray(uint32_t sz);

    /**
     * Read @em sz bytes of data from the packet into @em buffer. After
     * reading the data, the current position in the packet will advance by
     * @em sz bytes.
     * @param buffer Buffer to store the data that was read from the packet.
     * @param sz Number of bytes to read from the packet into the buffer.
     */
    void ReadArray(void *buffer, uint32_t sz);

    /**
     * Read the string @em str encoded with @em encoding from the packet. The
     * string should contain a null terminator.
     * @param encoding String encoding used. Can be one of:
     * - ENCODING_UTF8 (Unicode)
     * - ENCODING_CP932 (Japanese)
     * - ENCODING_CP1252 (US English)
     * @returns The string that was read.
     * @sa WriteString
     * @sa ReadString16
     * @sa ReadString32
     */
    String ReadString(Convert::Encoding_t encoding);

    /**
     * Read the string @em str encoded with @em encoding from the packet. The
     * size of the string is determined by a 16-bit unsigned integer in host
     * byte order that is read first. This size includes any null terminator.
     * @param encoding String encoding used. Can be one of:
     * - ENCODING_UTF8 (Unicode)
     * - ENCODING_CP932 (Japanese)
     * - ENCODING_CP1252 (US English)
     * @returns The string that was read.
     * @sa WriteString16
     * @sa ReadString16Big
     * @sa ReadString16Little
     */
    String ReadString16(Convert::Encoding_t encoding);

    /**
     * Read the string @em str encoded with @em encoding from the packet. The
     * size of the string is determined by a 16-bit unsigned integer in big
     * endian byte order that is read first. This size includes any null
     * terminator.
     * @param encoding String encoding used. Can be one of:
     * - ENCODING_UTF8 (Unicode)
     * - ENCODING_CP932 (Japanese)
     * - ENCODING_CP1252 (US English)
     * @returns The string that was read.
     * @sa WriteString16
     * @sa ReadString16
     * @sa ReadString16Little
     */
    String ReadString16Big(Convert::Encoding_t encoding);

    /**
     * Read the string @em str encoded with @em encoding from the packet. The
     * size of the string is determined by a 16-bit unsigned integer in little
     * endian byte order that is read first. This size includes any null
     * terminator.
     * @param encoding String encoding used. Can be one of:
     * - ENCODING_UTF8 (Unicode)
     * - ENCODING_CP932 (Japanese)
     * - ENCODING_CP1252 (US English)
     * @returns The string that was read.
     * @sa WriteString16
     * @sa ReadString16
     * @sa ReadString16Big
     */
    String ReadString16Little(Convert::Encoding_t encoding);

    /**
     * Read the string @em str encoded with @em encoding from the packet. The
     * size of the string is determined by a 32-bit unsigned integer in host
     * byte order that is read first. This size includes any null terminator.
     * @param encoding String encoding used. Can be one of:
     * - ENCODING_UTF8 (Unicode)
     * - ENCODING_CP932 (Japanese)
     * - ENCODING_CP1252 (US English)
     * @returns The string that was read.
     * @sa WriteString32
     * @sa ReadString32Big
     * @sa ReadString32Little
     */
    String ReadString32(Convert::Encoding_t encoding);

    /**
     * Read the string @em str encoded with @em encoding from the packet. The
     * size of the string is determined by a 32-bit unsigned integer in big
     * endian byte order that is read first. This size includes any null
     * terminator.
     * @param encoding String encoding used. Can be one of:
     * - ENCODING_UTF8 (Unicode)
     * - ENCODING_CP932 (Japanese)
     * - ENCODING_CP1252 (US English)
     * @returns The string that was read.
     * @sa WriteString32
     * @sa ReadString32
     * @sa ReadString32Little
     */
    String ReadString32Big(Convert::Encoding_t encoding);

    /**
     * Read the string @em str encoded with @em encoding from the packet. The
     * size of the string is determined by a 32-bit unsigned integer in little
     * endian byte order that is read first. This size includes any null
     * terminator.
     * @param encoding String encoding used. Can be one of:
     * - ENCODING_UTF8 (Unicode)
     * - ENCODING_CP932 (Japanese)
     * - ENCODING_CP1252 (US English)
     * @returns The string that was read.
     * @sa WriteString32
     * @sa ReadString32
     * @sa ReadString32Big
     */
    String ReadString32Little(Convert::Encoding_t encoding);

    /**
     * Read a 8-bit unsigned integer from the packet but do not advance the
     * current position in the packet. Useful for determining a length (e.g. of
     * a string) or version of the packet before actually doing read code.
     * @returns Value at the current position in the packet.
     * @sa ReadU8
     */
    uint8_t PeekU8();

    /**
     * Read a 16-bit unsigned integer in host byte order from the packet but do
     * not advance the current position in the packet. Useful for determining a
     * length (e.g. of a string) or version of the packet before actually doing
     * read code.
     * @returns Value at the current position in the packet.
     * @sa ReadU16
     * @sa PeekU16Big
     * @sa PeekU16Little
     */
    uint16_t PeekU16();

    /**
     * Read a 16-bit unsigned integer in big endian byte order from the packet
     * but do not advance the current position in the packet. Useful for
     * determining a length (e.g. of a string) or version of the packet before
     * actually doing read code.
     * @returns Value at the current position in the packet.
     * @sa ReadU16
     * @sa PeekU16
     * @sa PeekU16Little
     */
    uint16_t PeekU16Big();

    /**
     * Read a 16-bit unsigned integer in little endian byte order from the
     * packet but do not advance the current position in the packet. Useful for
     * determining a length (e.g. of a string) or version of the packet before
     * actually doing read code.
     * @returns Value at the current position in the packet.
     * @sa ReadU16
     * @sa PeekU16
     * @sa PeekU16Big
     */
    uint16_t PeekU16Little();

    /**
     * Read a 32-bit unsigned integer in host byte order from the packet but do
     * not advance the current position in the packet. Useful for determining a
     * length (e.g. of a string) or version of the packet before actually doing
     * read code.
     * @returns Value at the current position in the packet.
     * @sa ReadU32
     * @sa PeekU32Big
     * @sa PeekU32Little
     */
    uint32_t PeekU32();

    /**
     * Read a 32-bit unsigned integer in big endian byte order from the packet
     * but do not advance the current position in the packet. Useful for
     * determining a length (e.g. of a string) or version of the packet before
     * actually doing read code.
     * @returns Value at the current position in the packet.
     * @sa ReadU32
     * @sa PeekU32
     * @sa PeekU32Little
     */
    uint32_t PeekU32Big();

    /**
     * Read a 32-bit unsigned integer in little endian byte order from the
     * packet but do not advance the current position in the packet. Useful for
     * determining a length (e.g. of a string) or version of the packet before
     * actually doing read code.
     * @returns Value at the current position in the packet.
     * @sa ReadU32
     * @sa PeekU32
     * @sa PeekU32Big
     */
    uint32_t PeekU32Little();

    /**
     * Read a 8-bit unsigned integer from the packet and advance the current
     * position in the packet.
     * @returns Value at the current position in the packet.
     * @sa PeekU8
     * @sa WriteU8
     */
    uint8_t ReadU8();

    /**
     * Read a 8-bit signed integer from the packet and advance the current
     * position in the packet.
     * @returns Value at the current position in the packet.
     * @sa PeekS8
     * @sa WriteS8
     */
    int8_t ReadS8();

    /**
     * Read a 16-bit unsigned integer in host byte order from the packet and
     * advance the current position in the packet.
     * @returns Value at the current position in the packet.
     * @sa PeekU16
     * @sa WriteU16
     * @sa ReadU16Big
     * @sa ReadU16Little
     */
    uint16_t ReadU16();

    /**
     * Read a 16-bit unsigned integer in big endian byte order from the packet
     * and advance the current position in the packet.
     * @returns Value at the current position in the packet.
     * @sa PeekU16Big
     * @sa WriteU16Big
     * @sa ReadU16
     * @sa ReadU16Little
     */
    uint16_t ReadU16Big();

    /**
     * Read a 16-bit unsigned integer in little endian byte order from the
     * packet and advance the current position in the packet.
     * @returns Value at the current position in the packet.
     * @sa PeekU16Little
     * @sa WriteU16Little
     * @sa ReadU16
     * @sa ReadU16Big
     */
    uint16_t ReadU16Little();

    /**
     * Read a 16-bit signed integer in host byte order from the packet and
     * advance the current position in the packet.
     * @returns Value at the current position in the packet.
     * @sa WriteS16
     * @sa ReadS16Big
     * @sa ReadS16Little
     */
    int16_t ReadS16();

    /**
     * Read a 16-bit signed integer in big endian byte order from the packet
     * and advance the current position in the packet.
     * @returns Value at the current position in the packet.
     * @sa WriteS16Big
     * @sa ReadS16
     * @sa ReadS16Little
     */
    int16_t ReadS16Big();

    /**
     * Read a 16-bit signed integer in little endian byte order from the packet
     * and advance the current position in the packet.
     * @returns Value at the current position in the packet.
     * @sa WriteS16Little
     * @sa ReadS16
     * @sa ReadS16Big
     */
    int16_t ReadS16Little();

    /**
     * Read a 32-bit unsigned integer in host byte order from the packet and
     * advance the current position in the packet.
     * @returns Value at the current position in the packet.
     * @sa PeekU32
     * @sa WriteU32
     * @sa ReadU32Big
     * @sa ReadU32Little
     */
    uint32_t ReadU32();

    /**
     * Read a 32-bit unsigned integer in big endian byte order from the packet
     * and advance the current position in the packet.
     * @returns Value at the current position in the packet.
     * @sa PeekU32Big
     * @sa WriteU32Big
     * @sa ReadU32
     * @sa ReadU32Little
     */
    uint32_t ReadU32Big();

    /**
     * Read a 32-bit unsigned integer in little endian byte order from the
     * packet and advance the current position in the packet.
     * @returns Value at the current position in the packet.
     * @sa PeekU32Little
     * @sa WriteU32Little
     * @sa ReadU32
     * @sa ReadU32Big
     */
    uint32_t ReadU32Little();

    /**
     * Read a 32-bit signed integer in host byte order from the packet and
     * advance the current position in the packet.
     * @returns Value at the current position in the packet.
     * @sa WriteS32
     * @sa ReadS32Big
     * @sa ReadS32Little
     */
    int32_t ReadS32();

    /**
     * Read a 32-bit signed integer in big endian byte order from the packet
     * and advance the current position in the packet.
     * @returns Value at the current position in the packet.
     * @sa WriteS32Big
     * @sa ReadS32
     * @sa ReadS32Little
     */
    int32_t ReadS32Big();

    /**
     * Read a 32-bit signed integer in little endian byte order from the packet
     * and advance the current position in the packet.
     * @returns Value at the current position in the packet.
     * @sa WriteS32Little
     * @sa ReadS32
     * @sa ReadS32Big
     */
    int32_t ReadS32Little();

    /**
     * Read a 64-bit unsigned integer in host byte order from the packet and
     * advance the current position in the packet.
     * @returns Value at the current position in the packet.
     * @sa WriteU64
     * @sa ReadU64Big
     * @sa ReadU64Little
     */
    uint64_t ReadU64();

    /**
     * Read a 64-bit unsigned integer in big endian byte order from the packet
     * and advance the current position in the packet.
     * @returns Value at the current position in the packet.
     * @sa WriteU64Big
     * @sa ReadU64
     * @sa ReadU64Little
     */
    uint64_t ReadU64Big();

    /**
     * Read a 64-bit unsigned integer in little endian byte order from the
     * packet and advance the current position in the packet.
     * @returns Value at the current position in the packet.
     * @sa WriteU64Little
     * @sa ReadU64
     * @sa ReadU64Big
     */
    uint64_t ReadU64Little();

    /**
     * Read a 64-bit signed integer in host byte order from the packet and
     * advance the current position in the packet.
     * @returns Value at the current position in the packet.
     * @sa WriteS64
     * @sa ReadS64Big
     * @sa ReadS64Little
     */
    int64_t ReadS64();

    /**
     * Read a 64-bit signed integer in big endian byte order from the packet
     * and advance the current position in the packet.
     * @returns Value at the current position in the packet.
     * @sa WriteS64Big
     * @sa ReadS64
     * @sa ReadS64Little
     */
    int64_t ReadS64Big();

    /**
     * Read a 64-bit signed integer in little endian byte order from the packet
     * and advance the current position in the packet.
     * @returns Value at the current position in the packet.
     * @sa WriteS64Little
     * @sa ReadS64
     * @sa ReadS64Big
     */
    int64_t ReadS64Little();

    /**
     * Read a 32-bit floating point number from the packet and advance the
     * current position in the packet.
     * @returns Value at the current position in the packet.
     * @sa WriteFloat
     */
    float ReadFloat();

    /**
     * Seek to @em pos bytes from the beginning of the packet. The position
     * specified must be within the range of the data in the packet; otherwise,
     * a @ref PacketException will be thrown. The valid range is 0 bytes to
     * @ref Size() bytes (inclusive).
     * @param pos Number of bytes from the start of the packet data.
     */
    void Seek(uint32_t pos);

    /**
     * Advance @em bytes from the current position. This is essentially a seek
     * from the current position instead of the beginning of the packet. The
     * valid range of this method is from 0 bytes to @ref Left() bytes
     * (inclusive). If the value is outside this range, a @ref PacketException
     * will be thrown.
     * @param bytes Number of bytes to advance the current position.
     */
    void Skip(uint32_t bytes);

    /**
     * Rewind the current position to the beginning of the packet.
     */
    void Rewind();

    /**
     * Erase all data after the current position effectively making the packet
     * size equal to the current position.
     */
    void EraseRight();

    /**
     * Rewind the current position by @em bytes. The range of this method
     * should be from 0 bytes to @ref Tell() bytes. If the value is outside of
     * this range, a @ref PacketException will be thrown.
     * @param bytes Number of bytes to rewind the current position.
     */
    void Rewind(uint32_t bytes);

    /**
     * Empty the packet. This will set the size of the packet to zero and
     * rewind the current position back to the beginning of the packet.
     */
    void Clear();

    /**
     * Set the current position to the end of the packet. This will set
     * @ref Tell() equal to @ref Size().
     */
    void End();

    /**
     * Produce a hex dump of the packet in the log.
     */
    void HexDump() const;

    /**
     * Generate a string that contains a hex dump of the packet.
     * @returns String that contains the hex dump.
     */
    String Dump() const;

    /**
     * Return the current position in the packet.
     * @returns Current position in the packet.
     */
    uint32_t Tell() const;

    /**
     * Return the number of bytes left to be read. This is the number of bytes
     * between the current position and the size of the packet.
     * @returns Number of bytes left in the packet.
     */
    uint32_t Left() const;

    /**
     * Returns the number of bytes in the packet.
     * @returns Number of bytes in the packet.
     */
    uint32_t Size() const;

    /**
     * Returns the number of bytes in the packet.
     * @returns Number of bytes in the packet.
     */
    uint32_t Length() const;

    /**
     * Write @em sz bytes of packet data from the current position into the
     * packet @em other. If the number of bytes is less than @ref left(), a
     * PacketException will be thrown. If the other packet can't write @em sz
     * bytes, a PacketException will be thrown. The other packet will be
     * cleared and the current position will be reset to the beginning of the
     * packet.
     * @param other Packet to write the data into.
     * @param sz Number of bytes to copy into the other packet.
     */
    void Split(Packet& other, uint32_t sz);

    /**
     * Return direct access to the underlying packet buffer. Avoid using this
     * function as modifying or deleting the buffer can cause bugs. With
     * direct access, you must do your own bounds checking. An alternative
     * is to use @ref seek() or @ref rewind() and then call @ref readArray()
     * to copy the data out of the packet.
     * @returns Pointer to the packet data.
     */
    char* Data() const;

    /**
     * Return direct access to the underlying packet buffer and set the packet
     * size to @em sz bytes. Avoid using this function as modifying or deleting
     * the buffer can cause bugs. With direct access, you must do your own
     * bounds checking. An alternative is to use @ref Seek() or @ref Rewind()
     * and then call @ref ReadArray() or @ref WriteArray() to copy the data out
     * of or into the packet.
     * @param sz Number of bytes to set the packet size to.
     * @returns Pointer to the packet data.
     */
    char* Direct(uint32_t sz);

    /**
     * %Decompress from the cursor position @em sz bytes. After the
     * decompression the current position will remain the same.
     * @param sz Number of bytes to decompress.
     * @returns The uncompressed size or 0 if the decompression failed.
     */
    int32_t Decompress(int32_t sz);

    /**
     * %Compress from the cursor position @em sz bytes. After the compression
     * the current position will remain the same.
     * @param sz Number of bytes to compress.
     * @returns The compressed size or 0 if the compression failed.
     */
    int32_t Compress(int32_t sz);

    /**
     * @brief Ensure the packet data buffer is allocated.
     */
    void Allocate();

    /**
     * @brief Move the packet data from another Packet object into this one.
     * @param other Packet object to move the data from.
     */
    Packet& operator=(Packet&& other);

private:
    /**
     * Add @em count bytes to the size of the packet. If this exceeds the
     * maximum size of the packet, a PacketException will be thrown.
     * @param count Number of bytes to add to the packet.
     */
    void GrowPacket(uint32_t count);

    /// Current position in the packet.
    uint32_t mPosition;

    /// Size of the packet.
    uint32_t mSize;

    /// Pointer to the packet data.
    uint8_t *mData;
};

} // namespace libcomp

#endif // LIBCOMP_SRC_PACKET_H
