/*
 * CSCI 2122 Assignment 5
 * Weijun Huang B00934771
 * Code Description: Implement a simple cache simulator
 */

#include "cache.h"
#include <stdlib.h>
#include <string.h>

unsigned int num_lines_per_set;

// Define structs for cache, set, and line
struct Line {
    unsigned long tag;
    unsigned char *data;
};

struct Set {
    struct Line *lines;
};

struct Cache {
    struct Set *sets;
};

// Function to initialize the cache
static void init_cache() {
    // Calculate number of sets and lines per set based on cache size
    unsigned int num_sets = c_info.F_size / sizeof(struct Set);
    num_lines_per_set = sizeof(struct Set) / sizeof(struct Line);

    // Allocate memory for cache sets
    c_info.F_memory = malloc(num_sets * sizeof(struct Set));
    if (c_info.F_memory == NULL) {
        // Error handling
        exit(EXIT_FAILURE);
    }

    // Initialize cache sets and lines
    struct Cache *cache = (struct Cache *)c_info.F_memory;
    for (unsigned int i = 0; i < num_sets; ++i) {
        for (unsigned int j = 0; j < num_lines_per_set; ++j) {
            cache->sets[i].lines[j].tag = 0;
            cache->sets[i].lines[j].data = malloc(32);
            if (cache->sets[i].lines[j].data == NULL) {
                // Error handling
                exit(EXIT_FAILURE);
            }
            memset(cache->sets[i].lines[j].data, 0, 32);
        }
    }
}

// Function to get data from cache
int cache_get(unsigned long address, unsigned long *value) {
    // Check if cache is initialized, if not, initialize it
    if (c_info.F_memory == NULL) {
        init_cache();
    }

    // Process the request
    // Break up the address into tag, index, and offset
    unsigned long tag = address >> 12; // Assuming 12 bits for tag (total 32 bits)
    unsigned int index = (address >> 5) & 0x7FF; // Assuming 11 bits for index
    unsigned int offset = address & 0x1F; // Assuming 5 bits for offset

    // Access the cache set corresponding to the index
    struct Cache *cache = (struct Cache *)c_info.F_memory;
    struct Set *set = &(cache->sets[index]);

    // Check if any line in the set has the same tag
    for (unsigned int i = 0; i < num_lines_per_set; ++i) { // 使用num_lines_per_set
        if (set->lines[i].tag == tag) {
            // Cache hit
            memcpy(value, set->lines[i].data + offset, sizeof(unsigned long)); // Copy data to value
            return 1;
        }
    }

    // Cache miss
    // Select victim line (random or LRU) and update its tag
    unsigned int victim_index = rand() % num_lines_per_set;
    set->lines[victim_index].tag = tag;

    // Load data from main memory into the selected line
    memget((unsigned int)(address & ~0x1F), set->lines[victim_index].data, 32);

    return 0; // Return 0 on failure
}
