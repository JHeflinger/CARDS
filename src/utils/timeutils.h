#ifndef TIMEUTILS
#define TIMEUTILS

#include <stdint.h>

#ifdef _WIN32

#include <Windows.h>

#elif __linux__

#else
#error "Unknown operating system!"
#endif

uint32_t GetUnpreciseEpoch();

#endif