#include <cu/cu.h>
#include "plan/problem.h"

static void pVar(const plan_var_t *var, int var_size, FILE *fout)
{
    int i, j;

    fprintf(fout, "Vars[%d]:\n", var_size);
    for (i = 0; i < var_size; ++i){
        fprintf(fout, "[%d] name: `%s', range: %d, is_private: %d",
                i, var[i].name, var[i].range, var[i].is_private);
        fprintf(fout, ", is_val_private:");
        for (j = 0; var[i].is_val_private && j < var[i].range; ++j)
            fprintf(fout, " %d", var[i].is_val_private[j]);
        fprintf(fout, ", val_name:");
        for (j = 0; var[i].val_name && j < var[i].range; ++j)
            fprintf(fout, " `%s'", var[i].val_name[j]);
        fprintf(fout, ", ma_privacy: %d", var[i].ma_privacy);
        fprintf(fout, "\n");
    }
}

static void pInitState(const plan_state_pool_t *state_pool, plan_state_id_t sid, FILE *fout)
{
    plan_state_t *state;
    int i, size;

    state = planStateNew(state_pool->num_vars);
    planStatePoolGetState(state_pool, sid, state);
    size = planStateSize(state);
    fprintf(fout, "Initial state:");
    for (i = 0; i < size; ++i){
        fprintf(fout, " %d:%d", i, (int)planStateGet(state, i));
    }
    fprintf(fout, "\n");
    planStateDel(state);
}

static void pPartState(const plan_part_state_t *p, FILE *fout)
{
    plan_var_id_t var;
    plan_val_t val;
    int i;

    PLAN_PART_STATE_FOR_EACH(p, i, var, val){
        fprintf(fout, " %d:%d", (int)var, (int)val);
    }
}

static void pGoal(const plan_part_state_t *p, FILE *fout)
{
    fprintf(fout, "Goal:");
    pPartState(p, fout);
    fprintf(fout, "\n");
}

static void pOp(const plan_op_t *op, int op_size, FILE *fout)
{
    int i, j;

    fprintf(fout, "Ops[%d]:\n", op_size);
    for (i = 0; i < op_size; ++i){
        fprintf(fout, "[%d] cost: %d, gid: %d, owner: %d (%lx),"
               " private: %d, name: `%s'\n",
               i, (int)op[i].cost, op[i].global_id,
               op[i].owner, (unsigned long)op[i].ownerarr,
               op[i].is_private,
               op[i].name);
        fprintf(fout, "[%d] pre:", i);
        pPartState(op[i].pre, fout);
        fprintf(fout, ", eff:");
        pPartState(op[i].eff, fout);
        fprintf(fout, "\n");

        for (j = 0; j < op[i].cond_eff_size; ++j){
            fprintf(fout, "[%d] cond_eff[%d]:", i, j);
            fprintf(fout, " pre:");
            pPartState(op[i].cond_eff[j].pre, fout);
            fprintf(fout, ", eff:");
            pPartState(op[i].cond_eff[j].eff, fout);
            fprintf(fout, "\n");
        }
    }
}

static void pPrivateVal(const plan_problem_private_val_t *pv, int pvsize, FILE *fout)
{
    int i;

    fprintf(fout, "PrivateVal:");
    for (i = 0; i < pvsize; ++i){
        fprintf(fout, " %d:%d", pv[i].var, pv[i].val);
    }
    fprintf(fout, "\n");
}

static void pProblem(const plan_problem_t *p, FILE *fout)
{
    pVar(p->var, p->var_size, fout);
    fprintf(fout, "ma_privacy_var: %d\n", p->ma_privacy_var);
    pInitState(p->state_pool, p->initial_state, fout);
    pGoal(p->goal, fout);
    pOp(p->op, p->op_size, fout);
    fprintf(fout, "Succ Gen: %d\n", (int)(p->succ_gen != NULL));
}

TEST(testLoadFromProto)
{
    plan_problem_t *p;
    int flags;

    flags = PLAN_PROBLEM_USE_CG | PLAN_PROBLEM_PRUNE_DUPLICATES;
    p = planProblemFromProto("proto/rovers-p03.proto", flags);
    printf("---- testLoadFromProto ----\n");
    pProblem(p, stdout);
    printf("---- testLoadFromProto END ----\n");
    planProblemDel(p);

    flags = PLAN_PROBLEM_USE_CG;
    p = planProblemFromProto("proto/openstacks-p03.proto", flags);
    printf("---- testLoadFromProto ----\n");
    pProblem(p, stdout);
    printf("---- testLoadFromProto END ----\n");
    planProblemDel(p);

    flags = PLAN_PROBLEM_USE_CG | PLAN_PROBLEM_OP_UNIT_COST;
    p = planProblemFromProto("proto/openstacks-p03.proto", flags);
    printf("---- testLoadFromProto unit-cost----\n");
    pProblem(p, stdout);
    printf("---- testLoadFromProto unit-cost END ----\n");
    planProblemDel(p);
}

