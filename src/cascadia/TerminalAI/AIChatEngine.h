#pragma once

#include "pch.h"
#include "AIEngine.h"

namespace Microsoft::Terminal::AI
{
    // AIChat functionality - specialized for chat interactions
    class AIChatEngine : public AIEngine
    {
    public:
        AIChatEngine();
        
        winrt::fire_and_forget InitializeAsync() override;
        winrt::fire_and_forget ChatAsync(std::wstring_view message) override;
        winrt::fire_and_forget ExecuteCommandAsync(std::wstring_view command) override;
        
        // AIChat specific features
        void SetProvider(std::wstring_view provider);
        void SetModel(std::wstring_view model);
        void SetAPIKey(std::wstring_view apiKey);
        
        // Conversation management
        void ClearConversationHistory();
        void SetSystemMessage(std::wstring_view systemMessage);
        
        bool IsReady() const override;
        
    private:
        std::wstring _provider{ L"openai" };
        std::wstring _model{ L"gpt-4" };
        std::wstring _apiKey;
        std::vector<std::wstring> _conversationHistory;
        std::wstring _systemMessage;
        bool _initialized = false;
        
        winrt::fire_and_forget _makeAPIRequest(std::wstring_view endpoint, std::wstring_view payload);
        std::wstring _formatChatMessage(std::wstring_view role, std::wstring_view content);
        std::wstring _generateChatResponse(std::wstring_view message);
    };
}