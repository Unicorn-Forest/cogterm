// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include "CliiConnection.g.h"
#include "BaseTerminalConnection.h"
#include "../TerminalAI/JavaScriptRuntime.h"
#include <memory>

namespace winrt::Microsoft::Terminal::TerminalConnection::implementation
{
    struct CliiConnection : CliiConnectionT<CliiConnection>, BaseTerminalConnection<CliiConnection>
    {
        CliiConnection();
        
        void Initialize(Windows::Foundation::Collections::ValueSet const& settings) override;
        void Start() noexcept override;
        void WriteInput(winrt::array_view<char16_t const> const& data) override;
        void Resize(uint32_t rows, uint32_t columns) noexcept override;
        void Close() noexcept override;

        til::event<TerminalOutputHandler> TerminalOutput;

    private:
        std::unique_ptr<Microsoft::Terminal::AI::JavaScriptRuntime> _jsRuntime;
        std::wstring _scriptPath;
        bool _initialized = false;
        
        // Handle Clii-specific commands
        void _handleCliiCommand(std::wstring_view command);
        
        // Display available functions to the user
        void _displayAvailableFunctions();
        
        // Parse command line arguments for function calls
        std::map<std::wstring, std::wstring> _parseCommandArguments(std::wstring_view command);
        
        // Event handlers for JavaScript runtime
        void _onExecutionCompleted(Microsoft::Terminal::AI::JavaScriptRuntime const& sender, winrt::hstring const& result);
        void _onExecutionError(Microsoft::Terminal::AI::JavaScriptRuntime const& sender, winrt::hstring const& error);
        
        // Output text to terminal
        void _outputText(std::wstring_view text);
        
        // Show help information
        void _showHelp();
    };
}

namespace winrt::Microsoft::Terminal::TerminalConnection::factory_implementation
{
    BASIC_FACTORY(CliiConnection);
}