#include <strings.h>
#include <boruvka/hfunc.h>
#include <boruvka/alloc.h>
#include "plan/state_pool.h"


#define DATA_STATE 0
#define DATA_HTABLE 1

#define HTABLE_STATE(list) \
    BOR_LIST_ENTRY(list, plan_state_htable_t, htable)

struct _plan_state_htable_t {
    bor_list_t htable;
    plan_state_id_t state_id;
};
typedef struct _plan_state_htable_t plan_state_htable_t;

/** Returns true if the two given states are equal. */
_bor_inline int planStateEq(const plan_state_pool_t *pool,
                            plan_state_id_t s1,
                            plan_state_id_t s2);

/** Returns hash value of the specified state. */
_bor_inline bor_htable_key_t planStateHash(const plan_state_pool_t *pool,
                                           plan_state_id_t sid);
/** Inserts state into hash table and returns ID under which it is stored. */
_bor_inline plan_state_id_t insertIntoHTable(plan_state_pool_t *pool,
                                             plan_state_id_t sid);

/** Callbacks for bor_htable_t */
static bor_htable_key_t htableHash(const bor_list_t *key, void *ud);
static int htableEq(const bor_list_t *k1, const bor_list_t *k2, void *ud);

plan_state_pool_t *planStatePoolNew(const plan_var_t *var, int var_size)
{
    int state_size;
    plan_state_pool_t *pool;
    void *state_init;
    plan_state_htable_t htable_init;

    pool = BOR_ALLOC(plan_state_pool_t);
    pool->num_vars = var_size;

    pool->packer = planStatePackerNew(var, var_size);
    state_size = planStatePackerBufSize(pool->packer);

    pool->data = BOR_ALLOC_ARR(plan_data_arr_t *, 2);

    state_init = BOR_ALLOC_ARR(char, state_size);
    memset(state_init, 0, state_size);
    pool->data[0] = planDataArrNew(state_size, NULL, state_init);
    BOR_FREE(state_init);

    htable_init.state_id = PLAN_NO_STATE;
    pool->data[1] = planDataArrNew(sizeof(plan_state_htable_t),
                                   NULL, &htable_init);

    pool->data_size = 2;
    pool->htable = borHTableNew(htableHash, htableEq, (void *)pool);
    pool->num_states = 0;

    return pool;
}

void planStatePoolDel(plan_state_pool_t *pool)
{
    int i;

    if (pool->htable)
        borHTableDel(pool->htable);

    for (i = 0; i < pool->data_size; ++i){
        planDataArrDel(pool->data[i]);
    }
    BOR_FREE(pool->data);

    if (pool->packer)
        planStatePackerDel(pool->packer);

    BOR_FREE(pool);
}

plan_state_pool_t *planStatePoolClone(const plan_state_pool_t *sp)
{
    plan_state_pool_t *pool;
    int i;

    pool = BOR_ALLOC(plan_state_pool_t);
    memcpy(pool, sp, sizeof(*sp));
    pool->packer = planStatePackerClone(sp->packer);
    pool->data = BOR_ALLOC_ARR(plan_data_arr_t *, sp->data_size);
    for (i = 0; i < sp->data_size; ++i)
        pool->data[i] = planDataArrClone(sp->data[i]);

    pool->htable = borHTableNew(htableHash, htableEq, (void *)pool);
    pool->num_states = 0;
    for (i = 0; i < sp->num_states; ++i)
        insertIntoHTable(pool, i);

    return pool;
}

int planStatePoolDataReserve(plan_state_pool_t *pool,
                             size_t element_size,
                             plan_data_arr_el_init_fn init_fn,
                             const void *init_data)
{
    int data_id;

    data_id = pool->data_size;
    ++pool->data_size;
    pool->data = BOR_REALLOC_ARR(pool->data, plan_data_arr_t *,
                                 pool->data_size);
    pool->data[data_id] = planDataArrNew(element_size,
                                         init_fn, init_data);
    return data_id;
}

void *planStatePoolData(plan_state_pool_t *pool,
                        int data_id,
                        plan_state_id_t state_id)
{
    if (data_id >= pool->data_size)
        return NULL;

    return planDataArrGet(pool->data[data_id], state_id);
}

plan_state_id_t planStatePoolInsert(plan_state_pool_t *pool,
                                    const plan_state_t *state)
{
    plan_state_id_t sid;
    void *statebuf;

    // determine state ID
    sid = pool->num_states;

    // allocate a new state and initialize it with the given values
    statebuf = planDataArrGet(pool->data[DATA_STATE], sid);
    planStatePackerPack(pool->packer, state, statebuf);

    return insertIntoHTable(pool, sid);
}

