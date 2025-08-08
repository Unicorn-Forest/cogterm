// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include "pch.h"
#include <map>
#include <vector>
#include <memory>

namespace Microsoft::Terminal::AI
{
    // JavaScript execution runtime for Clii integration
    class JavaScriptRuntime
    {
    public:
        JavaScriptRuntime();
        ~JavaScriptRuntime();
        
        struct CliiFunction
        {
            std::wstring name;
            std::wstring description;
            std::map<std::wstring, std::wstring> parameters;
        };
        
        // Load and parse a JavaScript file containing Clii functions
        void LoadScript(std::wstring_view scriptPath);
        
        // Get list of available functions from loaded scripts
        std::vector<CliiFunction> GetAvailableFunctions();
        
        // Execute a specific function with given parameters
        winrt::fire_and_forget ExecuteFunctionAsync(std::wstring_view functionName, std::map<std::wstring, std::wstring> parameters);
        
        // Events for function execution results and errors
        winrt::event<winrt::Windows::Foundation::TypedEventHandler<JavaScriptRuntime, winrt::hstring>> ExecutionCompleted;
        winrt::event<winrt::Windows::Foundation::TypedEventHandler<JavaScriptRuntime, winrt::hstring>> ExecutionError;
        
        // Check if runtime is initialized and ready
        bool IsReady() const noexcept;
        
    private:
        // JavaScript engine context (placeholder for actual JS engine like V8 or Chakra)
        struct JSContext;
        std::unique_ptr<JSContext> _jsContext;
        
        // Registry of available functions
        std::map<std::wstring, CliiFunction> _functions;
        
        // Internal state
        bool _initialized = false;
        std::wstring _currentScriptPath;
        
        // Parse a Clii script and extract function definitions
        void _parseCliiScript(std::wstring_view scriptContent);
        
        // Extract function definitions from JavaScript code
        void _extractFunctionDefinitions();
        
        // Initialize the JavaScript engine
        void _initializeJSEngine();
        
        // Clean up JavaScript engine resources
        void _cleanupJSEngine();
        
        // Helper to fire execution completed event
        void _fireExecutionCompleted(const winrt::hstring& result);
        
        // Helper to fire execution error event
        void _fireExecutionError(const winrt::hstring& error);
    };
}