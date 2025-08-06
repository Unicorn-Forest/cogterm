#include "pch.h"
#include "AIAgent.h"

using namespace winrt;
using namespace Microsoft::Terminal::AI;

AIAgent::AIAgent(std::wstring_view agentName)
{
    _definition.name = agentName;
    _functionEngine = std::make_unique<FunctionCallingEngine>();
    _aiEngine = std::make_unique<BasicAIEngine>();
}

void AIAgent::LoadAgent(std::wstring_view agentPath)
{
    _loadAgentDefinition(agentPath);
    
    // Initialize the AI engine
    try
    {
        _aiEngine->InitializeAsync();
        
        // Set up event handlers for AI engine responses
        _aiEngine->ResponseReceived([this](auto&&, auto&& response) {
            _FireResponseReceived(response);
        });
        
        _aiEngine->ErrorOccurred([this](auto&&, auto&& error) {
            _FireErrorOccurred(error);
        });
    }
    catch (...)
    {
        _FireErrorOccurred(L"Failed to initialize AI engine for agent");
    }
}

winrt::fire_and_forget AIAgent::ExecuteAgentAsync(std::wstring_view userInput)
{
    try
    {
        // Check if the agent has tools available
        if (!_definition.tools.empty())
        {
            co_await _processWithTools(userInput);
        }
        else
        {
            // Process directly with AI engine if no tools are configured
            co_await _aiEngine->ChatAsync(userInput);
        }
    }
    catch (...)
    {
        _FireErrorOccurred(L"Error executing agent task");
    }
}

void AIAgent::_loadAgentDefinition(std::wstring_view agentPath)
{
    // TODO: Implement loading agent definition from file
    // This would parse a JSON/YAML file containing agent configuration
    
    // For now, set up a basic agent definition
    _definition.description = L"AI agent for terminal assistance";
    _definition.instructions = L"You are a helpful AI assistant integrated into the terminal.";
    _definition.tools.clear();
    _definition.documents.clear();
    
    // Example: Add some default tools
    _definition.tools.push_back(L"example_function");
    
    // Load functions if tools are defined
    if (!_definition.tools.empty() && _functionEngine)
    {
        // Extract directory from agent path for functions
        auto lastSlash = agentPath.rfind(L'/');
        if (lastSlash == std::wstring_view::npos)
        {
            lastSlash = agentPath.rfind(L'\\');
        }
        
        std::wstring functionsDir;
        if (lastSlash != std::wstring_view::npos)
        {
            functionsDir = std::wstring(agentPath.substr(0, lastSlash)) + L"/functions";
        }
        else
        {
            functionsDir = L"./functions";
        }
        
        _functionEngine->LoadFunctions(functionsDir);
    }
}

winrt::fire_and_forget AIAgent::_processWithTools(std::wstring_view userInput)
{
    // TODO: Implement intelligent tool selection and execution
    // This would analyze the user input, determine which tools to use,
    // execute them via the function calling engine, and integrate results
    
    // For now, just pass to the AI engine
    co_await _aiEngine->ChatAsync(userInput);
}