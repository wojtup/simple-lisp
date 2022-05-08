#ifndef GC_HPP
#define GC_HPP

#include <stddef.h>

/* Memory allocation functions - they measure how much memory is used */
void* safe_malloc(size_t size);
void safe_free(void* addr, size_t size);
int get_mem_usage();

/* Reclaim all garbage we find */
void reclaim();

#endif
