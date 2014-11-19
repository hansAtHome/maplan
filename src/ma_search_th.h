#ifndef __PLAN_MA_SEARCH_TH_H__
#define __PLAN_MA_SEARCH_TH_H__

#include <pthread.h>
#include <boruvka/fifo-sem.h>

#include <plan/search.h>
#include "ma_snapshot.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct _plan_ma_search_th_t {
    pthread_t thread;
    plan_search_t *search;
    plan_ma_comm_t *comm;
    plan_path_t *path;
    int solution_verify;

    int pub_state_reg;
    bor_fifo_sem_t msg_queue;
    plan_ma_snapshot_reg_t snapshot;

    int res; /*!< Result of search */
    plan_state_id_t goal;
    plan_cost_t goal_cost;
    plan_cost_t last_cost;
};
typedef struct _plan_ma_search_th_t plan_ma_search_th_t;

/**
 * Initializes search thread.
 */
void planMASearchThInit(plan_ma_search_th_t *th,
                        plan_search_t *search,
                        plan_ma_comm_t *comm,
                        plan_path_t *path);

/**
 * Frees allocated resources.
 */
void planMASearchThFree(plan_ma_search_th_t *th);

/**
 * Starts thread with search.
 */
void planMASearchThRun(plan_ma_search_th_t *th);

/**
 * Blocks until search thread ends.
 */
void planMASearchThJoin(plan_ma_search_th_t *th);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PLAN_MA_SEARCH_TH_H__ */
