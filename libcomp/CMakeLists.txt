# This file is part of COMP_hack.
#
# Copyright (C) 2010-2016 COMP_hack Team <compomega@tutanota.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(libcomp)

FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(OpenSSL REQUIRED)

MESSAGE("** Configuring ${PROJECT_NAME} **")

# Add a target to generate API documentation.
FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/api/libcomp)
#GENERATE_DOCS(doc/Doxyfile.in)

INCLUDE_DIRECTORIES(${ZLIB_INCLUDES})
INCLUDE_DIRECTORIES(${LIBCOMP_INCLUDES})
INCLUDE_DIRECTORIES(${ASIO_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${SQUIRREL_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${SQRAT_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${TTVFS_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${SQLITE3_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${LIBUV_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CASSANDRA_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

SET(SYSTEM_LIBRARIES "")

IF(BSD)
FIND_PACKAGE(ExecInfo REQUIRED)

SET(SYSTEM_LIBRARIES ${SYSTEM_LIBRARIES} ${EXECINFO_LIBRARY})
ENDIF(BSD)

SET(${PROJECT_NAME}_SRCS
    src/Compress.cpp
    src/Convert.cpp
    src/Database.cpp
    src/DatabaseCassandra.cpp
    src/DatabaseQuery.cpp
    src/DatabaseQueryCassandra.cpp
    src/DatabaseSQLite3.cpp
    src/Decrypt.cpp
    #src/EngineLocker.cpp
    src/Exception.cpp
    src/LobbyConnection.cpp
    src/Log.cpp
    #src/MemoryFile.cpp
    src/MessagePacket.cpp
    src/Packet.cpp
    src/PacketException.cpp
    #src/PacketScript.cpp
    src/PlatformWindows.cpp
    #src/PEFile.cpp
    src/ReadOnlyPacket.cpp
    src/RingBuffer.cpp
    src/ScriptEngine.cpp
    src/String.cpp
    #src/Structgen.cpp
    src/TcpConnection.cpp
    src/TcpServer.cpp
    #src/ThreadManager.cpp
    #src/XmlUtils.cpp

    #src/MiCIconData.cpp
    #src/MiCIconDataScript.cpp
    #src/MiCItemData.cpp
    #src/MiCItemDataScript.cpp
    #src/MiDynamicMapData.cpp
    #src/MiDynamicMapDataScript.cpp
    #src/MiSpotData.cpp
    #src/MiSpotDataScript.cpp
    #src/MiZoneData.cpp
    #src/MiZoneDataScript.cpp
)

IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/ConfigCustom.cpp)
    SET(${PROJECT_NAME}_SRCS ${${PROJECT_NAME}_SRCS} src/ConfigCustom.cpp)
ELSE()
    SET(${PROJECT_NAME}_SRCS ${${PROJECT_NAME}_SRCS} src/Config.cpp)
ENDIF()

# This is a list of all header files. Adding the header files here ensures they
# are listed in the source files for IDE projects.
SET(${PROJECT_NAME}_HDRS
    src/Compress.h
    src/Constants.h
    src/Convert.h
    src/Database.h
    src/DatabaseCassandra.h
    src/DatabaseQuery.h
    src/DatabaseQueryCassandra.h
    src/DatabaseSQLite3.h
    src/Decrypt.h
    src/Endian.h
    #src/EngineLocker.h
    src/Exception.h
    src/LobbyConnection.h
    src/Log.h
    #src/MemoryFile.h
    src/Message.h
    src/MessagePacket.h
    src/MessageQueue.h
    src/Packet.h
    src/PacketException.h
    #src/PacketScript.h
    #src/PEFile.h
    #src/PEFormat.h
    #src/Platform.h
    #src/PlatformLinux.h
    #src/PlatformWindows.h
    src/ReadOnlyPacket.h
    src/RingBuffer.h
    src/ScriptEngine.h
    src/String.h
    #src/Structgen.h
    src/TcpConnection.h
    src/TcpServer.h
    #src/ThreadManager.h
    #src/XmlUtils.h

    # These were generated and are not worth reading.
    src/LookupTableCP1252.h
    src/LookupTableCP932.h

    # Macros to temporarily disable warnings (wrapped around 3rd party libs).
    src/PushIgnore.h
    src/PopIgnore.h

    #src/MiCIconData.h
    #src/MiCIconDataScript.h
    #src/MiCItemData.h
    #src/MiCItemDataScript.h
    #src/MiDynamicMapData.h
    #src/MiDynamicMapDataScript.h
    #src/MiSpotData.h
    #src/MiSpotDataScript.h
    #src/MiZoneData.h
    #src/MiZoneDataScript.h
)

SET(${PROJECT_NAME}_SCHEMA
    schema/cicondata.xml
    schema/citemdata.xml
    schema/dynamicmapdata.xml
    schema/spotdata.xml
    schema/zonedata.xml
)

# Make an absolute path version of the schema list for the channel server to use as depends
FOREACH(schema ${${PROJECT_NAME}_SCHEMA})
    SET(LIBFROST_SCHEMA ${LIBFROST_SCHEMA} ${CMAKE_CURRENT_SOURCE_DIR}/${schema})
ENDFOREACH(schema ${${PROJECT_NAME}_SCHEMA})

# Make it available to the parent scope
SET(LIBFROST_SCHEMA ${LIBFROST_SCHEMA} PARENT_SCOPE)

#STRUCTGEN_XML(${PROJECT_NAME}_STRUCTS
SET(${PROJECT_NAME}_STRUCTS_REM
    # Main schema
    schema/master.xml

    # Included schemas
    ${${PROJECT_NAME}_SCHEMA}

    # Include paths
    # <none>

    # Output files
    MiCIconDataPriv.h
    MiCIconDataPriv.cpp
    MiCIconDataPrivScript.h
    MiCIconDataPrivScript.cpp
    MiCItemDataPriv.h
    MiCItemDataPriv.cpp
    MiCItemDataPrivScript.h
    MiCItemDataPrivScript.cpp
    MiCItemBaseData.h
    MiCItemBaseDataScript.h
    MiCItemBaseDataScript.cpp
    MiCItemMotionData.h
    MiCItemMotionDataScript.h
    MiCItemMotionDataScript.cpp
    MiCItemSPEffectData.h
    MiCItemSPEffectDataScript.h
    MiCItemSPEffectDataScript.cpp

    MiDynamicMapDataPriv.h
    MiDynamicMapDataPriv.cpp
    MiDynamicMapDataPrivScript.h
    MiDynamicMapDataPrivScript.cpp

    MiSpotDataPriv.h
    MiSpotDataPriv.cpp
    MiSpotDataPrivScript.h
    MiSpotDataPrivScript.cpp

    MiZoneDataPriv.h
    MiZoneDataPriv.cpp
    MiZoneDataPrivScript.h
    MiZoneDataPrivScript.cpp
    MiZoneBasicData.h
    MiZoneBasicDataScript.h
    MiZoneBasicDataScript.cpp
    MiZoneFileData.h
    MiZoneFileDataScript.h
    MiZoneFileDataScript.cpp
    MiZoneFogData.h
    MiZoneFogDataScript.h
    MiZoneFogDataScript.cpp
    MiZoneCameraData.h
    MiZoneCameraDataScript.h
    MiZoneCameraDataScript.cpp
    MiZoneSkyData.h
    MiZoneSkyDataScript.h
    MiZoneSkyDataScript.cpp
    MiZoneGouraudData.h
    MiZoneGouraudDataScript.h
    MiZoneGouraudDataScript.cpp
    MiZoneLensFlareData.h
    MiZoneLensFlareDataScript.h
    MiZoneLensFlareDataScript.cpp
    MiZoneLensFlareLayerData.h
    MiZoneLensFlareLayerDataScript.h
    MiZoneLensFlareLayerDataScript.cpp
    MiZoneBGMData.h
    MiZoneBGMDataScript.h
    MiZoneBGMDataScript.cpp
    MiZoneOtherData.h
    MiZoneOtherDataScript.h
    MiZoneOtherDataScript.cpp
)

ADD_LIBRARY(comp STATIC ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS}
    ${${PROJECT_NAME}_MOC_SRCS} ${${PROJECT_NAME}_STRUCTS})

TARGET_LINK_LIBRARIES(comp ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES}
    ${SYSTEM_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${SQLITE3_LIBRARIES}
    squirrel sqstdlib ttvfs ttvfs_zip cassandra ${LIBUV_LIBRARIES})

# List of unit tests to add to CTest.
SET(${PROJECT_NAME}_TEST_SRCS
    Cassandra
    Convert
    Decrypt
    DiffieHellman
    Packet
    ScriptEngine
    String
    #XmlUtils
)

IF(NOT BSD)
    # Add the unit tests.
    CREATE_GTESTS(LIBS comp SRCS ${${PROJECT_NAME}_TEST_SRCS})
ENDIF(NOT BSD)
