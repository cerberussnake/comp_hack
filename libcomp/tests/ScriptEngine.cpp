/**
 * @file libcomp/tests/ScriptEngine.cpp
 * @ingroup libcomp
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Test the Squirrel scripting language interface.
 *
 * This file is part of the COMP_hack Library (libcomp).
 *
 * Copyright (C) 2016 COMP_hack Team <compomega@tutanota.com>
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

#include <Log.h>
#include <ScriptEngine.h>

using namespace libcomp;

TEST(ScriptEngine, EvalCompileError)
{
    ScriptEngine engine;

    int errorCount = 0;

    // Check for a compile error that produces a log message.
    Log::GetSingletonPtr()->AddLogHook(
        [](Log::Level_t level, const String& msg, void *pUserData)
        {
            (void)msg;

            EXPECT_EQ(level, Log::LOG_LEVEL_ERROR);

            (*reinterpret_cast<int*>(pUserData))++;
        }, &errorCount);

    EXPECT_FALSE(engine.Eval("1=2"));
    EXPECT_EQ(errorCount, 1);

    Log::GetSingletonPtr()->ClearHooks();
}

TEST(ScriptEngine, EvalRuntimeError)
{
    ScriptEngine engine;

    int errorCount = 0;

    // Check for a runtime error that produces log messages.
    Log::GetSingletonPtr()->AddLogHook(
        [](Log::Level_t level, const String& msg, void *pUserData)
        {
            (void)msg;

            EXPECT_EQ(level, Log::LOG_LEVEL_ERROR);

            (*reinterpret_cast<int*>(pUserData))++;
        }, &errorCount);

    EXPECT_FALSE(engine.Eval("FunctionThatDoesNotExist()"));
    EXPECT_NE(errorCount, 0);

    Log::GetSingletonPtr()->ClearHooks();
}

TEST(ScriptEngine, EvalPrint)
{
    ScriptEngine engine;

    int messageCount = 0;

    // Check for a call to print() that produces a log message.
    Log::GetSingletonPtr()->AddLogHook(
        [](Log::Level_t level, const String& msg, void *pUserData)
        {
            EXPECT_EQ(msg, "SQUIRREL: Test\n");
            EXPECT_EQ(level, Log::LOG_LEVEL_INFO);

            (*reinterpret_cast<int*>(pUserData))++;
        }, &messageCount);

    EXPECT_TRUE(engine.Eval("print(\"Test\");"));
    EXPECT_EQ(messageCount, 1);

    Log::GetSingletonPtr()->ClearHooks();
}

TEST(ScriptEngine, ReadOnlyPacket)
{
    Log::GetSingletonPtr()->AddStandardOutputHook();

    ScriptEngine engine;

    EXPECT_TRUE(engine.Eval(
        "p <- Packet();\n"
        "p.WriteBlank(3);\n"
        "print(p.Size());\n"
    ));

    Log::GetSingletonPtr()->ClearHooks();
}

int main(int argc, char *argv[])
{
    try
    {
        ::testing::InitGoogleTest(&argc, argv);
    }
    catch(...)
    {
        return EXIT_FAILURE;
    }

    return RUN_ALL_TESTS();
}
