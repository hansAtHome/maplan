#include <strings.h>
#include <boruvka/alloc.h>
#include "plan/state.h"

#define INIT_ALLOC_SIZE 32


plan_state_pool_t *planStatePoolNew(size_t num_vars)
{
    plan_state_pool_t *pool;

    pool = BOR_ALLOC(plan_state_pool_t);
    pool->num_vars = num_vars;
    pool->states_allocated = INIT_ALLOC_SIZE * pool->num_vars;
    pool->states = BOR_ALLOC_ARR(unsigned, pool->states_allocated);
    pool->states_size = 0;

    return pool;
}

void planStatePoolDel(plan_state_pool_t *pool)
{
    if (pool->states)
        BOR_FREE(pool->states);
    BOR_FREE(pool);
}

plan_state_t planStatePoolNewState(plan_state_pool_t *pool)
{
    plan_state_t state;

    if (pool->states_size + pool->num_vars >= pool->states_allocated){
        // expand a pool array if not enough space for a new state
        pool->states_allocated *= 2;
        pool->states = BOR_REALLOC_ARR(pool->states, unsigned,
                                       pool->states_allocated);
    }

    state.val = pool->states + pool->states_size;
    pool->states_size += pool->num_vars;

    // initialize state
    bzero(state.val, sizeof(unsigned) * pool->num_vars);

    return state;
}




int planStateGet(const plan_state_t *state, unsigned var)
{
    return state->val[var];
}

void planStateSet(plan_state_t *state, unsigned var, unsigned val)
{
    state->val[var] = val;
}
