#include <string>
#include <filesystem>
#include "common/logging/log.h"
#include "common/path_util.h"

namespace Common::FS {
    // Stub implementation of GetUserPath
    std::filesystem::path GetUserPath(PathType type) {
        return std::filesystem::current_path();
    }
    
    // Stub implementation of PathToUTF8String
    std::string PathToUTF8String(const std::filesystem::path& path) {
        return path.string();
    }
}

namespace Common::Log {
    // Stub implementation of FmtLogMessageImpl
    void FmtLogMessageImpl(Class cls, Level lvl, const char* filename, unsigned int line,
                          const char* function, const char* format,
                          const fmt::v11::basic_format_args<fmt::v11::context>& args) {
        // No-op implementation for CLI tool
    }
}

namespace Config {
    // Stub implementation for trophy key function
    std::string getTrophyKey() {
        // Return dummy key for CLI tool
        return "0000000000000000000000000000000000000000000000000000000000000000";
    }
}