plan_state_id_t planStatePoolInsertPacked(plan_state_pool_t *pool,
                                          const void *packed_state)
{
    plan_state_id_t sid;
    void *statebuf;

    // determine state ID
    sid = pool->num_states;

    // allocate a new state and initialize it with the given values
    statebuf = planDataArrGet(pool->data[DATA_STATE], sid);
    memcpy(statebuf, packed_state, planStatePackerBufSize(pool->packer));

    return insertIntoHTable(pool, sid);
}

plan_state_id_t planStatePoolFind(plan_state_pool_t *pool,
                                  const plan_state_t *state)
{
    plan_state_id_t sid;
    plan_state_htable_t *htable;
    const plan_state_htable_t *hfound;
    bor_list_t *hstate;
    void *statebuf;

    // determine state ID
    sid = pool->num_states;

    // allocate a new state and initialize it with the given values
    statebuf = planDataArrGet(pool->data[DATA_STATE], sid);
    planStatePackerPack(pool->packer, state, statebuf);

    // allocate and initialize hash table element
    htable = planDataArrGet(pool->data[DATA_HTABLE], sid);
    htable->state_id = sid;

    // insert it into hash table
    hstate = borHTableFind(pool->htable, &htable->htable);

    if (hstate == NULL){
        return PLAN_NO_STATE;
    }

    hfound = HTABLE_STATE(hstate);
    return hfound->state_id;
}

void planStatePoolGetState(const plan_state_pool_t *pool,
                           plan_state_id_t sid,
                           plan_state_t *state)
{
    void *statebuf;

    if (sid >= pool->num_states)
        return;

    statebuf = planDataArrGet(pool->data[DATA_STATE], sid);
    planStatePackerUnpack(pool->packer, statebuf, state);
}

const void *planStatePoolGetPackedState(const plan_state_pool_t *pool,
                                        plan_state_id_t sid)
{
    if (sid >= pool->num_states)
        return NULL;
    return planDataArrGet(pool->data[DATA_STATE], sid);
}


_bor_inline int isSubsetPacked(const plan_state_pool_t *pool,
                               const plan_part_state_t *part_state,
                               plan_state_id_t sid)
{
    void *statebuf;

    // get corresponding state
    statebuf = planDataArrGet(pool->data[DATA_STATE], sid);
    return planPartStateIsSubsetPackedState(part_state, statebuf);
}

_bor_inline int isSubset(const plan_state_pool_t *pool,
                         const plan_part_state_t *part_state,
                         plan_state_id_t sid)
{
    PLAN_STATE_STACK(state, pool->num_vars);

    planStatePoolGetState(pool, sid, &state);
    return planPartStateIsSubsetState(part_state, &state);
}

int planStatePoolPartStateIsSubset(const plan_state_pool_t *pool,
                                   const plan_part_state_t *part_state,
                                   plan_state_id_t sid)
{
    if (sid >= pool->num_states)
        return 0;

    if (part_state->bufsize > 0)
        return isSubsetPacked(pool, part_state, sid);
    return isSubset(pool, part_state, sid);
}

_bor_inline plan_state_id_t applyPartStatePacked(plan_state_pool_t *pool,
                                                 const plan_part_state_t *ps,
                                                 plan_state_id_t sid)
{
    void *statebuf, *newstate;
    plan_state_id_t newid;

    // get corresponding state
    statebuf = planDataArrGet(pool->data[DATA_STATE], sid);

    // remember ID of the new state (if it will be inserted)
    newid = pool->num_states;

    // get buffer of the new state
    newstate = planDataArrGet(pool->data[DATA_STATE], newid);

    // apply partial state to the buffer of the new state
    planPartStateCreatePackedState(ps, statebuf, newstate);

    return insertIntoHTable(pool, newid);
}

_bor_inline plan_state_id_t applyPartState(plan_state_pool_t *pool,
                                           const plan_part_state_t *ps,
                                           plan_state_id_t sid)
{
    PLAN_STATE_STACK(state, pool->num_vars);

    planStatePoolGetState(pool, sid, &state);
    planPartStateUpdateState(ps, &state);
    return planStatePoolInsert(pool, &state);
}

