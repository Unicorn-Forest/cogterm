#include "pch.h"
#include "FunctionCallingEngine.h"

using namespace winrt;
using namespace Microsoft::Terminal::AI;

FunctionCallingEngine::FunctionCallingEngine()
{
}

void FunctionCallingEngine::LoadFunctions(std::wstring_view functionsDir)
{
    _functionsDirectory = functionsDir;
    _loadFunctionDefinitions();
}

winrt::fire_and_forget FunctionCallingEngine::ExecuteFunctionAsync(std::wstring_view functionName, std::wstring_view arguments)
{
    auto iter = _functions.find(std::wstring(functionName));
    if (iter == _functions.end())
    {
        co_return;
    }

    const auto& definition = iter->second;
    const auto scriptPath = definition.scriptPath;
    
    // Determine script type by extension and execute accordingly
    if (scriptPath.ends_with(L".sh") || scriptPath.ends_with(L".bash"))
    {
        co_await _executeBashFunction(scriptPath, arguments);
    }
    else if (scriptPath.ends_with(L".js"))
    {
        co_await _executeJavaScriptFunction(scriptPath, arguments);
    }
    else if (scriptPath.ends_with(L".py"))
    {
        co_await _executePythonFunction(scriptPath, arguments);
    }
}

std::vector<FunctionCallingEngine::FunctionDefinition> FunctionCallingEngine::GetAvailableFunctions()
{
    std::vector<FunctionDefinition> result;
    result.reserve(_functions.size());
    
    for (const auto& [name, definition] : _functions)
    {
        result.push_back(definition);
    }
    
    return result;
}

void FunctionCallingEngine::_loadFunctionDefinitions()
{
    _functions.clear();
    
    if (_functionsDirectory.empty())
    {
        return;
    }
    
    std::filesystem::path funcDir(_functionsDirectory);
    if (!std::filesystem::exists(funcDir) || !std::filesystem::is_directory(funcDir))
    {
        return;
    }
    
    try 
    {
        // Scan for .json definition files
        for (const auto& entry : std::filesystem::directory_iterator(funcDir))
        {
            if (!entry.is_regular_file())
            {
                continue;
            }
            
            const auto& path = entry.path();
            if (path.extension() != L".json")
            {
                continue;
            }
            
            // Read and parse the JSON function definition
            const auto jsonContent = til::io::read_file_as_utf8_string_if_exists(path);
            if (jsonContent.empty())
            {
                continue;
            }
            
            try 
            {
                Json::Value root;
                std::string errs;
                const std::unique_ptr<Json::CharReader> reader{ Json::CharReaderBuilder{}.newCharReader() };
                
                if (reader->parse(jsonContent.data(), jsonContent.data() + jsonContent.size(), &root, &errs))
                {
                    // Parse function definition from JSON
                    FunctionDefinition funcDef;
                    
                    if (root.isMember("name") && root["name"].isString())
                    {
                        funcDef.name = til::u8u16(root["name"].asString());
                    }
                    
                    if (root.isMember("description") && root["description"].isString())
                    {
                        funcDef.description = til::u8u16(root["description"].asString());
                    }
                    
                    if (root.isMember("parameters"))
                    {
                        Json::StreamWriterBuilder builder;
                        const auto parametersStr = Json::writeString(builder, root["parameters"]);
                        funcDef.parameters = til::u8u16(parametersStr);
                    }
                    
                    if (root.isMember("scriptPath") && root["scriptPath"].isString())
                    {
                        // Handle relative paths by making them relative to the functions directory
                        const auto scriptPathUtf8 = root["scriptPath"].asString();
                        std::filesystem::path scriptPath(til::u8u16(scriptPathUtf8));
                        if (scriptPath.is_relative())
                        {
                            scriptPath = funcDir / scriptPath;
                        }
                        funcDef.scriptPath = scriptPath.wstring();
                    }
                    
                    // Only add if we have required fields
                    if (!funcDef.name.empty() && !funcDef.scriptPath.empty())
                    {
                        _functions[funcDef.name] = std::move(funcDef);
                    }
                }
            }
            catch (...)
            {
                // Skip malformed JSON files
                continue;
            }
        }
    }
    catch (...)
    {
        // If directory scanning fails, just clear functions and continue
        _functions.clear();
    }
}

