#pragma once

#include "pch.h"

namespace Microsoft::Terminal::AI
{
    // Argc command parsing integration for intelligent CLI parsing and autocompletion
    class ArgcParser
    {
    public:
        ArgcParser();
        ~ArgcParser();

        // Structure to hold parsed command information
        struct ParsedCommand
        {
            std::wstring command;
            std::vector<std::wstring> arguments;
            std::map<std::wstring, std::wstring> options;
            std::vector<std::wstring> flags;
            std::wstring description;
            bool isValid = false;
        };

        // Structure for completion suggestions
        struct CompletionSuggestion
        {
            std::wstring completion;
            std::wstring description;
            std::wstring type; // "command", "option", "flag", "argument"
            int priority = 0;
        };

        // Initialize the Argc parser with available commands
        bool Initialize();

        // Parse a command line using Argc rules
        ParsedCommand ParseCommand(std::wstring_view commandLine);

        // Get completion suggestions for partial command
        std::vector<CompletionSuggestion> GetCompletions(std::wstring_view partialCommand);

        // Generate help text for a command
        std::wstring GenerateHelp(std::wstring_view command);

        // Check if a command is recognized by Argc
        bool IsValidCommand(std::wstring_view command);

        // Load command definitions from Argc scripts
        bool LoadCommandDefinitions(std::wstring_view definitionsPath);

        // Add a custom command definition
        bool AddCommandDefinition(std::wstring_view name, std::wstring_view definition);

        // Get list of available commands
        std::vector<std::wstring> GetAvailableCommands();

    private:
        bool _initialized = false;
        std::map<std::wstring, std::wstring> _commandDefinitions;
        std::map<std::wstring, std::vector<std::wstring>> _commandCompletions;

        // Internal helper methods
        bool _parseArgcScript(std::wstring_view scriptContent, std::wstring_view commandName);
        std::vector<std::wstring> _extractArgcTags(std::wstring_view scriptContent);
        ParsedCommand _parseWithArgcRules(std::wstring_view commandLine, std::wstring_view commandName);
        std::wstring _generateArgcEvaluation(std::wstring_view command, std::wstring_view args);
        bool _isArgcCommand(std::wstring_view command);

        // Command analysis helpers
        struct ArgcCommandInfo
        {
            std::wstring name;
            std::wstring description;
            std::vector<std::wstring> flags;
            std::vector<std::wstring> options;
            std::vector<std::wstring> arguments;
            std::vector<std::wstring> subcommands;
        };
        
        std::map<std::wstring, ArgcCommandInfo> _commandInfo;
        bool _analyzeArgcScript(std::wstring_view scriptContent, ArgcCommandInfo& info);
    };

    // Factory function for creating ArgcParser instances
    std::unique_ptr<ArgcParser> CreateArgcParser();
}