#include "pch.h"
#include "AIChatConnection.h"
#include "AIChatConnection.g.cpp"
#include "../TerminalAI/AIChatEngine.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Microsoft::Terminal::TerminalConnection;

static constexpr winrt::guid AIChatConnectionType = { 0x8f1e1e1a, 0x2b3c, 0x4d5e, { 0x9f, 0x8a, 0x1b, 0x2c, 0x3d, 0x4e, 0x5f, 0x6a } };

namespace winrt::Microsoft::Terminal::TerminalConnection::implementation
{
    AIChatConnection::AIChatConnection()
    {
        _aichatEngine = std::make_unique<Microsoft::Terminal::AI::AIChatEngine>();
        
        // Set up event handlers for AI chat engine
        _aichatEngine->ResponseReceived({ this, &AIChatConnection::_handleAIResponse });
        _aichatEngine->ErrorOccurred({ this, &AIChatConnection::_handleAIError });
    }
    
    winrt::guid AIChatConnection::ConnectionType() noexcept
    {
        return AIChatConnectionType;
    }
    
    void AIChatConnection::Initialize(ValueSet const& settings)
    {
        // Extract AI chat specific settings
        if (settings.HasKey(L"role"))
        {
            _currentRole = unbox_value<winrt::hstring>(settings.Lookup(L"role"));
        }
        
        if (settings.HasKey(L"sessionId"))
        {
            _sessionId = unbox_value<winrt::hstring>(settings.Lookup(L"sessionId"));
        }
        
        if (settings.HasKey(L"provider"))
        {
            _provider = unbox_value<winrt::hstring>(settings.Lookup(L"provider"));
        }
        
        if (settings.HasKey(L"model"))
        {
            _model = unbox_value<winrt::hstring>(settings.Lookup(L"model"));
        }
        
        if (settings.HasKey(L"apiKey"))
        {
            _apiKey = unbox_value<winrt::hstring>(settings.Lookup(L"apiKey"));
        }
        
        // Configure the AI chat engine
        _aichatEngine->SetProvider(_provider);
        _aichatEngine->SetModel(_model);
        _aichatEngine->SetAPIKey(_apiKey);
    }
    
    void AIChatConnection::Start()
    {
        if (_connected)
        {
            return;
        }
        
        _connected = true;
        
        // Initialize AI chat engine
        _aichatEngine->InitializeAsync();
        
        // Load session if specified
        if (!_sessionId.empty())
        {
            LoadSession(_sessionId);
        }
        
        // Welcome message
        _writeToTerminal(L"AI Chat Connection Started\r\n");
        _writeToTerminal(L"Role: " + std::wstring(_currentRole) + L"\r\n");
        _writeToTerminal(L"Provider: " + std::wstring(_provider) + L"\r\n");
        _writeToTerminal(L"Model: " + std::wstring(_model) + L"\r\n");
        if (!_sessionId.empty())
        {
            _writeToTerminal(L"Session: " + std::wstring(_sessionId) + L"\r\n");
        }
        _writeToTerminal(L"Type your message to start chatting...\r\n");
        _writeToTerminal(L"\r\n> ");
        
        // Notify connection state changed
        _connectionStateChangedHandlers(*this, ConnectionState::Connected);
    }
    
    void AIChatConnection::WriteInput(array_view<uint8_t const> data)
    {
        if (!_connected)
        {
            return;
        }
        
        // Convert input data to wide string
        std::string input(reinterpret_cast<const char*>(data.data()), data.size());
        std::wstring wideInput(input.begin(), input.end());
        
        // Add to input buffer
        _inputBuffer += wideInput;
        
        // Check for complete line (ended with \r\n or \n)
        size_t lineEnd = _inputBuffer.find(L'\n');
        if (lineEnd != std::wstring::npos)
        {
            // Extract message (remove \r\n)
            std::wstring message = _inputBuffer.substr(0, lineEnd);
            if (!message.empty() && message.back() == L'\r')
            {
                message.pop_back();
            }
            
            // Clear processed input from buffer
            _inputBuffer.erase(0, lineEnd + 1);
            
            // Echo the input
            _writeToTerminal(message + L"\r\n");
            
            // Process the message
            _processInput(message);
        }
    }
    
    void AIChatConnection::Resize(uint32_t rows, uint32_t columns)
    {
        // AI chat connection doesn't need to handle resize for now
        UNREFERENCED_PARAMETER(rows);
        UNREFERENCED_PARAMETER(columns);
    }
    
