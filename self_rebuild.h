#ifndef SELF_REBUILD

#include <chrono>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <filesystem>
#include <fstream>
#include <chrono>

const std::string TIMESTAMP_FILE = "TIMESTAMP";

inline long long get_file_mod_time(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        std::cerr << "Error: File does not exist: " << filepath << std::endl;
        return -1;
    }

    std::filesystem::file_time_type mod_time = std::filesystem::last_write_time(filepath);
    auto duration = mod_time.time_since_epoch();
    long long nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    
    return nanoseconds;
}

inline long long read_stored_time(const std::string& filepath) {
    std::ifstream inFile(filepath);
    if (!inFile) {
        return 0;
    }

    long long storedTime = 0;
    inFile >> storedTime;

    if (inFile.fail() && !inFile.eof()) {
         std::cerr << "Warning: Could not read timestamp file. Treating as first run." << std::endl;
         inFile.close();
         return 0;
    }

    inFile.close();
    return storedTime;
}

inline void write_stored_time(const std::string& filepath, long long modTime) {
    std::ofstream outFile(filepath);
    if (!outFile) {
        std::cerr << "Error: Could not open timestamp file for writing: " << filepath << std::endl;
        return;
    }
    outFile << modTime;
    outFile.close();
}

inline void _internal_ensure_target_file_exists(const std::string& target_file) {
    if (!std::filesystem::exists(target_file)) {
        std::cout << "Target file '" << target_file << "' not found. Creating it for testing..." << std::endl;
        std::ofstream newFile(target_file);
        newFile << "/* This is a sample file to monitor. */" << std::endl;
        newFile.close();
    }
}

inline bool _internal_is_modified(const std::string& target_file) {
    _internal_ensure_target_file_exists(target_file);

    long long current_time = get_file_mod_time(target_file);
    if (current_time == -1) {
        std::cerr << "Error: Could not get modification time for " << target_file << "." << std::endl;
        return false; 
    }

    long long stored_time = read_stored_time(TIMESTAMP_FILE);

    if (stored_time == 0) {
        std::cout << "[STATUS]: This is the first run." << std::endl;
        std::cout << "Storing initial timestamp..." << std::endl;
        write_stored_time(TIMESTAMP_FILE, current_time);
        return true;
    } else if (current_time != stored_time) {
        std::cout << "[STATUS]: " << target_file << " has changed." << std::endl;
        write_stored_time(TIMESTAMP_FILE, current_time);
        return true;
    } else {
        std::cout << "[STATUS] nothing changed" << std::endl;
        std::cout << "=====================================\n\n\n";
        return false;
    }
}

inline void _internal_self_rebuild(const std::string& target_file, std::string compiler, std::vector<std::string> args) {
    if (_internal_is_modified(target_file)) {
        std::string compile_cmd = compiler;
        for (size_t i = 0; i < args.size(); i++) {
            compile_cmd += args[i];
            compile_cmd += " ";
        }

        std::cout << "[BUILD CMD]: " << compile_cmd << std::endl;

        for (size_t i = 0; i < args.size(); i++) {
            std::cout << "[FLAG]: " << "[" << args[i] << "]" << std::endl;
        }

        std::system(compile_cmd.c_str());
        std::cout << "===========================================\n\n";
    }
}

#define IS_MODIFIED() _internal_is_modified(__FILE__)
#define SELF_REBUILD(compiler, args) _internal_self_rebuild(__FILE__, compiler, args)

#endif