TEST(testLoadFromProtoCondEff)
{
    plan_problem_t *p;

    p = planProblemFromProto("proto/CityCar-p3-2-2-0-1.proto",
                             PLAN_PROBLEM_USE_CG);
    printf("---- testLoadFromProtoCondEff ----\n");
    pProblem(p, stdout);
    printf("---- testLoadFromProtoCondEff END ----\n");
    planProblemDel(p);
}

static void pAgent(const plan_problem_t *p, FILE *fout)
{
    plan_op_t *private_op;
    int private_op_size;
    planProblemCreatePrivateProjOps(p->op, p->op_size, p->var, p->var_size,
                                    &private_op, &private_op_size);

    fprintf(fout, "++++ %s ++++\n", p->agent_name);
    fprintf(fout, "Agent ID: %d\n", p->agent_id);
    fprintf(fout, "Num agents: %d\n", p->num_agents);
    pVar(p->var, p->var_size, fout);
    fprintf(fout, "ma_privacy_var: %d\n", p->ma_privacy_var);
    pPrivateVal(p->private_val, p->private_val_size, fout);
    pInitState(p->state_pool, p->initial_state, fout);
    pGoal(p->goal, fout);
    pOp(p->op, p->op_size, fout);
    fprintf(fout, "Succ Gen: %d\n", (int)(p->succ_gen != NULL));
    fprintf(fout, "Proj op:\n");
    pOp(p->proj_op, p->proj_op_size, fout);
    fprintf(fout, "Private Proj op:\n");
    pOp(private_op, private_op_size, fout);
    fprintf(fout, "++++ %s END ++++\n", p->agent_name);

    planProblemDestroyOps(private_op, private_op_size);
}

static void testAgentProto(const char *proto, FILE *fout, int flags)
{
    plan_problem_agents_t *agents;
    int i;

    agents = planProblemAgentsFromProto(proto, flags);
    assertNotEquals(agents, NULL);
    if (agents == NULL)
        return;

    fprintf(fout, "---- %s ----\n", proto);
    pProblem(&agents->glob, fout);
    for (i = 0; i < agents->agent_size; ++i)
        pAgent(agents->agent + i, fout);

    fprintf(fout, "---- %s END ----\n", proto);
    planProblemAgentsDel(agents);
}

TEST(testLoadAgentFromProto)
{
    int flags;

    flags = PLAN_PROBLEM_USE_CG | PLAN_PROBLEM_PRUNE_DUPLICATES;
    testAgentProto("proto/rovers-p03.proto", stdout, flags);
    testAgentProto("proto/depot-pfile1.proto", stdout, flags);
    flags = PLAN_PROBLEM_USE_CG;
    testAgentProto("proto/openstacks-p03.proto", stdout, flags);
    flags = PLAN_PROBLEM_USE_CG | PLAN_PROBLEM_OP_UNIT_COST;
    testAgentProto("proto/openstacks-p03.proto", stdout, flags);
}

static void cloneFromProto(const char *proto, int flags, FILE *f1, FILE *f2)
{
    plan_problem_t *p1, *p2;

    p1 = planProblemFromProto(proto, flags);
    p2 = planProblemClone(p1);

    assertNotEquals(p1->var, p2->var);
    assertNotEquals(p1->op, p2->op);
    assertNotEquals(p1->goal, p2->goal);
    assertNotEquals(p1->state_pool, p2->state_pool);
    assertNotEquals(p1->succ_gen, p2->succ_gen);

    assertEquals(p1->state_pool->num_states, p2->state_pool->num_states);
    assertEquals(p1->state_pool->num_states, 1);

    fprintf(f1, "---- %s %x ----\n", proto, flags);
    pProblem(p1, f1);
    fprintf(f1, "---- %s %x END ----\n", proto, flags);

    fprintf(f2, "---- %s %x ----\n", proto, flags);
    pProblem(p2, f2);
    fprintf(f2, "---- %s %x END ----\n", proto, flags);

    planProblemDel(p1);
    planProblemDel(p2);
}