    void AIChatConnection::Close()
    {
        if (!_connected)
        {
            return;
        }
        
        _connected = false;
        
        // Save session if specified
        if (!_sessionId.empty())
        {
            SaveSession(_sessionId);
        }
        
        _writeToTerminal(L"\r\nAI Chat Connection Closed\r\n");
        
        // Notify connection state changed
        _connectionStateChangedHandlers(*this, ConnectionState::Closed);
    }
    
    void AIChatConnection::SetRole(winrt::hstring const& role)
    {
        _currentRole = role;
        if (_connected)
        {
            _writeToTerminal(L"Role changed to: " + std::wstring(role) + L"\r\n> ");
        }
    }
    
    void AIChatConnection::LoadSession(winrt::hstring const& sessionId)
    {
        _sessionId = sessionId;
        // TODO: Implement actual session loading from storage
        if (_connected)
        {
            _writeToTerminal(L"Session loaded: " + std::wstring(sessionId) + L"\r\n");
        }
    }
    
    void AIChatConnection::SaveSession(winrt::hstring const& sessionId)
    {
        _sessionId = sessionId;
        // TODO: Implement actual session saving to storage
        if (_connected)
        {
            _writeToTerminal(L"Session saved: " + std::wstring(sessionId) + L"\r\n");
        }
    }
    
    winrt::event_token AIChatConnection::ConnectionStateChanged(ConnectionStateChangedEventHandler const& handler)
    {
        return _connectionStateChangedHandlers.add(handler);
    }
    
    void AIChatConnection::ConnectionStateChanged(winrt::event_token const& token) noexcept
    {
        _connectionStateChangedHandlers.remove(token);
    }
    
    winrt::event_token AIChatConnection::TerminalOutput(TerminalOutputEventHandler const& handler)
    {
        return _terminalOutputHandlers.add(handler);
    }
    
    void AIChatConnection::TerminalOutput(winrt::event_token const& token) noexcept
    {
        _terminalOutputHandlers.remove(token);
    }
    
    void AIChatConnection::_processInput(std::wstring_view input)
    {
        if (input.empty())
        {
            _writeToTerminal(L"> ");
            return;
        }
        
        // Check for special commands
        if (input.starts_with(L"/"))
        {
            if (input == L"/help")
            {
                _writeToTerminal(L"AI Chat Commands:\r\n");
                _writeToTerminal(L"  /help - Show this help\r\n");
                _writeToTerminal(L"  /role <role> - Set current role\r\n");
                _writeToTerminal(L"  /save <session> - Save current session\r\n");
                _writeToTerminal(L"  /load <session> - Load a session\r\n");
                _writeToTerminal(L"  /clear - Clear conversation history\r\n");
                _writeToTerminal(L"> ");
                return;
            }
            else if (input.starts_with(L"/role "))
            {
                std::wstring newRole = std::wstring(input.substr(6));
                SetRole(winrt::hstring(newRole));
                return;
            }
            else if (input.starts_with(L"/save "))
            {
                std::wstring sessionName = std::wstring(input.substr(6));
                SaveSession(winrt::hstring(sessionName));
                _writeToTerminal(L"> ");
                return;
            }
            else if (input.starts_with(L"/load "))
            {
                std::wstring sessionName = std::wstring(input.substr(6));
                LoadSession(winrt::hstring(sessionName));
                _writeToTerminal(L"> ");
                return;
            }
            else if (input == L"/clear")
            {
                // TODO: Clear conversation history in engine
                _writeToTerminal(L"Conversation history cleared.\r\n> ");
                return;
            }
            else
            {
                _writeToTerminal(L"Unknown command. Type /help for available commands.\r\n> ");
                return;
            }
        }
        
        // Send message to AI chat engine
        _aichatEngine->ChatAsync(input);
        // Response will come via event handler
    }
    
    void AIChatConnection::_writeToTerminal(std::wstring_view text)
    {
        if (!_connected)
        {
            return;
        }
        
        // Convert to byte array for terminal output
        std::string output(text.begin(), text.end());
        auto outputData = winrt::array_view<uint8_t const>(
            reinterpret_cast<const uint8_t*>(output.data()), 
            output.size()
        );
        
        _terminalOutputHandlers(*this, outputData);
    }
    
    void AIChatConnection::_handleAIResponse(IInspectable const&, winrt::hstring const& response)
    {
        _writeToTerminal(std::wstring(response) + L"\r\n\r\n> ");
    }
    
    void AIChatConnection::_handleAIError(IInspectable const&, winrt::hstring const& error)
    {
        _writeToTerminal(L"AI Error: " + std::wstring(error) + L"\r\n> ");
    }
}