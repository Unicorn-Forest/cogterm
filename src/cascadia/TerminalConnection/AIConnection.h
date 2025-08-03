#pragma once

#include "pch.h"
#include "AIConnection.g.h"
#include "BaseTerminalConnection.h"
#include "../TerminalAI/AIEngine.h"

namespace winrt::Microsoft::Terminal::TerminalConnection::implementation
{
    // AI-powered terminal connection for basic AI integration
    struct AIConnection : public BaseTerminalConnection<AIConnection>
    {
    public:
        AIConnection();
        
        void Initialize(Windows::Foundation::Collections::ValueSet const& settings) override;
        void Start() override;
        void WriteInput(array_view<uint8_t const> data) override;
        void Resize(uint32_t rows, uint32_t columns) override;
        void Close() override;
        
        // AI-specific methods
        void SetAIProvider(winrt::hstring const& provider);
        void SetAIModel(winrt::hstring const& model);
        
        winrt::event_token ConnectionStateChanged(Microsoft::Terminal::TerminalConnection::ConnectionStateChangedEventHandler const& handler);
        void ConnectionStateChanged(winrt::event_token const& token) noexcept;
        
        winrt::event_token TerminalOutput(Microsoft::Terminal::TerminalConnection::TerminalOutputEventHandler const& handler);
        void TerminalOutput(winrt::event_token const& token) noexcept;
        
    private:
        std::unique_ptr<Microsoft::Terminal::AI::BasicAIEngine> _aiEngine;
        winrt::hstring _aiProvider{ L"basic" };
        winrt::hstring _aiModel{ L"demo" };
        std::wstring _inputBuffer;
        bool _connected = false;
        
        winrt::event<Microsoft::Terminal::TerminalConnection::ConnectionStateChangedEventHandler> _connectionStateChangedHandlers;
        winrt::event<Microsoft::Terminal::TerminalConnection::TerminalOutputEventHandler> _terminalOutputHandlers;
        
        void _processAICommand(std::wstring_view command);
        void _writeToTerminal(std::wstring_view text);
        void _handleAIResponse(winrt::Windows::Foundation::IInspectable const& sender, winrt::hstring const& response);
        void _handleAIError(winrt::Windows::Foundation::IInspectable const& sender, winrt::hstring const& error);
    };
}

namespace winrt::Microsoft::Terminal::TerminalConnection::factory_implementation
{
    struct AIConnection : AIConnectionT<AIConnection, implementation::AIConnection>
    {
    };
}