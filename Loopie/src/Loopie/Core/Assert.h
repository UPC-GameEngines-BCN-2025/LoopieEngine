#pragma once

#include "Loopie/Core/Log.h"
#include "Loopie/Core/PlatformChecker.h"

#include <string>
#include <filesystem>

namespace Loopie {
	#ifdef LOOPIE_WINDOWS
		#define LOOPIE_DEBUGBREAK __debugbreak()
	#elif defined(LOOPIE_LINUX) || defined(LOOPIE_MACOS)
		#include <signal.h>
		#define LOOPIE_DEBUGBREAK raise(SIGTRAP)
	#endif

	#define ASSERT(check, msg, ...) if (check){ Log::Critical((std::string("(") + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ") -> " + msg).c_str(), ##__VA_ARGS__); LOOPIE_DEBUGBREAK; }
}