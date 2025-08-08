// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include "AISettingsPage.g.h"
#include "ViewModelHelpers.h"

namespace winrt::TerminalSettingsEditor::implementation
{
    struct AISettingsPageViewModel : public ViewModelHelpers::ViewModelHelper<AISettingsPageViewModel>
    {
    public:
        // Constructor
        AISettingsPageViewModel();

        // Property getters/setters
        GETSET_BINDABLE_ENUM_SETTING(bool, EnableAIChat, false);
        GETSET_BINDABLE_ENUM_SETTING(bool, EnableFunctionCalling, false);
        GETSET_BINDABLE_ENUM_SETTING(winrt::hstring, AIProvider, L"");
        GETSET_BINDABLE_ENUM_SETTING(winrt::hstring, APIKey, L"");
        GETSET_BINDABLE_ENUM_SETTING(winrt::hstring, DefaultModel, L"");
    };

    struct AISettingsPage : AISettingsPageT<AISettingsPage>
    {
    public:
        AISettingsPage();

        void ViewChanging(Windows::Foundation::IInspectable const& sender,
                          Windows::UI::Xaml::Controls::ScrollViewerViewChangingEventArgs const& e);

        WINRT_CALLBACK(PropertyChanged, Windows::UI::Xaml::Data::PropertyChangedEventHandler);
        WINRT_PROPERTY(TerminalSettingsEditor::AISettingsPageViewModel, ViewModel, nullptr);

    private:
        void _UpdateWithSettings();

    };
}

namespace winrt::TerminalSettingsEditor::factory_implementation
{
    BASIC_FACTORY(AISettingsPage);
}