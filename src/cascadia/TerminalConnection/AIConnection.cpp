#include "pch.h"
#include "AIConnection.h"
#include "AIConnection.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Microsoft::Terminal::TerminalConnection;

namespace winrt::Microsoft::Terminal::TerminalConnection::implementation
{
    AIConnection::AIConnection()
    {
        _aiEngine = std::make_unique<Microsoft::Terminal::AI::BasicAIEngine>();
        
        // Set up event handlers for AI engine
        _aiEngine->ResponseReceived({ this, &AIConnection::_handleAIResponse });
        _aiEngine->ErrorOccurred({ this, &AIConnection::_handleAIError });
    }
    
    void AIConnection::Initialize(ValueSet const& settings)
    {
        // Extract AI-specific settings
        if (settings.HasKey(L"aiProvider"))
        {
            _aiProvider = unbox_value<winrt::hstring>(settings.Lookup(L"aiProvider"));
        }
        
        if (settings.HasKey(L"aiModel"))
        {
            _aiModel = unbox_value<winrt::hstring>(settings.Lookup(L"aiModel"));
        }
    }
    
    void AIConnection::Start()
    {
        if (_connected)
        {
            return;
        }
        
        _connected = true;
        
        // Initialize AI engine
        _aiEngine->InitializeAsync();
        
        // Welcome message
        _writeToTerminal(L"AI Terminal Connection Started\r\n");
        _writeToTerminal(L"Provider: " + std::wstring(_aiProvider) + L"\r\n");
        _writeToTerminal(L"Model: " + std::wstring(_aiModel) + L"\r\n");
        _writeToTerminal(L"Type 'ai help' for assistance or just enter commands normally.\r\n");
        _writeToTerminal(L"AI> ");
        
        // Notify connection state changed
        _connectionStateChangedHandlers(*this, ConnectionState::Connected);
    }
    
    void AIConnection::WriteInput(array_view<uint8_t const> data)
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
            // Extract command (remove \r\n)
            std::wstring command = _inputBuffer.substr(0, lineEnd);
            if (!command.empty() && command.back() == L'\r')
            {
                command.pop_back();
            }
            
            // Clear processed input from buffer
            _inputBuffer.erase(0, lineEnd + 1);
            
            // Echo the input
            _writeToTerminal(command + L"\r\n");
            
            // Process the command
            _processAICommand(command);
        }
    }
    
    void AIConnection::Resize(uint32_t rows, uint32_t columns)
    {
        // AI connection doesn't need to handle resize for now
        UNREFERENCED_PARAMETER(rows);
        UNREFERENCED_PARAMETER(columns);
    }
    
    void AIConnection::Close()
    {
        if (!_connected)
        {
            return;
        }
        
        _connected = false;
        _writeToTerminal(L"\r\nAI Terminal Connection Closed\r\n");
        
        // Notify connection state changed
        _connectionStateChangedHandlers(*this, ConnectionState::Closed);
    }
    
    void AIConnection::SetAIProvider(winrt::hstring const& provider)
    {
        _aiProvider = provider;
    }
    
    void AIConnection::SetAIModel(winrt::hstring const& model)
    {
        _aiModel = model;
    }
    
    winrt::event_token AIConnection::ConnectionStateChanged(ConnectionStateChangedEventHandler const& handler)
    {
        return _connectionStateChangedHandlers.add(handler);
    }
    
    void AIConnection::ConnectionStateChanged(winrt::event_token const& token) noexcept
    {
        _connectionStateChangedHandlers.remove(token);
    }
    
    winrt::event_token AIConnection::TerminalOutput(TerminalOutputEventHandler const& handler)
    {
        return _terminalOutputHandlers.add(handler);
    }
    
    void AIConnection::TerminalOutput(winrt::event_token const& token) noexcept
    {
        _terminalOutputHandlers.remove(token);
    }
    
    void AIConnection::_processAICommand(std::wstring_view command)
    {
        if (command.empty())
        {
            _writeToTerminal(L"AI> ");
            return;
        }
        
        // Check for AI-specific commands
        if (command.starts_with(L"ai "))
        {
            std::wstring aiCommand = std::wstring(command.substr(3));
            if (aiCommand == L"help")
            {
                _writeToTerminal(L"AI Commands:\r\n");
                _writeToTerminal(L"  ai help - Show this help\r\n");
                _writeToTerminal(L"  ai chat <message> - Start AI chat\r\n");
                _writeToTerminal(L"  ai function <name> <args> - Execute AI function\r\n");
                _writeToTerminal(L"  Any other command will be processed with AI assistance\r\n");
            }
            else if (aiCommand.starts_with(L"chat "))
            {
                std::wstring message = aiCommand.substr(5);
                _aiEngine->ChatAsync(message);
                return; // Response will come via event handler
            }
            else if (aiCommand.starts_with(L"function "))
            {
                std::wstring funcCall = aiCommand.substr(9);
                size_t spacePos = funcCall.find(L' ');
                std::wstring funcName = spacePos != std::wstring::npos ? funcCall.substr(0, spacePos) : funcCall;
                std::wstring args = spacePos != std::wstring::npos ? funcCall.substr(spacePos + 1) : L"";
                _aiEngine->ExecuteFunctionAsync(funcName, args);
                return; // Response will come via event handler
            }
        }
        else
        {
            // Process regular command with AI assistance
            _aiEngine->ProcessCommandAsync(command);
            return; // Response will come via event handler
        }
        
        _writeToTerminal(L"AI> ");
    }
    
    void AIConnection::_writeToTerminal(std::wstring_view text)
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
    
    void AIConnection::_handleAIResponse(IInspectable const&, winrt::hstring const& response)
    {
        _writeToTerminal(std::wstring(response) + L"\r\nAI> ");
    }
    
    void AIConnection::_handleAIError(IInspectable const&, winrt::hstring const& error)
    {
        _writeToTerminal(L"AI Error: " + std::wstring(error) + L"\r\nAI> ");
    }
}