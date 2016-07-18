/**
 * @file libcomp/src/Constants.h
 * @ingroup libcomp
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Constant values used throughout the applications.
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

#ifndef LIBCOMP_SRC_CONSTANTS_H
#define LIBCOMP_SRC_CONSTANTS_H

namespace libcomp
{

/// Size of the stack that is used to talk to a Squirrel VM.
#define SQUIRREL_STACK_SIZE (1024)

/// Maximum number of bytes in a packet.
#define MAX_PACKET_SIZE (16384)

/// Maximum number of calls to trace when generating the backtrace.
#define MAX_BACKTRACE_DEPTH (100)

/// Maximum number of clients that can be connected at a given time.
#define MAX_CLIENT_CONNECTIONS (4096)

/// Maximum supported character level.
#define MAX_LEVEL (99)

/// Number of expertise.
#define EXPERTISE_COUNT (38)

/// Time until the client is expected to timeout and disconnect (in seconds).
#define TIMEOUT_CLIENT (15)

/// Time until the server should close the socket (in seconds).
/// It is the assumed the client has reached @ref TIMEOUT_CLIENT and due to
/// network issues is not able to confirm socket closure on their end.
#define TIMEOUT_SOCKET (17)

/// Chat message is only visible by the person who sent it.
#define CHAT_VISIBILITY_SELF   (0)

/// Chat message is only visible to characters near the sender.
#define CHAT_VISIBILITY_RANGE  (1)

/// Chat message is only visible to characters in the same zone.
#define CHAT_VISIBILITY_ZONE   (2)

/// Chat message is only visible to characters in the same party.
#define CHAT_VISIBILITY_PARTY  (3)

/// Chat message is only visible to characters in the same clan.
#define CHAT_VISIBILITY_KLAN   (4)

/// Chat message is only visible to characters on the same PvP team.
#define CHAT_VISIBILITY_TEAM   (5)

/// Chat message is visible to all characters.
#define CHAT_VISIBILITY_GLOBAL (6)

/// Chat message is visible to all GMs.
#define CHAT_VISIBILITY_GMS    (7)

/// Number of G1 times stored.
#define G1_TIME_COUNT (18)

/// Number of friends that can be registered in the friend list.
#define FRIEND_COUNT (100)

/// Number of valuable mask bytes to send to the client.
#define VALUABLE_MASK_COUNT (64)

/// Max number of valuable mask bytes stored in the character data.
#define VALUABLE_MASK_MAX   (64)

/// Number of quest mask bytes to send to the client.
#define QUEST_MASK_COUNT (128)

/// Max number of quest mask bytes stored in the character data.
#define QUEST_MASK_MAX   (512)

/// Highest possible account ID (lower to preserve the PC registry IDs).
#define MAX_ACCOUNT_ID (0x07FFFFFF)

/// Maximum number of characters for an account.
#define MAX_CHARACTER (20)

/// How many item storage boxes are avaliable via the user interface.
#define ITEM_BOX_COUNT (10)

/// The skill activation contains no extra information.
#define ACTIVATION_NOTARGET (0)

/// The skill activation contains a target UID.
#define ACTIVATION_TARGET   (1)

/// The skill activation contains a demon UID.
#define ACTIVATION_DEMON    (2)

/// The skill activation contains an item UID.
#define ACTIVATION_ITEM     (3)

/// The maximum skill activation extra info type value.
#define ACTIVATION_MAX      (4)

/// Cost type for HP.
#define COST_TYPE_HP   (0)

/// Cost type for MP.
#define COST_TYPE_MP   (1)

/// Cost type for an item.
#define COST_TYPE_ITEM (2)

/// Maximum cost type value.
#define COST_TYPE_MAX  (3)

/// A fixed cost number.
#define COST_NUMREP_FIXED   (0)

/// A percentage cost number.
#define COST_NUMREP_PERCENT (1)

/// Maximum cost number representation value.
#define COST_NUMREP_MAX     (2)

/// Client has not attempted to login yet.
#define LOGIN_STATE_WAIT (0)

/// Client has sent a valid account name.
#define LOGIN_STATE_VALID_ACCT (1)

/// Client has authenticated.
#define LOGIN_STATE_AUTHENTICATED (2)

/// Client has sent a P0004_sendData packet.
#define LOGIN_STATE_GOT_SEND_DATA (3)

/// Character data has been loaded by the database thread.
#define LOGIN_STATE_HAVE_PCDATA (4)

/// Client sent a P0004_sendData packet and the character data has been loaded.
#define LOGIN_STATE_PENDING_REPLY (5)

/// Client has been notified of a successful login.
#define LOGIN_STATE_LOGGED_IN (6)

/// Client is attempting to log out.
#define LOGIN_STATE_PENDING_LOGOUT (7)

/**
 * Equip types used to determine the type of and where to place an equip.
 * @sa BfCharacterData::equip
 * @sa BfCharacterData::equipItem
 */
typedef enum _EquipType
{
    /// Item is not an equip
    EQUIP_TYPE_NONE = -1,
    /// Items like helmets, hats, or decorative flowers
    EQUIP_TYPE_HEAD = 0,
    /// Items like masks or cigarettes
    EQUIP_TYPE_FACE,
    /// Typically necklaces
    EQUIP_TYPE_NECK,
    /// Items like dresses, shirts, or costumes
    EQUIP_TYPE_TOP,
    /// Items like gloves or wrist bands
    EQUIP_TYPE_ARMS,
    /// Items like pants or skirts
    EQUIP_TYPE_BOTTOM,
    /// Items like shoes, boots, or sandals
    EQUIP_TYPE_FEET,
    /// Typically a COMP
    EQUIP_TYPE_COMP,
    /// Typically a ring
    EQUIP_TYPE_RING,
    /// Typically an earring
    EQUIP_TYPE_EARRING,
    /// Items like flaming auras or floating snow flakes
    EQUIP_TYPE_EXTRA,
    /// Items like cloaks or guitars
    EQUIP_TYPE_BACK,
    /// Items like talismans or charms
    EQUIP_TYPE_TALISMAN,
    /// Items like swords, guns, or staffs
    EQUIP_TYPE_WEAPON,
    /// Typically bullets
    EQUIP_TYPE_BULLETS
}EquipType;

} // namespace libcomp

#endif // LIBCOMP_SRC_CONSTANTS_H
