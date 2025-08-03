#include "pch.h"
#include "AISettings.h"
#include "AISettings.g.cpp"
#include "JsonUtils.h"
#include "TerminalSettingsSerializationHelpers.h"

using namespace winrt::Microsoft::Terminal::Settings::Model::implementation;

static constexpr std::string_view AIProviderKey{ "aiProvider" };
static constexpr std::string_view APIKeyKey{ "apiKey" };
static constexpr std::string_view DefaultModelKey{ "defaultModel" };
static constexpr std::string_view EnableFunctionCallingKey{ "enableFunctionCalling" };
static constexpr std::string_view EnableAIChatKey{ "enableAIChat" };
static constexpr std::string_view EnableAutoCompletionKey{ "enableAutoCompletion" };
static constexpr std::string_view FunctionsDirectoryKey{ "functionsDirectory" };
static constexpr std::string_view AgentsDirectoryKey{ "agentsDirectory" };

namespace winrt::Microsoft::Terminal::Settings::Model::implementation
{
    AISettings::AISettings(winrt::guid const& guid) :
        _guid(guid)
    {
    }
    
    winrt::com_ptr<AISettings> AISettings::FromJson(const Json::Value& json)
    {
        const auto result = winrt::make_self<AISettings>();
        result->LayerJson(json);
        return result;
    }
    
    winrt::com_ptr<AISettings> AISettings::Copy(const winrt::com_ptr<AISettings>& source)
    {
        const auto result = winrt::make_self<AISettings>();
        
        result->_AIProvider = source->_AIProvider;
        result->_APIKey = source->_APIKey;
        result->_DefaultModel = source->_DefaultModel;
        result->_EnableFunctionCalling = source->_EnableFunctionCalling;
        result->_EnableAIChat = source->_EnableAIChat;
        result->_EnableAutoCompletion = source->_EnableAutoCompletion;
        result->_FunctionsDirectory = source->_FunctionsDirectory;
        result->_AgentsDirectory = source->_AgentsDirectory;
        
        return result;
    }
    
    Json::Value AISettings::ToJson() const
    {
        Json::Value json{ Json::ValueType::objectValue };
        
        JsonUtils::SetValueForKey(json, AIProviderKey, _AIProvider);
        JsonUtils::SetValueForKey(json, APIKeyKey, _APIKey);
        JsonUtils::SetValueForKey(json, DefaultModelKey, _DefaultModel);
        JsonUtils::SetValueForKey(json, EnableFunctionCallingKey, _EnableFunctionCalling);
        JsonUtils::SetValueForKey(json, EnableAIChatKey, _EnableAIChat);
        JsonUtils::SetValueForKey(json, EnableAutoCompletionKey, _EnableAutoCompletion);
        JsonUtils::SetValueForKey(json, FunctionsDirectoryKey, _FunctionsDirectory);
        JsonUtils::SetValueForKey(json, AgentsDirectoryKey, _AgentsDirectory);
        
        return json;
    }
    
    void AISettings::LayerJson(const Json::Value& json)
    {
        JsonUtils::GetValueForKey(json, AIProviderKey, _AIProvider);
        JsonUtils::GetValueForKey(json, APIKeyKey, _APIKey);
        JsonUtils::GetValueForKey(json, DefaultModelKey, _DefaultModel);
        JsonUtils::GetValueForKey(json, EnableFunctionCallingKey, _EnableFunctionCalling);
        JsonUtils::GetValueForKey(json, EnableAIChatKey, _EnableAIChat);
        JsonUtils::GetValueForKey(json, EnableAutoCompletionKey, _EnableAutoCompletion);
        JsonUtils::GetValueForKey(json, FunctionsDirectoryKey, _FunctionsDirectory);
        JsonUtils::GetValueForKey(json, AgentsDirectoryKey, _AgentsDirectory);
    }
    
    void AISettings::ExpandEnvironmentVariables()
    {
        _FunctionsDirectory = _expandPath(_FunctionsDirectory);
        _AgentsDirectory = _expandPath(_AgentsDirectory);
    }
    
    bool AISettings::HasUnfocusedAppearance() const
    {
        return false; // AI settings don't have appearance
    }
    
    bool AISettings::ShouldUseParentProcess() const
    {
        return false; // AI settings don't use parent process
    }
    
    winrt::hstring AISettings::_expandPath(winrt::hstring path)
    {
        if (path.empty())
        {
            return path;
        }
        
        // Simple environment variable expansion for %USERPROFILE%
        std::wstring pathStr{ path };
        size_t pos = pathStr.find(L"%USERPROFILE%");
        if (pos != std::wstring::npos)
        {
            wchar_t userProfile[MAX_PATH];
            if (GetEnvironmentVariableW(L"USERPROFILE", userProfile, MAX_PATH))
            {
                pathStr.replace(pos, 13, userProfile); // 13 is length of "%USERPROFILE%"
            }
        }
        
        return winrt::hstring{ pathStr };
    }
}