#pragma once

#include "pch.h"

namespace Microsoft::Terminal::AI
{
    // LLM function calling support
    class FunctionCallingEngine
    {
    public:
        FunctionCallingEngine();
        
        struct FunctionDefinition
        {
            std::wstring name;
            std::wstring description;
            std::wstring parameters;
            std::wstring scriptPath;
        };
        
        void LoadFunctions(std::wstring_view functionsDir);
        winrt::fire_and_forget ExecuteFunctionAsync(std::wstring_view functionName, std::wstring_view arguments);
        std::vector<FunctionDefinition> GetAvailableFunctions();
        
    private:
        std::map<std::wstring, FunctionDefinition> _functions;
        std::wstring _functionsDirectory;
        
        void _loadFunctionDefinitions();
        winrt::fire_and_forget _executeBashFunction(std::wstring_view scriptPath, std::wstring_view args);
        winrt::fire_and_forget _executeJavaScriptFunction(std::wstring_view scriptPath, std::wstring_view args);
        winrt::fire_and_forget _executePythonFunction(std::wstring_view scriptPath, std::wstring_view args);
    };
}