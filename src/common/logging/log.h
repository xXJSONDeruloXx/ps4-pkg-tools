// Minimal stub logging header for pruned build
#pragma once
#include <cstdio>

enum CommonLogClass { Common_Filesystem };

#define LOG_ERROR(cls, fmt, ...) std::fprintf(stderr, "[ERR] " fmt "\n", ##__VA_ARGS__)
#define LOG_CRITICAL(cls, fmt, ...) std::fprintf(stderr, "[CRT] " fmt "\n", ##__VA_ARGS__)

