// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"
#include "AISettingsPage.h"
#include "AISettingsPage.g.cpp"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;

namespace winrt::TerminalSettingsEditor::implementation
{
    AISettingsPageViewModel::AISettingsPageViewModel()
    {
        // Initialize with default values
        EnableAIChat(false);
        EnableFunctionCalling(false);
        AIProvider(L"");
        APIKey(L"");
        DefaultModel(L"");
    }

    AISettingsPage::AISettingsPage()
    {
        InitializeComponent();

        _ViewModel = winrt::make<TerminalSettingsEditor::implementation::AISettingsPageViewModel>();

        _UpdateWithSettings();
    }

    void AISettingsPage::ViewChanging(IInspectable const& /*sender*/, ScrollViewerViewChangingEventArgs const& /*e*/)
    {
        // Update settings when the view changes
        _UpdateWithSettings();
    }

    void AISettingsPage::_UpdateWithSettings()
    {
        // TODO: In the future, this would load settings from the global settings store
        // For now, we'll use the default values from the ViewModel
        
        // Example of how this might work:
        // auto globalSettings = _GlobalSettings.lock();
        // if (globalSettings)
        // {
        //     _ViewModel.EnableAIChat(globalSettings->EnableAIChat());
        //     _ViewModel.EnableFunctionCalling(globalSettings->EnableFunctionCalling());
        //     _ViewModel.AIProvider(globalSettings->AIProvider());
        //     _ViewModel.DefaultModel(globalSettings->DefaultModel());
        //     // Note: API Key would typically not be displayed for security reasons
        // }
    }
}