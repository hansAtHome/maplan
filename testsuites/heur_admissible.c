#include <cu/cu.h>
#include "plan/search.h"

#include "state_pool.h"

typedef plan_heur_t *(*new_heur_fn)(const plan_problem_t *prob);

static plan_heur_t *heurLMCut(const plan_problem_t *p)
{
    return planHeurLMCutNew(p->var, p->var_size, p->goal, p->op, p->op_size);
}

static plan_heur_t *heurMax(const plan_problem_t *p)
{
    return planHeurRelaxMaxNew(p->var, p->var_size, p->goal, p->op, p->op_size);
}

static void checkOptimalCost(new_heur_fn new_heur, const char *proto)
{
    plan_search_astar_params_t params;
    plan_search_t *search;
    plan_path_t path;
    plan_problem_t *p;
    plan_path_op_t *op;
    plan_cost_t cost;
    plan_state_space_node_t *node;

    planSearchAStarParamsInit(&params);
    p = planProblemFromProto(proto, PLAN_PROBLEM_USE_CG);
    params.search.prob = p;
    params.search.heur = new_heur(p);
    params.search.heur_del = 1;
    search = planSearchAStarNew(&params);

    planPathInit(&path);
    assertEquals(planSearchRun(search, &path), PLAN_SEARCH_FOUND);

    cost = planPathCost(&path);
    BOR_LIST_FOR_EACH_ENTRY(&path, plan_path_op_t, op, path){
        node = planStateSpaceNode(search->state_space, op->from_state);
        assertTrue(node->heuristic <= cost);
        node = planStateSpaceNode(search->state_space, op->to_state);
        assertTrue(node->heuristic - op->cost <= cost);
        cost -= op->cost;
    }

    planPathFree(&path);
    planSearchDel(search);
    planProblemDel(p);
}

static void checkOptimalCost2(new_heur_fn new_heur, const char *proto,
                              const char *states, const char *costs)
{
    plan_problem_t *p;
    plan_heur_t *heur;
    plan_heur_res_t res;
    state_pool_t state_pool;
    plan_state_t *state;
    FILE *fcosts;
    int cost, si;

    fcosts = fopen(costs, "r");

    p = planProblemFromProto(proto, PLAN_PROBLEM_USE_CG);
    state = planStateNew(p->state_pool);
    statePoolInit(&state_pool, states);
    heur = new_heur(p);

    for (si = 0; statePoolNext(&state_pool, state) == 0; ++si){
        planHeurResInit(&res);
        planHeur(heur, state, &res);
        fscanf(fcosts, "%d", &cost);
        assertTrue((int)res.heur <= cost);
    }

    planHeurDel(heur);
    statePoolFree(&state_pool);
    planStateDel(state);
    planProblemDel(p);
    fclose(fcosts);
}

TEST(testHeurAdmissibleLMCut)
{
    checkOptimalCost(heurLMCut, "../data/ma-benchmarks/depot/pfile1.proto");
    checkOptimalCost(heurLMCut, "../data/ma-benchmarks/depot/pfile2.proto");
    checkOptimalCost(heurLMCut, "../data/ma-benchmarks/rovers/p01.proto");
    checkOptimalCost(heurLMCut, "../data/ma-benchmarks/rovers/p02.proto");
    checkOptimalCost(heurLMCut, "../data/ma-benchmarks/rovers/p03.proto");

    checkOptimalCost2(heurLMCut,
                      "../data/ma-benchmarks/depot/pfile1.proto",
                      "states/depot-pfile1.txt",
                      "states/depot-pfile1.cost.txt");
    checkOptimalCost2(heurLMCut,
                      "../data/ma-benchmarks/driverlog/pfile1.proto",
                      "states/driverlog-pfile1.txt",
                      "states/driverlog-pfile1.cost.txt");
    checkOptimalCost2(heurLMCut,
                      "../data/ma-benchmarks/rovers/p03.proto",
                      "states/rovers-p03.txt",
                      "states/rovers-p03.cost.txt");
}

TEST(testHeurAdmissibleMax)
{
    checkOptimalCost(heurMax, "../data/ma-benchmarks/depot/pfile1.proto");
    checkOptimalCost(heurMax, "../data/ma-benchmarks/depot/pfile2.proto");
    checkOptimalCost(heurMax, "../data/ma-benchmarks/rovers/p01.proto");
    checkOptimalCost(heurMax, "../data/ma-benchmarks/rovers/p02.proto");
    checkOptimalCost(heurMax, "../data/ma-benchmarks/rovers/p03.proto");

    checkOptimalCost2(heurMax,
                      "../data/ma-benchmarks/depot/pfile1.proto",
                      "states/depot-pfile1.txt",
                      "states/depot-pfile1.cost.txt");
    checkOptimalCost2(heurMax,
                      "../data/ma-benchmarks/driverlog/pfile1.proto",
                      "states/driverlog-pfile1.txt",
                      "states/driverlog-pfile1.cost.txt");
    checkOptimalCost2(heurMax,
                      "../data/ma-benchmarks/rovers/p03.proto",
                      "states/rovers-p03.txt",
                      "states/rovers-p03.cost.txt");
}
