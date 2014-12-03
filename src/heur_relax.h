#ifndef __PLAN_HEUR_RELAX_H__
#define __PLAN_HEUR_RELAX_H__

#include "fact_op_cross_ref.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define PLAN_HEUR_RELAX_TYPE_ADD 0
#define PLAN_HEUR_RELAX_TYPE_MAX 1

struct _plan_heur_relax_op_t {
    int unsat;         /*!< Number of unsatisfied preconditions */
    plan_cost_t value; /*!< Value assigned to the operator */
    plan_cost_t cost;  /*!< Cost of the operator */
};
typedef struct _plan_heur_relax_op_t plan_heur_relax_op_t;

struct _plan_heur_relax_fact_t {
    plan_cost_t value; /*!< Value assigned to the fact */
    int reached_by_op; /*!< ID of the operator that reached this fact */
};
typedef struct _plan_heur_relax_fact_t plan_heur_relax_fact_t;

struct _plan_heur_relax_t {
    plan_fact_op_cross_ref_t cref; /*!< Cross referenced ops and facts */
    plan_heur_relax_op_t *op;
    plan_heur_relax_op_t *op_init; /*!< Pre-initialization of .op[] array */
    plan_heur_relax_fact_t *fact;
    plan_heur_relax_fact_t *fact_init; /*!< Pre-init of .fact[] array */
};
typedef struct _plan_heur_relax_t plan_heur_relax_t;

/**
 * Initialize relaxation heuristic.
 */
void planHeurRelaxInit(plan_heur_relax_t *relax,
                       const plan_var_t *var, int var_size,
                       const plan_part_state_t *goal,
                       const plan_op_t *op, int op_size);

/**
 * Frees allocated resources.
 */
void planHeurRelaxFree(plan_heur_relax_t *relax);

/**
 * Runs relaxation from the specified state until goal is reached.
 */
void planHeurRelaxRun(plan_heur_relax_t *relax, int type,
                      const plan_state_t *state);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PLAN_HEUR_RELAX_H__ */