plan_state_id_t planStatePoolApplyPartState(plan_state_pool_t *pool,
                                            const plan_part_state_t *ps,
                                            plan_state_id_t sid)
{
    if (sid >= pool->num_states)
        return PLAN_NO_STATE;

    if (ps->bufsize > 0){
        return applyPartStatePacked(pool, ps, sid);
    }else{
        return applyPartState(pool, ps, sid);
    }
}


_bor_inline plan_state_id_t applyPartStatesPacked(plan_state_pool_t *pool,
                                                  const plan_part_state_t **ps,
                                                  int ps_len,
                                                  plan_state_id_t sid)
{
    void *statebuf, *newstate;
    plan_state_id_t newid;
    int i;

    // get corresponding state
    statebuf = planDataArrGet(pool->data[DATA_STATE], sid);

    // remember ID of the new state (if it will be inserted)
    newid = pool->num_states;

    // get buffer of the new state
    newstate = planDataArrGet(pool->data[DATA_STATE], newid);

    // apply partial state to the buffer of the new state
    planPartStateCreatePackedState(ps[0], statebuf, newstate);
    for (i = 1; i < ps_len; ++i){
        planPartStateUpdatePackedState(ps[i], newstate);
    }

    return insertIntoHTable(pool, newid);
}

_bor_inline plan_state_id_t applyPartStates(plan_state_pool_t *pool,
                                            const plan_part_state_t **ps,
                                            int ps_len,
                                            plan_state_id_t sid)
{
    PLAN_STATE_STACK(state, pool->num_vars);
    int i;

    planStatePoolGetState(pool, sid, &state);
    for (i = 0; i < ps_len; ++i)
        planPartStateUpdateState(ps[i], &state);
    return planStatePoolInsert(pool, &state);
}

plan_state_id_t planStatePoolApplyPartStates(plan_state_pool_t *pool,
                                             const plan_part_state_t **part_states,
                                             int part_states_len,
                                             plan_state_id_t sid)
{
    if (sid >= pool->num_states || part_states_len <= 0)
        return PLAN_NO_STATE;

    if (part_states[0]->bufsize > 0)
        return applyPartStatesPacked(pool, part_states, part_states_len, sid);
    return applyPartStates(pool, part_states, part_states_len, sid);
}



_bor_inline int planStateEq(const plan_state_pool_t *pool,
                            plan_state_id_t s1id,
                            plan_state_id_t s2id)
{
    void *s1 = planDataArrGet(pool->data[DATA_STATE], s1id);
    void *s2 = planDataArrGet(pool->data[DATA_STATE], s2id);
    size_t size = planStatePackerBufSize(pool->packer);
    return memcmp(s1, s2, size) == 0;
}

_bor_inline bor_htable_key_t planStateHash(const plan_state_pool_t *pool,
                                           plan_state_id_t sid)
{
    void *buf = planDataArrGet(pool->data[DATA_STATE], sid);
    return borCityHash_64(buf, planStatePackerBufSize(pool->packer));
}

_bor_inline plan_state_id_t insertIntoHTable(plan_state_pool_t *pool,
                                             plan_state_id_t sid)
{
    plan_state_htable_t *htable;
    bor_list_t *hstate;
    const plan_state_htable_t *hfound;

    // allocate and initialize hash table element
    htable = planDataArrGet(pool->data[DATA_HTABLE], sid);
    htable->state_id = sid;

    // insert it into hash table
    hstate = borHTableInsertUnique(pool->htable, &htable->htable);

    if (hstate == NULL){
        // NULL is returned if the element was inserted into table, so
        // increase number of elements in the pool
        ++pool->num_states;
        return sid;

    }else{
        // If the non-NULL was returned, it means that the same state was
        // already in hash table.
        hfound = HTABLE_STATE(hstate);
        return hfound->state_id;
    }
}


static bor_htable_key_t htableHash(const bor_list_t *key, void *ud)
{
    const plan_state_htable_t *s = HTABLE_STATE(key);
    plan_state_pool_t *pool = (plan_state_pool_t *)ud;

    return planStateHash(pool, s->state_id);
}

static int htableEq(const bor_list_t *k1, const bor_list_t *k2, void *ud)
{
    const plan_state_htable_t *s1 = HTABLE_STATE(k1);
    const plan_state_htable_t *s2 = HTABLE_STATE(k2);
    plan_state_pool_t *pool = (plan_state_pool_t *)ud;

    return planStateEq(pool, s1->state_id, s2->state_id);
}
