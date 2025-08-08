// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include "AISettingsPage.g.h"
#include "ViewModelHelpers.h"

namespace winrt::TerminalSettingsEditor::implementation
{
    struct AISettingsPage : AISettingsPageT<AISettingsPage>
    {
    public:
        AISettingsPage();

        void ViewChanging(Windows::Foundation::IInspectable const& sender,
                          Windows::UI::Xaml::Controls::ScrollViewerViewChangingEventArgs const& e);

        WINRT_CALLBACK(PropertyChanged, Windows::UI::Xaml::Data::PropertyChangedEventHandler);
        WINRT_PROPERTY(Microsoft::Terminal::Settings::Model::AISettings, AISettings, nullptr);

    private:
        void _UpdateWithSettings();
    };
}

namespace winrt::TerminalSettingsEditor::factory_implementation
{
    BASIC_FACTORY(AISettingsPage);
}