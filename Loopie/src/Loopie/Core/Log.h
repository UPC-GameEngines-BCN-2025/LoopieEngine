#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Loopie {
	class Log
	{
	public:
		static void Init();

		template <typename... Args>
		static void Trace(const char* msg, Args&&... args)
		{
			spdlog::trace(msg, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static void Info(const char* msg, Args&&... args)
		{
			spdlog::info(msg, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static void Debug(const char* msg, Args&&... args)
		{
			spdlog::debug(msg, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static void Warn(const char* msg, Args&&... args)
		{
			spdlog::warn(msg, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static void Error(const char* msg, Args&&... args)
		{
			spdlog::error(msg, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static void Critical(const char* msg, Args&&... args)
		{
			spdlog::critical(msg, std::forward<Args>(args)...);
		}
	};
}