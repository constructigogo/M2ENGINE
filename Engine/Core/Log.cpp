//
// Created by Constantin on 03/08/2023.
//

#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace Engine {

    std::shared_ptr<spdlog::logger> Log::CoreLogger;
    std::shared_ptr<spdlog::logger> Log::ClientLogger;

    void Log::Init() {
        spdlog::set_pattern("%^[%T] %n: %v%$");
        CoreLogger = spdlog::stdout_color_mt("Engine");
        CoreLogger->set_level(spdlog::level::trace);
        ClientLogger = spdlog::stdout_color_mt("Debug");
        ClientLogger->set_level(spdlog::level::trace);
        ENGINE_TRACE("Init Logger");
    }


} // Engine