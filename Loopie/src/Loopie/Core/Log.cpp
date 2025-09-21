#include "Log.h"

namespace Loopie {

	void Log::Init() {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        console_sink->set_pattern("%^[%T][Loopie-%l]: %v%$");

        auto logger = std::make_shared<spdlog::logger>("LoopieLogger", console_sink);
        spdlog::set_default_logger(logger);
	}
}