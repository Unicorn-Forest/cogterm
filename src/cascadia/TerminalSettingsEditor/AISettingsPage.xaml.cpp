// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"
#include "AISettingsPage.h"
#include "AISettingsPage.g.cpp"
#include "..\TerminalSettingsModel\AISettings.h"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Microsoft::Terminal::Settings::Model;

namespace winrt::TerminalSettingsEditor::implementation
{
    AISettingsPage::AISettingsPage()
    {
        InitializeComponent();

        // Initialize with default AI settings
        _AISettings = winrt::make<Model::implementation::AISettings>();
        
        _UpdateWithSettings();
    }

    void AISettingsPage::ViewChanging(IInspectable const& /*sender*/, ScrollViewerViewChangingEventArgs const& /*e*/)
    {
        // Update settings when the view changes
        _UpdateWithSettings();
    }

    void AISettingsPage::_UpdateWithSettings()
    {
        if (_AISettings)
        {
            // Settings binding will be handled automatically through x:Bind in XAML
            // The AISettings object will be used directly as the binding source
        }
    }
}