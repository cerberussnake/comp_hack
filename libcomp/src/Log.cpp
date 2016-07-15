/**
 * @file libcomp/src/Log.cpp
 * @ingroup libcomp
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Routines to log messages to the console and/or a file.
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

#include "Log.h"

#include <iostream>
#include <cassert>

using namespace libcomp;

/**
 * @internal
 * Singleton pointer for the Log class.
 */
static Log *gLogInst = nullptr;

/*
 * Black       0;30     Dark Gray     1;30
 * Blue        0;34     Light Blue    1;34
 * Green       0;32     Light Green   1;32
 * Cyan        0;36     Light Cyan    1;36
 * Red         0;31     Light Red     1;31
 * Purple      0;35     Light Purple  1;35
 * Brown       0;33     Yellow        1;33
 * Light Gray  0;37     White         1;37
 */

/**
 * Log hook to send all log messages to standard output. This hook will color
 * all log messages depending on their log level.
 * @param level Numeric level representing the log level.
 * @param msg The message to write to standard output.
 * @param pUserData User defined data that was passed with the hook to
 * @ref Log::AddLogHook.
 */
static void LogToStandardOutput(Log::Level_t level,
    const String& msg, void *pUserData)
{
    // Console colors for each log level.
    static const String gLogColors[Log::LOG_LEVEL_COUNT] = {
        "\e[1;32;40m", // Debug
        "\e[37;40m",   // Info
        "\e[1;33;40m", // Warning
        "\e[1;31;40m", // Error
        "\e[1;37;41m", // Critical
    };

    // This hook has no user data.
    (void)pUserData;

    if(0 > level || Log::LOG_LEVEL_COUNT <= level)
    {
        level = Log::LOG_LEVEL_CRITICAL;
    }

    // Split the message into lines. Each line will be individually colored.
    std::list<String> msgs = msg.Split("\n");
    String last = msgs.back();
    msgs.pop_back();

    // Each log level has a different color scheme.
    for(String m : msgs)
    {
        std::cout << gLogColors[level] << m.ToUtf8()
            << "\e[0K\e[0m" << std::endl;
    }

    // If there is more on the last line, print it as well.
    if(!last.IsEmpty())
    {
        std::cout << gLogColors[level] << last.ToUtf8() << "\e[0K\e[0m";
    }

    // Flush the output so the log messages are immediately avaliable.
    std::cout.flush();
}

Log::Log() : mLogFile(nullptr)
{
    // Default all log levels to enabled.
    for(int i = 0; i < LOG_LEVEL_COUNT; ++i)
    {
        mLogEnables[i] = true;
    }
}

Log::~Log()
{
    // Lock the muxtex.
    std::lock_guard<std::mutex> lock(mLock);

    // Clear the last line before the server exits.
    std::cout << "\e[0K\e[0m";

    // Close the log file.
    delete mLogFile;
    mLogFile = nullptr;

    // Remove the singleton pointer.
    gLogInst = nullptr;
}

Log* Log::GetSingletonPtr()
{
    // If the singleton does not exist, create it and ensure there is a valid
    // pointer before returning it.
    if(nullptr == gLogInst)
    {
        gLogInst = new Log;
    }

    assert(nullptr != gLogInst);

    return gLogInst;
}

void Log::LogMessage(Log::Level_t level, const String& msg)
{
    // Prepend these to messages.
    static const String gLogMessages[Log::LOG_LEVEL_COUNT] = {
        "DEBUG: %1",
        "%1",
        "WARNING: %1",
        "ERROR: %1",
        "CRITICAL: %1",
    };

    // Log a critical error message. If the configuration option is true, log
    // the message to the log file. Regardless, pass the message to all the
    // log hooks for processing. Critical messages have the text "CRITICAL: "
    // appended to them.
    if(0 > level || LOG_LEVEL_COUNT <= level || !mLogEnables[level])
        return;

    String final = String(gLogMessages[level]).Arg(msg);

    // Lock the muxtex.
    std::lock_guard<std::mutex> lock(mLock);

    if(nullptr != mLogFile)
    {
        std::vector<char> data = final.Data();

        mLogFile->write(&data[0],
            (std::streamsize)(data.size() * sizeof(char)));
        mLogFile->flush();
    }

    // Call all hooks.
    for(auto i : mHooks)
    {
        (*i.first)(level, final, i.second);
    }
}

String Log::GetLogPath() const
{
    return mLogPath;
}

void Log::SetLogPath(const String& path)
{
    // Set the log path.
    mLogPath = path;

    // Lock the muxtex.
    std::lock_guard<std::mutex> lock(mLock);

    // Close the old log file if it's open.
    if(nullptr != mLogFile)
    {
        delete mLogFile;
        mLogFile = nullptr;
    }

    // If the log path isn't empty, create a new log file. The file will be
    // truncated first.
    if(!mLogPath.IsEmpty())
    {
        mLogFile = new std::ofstream();
        mLogFile->open(mLogPath.C(), std::ofstream::out |
            std::ofstream::trunc);
        mLogFile->flush();

        // If this failed, close it.
        if(!mLogFile->good())
        {
            delete mLogFile;
            mLogFile = nullptr;
            mLogPath.Clear();
        }
    }
}

void Log::AddLogHook(Log::Hook_t func, void *data)
{
    // Lock the muxtex.
    std::lock_guard<std::mutex> lock(mLock);

    // Add the specified log hook.
    mHooks[func] = data;
}

void Log::AddStandardOutputHook()
{
    // Add the default hook to log all messages to the terminal.
    AddLogHook(&LogToStandardOutput);
}

void Log::ClearHooks()
{
    // Lock the muxtex.
    std::lock_guard<std::mutex> lock(mLock);

    // Remove all hooks.
    mHooks.clear();
}

bool Log::GetLogLevelEnabled(Level_t level) const
{
    // Sanity check.
    if(0 > level || LOG_LEVEL_COUNT <= level)
    {
        return false;
    }

    // Get if the level is enabled.
    return mLogEnables[level];
}

void Log::SetLogLevelEnabled(Level_t level, bool enabled)
{
    // Sanity check.
    if(0 > level || LOG_LEVEL_COUNT <= level)
    {
        return;
    }

    // Lock the mutex.
    std::lock_guard<std::mutex> lock(mLock);

    // Set if the level is enabled.
    mLogEnables[level] = enabled;
}
