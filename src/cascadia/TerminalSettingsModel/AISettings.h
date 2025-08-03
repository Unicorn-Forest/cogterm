#pragma once

#include "AISettings.g.h"
#include "IInheritable.h"

namespace winrt::Microsoft::Terminal::Settings::Model::implementation
{
    struct AISettings : public AISettingsT<AISettings>, IInheritable<AISettings>
    {
        AISettings() = default;
        AISettings(winrt::guid const& guid);
        
        // AI Provider settings
        WINRT_PROPERTY(winrt::hstring, AIProvider, L"openai");
        WINRT_PROPERTY(winrt::hstring, APIKey, L"");
        WINRT_PROPERTY(winrt::hstring, DefaultModel, L"gpt-4");
        
        // Feature toggles
        WINRT_PROPERTY(bool, EnableFunctionCalling, true);
        WINRT_PROPERTY(bool, EnableAIChat, true);
        WINRT_PROPERTY(bool, EnableAutoCompletion, false);
        
        // Directory settings
        WINRT_PROPERTY(winrt::hstring, FunctionsDirectory, L"%USERPROFILE%\\.terminal\\functions");
        WINRT_PROPERTY(winrt::hstring, AgentsDirectory, L"%USERPROFILE%\\.terminal\\agents");
        
        static winrt::com_ptr<AISettings> FromJson(const Json::Value& json);
        static winrt::com_ptr<AISettings> Copy(const winrt::com_ptr<AISettings>& source);
        
        Json::Value ToJson() const;
        void LayerJson(const Json::Value& json);
        
        void ExpandEnvironmentVariables();
        
        bool HasUnfocusedAppearance() const;
        bool ShouldUseParentProcess() const;
        
    private:
        winrt::guid _guid{};
        
        static winrt::hstring _expandPath(winrt::hstring path);
    };
}

namespace winrt::Microsoft::Terminal::Settings::Model::factory_implementation
{
    BASIC_FACTORY(AISettings);
}