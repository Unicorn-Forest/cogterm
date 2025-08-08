#pragma once

#include "pch.h"
#include "AIEngine.h"
#include "FunctionCallingEngine.h"

namespace Microsoft::Terminal::AI
{
    // AI agent system
    class AIAgent
    {
    public:
        AIAgent(std::wstring_view agentName);
        
        struct AgentDefinition
        {
            std::wstring name;
            std::wstring description;
            std::wstring instructions;
            std::vector<std::wstring> tools;
            std::vector<std::wstring> documents;
        };
        
        void LoadAgent(std::wstring_view agentPath);
        winrt::fire_and_forget ExecuteAgentAsync(std::wstring_view userInput);
        
        // Events for agent responses and errors
        winrt::event<winrt::Windows::Foundation::TypedEventHandler<AIAgent, winrt::hstring>> ResponseReceived;
        winrt::event<winrt::Windows::Foundation::TypedEventHandler<AIAgent, winrt::hstring>> ErrorOccurred;
        
    private:
        AgentDefinition _definition;
        std::unique_ptr<FunctionCallingEngine> _functionEngine;
        std::unique_ptr<AIEngine> _aiEngine;
        
        void _loadAgentDefinition(std::wstring_view agentPath);
        winrt::fire_and_forget _processWithTools(std::wstring_view userInput);
        
        // Helper methods to fire events
        void _FireResponseReceived(const winrt::hstring& response)
        {
            ResponseReceived(*this, response);
        }
        
        void _FireErrorOccurred(const winrt::hstring& error)
        {
            ErrorOccurred(*this, error);
        }
    };
}