winrt::fire_and_forget FunctionCallingEngine::_executeBashFunction(std::wstring_view scriptPath, std::wstring_view args)
{
    co_await winrt::resume_background();
    
    try 
    {
        // Build command line: bash.exe "script_path" arguments
        std::wstring cmdline = L"bash.exe \"";
        cmdline += scriptPath;
        cmdline += L"\" ";
        cmdline += args;
        
        STARTUPINFOW si = {};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE; // Hide the window
        
        PROCESS_INFORMATION pi = {};
        
        // Create the process
        if (CreateProcessW(
            nullptr,              // Application name (use command line)
            cmdline.data(),       // Command line
            nullptr,              // Process attributes
            nullptr,              // Thread attributes  
            FALSE,                // Inherit handles
            CREATE_NO_WINDOW,     // Creation flags - no window
            nullptr,              // Environment
            nullptr,              // Current directory
            &si,                  // Startup info
            &pi))                 // Process information
        {
            // Wait for the process to complete (with a timeout)
            const DWORD waitResult = WaitForSingleObject(pi.hProcess, 30000); // 30 second timeout
            
            // Clean up handles
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
    catch (...)
    {
        // Log error or handle failure silently
    }
}

winrt::fire_and_forget FunctionCallingEngine::_executeJavaScriptFunction(std::wstring_view scriptPath, std::wstring_view args)
{
    co_await winrt::resume_background();
    
    try 
    {
        // Build command line: node.exe "script_path" arguments
        std::wstring cmdline = L"node.exe \"";
        cmdline += scriptPath;
        cmdline += L"\" ";
        cmdline += args;
        
        STARTUPINFOW si = {};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE; // Hide the window
        
        PROCESS_INFORMATION pi = {};
        
        // Create the process
        if (CreateProcessW(
            nullptr,              // Application name (use command line)
            cmdline.data(),       // Command line
            nullptr,              // Process attributes
            nullptr,              // Thread attributes  
            FALSE,                // Inherit handles
            CREATE_NO_WINDOW,     // Creation flags - no window
            nullptr,              // Environment
            nullptr,              // Current directory
            &si,                  // Startup info
            &pi))                 // Process information
        {
            // Wait for the process to complete (with a timeout)
            const DWORD waitResult = WaitForSingleObject(pi.hProcess, 30000); // 30 second timeout
            
            // Clean up handles
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
    catch (...)
    {
        // Log error or handle failure silently
    }
}

winrt::fire_and_forget FunctionCallingEngine::_executePythonFunction(std::wstring_view scriptPath, std::wstring_view args)
{
    co_await winrt::resume_background();
    
    try 
    {
        // Build command line: python.exe "script_path" arguments
        std::wstring cmdline = L"python.exe \"";
        cmdline += scriptPath;
        cmdline += L"\" ";
        cmdline += args;
        
        STARTUPINFOW si = {};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE; // Hide the window
        
        PROCESS_INFORMATION pi = {};
        
        // Create the process
        if (CreateProcessW(
            nullptr,              // Application name (use command line)
            cmdline.data(),       // Command line
            nullptr,              // Process attributes
            nullptr,              // Thread attributes  
            FALSE,                // Inherit handles
            CREATE_NO_WINDOW,     // Creation flags - no window
            nullptr,              // Environment
            nullptr,              // Current directory
            &si,                  // Startup info
            &pi))                 // Process information
        {
            // Wait for the process to complete (with a timeout)
            const DWORD waitResult = WaitForSingleObject(pi.hProcess, 30000); // 30 second timeout
            
            // Clean up handles
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
    catch (...)
    {
        // Log error or handle failure silently
    }
}