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

// Process a single PKG file
bool ProcessPkg(const std::filesystem::path& pkgPath, const std::filesystem::path& baseOutDir) {
    std::cout << "\nProcessing PKG: " << pkgPath.filename() << std::endl;
    
    // Check if PKG file exists
    if (!std::filesystem::exists(pkgPath)) {
        std::cerr << "Error: PKG file not found: " << pkgPath << "\n";
        return false;
    }
    
    std::filesystem::path outDir = baseOutDir;
    
    // Create output directory if it doesn't exist
    if (!std::filesystem::exists(outDir)) {
        try {
            std::filesystem::create_directories(outDir);
        } catch (const std::exception& e) {
            std::cerr << "Error creating output directory: " << e.what() << "\n";
            return false;
        }
    }
    
    // Open the PKG to read metadata
    PKG pkg;
    std::string failReason;
    
    if (!pkg.Open(pkgPath, failReason)) {
        std::cerr << "Failed to open PKG file: " << failReason << "\n";
        return false;
    }

    // Get the title ID
    std::string titleID = std::string(pkg.GetTitleID());
    
    // Create a properly structured output path
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
    std::cout << "Extracting PKG header and metadata..." << std::endl;
    if (!pkg.Extract(pkgPath, actualOutDir, failReason)) {
        std::cerr << "Extraction failed: " << failReason << "\n";
        return false;
    }
    
    // Get count of files to extract
    u32 numFiles = pkg.GetNumberOfFiles();
    std::cout << "Found " << numFiles << " files to extract..." << std::endl;
    
    // Create an array of indices to extract
    std::vector<int> indices;
    for (u32 i = 0; i < numFiles; i++) {
        indices.push_back(static_cast<int>(i));
    }
    
    // Extract each file individually
    int extractedCount = 0;
    int failedCount = 0;
    
    for (int index : indices) {
        std::cout << "Extracting file " << (index + 1) << " of " << numFiles << "..." << std::endl;
        
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

    std::cout << "Extraction complete: " << extractedCount << " files extracted, " 
              << failedCount << " files failed." << std::endl;
    std::cout << "Files extracted to: " << actualOutDir << "\n";
    
    return failedCount == 0; // Return true if all files were extracted successfully
}

int main(int argc, char** argv) {
    // Check for directory mode flag
    if (argc >= 3 && std::string(argv[1]) == "--dir") {
        std::filesystem::path sourceDir = argv[2];
        std::filesystem::path outputBaseDir;
        
        // Use the source directory as the default output if no output directory is specified
        if (argc == 3) {
            outputBaseDir = sourceDir;
            std::cout << "No output directory specified. Using source directory: " << outputBaseDir << std::endl;
        } else {
            outputBaseDir = argv[3];
        }
        
        if (!std::filesystem::exists(sourceDir) || !std::filesystem::is_directory(sourceDir)) {
            std::cerr << "Error: Source directory not found or not a directory: " << sourceDir << "\n";
            return 1;
        }
        
        // Find all PKG files in the directory
        std::vector<std::filesystem::path> pkgFiles;
        std::cout << "Searching for PKG files in: " << sourceDir << " (this may take a moment)...\n";
        
        for (const auto& entry : std::filesystem::recursive_directory_iterator(sourceDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".pkg") {
                pkgFiles.push_back(entry.path());
            }
        }
        
        if (pkgFiles.empty()) {
            std::cout << "No PKG files found in the specified directory.\n";
            return 0;
        }
        
        std::cout << "Found " << pkgFiles.size() << " PKG files to process.\n";
        
        // Process each PKG file
        int successCount = 0;
        int failedCount = 0;
        
        for (size_t i = 0; i < pkgFiles.size(); i++) {
            const auto& pkgPath = pkgFiles[i];
            std::cout << "\n[" << (i + 1) << "/" << pkgFiles.size() << "] Processing " 
                      << pkgPath.filename() << "...\n";
            
            if (ProcessPkg(pkgPath, outputBaseDir)) {
                std::cout << "Successfully processed " << pkgPath.filename() << "\n";
                successCount++;
            } else {
                std::cout << "Failed to process " << pkgPath.filename() << "\n";
                failedCount++;
            }
        }
        
        std::cout << "\nBatch processing complete: " << successCount << " successful, " 
                  << failedCount << " failed.\n";
        
        return failedCount > 0 ? 1 : 0;
    } 
    // Standard single file processing mode
    else if (argc >= 2 && argc <= 3) {
        std::filesystem::path pkgPath = argv[1];
        std::filesystem::path outDir;
        
        // Use the PKG's parent directory as the default output if no output directory is specified
        if (argc == 2) {
            outDir = pkgPath.parent_path();
            std::cout << "No output directory specified. Using PKG parent directory: " << outDir << std::endl;
        } else {
            outDir = argv[2];
        }
        
        return ProcessPkg(pkgPath, outDir) ? 0 : 1;
    }
    // Invalid arguments
    else {
        std::cerr << "Usage: ps4-pkg-tool <path/to/pkg> [path/to/output]\n";
        std::cerr << "   OR: ps4-pkg-tool --dir <directory/with/pkgs> [path/to/output]\n";
        std::cerr << "       If output path is omitted, the PKG will be extracted to its parent directory\n";
        return 1;
    }
}
