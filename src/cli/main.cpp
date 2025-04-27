#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include "core/file_format/pkg.h"
#include "common/logging/log.h"

// Helper function to safely extract a single file index
bool ExtractSingleFile(PKG& pkg, int index) {
    try {
        pkg.ExtractFiles(index);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception extracting file " << index << ": " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "Unknown error extracting file " << index << std::endl;
        return false;
    }
}

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: ps4-pkg-tool <path/to/pkg> [path/to/output]\n";
        std::cerr << "       If output path is omitted, the PKG will be extracted to its parent directory\n";
        return 1;
    }

    std::filesystem::path pkgPath = argv[1];
    std::filesystem::path outDir;
    
    // Use the PKG's parent directory as the default output if no output directory is specified
    if (argc == 2) {
        outDir = pkgPath.parent_path();
        std::cout << "No output directory specified. Using PKG parent directory: " << outDir << std::endl;
    } else {
        outDir = argv[2];
    }

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
    
    // We need to ensure that the directory structure is set up correctly for 
    // the PKG extraction. The extractor will try to create a directory with 
    // the title ID at the parent path level.
    PKG pkg;
    std::string failReason;

    // First just try to open the PKG to read metadata
    if (!pkg.Open(pkgPath, failReason)) {
        std::cerr << "Failed to open PKG file: " << failReason << "\n";
        return 2;
    }

    // Get the title ID
    std::string titleID = std::string(pkg.GetTitleID());
    
    // Create a properly structured output path
    // If the outDir itself ends with the title ID, we're good
    // Otherwise, we need to create a subdir with the title ID
    std::filesystem::path actualOutDir;
    if (outDir.filename().string() == titleID) {
        actualOutDir = outDir;
    } else {
        actualOutDir = outDir / titleID;
        if (!std::filesystem::exists(actualOutDir)) {
            std::filesystem::create_directories(actualOutDir);
        }
    }

    std::cout << "Extracting PKG: " << pkgPath.filename() << "\n";
    std::cout << "Output directory: " << actualOutDir << "\n";
    std::cout << "Title ID: " << titleID << std::endl;
    std::cout << "PKG Size: " << pkg.GetPkgSize() << " bytes" << std::endl;
    std::cout << "Content Flags: " << pkg.GetPkgFlags() << std::endl;
    
    // Extract metadata and headers
    std::cout << "\nExtracting PKG header and metadata..." << std::endl;
    if (!pkg.Extract(pkgPath, actualOutDir, failReason)) {
        std::cerr << "Extraction failed: " << failReason << "\n";
        return 3;
    }
    
    // Get count of files to extract
    u32 numFiles = pkg.GetNumberOfFiles();
    std::cout << "Found " << numFiles << " files to extract..." << std::endl;
    
    // Create an array of indices to extract
    std::vector<int> indices;
    for (u32 i = 0; i < numFiles; i++) {
        indices.push_back(static_cast<int>(i));
    }
    
    // Extract each file individually, using a new PKG instance for each
    // This mimics the approach used in the GUI version with QtConcurrent::map
    int extractedCount = 0;
    int failedCount = 0;
    
    // Process each file with independent PKG instances
    for (int index : indices) {
        std::cout << "Extracting file " << (index + 1) << " of " << numFiles << "..." << std::endl;
        
        // Use the same PKG instance but process one file at a time with proper error handling
        if (ExtractSingleFile(pkg, index)) {
            extractedCount++;
        } else {
            failedCount++;
        }
        
        // Show progress every 10 files
        if ((index + 1) % 10 == 0 || (index + 1) == numFiles) {
            std::cout << "Progress: " << (index + 1) << " / " << numFiles << " files" << std::endl;
        }
    }

    std::cout << "\nExtraction complete: " << extractedCount << " files extracted, " 
              << failedCount << " files failed." << std::endl;
    std::cout << "Files extracted to: " << actualOutDir << "\n";
    return 0;
}