TEST(testLoadFromProtoClone)
{
    int flags;
    FILE *f1, *f2;

    f1 = fopen("regressions/temp.load-from-file-cmp-from-proto.out", "w");
    f2 = fopen("regressions/tmp.temp.load-from-file-cmp-from-proto.out", "w");
    if (f1 == NULL || f2 == NULL){
        fprintf(stderr, "Could not open files for comparison!!\n");
        return;
    }

    flags = PLAN_PROBLEM_USE_CG | PLAN_PROBLEM_PRUNE_DUPLICATES;
    cloneFromProto("proto/rovers-p03.proto", flags, f1, f2);
    cloneFromProto("proto/depot-pfile5.proto", flags, f1, f2);
    cloneFromProto("proto/CityCar-p3-2-2-0-1.proto", flags, f1, f2);

    flags = PLAN_PROBLEM_USE_CG;
    cloneFromProto("proto/rovers-p03.proto", flags, f1, f2);
    cloneFromProto("proto/depot-pfile5.proto", flags, f1, f2);
    cloneFromProto("proto/CityCar-p3-2-2-0-1.proto", flags, f1, f2);

    flags = 0;
    cloneFromProto("proto/rovers-p03.proto", flags, f1, f2);
    cloneFromProto("proto/depot-pfile5.proto", flags, f1, f2);
    cloneFromProto("proto/CityCar-p3-2-2-0-1.proto", flags, f1, f2);

    fclose(f1);
    fclose(f2);
}


static void cloneAgentFromProto(const char *proto, int flags, FILE *f1, FILE *f2)
{
    plan_problem_agents_t *p1, *p2;
    int i;

    p1 = planProblemAgentsFromProto(proto, flags);
    p2 = planProblemAgentsClone(p1);

    assertNotEquals(p1->glob.var, p2->glob.var);
    assertNotEquals(p1->glob.op, p2->glob.op);
    assertNotEquals(p1->glob.goal, p2->glob.goal);
    assertNotEquals(p1->glob.state_pool, p2->glob.state_pool);
    assertNotEquals(p1->glob.succ_gen, p2->glob.succ_gen);

    assertEquals(p1->glob.state_pool->num_states, p2->glob.state_pool->num_states);
    assertEquals(p1->glob.state_pool->num_states, 1);

    fprintf(f1, "---- %s %x ----\n", proto, flags);
    pProblem(&p1->glob, f1);
    for (i = 0; i < p1->agent_size; ++i)
        pAgent(p1->agent + i, f1);
    fprintf(f1, "---- %s %x END ----\n", proto, flags);

    fprintf(f2, "---- %s %x ----\n", proto, flags);
    pProblem(&p2->glob, f2);
    for (i = 0; i < p2->agent_size; ++i)
        pAgent(p2->agent + i, f2);
    fprintf(f2, "---- %s %x END ----\n", proto, flags);

    planProblemAgentsDel(p1);
    planProblemAgentsDel(p2);
}

TEST(testLoadAgentFromProtoClone)
{
    int flags;
    FILE *f1, *f2;

    f1 = fopen("regressions/temp.load-from-file-agent-cmp-from-proto.out", "w");
    f2 = fopen("regressions/tmp.temp.load-from-file-agent-cmp-from-proto.out", "w");
    if (f1 == NULL || f2 == NULL){
        fprintf(stderr, "Could not open files for comparison!!\n");
        return;
    }

    flags = PLAN_PROBLEM_USE_CG | PLAN_PROBLEM_PRUNE_DUPLICATES;
    cloneAgentFromProto("proto/rovers-p03.proto", flags, f1, f2);
    cloneAgentFromProto("proto/depot-pfile5.proto", flags, f1, f2);

    flags = PLAN_PROBLEM_USE_CG;
    cloneAgentFromProto("proto/rovers-p03.proto", flags, f1, f2);
    cloneAgentFromProto("proto/depot-pfile5.proto", flags, f1, f2);

    flags = 0;
    cloneAgentFromProto("proto/rovers-p03.proto", flags, f1, f2);
    cloneAgentFromProto("proto/depot-pfile5.proto", flags, f1, f2);

    fclose(f1);
    fclose(f2);
}

static void loadFactoredProto(const char *fn, int flags)
{
    plan_problem_t *p;
    p = planProblemFromProto(fn, flags);
    printf("---- testLoadFromFactoredProto: %s ----\n", fn);
    pAgent(p, stdout);
    printf("---- testLoadFromFactoredProto: %s END ----\n", fn);
    planProblemDel(p);
}

TEST(testLoadFromFactoredProto)
{
    int flags;

    flags = 0;
    loadFactoredProto("proto/driverlog-pfile1-driver1.proto", flags);
    loadFactoredProto("proto/driverlog-pfile1-driver2.proto", flags);
    loadFactoredProto("proto/rovers-p03-rover0.proto", flags);
    loadFactoredProto("proto/rovers-p03-rover1.proto", flags);

    flags  = PLAN_PROBLEM_MA_STATE_PRIVACY;
    flags |= PLAN_PROBLEM_NUM_AGENTS(2);
    loadFactoredProto("proto/driverlog-pfile1-driver1.proto", flags);
    loadFactoredProto("proto/driverlog-pfile1-driver2.proto", flags);

    flags  = PLAN_PROBLEM_MA_STATE_PRIVACY;
    flags |= PLAN_PROBLEM_NUM_AGENTS(15);
    loadFactoredProto("proto/rovers-p03-rover0.proto", flags);
    loadFactoredProto("proto/rovers-p03-rover1.proto", flags);
}
