#include "pch.h"
#include "AIChatEngine.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;

namespace Microsoft::Terminal::AI
{
    AIChatEngine::AIChatEngine()
    {
    }
    
    winrt::fire_and_forget AIChatEngine::InitializeAsync()
    {
        // Simulate initialization delay for API connection
        co_await winrt::resume_after(std::chrono::milliseconds(150));
        
        _initialized = true;
        
        // Set default system message for chat
        if (_systemMessage.empty())
        {
            _systemMessage = L"You are a helpful AI assistant integrated into Windows Terminal. You help users with terminal tasks, provide command suggestions, and answer questions about computing topics.";
        }
        
        _FireResponseReceived(L"AI Chat Engine initialized successfully");
    }
    
    winrt::fire_and_forget AIChatEngine::ChatAsync(std::wstring_view message)
    {
        if (!_initialized)
        {
            _FireErrorOccurred(L"AI Chat Engine not initialized");
            co_return;
        }
        
        if (message.empty())
        {
            _FireErrorOccurred(L"Message cannot be empty");
            co_return;
        }
        
        // Add user message to conversation history
        _conversationHistory.push_back(_formatChatMessage(L"user", message));
        
        // Simulate API processing delay
        co_await winrt::resume_after(std::chrono::milliseconds(500));
        
        // Generate response (this would normally call the actual API)
        auto response = _generateChatResponse(message);
        
        // Add assistant response to conversation history
        _conversationHistory.push_back(_formatChatMessage(L"assistant", response));
        
        _FireResponseReceived(winrt::hstring(response));
    }
    
    winrt::fire_and_forget AIChatEngine::ExecuteCommandAsync(std::wstring_view command)
    {
        if (!_initialized)
        {
            _FireErrorOccurred(L"AI Chat Engine not initialized");
            co_return;
        }
        
        // For chat engine, treat commands as chat messages
        co_await ChatAsync(std::wstring(L"Execute this command: ") + std::wstring(command));
    }
    
    void AIChatEngine::SetProvider(std::wstring_view provider)
    {
        _provider = provider;
    }
    
    void AIChatEngine::SetModel(std::wstring_view model)
    {
        _model = model;
    }
    
    void AIChatEngine::SetAPIKey(std::wstring_view apiKey)
    {
        _apiKey = apiKey;
    }
    
    void AIChatEngine::ClearConversationHistory()
    {
        _conversationHistory.clear();
    }
    
    void AIChatEngine::SetSystemMessage(std::wstring_view systemMessage)
    {
        _systemMessage = systemMessage;
    }
    
    bool AIChatEngine::IsReady() const
    {
        return _initialized;
    }
    
    winrt::fire_and_forget AIChatEngine::_makeAPIRequest(std::wstring_view endpoint, std::wstring_view payload)
    {
        // TODO: Implement actual API request logic here
        // This would make HTTP requests to OpenAI, Azure, or other providers
        co_await winrt::resume_after(std::chrono::milliseconds(100));
        
        // For now, this is a placeholder for future implementation
        UNREFERENCED_PARAMETER(endpoint);
        UNREFERENCED_PARAMETER(payload);
    }
    
    std::wstring AIChatEngine::_formatChatMessage(std::wstring_view role, std::wstring_view content)
    {
        return std::wstring(role) + L": " + std::wstring(content);
    }
    
    std::wstring AIChatEngine::_generateChatResponse(std::wstring_view message)
    {
        // This is a simplified response generator for demonstration
        // In a real implementation, this would format the request and call the API
        
        std::wstring response = L"AI: ";
        
        // Check for common patterns and provide helpful responses
        if (message.find(L"hello") != std::wstring::npos || message.find(L"hi") != std::wstring::npos)
        {
            response += L"Hello! I'm your AI assistant integrated into Windows Terminal. How can I help you today?";
        }
        else if (message.find(L"help") != std::wstring::npos)
        {
            response += L"I can help you with:\n";
            response += L"• Command suggestions and explanations\n";
            response += L"• PowerShell, CMD, and Linux terminal commands\n";
            response += L"• File system navigation\n";
            response += L"• Git operations\n";
            response += L"• Development workflows\n";
            response += L"What specific area would you like assistance with?";
        }
        else if (message.find(L"git") != std::wstring::npos)
        {
            response += L"I can help with Git operations! Common commands include:\n";
            response += L"• git status - Check repository status\n";
            response += L"• git add . - Stage all changes\n";
            response += L"• git commit -m \"message\" - Commit changes\n";
            response += L"• git push - Push to remote repository\n";
            response += L"• git pull - Pull latest changes\n";
            response += L"What specific Git task are you trying to accomplish?";
        }
        else if (message.find(L"powershell") != std::wstring::npos || message.find(L"pwsh") != std::wstring::npos)
        {
            response += L"PowerShell is great for automation and system administration! Some useful commands:\n";
            response += L"• Get-ChildItem (ls) - List directory contents\n";
            response += L"• Set-Location (cd) - Change directory\n";
            response += L"• Get-Process - List running processes\n";
            response += L"• Get-Service - List system services\n";
            response += L"What PowerShell task would you like help with?";
        }
        else if (message.find(L"file") != std::wstring::npos || message.find(L"directory") != std::wstring::npos)
        {
            response += L"For file and directory operations, you can use:\n";
            response += L"• ls/dir - List contents\n";
            response += L"• cd - Change directory\n";
            response += L"• mkdir - Create directory\n";
            response += L"• cp/copy - Copy files\n";
            response += L"• mv/move - Move or rename files\n";
            response += L"• rm/del - Delete files\n";
            response += L"What file operation do you need help with?";
        }
        else
        {
            response += L"I understand you're asking about: \"" + std::wstring(message) + L"\"\n\n";
            response += L"I'm here to help with terminal tasks and command-line operations. Could you provide more details about what you're trying to accomplish? ";
            response += L"For example, are you looking for help with:\n";
            response += L"• A specific command or script\n";
            response += L"• File management\n";
            response += L"• Development tools\n";
            response += L"• System administration\n";
            response += L"• Something else?";
        }
        
        return response;
    }
}