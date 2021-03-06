/**
 * @file libcomp/src/TcpConnection.cpp
 * @ingroup libcomp
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Base TCP/IP connection class.
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

#include "TcpConnection.h"

#include "Constants.h"
#include "Log.h"

using namespace libcomp;

TcpConnection::TcpConnection(asio::io_service& io_service) :
    mSocket(io_service), mDiffieHellman(nullptr), mStatus(
    TcpConnection::STATUS_NOT_CONNECTED), mRole(TcpConnection::ROLE_CLIENT),
    mRemoteAddress("0.0.0.0")
{
}

TcpConnection::TcpConnection(asio::ip::tcp::socket& socket,
    DH *pDiffieHellman) : mSocket(std::move(socket)),
    mDiffieHellman(pDiffieHellman), mStatus(TcpConnection::STATUS_CONNECTED),
    mRole(TcpConnection::ROLE_SERVER), mRemoteAddress("0.0.0.0")
{
    // Cache the remote address.
    try
    {
        mRemoteAddress = mSocket.remote_endpoint().address().to_string();
    }
    catch(...)
    {
        // Just use the cache.
    }
}

TcpConnection::~TcpConnection()
{
    if(nullptr != mDiffieHellman)
    {
        DH_free(mDiffieHellman);
    }
}

bool TcpConnection::Connect(const String& host, int port)
{
    bool result = false;

    // Modified from: http://stackoverflow.com/questions/5486113/
    asio::ip::tcp::resolver resolver(mSocket.get_io_service());

    // Setup the query with the given port (if any).
    asio::ip::tcp::resolver::query query(host.ToUtf8(),
        0 < port ? String("%1").Arg(port).ToUtf8() : std::string());

    // Resolve the hostname.
    asio::ip::tcp::resolver::iterator it = resolver.resolve(query);

    // If the hostname resolved, connect to it.
    if(asio::ip::tcp::resolver::iterator() != it)
    {
        Connect(*it);
        result = true;
    }

    return result;
}

void TcpConnection::SendPacket(Packet& packet)
{
    ReadOnlyPacket copy(packet);

    SendPacket(copy);
}
void TcpConnection::SendPacket(ReadOnlyPacket& packet)
{
    bool firstPacket;

    {
        std::lock_guard<std::mutex> guard(mOutgoingMutex);

        firstPacket = mOutgoingPackets.empty();
        mOutgoingPackets.push_back(std::move(packet));
    }

    if(firstPacket)
    {
        SendNextPacket();
    }
}

bool TcpConnection::RequestPacket(uint32_t size)
{
    bool result = false;

    // Make sure the buffer is there.
    mReceivedPacket.Allocate();

#ifdef COMP_HACK_DEBUG
    if(0 < mReceivedPacket.Size())
    {
        LOG_DEBUG(String("TcpConnection::RequestPacket() called when there is "
            "still %1 bytes in the buffer.\n").Arg(mReceivedPacket.Size()));
    }
#endif // COMP_HACK_DEBUG

    // Get direct access to the buffer.
    char *pDestination = mReceivedPacket.Data();

    if(0 != size && MAX_PACKET_SIZE >= (mReceivedPacket.Size() + size) &&
        nullptr != pDestination)
    {
        // Calculate where to write the data.
        pDestination += mReceivedPacket.Size();

        // Request packet data from the socket.
        mSocket.async_receive(asio::buffer(pDestination, size), 0,
            [this](asio::error_code errorCode, std::size_t length)
            {
                if(errorCode)
                {
                    SocketError();
                }
                else
                {
                    // Adjust the size of the packet.
                    (void)mReceivedPacket.Direct(mReceivedPacket.Size() +
                        length);
                    mReceivedPacket.Rewind();

                    // It's up to this callback to remove the data from the
                    // packet either by calling std::move() or packet.Clear().
                    PacketReceived(mReceivedPacket);

#ifdef COMP_HACK_DEBUG
                    if(0 < mReceivedPacket.Size())
                    {
                        LOG_DEBUG(String("TcpConnection::PacketReceived() "
                            "was called and it left %1 bytes in the buffer.\n"
                            ).Arg(mReceivedPacket.Size()));
                    }
#endif // COMP_HACK_DEBUG
                }
            });

        // Success.
        result = true;
    }

    return result;
}

TcpConnection::Role_t TcpConnection::GetRole() const
{
    return mRole;
}

TcpConnection::ConnectionStatus_t TcpConnection::GetStatus() const
{
    return mStatus;
}

String TcpConnection::GetRemoteAddress() const
{
    return mRemoteAddress;
}

void TcpConnection::SetSelf(const std::weak_ptr<libcomp::TcpConnection>& self)
{
    mSelf = self;
}

void TcpConnection::Connect(const asio::ip::tcp::endpoint& endpoint)
{
    mStatus = STATUS_CONNECTING;

    // Make sure we remove any remote address cache.
    mRemoteAddress = "0.0.0.0";

    mSocket.async_connect(endpoint, [this](const asio::error_code errorCode){
        if(errorCode)
        {
            mStatus = STATUS_NOT_CONNECTED;

            ConnectionFailed();
        }
        else
        {
            mStatus = STATUS_CONNECTED;

            // Cache the remote address.
            try
            {
                mRemoteAddress = mSocket.remote_endpoint().address(
                    ).to_string();
            }
            catch(...)
            {
                // Just use the cache.
            }

            ConnectionSuccess();
        }
    });
}

void TcpConnection::SendNextPacket()
{
    std::lock_guard<std::mutex> guard(mOutgoingMutex);

    if(!mOutgoingPackets.empty())
    {
        ReadOnlyPacket& packet = mOutgoingPackets.front();

        mSocket.async_send(asio::buffer(packet.ConstData(), packet.Size()), 0,
            [this](asio::error_code errorCode, std::size_t length)
            {
                bool sendAnother = false;
                bool packetOk = false;

                ReadOnlyPacket packet;

                if(errorCode)
                {
                    SocketError();
                }
                else
                {
                    std::lock_guard<std::mutex> guard(mOutgoingMutex);

                    if(mOutgoingPackets.empty() || length !=
                        mOutgoingPackets.front().Size())
                    {
                        SocketError();
                    }
                    else
                    {
                        packet = mOutgoingPackets.front();

                        mOutgoingPackets.pop_front();

                        sendAnother = !mOutgoingPackets.empty();
                        packetOk = true;
                    }
                }

                if(packetOk)
                {
                    PacketSent(packet);

                    if(sendAnother)
                    {
                        SendNextPacket();
                    }
                }
            });
    }
}

void TcpConnection::SocketError(const String& errorMessage)
{
    if(!errorMessage.IsEmpty())
    {
        LOG_ERROR(String("Socket error for client from %1:  %2\n").Arg(
            GetRemoteAddress()).Arg(errorMessage));
    }

    mSocket.close();
    mStatus = STATUS_NOT_CONNECTED;
}

void TcpConnection::ConnectionFailed()
{
}

void TcpConnection::ConnectionSuccess()
{
}

void TcpConnection::PacketSent(ReadOnlyPacket& packet)
{
    (void)packet;
}

void TcpConnection::PacketReceived(Packet& packet)
{
    packet.Clear();
}

void TcpConnection::SetEncryptionKey(const std::vector<char>& data)
{
    SetEncryptionKey(&data[0], data.size());
}

void TcpConnection::SetEncryptionKey(const void *pData, size_t dataSize)
{
    if(nullptr != pData && BF_NET_KEY_BYTE_SIZE <= dataSize)
    {
        BF_set_key(&mEncryptionKey, BF_NET_KEY_BYTE_SIZE,
            reinterpret_cast<const unsigned char*>(pData));
    }
}

String TcpConnection::GetDiffieHellmanPrime(const DH *pDiffieHellman)
{
    String prime;

    if(nullptr != pDiffieHellman && nullptr != pDiffieHellman->p)
    {
        char *pHexResult = BN_bn2hex(pDiffieHellman->p);

        if(nullptr != pHexResult)
        {
            prime = pHexResult;

            OPENSSL_free(pHexResult);

            if(DH_KEY_HEX_SIZE != prime.Length())
            {
                prime.Clear();
            }
        }
    }

    return prime;
}

String TcpConnection::GenerateDiffieHellmanPublic(DH *pDiffieHellman)
{
    String publicKey;

    if(nullptr != pDiffieHellman && nullptr != pDiffieHellman->p &&
        nullptr != pDiffieHellman->g && 1 == DH_generate_key(pDiffieHellman) &&
        nullptr != pDiffieHellman->pub_key)
    {
        char *pHexResult = BN_bn2hex(pDiffieHellman->pub_key);

        if(nullptr != pHexResult)
        {
            publicKey = String(pHexResult).RightJustified(
                DH_KEY_HEX_SIZE, '0');

            OPENSSL_free(pHexResult);
        }
    }

    return publicKey;
}

std::vector<char> TcpConnection::GenerateDiffieHellmanSharedData(
    DH *pDiffieHellman, const String& otherPublic)
{
    std::vector<char> data;

    unsigned char sharedData[DH_SHARED_DATA_SIZE];

    if(nullptr != pDiffieHellman && nullptr != pDiffieHellman->p &&
        nullptr != pDiffieHellman->g &&  nullptr != pDiffieHellman->pub_key &&
        DH_SHARED_DATA_SIZE == DH_size(pDiffieHellman) &&
        DH_KEY_HEX_SIZE == otherPublic.Length())
    {
        BIGNUM *pOtherPublic = nullptr;

        if(0 < BN_hex2bn(&pOtherPublic, otherPublic.C()) &&
            nullptr != pOtherPublic && BF_NET_KEY_BYTE_SIZE <=
            DH_compute_key(sharedData, pOtherPublic, pDiffieHellman))
        {
            data.insert(data.begin(),
                reinterpret_cast<const char*>(sharedData),
                reinterpret_cast<const char*>(sharedData) +
                DH_SHARED_DATA_SIZE);
        }

        if(nullptr != pOtherPublic)
        {
            BN_clear_free(pOtherPublic);
            pOtherPublic = nullptr;
        }
    }

    return data;
}

void TcpConnection::BroadcastPacket(const std::list<std::shared_ptr<
    TcpConnection>>& connections, Packet& packet)
{
    ReadOnlyPacket copy(packet);

    BroadcastPacket(connections, copy);
}

void TcpConnection::BroadcastPacket(const std::list<std::shared_ptr<
    TcpConnection>>& connections, ReadOnlyPacket& packet)
{
    for(auto connection : connections)
    {
        if(connection)
        {
            connection->SendPacket(packet);
        }
    }
}
