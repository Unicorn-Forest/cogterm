#pragma once

#include "pch.h"
#include "AIChatConnection.g.h"
#include "BaseTerminalConnection.h"

namespace winrt::Microsoft::Terminal::TerminalConnection::implementation
{
    // Forward declaration
    namespace AI = Microsoft::Terminal::AI;
    
    // Direct AI chat integration
    struct AIChatConnection : public BaseTerminalConnection<AIChatConnection>
    {
    public:
        AIChatConnection();
        
        static winrt::guid ConnectionType() noexcept;
        
        void Initialize(Windows::Foundation::Collections::ValueSet const& settings) override;
        void Start() override;
        void WriteInput(array_view<uint8_t const> data) override;
        void Resize(uint32_t rows, uint32_t columns) override;
        void Close() override;
        
        // AIChat specific methods
        void SetRole(winrt::hstring const& role);
        void LoadSession(winrt::hstring const& sessionId);
        void SaveSession(winrt::hstring const& sessionId);
        
        // Properties
        winrt::hstring Role() const noexcept { return _currentRole; }
        void Role(winrt::hstring const& value) noexcept { _currentRole = value; }
        
        winrt::hstring SessionId() const noexcept { return _sessionId; }
        void SessionId(winrt::hstring const& value) noexcept { _sessionId = value; }
        
        winrt::hstring Provider() const noexcept { return _provider; }
        void Provider(winrt::hstring const& value) noexcept { _provider = value; }
        
        winrt::hstring Model() const noexcept { return _model; }
        void Model(winrt::hstring const& value) noexcept { _model = value; }
        
        winrt::hstring APIKey() const noexcept { return _apiKey; }
        void APIKey(winrt::hstring const& value) noexcept { _apiKey = value; }
        
        winrt::event_token ConnectionStateChanged(Microsoft::Terminal::TerminalConnection::ConnectionStateChangedEventHandler const& handler);
        void ConnectionStateChanged(winrt::event_token const& token) noexcept;
        
        winrt::event_token TerminalOutput(Microsoft::Terminal::TerminalConnection::TerminalOutputEventHandler const& handler);
        void TerminalOutput(winrt::event_token const& token) noexcept;
        
    private:
        std::unique_ptr<AI::AIChatEngine> _aichatEngine;
        winrt::hstring _currentRole{ L"user" };
        winrt::hstring _sessionId;
        winrt::hstring _provider{ L"openai" };
        winrt::hstring _model{ L"gpt-4" };
        winrt::hstring _apiKey;
        std::wstring _inputBuffer;
        bool _connected = false;
        
        winrt::event<Microsoft::Terminal::TerminalConnection::ConnectionStateChangedEventHandler> _connectionStateChangedHandlers;
        winrt::event<Microsoft::Terminal::TerminalConnection::TerminalOutputEventHandler> _terminalOutputHandlers;
        
        void _handleAIResponse(winrt::Windows::Foundation::IInspectable const& sender, winrt::hstring const& response);
        void _handleAIError(winrt::Windows::Foundation::IInspectable const& sender, winrt::hstring const& error);
        void _writeToTerminal(std::wstring_view text);
        void _processInput(std::wstring_view input);
    };
}

namespace winrt::Microsoft::Terminal::TerminalConnection::factory_implementation
{
    struct AIChatConnection : AIChatConnectionT<AIChatConnection, implementation::AIChatConnection>
    {
    };
}