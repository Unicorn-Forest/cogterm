#pragma once

#include <windows.h>
#include <unknwn.h>
#include <hstring.h>
#include <restrictederrorinfo.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>

#include <winrt/Microsoft.Terminal.Core.h>
#include <winrt/Microsoft.Terminal.Settings.Model.h>

// Standard library
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <filesystem>

// WIL
#include <wil/common.h>
#include <wil/result.h>
#include <wil/wistd_memory.h>

// JSON
#include <json/json.h>

// Manually include til after we include Windows.Foundation to give it winrt superpowers
#include "til.h"
#include <til/io.h>