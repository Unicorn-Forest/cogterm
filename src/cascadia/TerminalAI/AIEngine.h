#pragma once

#include "pch.h"

namespace Microsoft::Terminal::AI
{
    // Core AI engine interface for terminal AI integration
    class AIEngine
    {
    public:
        virtual ~AIEngine() = default;
        
        // Initialize the AI engine with configuration
        virtual winrt::fire_and_forget InitializeAsync() = 0;
        
        // Process a command with AI assistance
        virtual winrt::fire_and_forget ProcessCommandAsync(std::wstring_view command) = 0;
        
        // Start an AI chat session
        virtual winrt::fire_and_forget ChatAsync(std::wstring_view message) = 0;
        
        // Execute an AI function with arguments
        virtual winrt::fire_and_forget ExecuteFunctionAsync(std::wstring_view functionName, std::wstring_view args) = 0;
        
        // Check if the engine is initialized and ready
        virtual bool IsReady() const = 0;
        
        // Events for AI responses and errors
        winrt::event<winrt::Windows::Foundation::TypedEventHandler<AIEngine, winrt::hstring>> ResponseReceived;
        winrt::event<winrt::Windows::Foundation::TypedEventHandler<AIEngine, winrt::hstring>> ErrorOccurred;
        
    protected:
        // Helper method to fire response received event
        void _FireResponseReceived(const winrt::hstring& response)
        {
            ResponseReceived(*this, response);
        }
        
        // Helper method to fire error occurred event
        void _FireErrorOccurred(const winrt::hstring& error)
        {
            ErrorOccurred(*this, error);
        }
    };
    
    // Basic AI engine implementation for testing and demonstration
    class BasicAIEngine : public AIEngine
    {
    public:
        BasicAIEngine();
        
        winrt::fire_and_forget InitializeAsync() override;
        winrt::fire_and_forget ProcessCommandAsync(std::wstring_view command) override;
        winrt::fire_and_forget ChatAsync(std::wstring_view message) override;
        winrt::fire_and_forget ExecuteFunctionAsync(std::wstring_view functionName, std::wstring_view args) override;
        bool IsReady() const override;
        
    private:
        bool _initialized = false;
        
        // Simulate AI processing with basic responses
        winrt::hstring _generateBasicResponse(std::wstring_view input);
    };
}