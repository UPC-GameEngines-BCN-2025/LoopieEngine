#pragma once

#include "Loopie/Core/Log.h"

#include <string>
#include <filesystem>


namespace Loopie {

#define ASSERT(check, msg, ...) if (check){ Log::Critical((std::string("(") + std::filesystem::path(__FILE__).filename().string() + ":" + STRINGIFY(__LINE__) + ") -> " + msg).c_str(), ##__VA_ARGS__); __debugbreak(); }


#define STRINGIFY_DETAIL(x) #x
#define STRINGIFY(x) STRINGIFY_DETAIL(x)
}