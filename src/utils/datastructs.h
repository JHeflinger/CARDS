#ifndef DATASTRUCTS
#define DATASTRUCTS

#include <stdint.h>

#define ENABLE_ARRLIST(T) \
typedef struct {          \
	uint16_t size;        \
} ARRLIST_##T;

#endif
