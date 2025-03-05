//
// This file contains all of the implementations of the replacement_policy
// constructors from the replacement_policies.h file.
//
// It also contains stubs of all of the functions that are added to each
// replacement_policy struct at construction time.
//
// ============================================================================
// NOTE: It is recommended that you read the comments in the
// replacement_policies.h file for further context on what each function is
// for.
// ============================================================================
//

#include "replacement_policies.h"

// LRU Replacement Policy
// ============================================================================
// TODO feel free to create additional structs/enums as necessary

struct lru_metadata {
    uint32_t **order;
};

void lru_cache_access(struct replacement_policy *replacement_policy,
                      struct cache_system *cache_system, uint32_t set_idx, uint32_t tag)
{
    // TODO update the LRU replacement policy state given a new memory access

    struct lru_metadata *lru = (struct lru_metadata *)replacement_policy->data;
    uint32_t *set_order = lru->order[set_idx];

    int index = -1;
    for (int i = 0; i < cache_system->associativity; i++) {
        if (cache_system->cache_lines[set_idx * cache_system->associativity + i].tag == tag) {
            index = i;
            break;
        }
    }

    if (index == -1) return;

    uint32_t accessed_line = set_order[index];
    for (int i = index; i > 0; i--) {
        set_order[i] = set_order[i - 1]; 
    }
    set_order[0] = accessed_line;

}

uint32_t lru_eviction_index(struct replacement_policy *replacement_policy,
                            struct cache_system *cache_system, uint32_t set_idx)
{
    // TODO return the index within the set that should be evicted.
    struct lru_metadata *lru = (struct lru_metadata *)replacement_policy->data;
    uint32_t *set_order = lru->order[set_idx];

    printf("LRU Set %d Order (MRU → LRU): ", set_idx);
    for (int i = 0; i < cache_system->associativity; i++) {
        printf("%d ", set_order[i]);  // Print the cache line index in order
    }
    printf("\n");

    printf("Evicted Index: %d\n", set_order[cache_system->associativity - 1]);

    return set_order[cache_system->associativity - 1];
}

void lru_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{
    // TODO cleanup any additional memory that you allocated in the
    // lru_replacement_policy_new function.

}

struct replacement_policy *lru_replacement_policy_new(uint32_t sets, uint32_t associativity)
{
    struct replacement_policy *lru_rp = calloc(1, sizeof(struct replacement_policy));
    lru_rp->cache_access = &lru_cache_access;
    lru_rp->eviction_index = &lru_eviction_index;
    lru_rp->cleanup = &lru_replacement_policy_cleanup;

    // TODO allocate any additional memory to store metadata here and assign to
    // lru_rp->data.

    // Allocate LRU metadata
    struct lru_metadata *lru = malloc(sizeof(struct lru_metadata));
    lru->order = malloc(sets * sizeof(uint32_t *));
    for (uint32_t i = 0; i < sets; i++) {
        lru->order[i] = malloc(associativity * sizeof(uint32_t));
        for (uint32_t j = 0; j < associativity; j++) {
            lru->order[i][j] = j; // Initialize LRU order
        }
    }

    lru_rp->data = lru;

    return lru_rp;
}

// RAND Replacement Policy
// ============================================================================
void rand_cache_access(struct replacement_policy *replacement_policy,
                       struct cache_system *cache_system, uint32_t set_idx, uint32_t tag)
{
    // TODO update the RAND replacement policy state given a new memory access
    // DONE
}

uint32_t rand_eviction_index(struct replacement_policy *replacement_policy,
                             struct cache_system *cache_system, uint32_t set_idx)
{
    // TODO return the index within the set that should be evicted.
    // DONE

    // compute start index of the set
    int set_start_idx = set_idx * cache_system->associativity;

    // generate a random index using rand number generator
    // range bounded by the associativity of the cache
    int rand_offset = rand() % cache_system->associativity;

    int evicted_index = rand_offset;

    // return eviction index
    return evicted_index;
}

void rand_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{
    // TODO cleanup any additional memory that you allocated in the
    // rand_replacement_policy_new function.
    // DONE

    // free(replacement_policy);
}

struct replacement_policy *rand_replacement_policy_new(uint32_t sets, uint32_t associativity)
{
    // Seed randomness
    //srand(time(NULL));

    static int seeded = 0;
    if (!seeded) {
        srand(time(NULL));
        seeded = 1;
    }

    struct replacement_policy *rand_rp = malloc(sizeof(struct replacement_policy));
    rand_rp->cache_access = &rand_cache_access;
    rand_rp->eviction_index = &rand_eviction_index;
    rand_rp->cleanup = &rand_replacement_policy_cleanup;

    // TODO allocate any additional memory to store metadata here and assign to
    // rand_rp->data.
    // DONE

    rand_rp->data = rand_rp;

    return rand_rp;
}

// LRU_PREFER_CLEAN Replacement Policy
// ============================================================================
void lru_prefer_clean_cache_access(struct replacement_policy *replacement_policy,
                                   struct cache_system *cache_system, uint32_t set_idx,
                                   uint32_t tag)
{
    // TODO update the LRU_PREFER_CLEAN replacement policy state given a new
    // memory access
    // NOTE: you may be able to share code with the LRU policy

    // should be the same as the LRU replacement policy
    lru_cache_access(replacement_policy, cache_system, set_idx, tag);
}

uint32_t lru_prefer_clean_eviction_index(struct replacement_policy *replacement_policy,
                                         struct cache_system *cache_system, uint32_t set_idx)
{
    // TODO return the index within the set that should be evicted.
    struct lru_metadata *lru = (struct lru_metadata *)replacement_policy->data;
    uint32_t *set_order = lru->order[set_idx];

    int lru_dirty_index = -1; 

    for (int i = cache_system->associativity - 1; i >= 0; i--) {
        uint32_t evict_index = set_order[i];
        struct cache_line *cl = &cache_system->cache_lines[set_idx * cache_system->associativity + evict_index];

        if (cl->status != MODIFIED) {
            return evict_index;
        }
        if (lru_dirty_index == -1) {
            lru_dirty_index = evict_index;
        }
    }

    return lru_dirty_index;
}

void lru_prefer_clean_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{
    // TODO cleanup any additional memory that you allocated in the
    // lru_prefer_clean_replacement_policy_new function.

    lru_replacement_policy_cleanup(replacement_policy);
}

struct replacement_policy *lru_prefer_clean_replacement_policy_new(uint32_t sets,
                                                                   uint32_t associativity)
{
    struct replacement_policy *lru_prefer_clean_rp = malloc(sizeof(struct replacement_policy));
    lru_prefer_clean_rp->cache_access = &lru_prefer_clean_cache_access;
    lru_prefer_clean_rp->eviction_index = &lru_prefer_clean_eviction_index;
    lru_prefer_clean_rp->cleanup = &lru_prefer_clean_replacement_policy_cleanup;

    // TODO allocate any additional memory to store metadata here and assign to
    // lru_prefer_clean_rp->data.

    lru_prefer_clean_rp->data = lru_replacement_policy_new(sets, associativity)->data;

    return lru_prefer_clean_rp;
}
