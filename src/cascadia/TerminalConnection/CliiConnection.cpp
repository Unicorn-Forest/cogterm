// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"
#include "CliiConnection.h"
#include <sstream>
#include <regex>

#include "CliiConnection.g.cpp"

namespace winrt::Microsoft::Terminal::TerminalConnection::implementation
{
    CliiConnection::CliiConnection()
    {
        _jsRuntime = std::make_unique<Microsoft::Terminal::AI::JavaScriptRuntime>();
        
        // Subscribe to JavaScript runtime events
        _jsRuntime->ExecutionCompleted({ this, &CliiConnection::_onExecutionCompleted });
        _jsRuntime->ExecutionError({ this, &CliiConnection::_onExecutionError });
    }

    void CliiConnection::Initialize(Windows::Foundation::Collections::ValueSet const& settings)
    {
        if (settings)
        {
            // Get script path from settings
            if (auto scriptPathObj = settings.TryLookup(L"scriptPath"))
            {
                if (auto scriptPath = scriptPathObj.try_as<winrt::hstring>())
                {
                    _scriptPath = scriptPath.c_str();
                }
            }
        }
        
        // Default script path if none provided
        if (_scriptPath.empty())
        {
            _scriptPath = L"clii_functions.js";
        }
    }

    void CliiConnection::Start() noexcept
    try
    {
        _transitionToState(ConnectionState::Connecting);
        
        // Load the JavaScript file
        if (!_scriptPath.empty())
        {
            _jsRuntime->LoadScript(_scriptPath);
        }
        
        _initialized = true;
        _transitionToState(ConnectionState::Connected);
        
        // Welcome message
        _outputText(L"Clii JavaScript Runtime initialized.\r\n");
        _outputText(L"Type 'help' for available commands.\r\n");
        _outputText(L"clii> ");
    }
    CATCH_LOG()

    void CliiConnection::WriteInput(winrt::array_view<char16_t const> const& data)
    try
    {
        if (!_isConnected())
        {
            return;
        }
        
        const auto input = winrt_array_to_wstring_view(data);
        std::wstring command{input};
        
        // Remove newline characters
        command.erase(std::remove(command.begin(), command.end(), L'\r'), command.end());
        command.erase(std::remove(command.begin(), command.end(), L'\n'), command.end());
        
        // Echo the input
        _outputText(command);
        _outputText(L"\r\n");
        
        if (!command.empty())
        {
            _handleCliiCommand(command);
        }
        
        // Show prompt again
        _outputText(L"clii> ");
    }
    CATCH_LOG()

    void CliiConnection::Resize(uint32_t /*rows*/, uint32_t /*columns*/) noexcept
    {
        // No special handling needed for Clii connection
    }

    void CliiConnection::Close() noexcept
    try
    {
        _transitionToState(ConnectionState::Closed);
    }
    CATCH_LOG()

    void CliiConnection::_handleCliiCommand(std::wstring_view command)
    {
        if (command == L"help")
        {
            _showHelp();
        }
        else if (command == L"list" || command == L"functions")
        {
            _displayAvailableFunctions();
        }
        else if (command.starts_with(L"run "))
        {
            // Extract function name and arguments
            std::wstring functionCall = std::wstring(command.substr(4)); // Remove "run "
            size_t spacePos = functionCall.find(L' ');
            
            std::wstring functionName;
            std::wstring arguments;
            
            if (spacePos != std::wstring::npos)
            {
                functionName = functionCall.substr(0, spacePos);
                arguments = functionCall.substr(spacePos + 1);
            }
            else
            {
                functionName = functionCall;
            }
            
            if (!functionName.empty())
            {
                auto params = _parseCommandArguments(arguments);
                _outputText(L"Executing function: " + functionName + L"\r\n");
                _jsRuntime->ExecuteFunctionAsync(functionName, params);
            }
            else
            {
                _outputText(L"Error: No function name specified.\r\n");
            }
        }
        else if (!command.empty())
        {
            // Try to execute as a direct function call
            auto params = _parseCommandArguments(command);
            if (!params.empty() || !command.empty())
            {
                // Assume first word is function name
                size_t spacePos = command.find(L' ');
                std::wstring functionName = std::wstring(command.substr(0, spacePos == std::wstring::npos ? command.length() : spacePos));
                
                _outputText(L"Executing function: " + functionName + L"\r\n");
                _jsRuntime->ExecuteFunctionAsync(functionName, params);
            }
        }
    }

    void CliiConnection::_displayAvailableFunctions()
    {
        if (!_jsRuntime->IsReady())
        {
            _outputText(L"JavaScript runtime not ready. Please load a script first.\r\n");
            return;
        }
        
        auto functions = _jsRuntime->GetAvailableFunctions();
        if (functions.empty())
        {
            _outputText(L"No functions available. Please load a script with function definitions.\r\n");
            return;
        }
        
        _outputText(L"Available functions:\r\n");
        for (const auto& func : functions)
        {
            _outputText(L"  " + func.name + L": " + func.description + L"\r\n");
            if (!func.parameters.empty())
            {
                _outputText(L"    Parameters:\r\n");
                for (const auto& param : func.parameters)
                {
                    _outputText(L"      " + param.first + L": " + param.second + L"\r\n");
                }
            }
        }
    }

    std::map<std::wstring, std::wstring> CliiConnection::_parseCommandArguments(std::wstring_view command)
    {
        std::map<std::wstring, std::wstring> params;
        
        // Simple parsing for key=value pairs
        std::wstring cmd{command};
        std::wregex paramRegex(LR"((\w+)=([^\s]+))");
        std::wsregex_iterator iter(cmd.begin(), cmd.end(), paramRegex);
        std::wsregex_iterator end;
        
        for (; iter != end; ++iter)
        {
            const std::wsmatch& match = *iter;
            params[match[1].str()] = match[2].str();
        }
        
        return params;
    }

    void CliiConnection::_onExecutionCompleted(Microsoft::Terminal::AI::JavaScriptRuntime const& /*sender*/, winrt::hstring const& result)
    {
        _outputText(L"Result: " + std::wstring{result.c_str()} + L"\r\n");
    }

    void CliiConnection::_onExecutionError(Microsoft::Terminal::AI::JavaScriptRuntime const& /*sender*/, winrt::hstring const& error)
    {
        _outputText(L"Error: " + std::wstring{error.c_str()} + L"\r\n");
    }

    void CliiConnection::_outputText(std::wstring_view text)
    {
        TerminalOutput.raise(winrt::hstring{text});
    }

    void CliiConnection::_showHelp()
    {
        _outputText(L"Clii JavaScript Runtime Commands:\r\n");
        _outputText(L"  help                    - Show this help message\r\n");
        _outputText(L"  list | functions        - List available JavaScript functions\r\n");
        _outputText(L"  run <function> [args]   - Execute a JavaScript function\r\n");
        _outputText(L"  <function> [args]       - Direct function execution\r\n");
        _outputText(L"\r\n");
        _outputText(L"Arguments format: key=value key2=value2\r\n");
        _outputText(L"Example: run myFunc arg1=hello arg2=world\r\n");
    }
}