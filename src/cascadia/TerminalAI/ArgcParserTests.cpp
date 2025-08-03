// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"
#include "../TerminalAI/ArgcParser.h"

using namespace Microsoft::Terminal::AI;
using namespace std;

class ArgcParserTests
{
    TEST_CLASS(ArgcParserTests);

    TEST_METHOD(TestBasicInitialization)
    {
        auto parser = CreateArgcParser();
        VERIFY_IS_NOT_NULL(parser);
        VERIFY_IS_TRUE(parser->Initialize());
    }

    TEST_METHOD(TestBasicCommandParsing)
    {
        auto parser = CreateArgcParser();
        VERIFY_IS_NOT_NULL(parser);
        VERIFY_IS_TRUE(parser->Initialize());

        auto parsed = parser->ParseCommand(L"ls -la /home");
        VERIFY_ARE_EQUAL(L"ls", parsed.command);
        VERIFY_IS_TRUE(parsed.isValid);
        VERIFY_ARE_EQUAL(size_t(1), parsed.arguments.size());
        VERIFY_ARE_EQUAL(L"/home", parsed.arguments[0]);
        VERIFY_ARE_EQUAL(size_t(1), parsed.flags.size());
        VERIFY_ARE_EQUAL(L"-la", parsed.flags[0]);
    }

    TEST_METHOD(TestCommandCompletions)
    {
        auto parser = CreateArgcParser();
        VERIFY_IS_NOT_NULL(parser);
        VERIFY_IS_TRUE(parser->Initialize());

        auto completions = parser->GetCompletions(L"ls");
        VERIFY_IS_TRUE(completions.size() > 0);
        
        // Should suggest ls command
        bool foundLs = false;
        for (const auto& completion : completions)
        {
            if (completion.completion == L"ls")
            {
                foundLs = true;
                break;
            }
        }
        VERIFY_IS_TRUE(foundLs);
    }

    TEST_METHOD(TestArgumentCompletions)
    {
        auto parser = CreateArgcParser();
        VERIFY_IS_NOT_NULL(parser);
        VERIFY_IS_TRUE(parser->Initialize());

        auto completions = parser->GetCompletions(L"ls -");
        VERIFY_IS_TRUE(completions.size() > 0);
        
        // Should suggest flags for ls
        bool foundFlag = false;
        for (const auto& completion : completions)
        {
            if (completion.completion.starts_with(L"-"))
            {
                foundFlag = true;
                break;
            }
        }
        VERIFY_IS_TRUE(foundFlag);
    }

    TEST_METHOD(TestHelpGeneration)
    {
        auto parser = CreateArgcParser();
        VERIFY_IS_NOT_NULL(parser);
        VERIFY_IS_TRUE(parser->Initialize());

        auto help = parser->GenerateHelp(L"ls");
        VERIFY_IS_FALSE(help.empty());
        VERIFY_IS_TRUE(help.find(L"USAGE") != wstring::npos);
    }

    TEST_METHOD(TestValidCommand)
    {
        auto parser = CreateArgcParser();
        VERIFY_IS_NOT_NULL(parser);
        VERIFY_IS_TRUE(parser->Initialize());

        VERIFY_IS_TRUE(parser->IsValidCommand(L"ls"));
        VERIFY_IS_TRUE(parser->IsValidCommand(L"git"));
        VERIFY_IS_FALSE(parser->IsValidCommand(L"nonexistentcommand"));
    }

    TEST_METHOD(TestAvailableCommands)
    {
        auto parser = CreateArgcParser();
        VERIFY_IS_NOT_NULL(parser);
        VERIFY_IS_TRUE(parser->Initialize());

        auto commands = parser->GetAvailableCommands();
        VERIFY_IS_TRUE(commands.size() > 0);
        
        // Should include basic commands
        bool foundLs = false, foundGit = false;
        for (const auto& cmd : commands)
        {
            if (cmd == L"ls") foundLs = true;
            if (cmd == L"git") foundGit = true;
        }
        VERIFY_IS_TRUE(foundLs);
        VERIFY_IS_TRUE(foundGit);
    }
};