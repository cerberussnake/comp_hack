/**
 * @file libcomp/tests/DiffieHellman.cpp
 * @ingroup libcomp
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Test the Diffie-Hellman key exchange.
 *
 * This file is part of the COMP_hack Library (libcomp).
 *
 * Copyright (C) 2014-2016 COMP_hack Team <compomega@tutanota.com>
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

#include <PushIgnore.h>
#include <gtest/gtest.h>
#include <PopIgnore.h>

#include <TcpServer.h>
#include <TcpConnection.h>

using namespace libcomp;

TEST(DiffieHellman, GenerateSaveLoad)
{
    DH *pDiffieHellman = TcpServer::GenerateDiffieHellman();
    ASSERT_NE(pDiffieHellman, nullptr);

    String prime = TcpConnection::GetDiffieHellmanPrime(pDiffieHellman);
    ASSERT_EQ(prime.Length(), DH_KEY_HEX_SIZE);

    DH *pCopy = TcpServer::CopyDiffieHellman(pDiffieHellman);
    ASSERT_NE(pCopy, nullptr);

    std::vector<char> data = TcpServer::SaveDiffieHellman(pDiffieHellman);
    ASSERT_EQ(data.size(), DH_SHARED_DATA_SIZE);

    ASSERT_EQ(TcpConnection::GetDiffieHellmanPrime(pCopy), prime);

    DH_free(pDiffieHellman);
    DH_free(pCopy);

    pDiffieHellman = TcpServer::LoadDiffieHellman(data);
    ASSERT_NE(pDiffieHellman, nullptr);

    ASSERT_EQ(TcpConnection::GetDiffieHellmanPrime(pDiffieHellman), prime);

    DH_free(pDiffieHellman);
}

TEST(DiffieHellman, KeyExchange)
{
    DH *pClient = nullptr;
    DH *pServer = nullptr;

    // (server=>client) First packet.
    // Sends base, prime, and server public.
    pServer = TcpServer::GenerateDiffieHellman();
    ASSERT_NE(pServer, nullptr);

    String prime = TcpConnection::GetDiffieHellmanPrime(pServer);
    ASSERT_EQ(prime.Length(), DH_KEY_HEX_SIZE);

    String serverPublic = TcpConnection::GenerateDiffieHellmanPublic(pServer);
    ASSERT_EQ(serverPublic.Length(), DH_KEY_HEX_SIZE);

    // (client=>server) Second packet.
    // Gets base, prime, and server public.
    // Sends client public.
    // Calculates client copy of shared data.
    pClient = TcpServer::LoadDiffieHellman(prime);
    ASSERT_NE(pClient, nullptr);

    String clientPublic = TcpConnection::GenerateDiffieHellmanPublic(pClient);
    ASSERT_EQ(clientPublic.Length(), DH_KEY_HEX_SIZE);

    std::vector<char> clientData =
        TcpConnection::GenerateDiffieHellmanSharedData(pClient, serverPublic);
    ASSERT_EQ(clientData.size(), DH_SHARED_DATA_SIZE);

    // (server) Third packet.
    // Gets client public.
    // Calculates server copy of shared data.
    std::vector<char> serverData =
        TcpConnection::GenerateDiffieHellmanSharedData(pServer, clientPublic);
    ASSERT_EQ(serverData.size(), DH_SHARED_DATA_SIZE);

    // Check they have the same data.
    ASSERT_EQ(memcmp(&serverData[0], &clientData[0], serverData.size()), 0);

    DH_free(pClient);
    DH_free(pServer);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
