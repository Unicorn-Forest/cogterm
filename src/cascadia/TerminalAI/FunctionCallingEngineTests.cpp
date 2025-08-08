// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"
#include "FunctionCallingEngine.h"

using namespace Microsoft::Terminal::AI;
using namespace std;

class FunctionCallingEngineTests
{
    TEST_CLASS(FunctionCallingEngineTests);

    TEST_METHOD(TestBasicInitialization)
    {
        FunctionCallingEngine engine;
        auto functions = engine.GetAvailableFunctions();
        VERIFY_ARE_EQUAL(size_t(0), functions.size());
    }

    TEST_METHOD(TestLoadFunctions)
    {
        FunctionCallingEngine engine;
        engine.LoadFunctions(L"/tmp/test-functions");
        
        auto functions = engine.GetAvailableFunctions();
        VERIFY_IS_TRUE(functions.size() > 0);
        
        // Check if we loaded our test functions
        bool foundHello = false;
        bool foundListFiles = false;
        
        for (const auto& func : functions)
        {
            if (func.name == L"hello")
            {
                foundHello = true;
                VERIFY_ARE_EQUAL(L"Prints a hello message", func.description);
                VERIFY_IS_FALSE(func.scriptPath.empty());
                VERIFY_IS_TRUE(func.scriptPath.ends_with(L"hello.py"));
            }
            else if (func.name == L"list_files")
            {
                foundListFiles = true;
                VERIFY_ARE_EQUAL(L"Lists files in the current directory", func.description);
                VERIFY_IS_FALSE(func.scriptPath.empty());
                VERIFY_IS_TRUE(func.scriptPath.ends_with(L"list_files.sh"));
            }
        }
        
        VERIFY_IS_TRUE(foundHello);
        VERIFY_IS_TRUE(foundListFiles);
    }

    TEST_METHOD(TestLoadFunctionsNonExistentDirectory)
    {
        FunctionCallingEngine engine;
        engine.LoadFunctions(L"/nonexistent/directory");
        
        auto functions = engine.GetAvailableFunctions();
        VERIFY_ARE_EQUAL(size_t(0), functions.size());
    }

    TEST_METHOD(TestLoadFunctionsEmptyDirectory)
    {
        FunctionCallingEngine engine;
        engine.LoadFunctions(L"");
        
        auto functions = engine.GetAvailableFunctions();
        VERIFY_ARE_EQUAL(size_t(0), functions.size());
    }
};