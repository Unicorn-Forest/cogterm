// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"
#include "JavaScriptRuntime.h"
#include <fstream>
#include <sstream>
#include <regex>

namespace Microsoft::Terminal::AI
{
    // Placeholder structure for JavaScript engine context
    struct JavaScriptRuntime::JSContext
    {
        // This would contain actual JavaScript engine state
        // For now, it's a placeholder for future V8/Chakra integration
        bool initialized = false;
        std::wstring lastError;
    };

    JavaScriptRuntime::JavaScriptRuntime()
    {
        _jsContext = std::make_unique<JSContext>();
        _initializeJSEngine();
    }

    JavaScriptRuntime::~JavaScriptRuntime()
    {
        _cleanupJSEngine();
    }

    void JavaScriptRuntime::LoadScript(std::wstring_view scriptPath)
    {
        try
        {
            _currentScriptPath = scriptPath;
            
            // Read the script file
            std::wifstream file{ std::wstring{scriptPath} };
            if (!file.is_open())
            {
                _fireExecutionError(L"Failed to open script file: " + std::wstring{scriptPath});
                return;
            }
            
            std::wstringstream buffer;
            buffer << file.rdbuf();
            std::wstring scriptContent = buffer.str();
            file.close();
            
            // Parse the script content
            _parseCliiScript(scriptContent);
            
            _initialized = true;
        }
        catch (const std::exception& ex)
        {
            std::wstring error = L"Error loading script: ";
            error += std::wstring(ex.what(), ex.what() + strlen(ex.what()));
            _fireExecutionError(error);
        }
    }

    std::vector<JavaScriptRuntime::CliiFunction> JavaScriptRuntime::GetAvailableFunctions()
    {
        std::vector<CliiFunction> functions;
        for (const auto& pair : _functions)
        {
            functions.push_back(pair.second);
        }
        return functions;
    }

    winrt::fire_and_forget JavaScriptRuntime::ExecuteFunctionAsync(std::wstring_view functionName, std::map<std::wstring, std::wstring> parameters)
    {
        co_await winrt::resume_background();
        
        try
        {
            // Find the function
            auto functionIt = _functions.find(std::wstring{functionName});
            if (functionIt == _functions.end())
            {
                _fireExecutionError(L"Function not found: " + std::wstring{functionName});
                co_return;
            }
            
            // For now, simulate function execution
            // In a real implementation, this would execute JavaScript code
            std::wstringstream result;
            result << L"Executed function: " << functionName << L" with parameters: ";
            for (const auto& param : parameters)
            {
                result << param.first << L"=" << param.second << L"; ";
            }
            
            // Simulate some processing time
            co_await winrt::resume_after(std::chrono::milliseconds(100));
            
            _fireExecutionCompleted(result.str());
        }
        catch (const std::exception& ex)
        {
            std::wstring error = L"Error executing function: ";
            error += std::wstring(ex.what(), ex.what() + strlen(ex.what()));
            _fireExecutionError(error);
        }
    }

    bool JavaScriptRuntime::IsReady() const noexcept
    {
        return _initialized && _jsContext && _jsContext->initialized;
    }

    void JavaScriptRuntime::_parseCliiScript(std::wstring_view scriptContent)
    {
        // Clear existing functions
        _functions.clear();
        
        std::wstring content{scriptContent};
        
        // Find all JSDoc comments and their positions
        std::wregex commentRegex(LR"(/\*\*[\s\S]*?\*/)");
        std::wsregex_iterator commentIter(content.begin(), content.end(), commentRegex);
        std::wsregex_iterator commentEnd;
        
        std::map<size_t, std::wstring> comments;
        for (; commentIter != commentEnd; ++commentIter)
        {
            comments[commentIter->position() + commentIter->length()] = commentIter->str();
        }
        
        // Find all function definitions
        std::wregex functionRegex(LR"(function\s+(\w+)\s*\([^)]*\)\s*\{)");
        std::wsregex_iterator funcIter(content.begin(), content.end(), functionRegex);
        std::wsregex_iterator funcEnd;
        
        for (; funcIter != funcEnd; ++funcIter)
        {
            const std::wsmatch& match = *funcIter;
            std::wstring functionName = match[1].str();
            
            CliiFunction func;
            func.name = functionName;
            func.description = L"JavaScript function: " + functionName;
            
            // Find the closest comment before this function
            auto functionPos = funcIter->position();
            std::wstring closestComment;
            size_t closestPos = 0;
            
            for (const auto& commentPair : comments)
            {
                if (commentPair.first <= functionPos && commentPair.first > closestPos)
                {
                    closestPos = commentPair.first;
                    closestComment = commentPair.second;
                }
            }
            
            if (!closestComment.empty())
            {
                // Extract description and parameters from JSDoc
                std::wregex descRegex(LR"(\*\s*([^@\*]+))");
                std::wsmatch descMatch;
                if (std::regex_search(closestComment, descMatch, descRegex))
                {
                    func.description = descMatch[1].str();
                    // Trim whitespace
                    func.description.erase(0, func.description.find_first_not_of(L" \t\r\n"));
                    func.description.erase(func.description.find_last_not_of(L" \t\r\n") + 1);
                }
                
                // Parse parameters from JSDoc
                std::wregex paramRegex(LR"(@param\s+\{([^}]+)\}\s+(\w+)\s+([^\r\n]*))");
                std::wsregex_iterator paramIter(closestComment.begin(), closestComment.end(), paramRegex);
                std::wsregex_iterator paramEnd;
                
                for (; paramIter != paramEnd; ++paramIter)
                {
                    const std::wsmatch& paramMatch = *paramIter;
                    func.parameters[paramMatch[2].str()] = paramMatch[3].str();
                }
            }
            
            _functions[functionName] = func;
        }
    }

    void JavaScriptRuntime::_extractFunctionDefinitions()
    {
        // This method is called by _parseCliiScript
        // Additional processing could be done here if needed
    }

    void JavaScriptRuntime::_initializeJSEngine()
    {
        if (_jsContext)
        {
            // Initialize JavaScript engine
            // For now, just mark as initialized
            _jsContext->initialized = true;
        }
    }

    void JavaScriptRuntime::_cleanupJSEngine()
    {
        if (_jsContext)
        {
            _jsContext->initialized = false;
        }
    }

    void JavaScriptRuntime::_fireExecutionCompleted(const winrt::hstring& result)
    {
        ExecutionCompleted(*this, result);
    }

    void JavaScriptRuntime::_fireExecutionError(const winrt::hstring& error)
    {
        ExecutionError(*this, error);
    }
}