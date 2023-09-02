#pragma once

#include <Windows.h>
#include <Shlobj.h>
#include <Shlobj_core.h>
#include <tchar.h>
#include <array>
#include <fstream>
#include <filesystem>
#include <string>
#include <string_view>
#include <regex>
#include <memory>
#include <unordered_set>
#include <unordered_map>

#include <urmem.hpp>
#include <nlohmann/json.hpp>

#include "resource.h"

using json = nlohmann::json;
