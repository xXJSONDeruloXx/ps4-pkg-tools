#include <iostream>
#include <filesystem>
#include <string>
#include "core/file_format/pkg.h"
#include "common/logging/log.h"

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: ps4-pkg-tool <path/to/pkg> <path/to/output>\n";
        return 1;
    }

    std::filesystem::path pkgPath = argv[1];
    std::filesystem::path outDir = argv[2];

    if (!std::filesystem::exists(pkgPath)) {
        std::cerr << "Error: PKG file not found: " << pkgPath << "\n";
        return 1;
    }

    // Create output directory if it doesn't exist
    if (!std::filesystem::exists(outDir)) {
        try {
            std::filesystem::create_directories(outDir);
        } catch (const std::exception& e) {
            std::cerr << "Error creating output directory: " << e.what() << "\n";
            return 1;
        }
    }

    PKG pkg;
    std::string failReason;

    std::cout << "Extracting PKG: " << pkgPath.filename() << "\n";
    std::cout << "Output directory: " << outDir << "\n";

    if (!pkg.Extract(pkgPath, outDir, failReason)) {
        std::cerr << "Extraction failed: " << failReason << "\n";
        return 2;
    }
    
    // Extract all files
    u32 numFiles = pkg.GetNumberOfFiles();
    std::cout << "Extracting " << numFiles << " files...\n";
    
    for (u32 i = 0; i < numFiles; i++) {
        pkg.ExtractFiles(i);
    }

    std::cout << "Extraction completed successfully\n";
    std::cout << "Files extracted to: " << outDir << "\n";
    return 0;
}
