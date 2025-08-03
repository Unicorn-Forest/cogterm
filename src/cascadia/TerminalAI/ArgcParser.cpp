#include "pch.h"
#include "ArgcParser.h"

using namespace Microsoft::Terminal::AI;

ArgcParser::ArgcParser()
{
}

ArgcParser::~ArgcParser()
{
}

bool ArgcParser::Initialize()
{
    if (_initialized)
    {
        return true;
    }

    try
    {
        // Initialize with basic built-in commands that Argc commonly handles
        _commandInfo[L"ls"] = { L"ls", L"List directory contents", {L"-l", L"-a", L"-h"}, {L"--color"}, {L"path"}, {} };
        _commandInfo[L"grep"] = { L"grep", L"Search text patterns", {L"-i", L"-v", L"-n"}, {L"--include", L"--exclude"}, {L"pattern", L"file"}, {} };
        _commandInfo[L"git"] = { L"git", L"Git version control", {L"--version"}, {L"--git-dir"}, {L"command"}, {L"add", L"commit", L"push", L"pull", L"status"} };
        
        _initialized = true;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

ArgcParser::ParsedCommand ArgcParser::ParseCommand(std::wstring_view commandLine)
{
    ParsedCommand result;
    
    if (!_initialized || commandLine.empty())
    {
        return result;
    }

    try
    {
        // Basic command line parsing
        std::wistringstream iss(std::wstring(commandLine));
        std::wstring token;
        std::vector<std::wstring> tokens;
        
        while (iss >> token)
        {
            tokens.push_back(token);
        }

        if (tokens.empty())
        {
            return result;
        }

        result.command = tokens[0];
        
        // Check if this is a known Argc command
        auto it = _commandInfo.find(result.command);
        if (it != _commandInfo.end())
        {
            result.isValid = true;
            result.description = it->second.description;
            
            // Parse arguments, options, and flags
            for (size_t i = 1; i < tokens.size(); ++i)
            {
                const auto& token = tokens[i];
                
                if (token.starts_with(L"--"))
                {
                    // Long option or flag
                    auto equalPos = token.find(L'=');
                    if (equalPos != std::wstring::npos)
                    {
                        // Option with value
                        auto optionName = token.substr(0, equalPos);
                        auto optionValue = token.substr(equalPos + 1);
                        result.options[optionName] = optionValue;
                    }
                    else
                    {
                        // Check if it's a known flag or option
                        bool isFlag = std::find(it->second.flags.begin(), it->second.flags.end(), token) != it->second.flags.end();
                        if (isFlag)
                        {
                            result.flags.push_back(token);
                        }
                        else if (i + 1 < tokens.size() && !tokens[i + 1].starts_with(L"-"))
                        {
                            // Option with separate value
                            result.options[token] = tokens[++i];
                        }
                        else
                        {
                            result.flags.push_back(token);
                        }
                    }
                }
                else if (token.starts_with(L"-") && token.length() > 1)
                {
                    // Short option or flag
                    if (i + 1 < tokens.size() && !tokens[i + 1].starts_with(L"-"))
                    {
                        // Option with value
                        result.options[token] = tokens[++i];
                    }
                    else
                    {
                        result.flags.push_back(token);
                    }
                }
                else
                {
                    // Positional argument
                    result.arguments.push_back(token);
                }
            }
        }
        else
        {
            // Unknown command, treat as simple parsing
            for (size_t i = 1; i < tokens.size(); ++i)
            {
                result.arguments.push_back(tokens[i]);
            }
        }
    }
    catch (...)
    {
        result.isValid = false;
    }

    return result;
}

std::vector<ArgcParser::CompletionSuggestion> ArgcParser::GetCompletions(std::wstring_view partialCommand)
{
    std::vector<CompletionSuggestion> suggestions;
    
    if (!_initialized || partialCommand.empty())
    {
        return suggestions;
    }

    try
    {
        std::wistringstream iss(std::wstring(partialCommand));
        std::wstring firstToken;
        iss >> firstToken;
        
        if (iss.eof())
        {
            // Completing command name
            for (const auto& [command, info] : _commandInfo)
            {
                if (command.starts_with(firstToken))
                {
                    CompletionSuggestion suggestion;
                    suggestion.completion = command;
                    suggestion.description = info.description;
                    suggestion.type = L"command";
                    suggestion.priority = 100;
                    suggestions.push_back(suggestion);
                }
            }
        }
        else
        {
            // Completing arguments, options, or flags for known command
            auto it = _commandInfo.find(firstToken);
            if (it != _commandInfo.end())
            {
                std::wstring remaining;
                std::getline(iss, remaining);
                if (!remaining.empty() && remaining[0] == L' ')
                {
                    remaining = remaining.substr(1);
                }
                
                // Get the last token for completion
                std::wstring lastToken;
                if (!remaining.empty())
                {
                    std::wistringstream remainingStream(remaining);
                    std::wstring token;
                    while (remainingStream >> token)
                    {
                        lastToken = token;
                    }
                }
                
                // Suggest flags
                for (const auto& flag : it->second.flags)
                {
                    if (lastToken.empty() || flag.starts_with(lastToken))
                    {
                        CompletionSuggestion suggestion;
                        suggestion.completion = flag;
                        suggestion.description = L"Flag for " + it->second.name;
                        suggestion.type = L"flag";
                        suggestion.priority = 80;
                        suggestions.push_back(suggestion);
                    }
                }
                
                // Suggest options
                for (const auto& option : it->second.options)
                {
                    if (lastToken.empty() || option.starts_with(lastToken))
                    {
                        CompletionSuggestion suggestion;
                        suggestion.completion = option;
                        suggestion.description = L"Option for " + it->second.name;
                        suggestion.type = L"option";
                        suggestion.priority = 70;
                        suggestions.push_back(suggestion);
                    }
                }
                
                // Suggest subcommands
                for (const auto& subcommand : it->second.subcommands)
                {
                    if (lastToken.empty() || subcommand.starts_with(lastToken))
                    {
                        CompletionSuggestion suggestion;
                        suggestion.completion = subcommand;
                        suggestion.description = L"Subcommand of " + it->second.name;
                        suggestion.type = L"command";
                        suggestion.priority = 90;
                        suggestions.push_back(suggestion);
                    }
                }
            }
        }
        
        // Sort by priority (higher first)
        std::sort(suggestions.begin(), suggestions.end(), 
                  [](const CompletionSuggestion& a, const CompletionSuggestion& b) {
                      return a.priority > b.priority;
                  });
    }
    catch (...)
    {
        // Return empty suggestions on error
    }

    return suggestions;
}

std::wstring ArgcParser::GenerateHelp(std::wstring_view command)
{
    if (!_initialized)
    {
        return L"Argc parser not initialized";
    }

    auto it = _commandInfo.find(std::wstring(command));
    if (it == _commandInfo.end())
    {
        return L"Unknown command: " + std::wstring(command);
    }

    std::wstring help = L"USAGE: " + it->second.name;
    
    if (!it->second.flags.empty() || !it->second.options.empty())
    {
        help += L" [OPTIONS]";
    }
    
    for (const auto& arg : it->second.arguments)
    {
        help += L" <" + arg + L">";
    }
    
    if (!it->second.subcommands.empty())
    {
        help += L" [SUBCOMMAND]";
    }
    
    help += L"\n\n" + it->second.description + L"\n";
    
    if (!it->second.arguments.empty())
    {
        help += L"\nARGUMENTS:\n";
        for (const auto& arg : it->second.arguments)
        {
            help += L"  <" + arg + L">    " + arg + L" argument\n";
        }
    }
    
    if (!it->second.flags.empty() || !it->second.options.empty())
    {
        help += L"\nOPTIONS:\n";
        for (const auto& flag : it->second.flags)
        {
            help += L"  " + flag + L"    Flag option\n";
        }
        for (const auto& option : it->second.options)
        {
            help += L"  " + option + L" <VALUE>    Option parameter\n";
        }
    }
    
    if (!it->second.subcommands.empty())
    {
        help += L"\nSUBCOMMANDS:\n";
        for (const auto& subcommand : it->second.subcommands)
        {
            help += L"  " + subcommand + L"    " + subcommand + L" subcommand\n";
        }
    }

    return help;
}

bool ArgcParser::IsValidCommand(std::wstring_view command)
{
    if (!_initialized)
    {
        return false;
    }
    
    return _commandInfo.find(std::wstring(command)) != _commandInfo.end();
}

bool ArgcParser::LoadCommandDefinitions(std::wstring_view definitionsPath)
{
    // For now, return true as we have built-in definitions
    // In a full implementation, this would load from Argc script files
    return true;
}

bool ArgcParser::AddCommandDefinition(std::wstring_view name, std::wstring_view definition)
{
    if (!_initialized)
    {
        return false;
    }

    try
    {
        ArgcCommandInfo info;
        info.name = name;
        info.description = definition;
        
        // Basic parsing of definition - in a full implementation,
        // this would parse Argc comment tags
        _commandInfo[std::wstring(name)] = info;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

std::vector<std::wstring> ArgcParser::GetAvailableCommands()
{
    std::vector<std::wstring> commands;
    
    if (!_initialized)
    {
        return commands;
    }

    for (const auto& [command, info] : _commandInfo)
    {
        commands.push_back(command);
    }
    
    return commands;
}

bool ArgcParser::_parseArgcScript(std::wstring_view scriptContent, std::wstring_view commandName)
{
    // Placeholder for full Argc script parsing
    // In a complete implementation, this would parse comment tags like:
    // # @describe "Command description"
    // # @flag -f --flag "Flag description"  
    // # @option --option "Option description"
    // # @arg argument "Argument description"
    return true;
}

std::vector<std::wstring> ArgcParser::_extractArgcTags(std::wstring_view scriptContent)
{
    std::vector<std::wstring> tags;
    // Placeholder for extracting @ tags from Argc scripts
    return tags;
}

ArgcParser::ParsedCommand ArgcParser::_parseWithArgcRules(std::wstring_view commandLine, std::wstring_view commandName)
{
    // Placeholder for advanced Argc rule-based parsing
    return ParseCommand(commandLine);
}

std::wstring ArgcParser::_generateArgcEvaluation(std::wstring_view command, std::wstring_view args)
{
    // Placeholder for generating Argc evaluation strings
    return L"eval \"$(argc --argc-eval \"" + std::wstring(command) + L"\" " + std::wstring(args) + L")\"";
}

bool ArgcParser::_isArgcCommand(std::wstring_view command)
{
    return IsValidCommand(command);
}

bool ArgcParser::_analyzeArgcScript(std::wstring_view scriptContent, ArgcCommandInfo& info)
{
    // Placeholder for analyzing Argc scripts to extract command information
    return true;
}

std::unique_ptr<ArgcParser> Microsoft::Terminal::AI::CreateArgcParser()
{
    auto parser = std::make_unique<ArgcParser>();
    if (parser->Initialize())
    {
        return parser;
    }
    return nullptr;
}