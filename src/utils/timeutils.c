#include "timeutils.h"

#ifdef _WIN32

uint32_t GetUnpreciseEpoch() {
    FILETIME ft;
    ULONGLONG epoch_time_ms;
    GetSystemTimePreciseAsFileTime(&ft);
    epoch_time_ms = ((ULONGLONG)ft.dwHighDateTime << 32 | ft.dwLowDateTime) / 10000ULL;
    return epoch_time_ms - 0xffffffff;
}

#elif __linux__

uint32_t GetUnpreciseEpoch() {
    
}

#else
#error "Unknown operating system!"
#endif

