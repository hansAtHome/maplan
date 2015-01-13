#include <boruvka/alloc.h>

#include "plan/ma_msg.h"
#include "ma_msg.pb.h"

#define PROTO(msg) ((PlanMAMsg *)(msg))

static int snapshot_token_counter = 0;

static int stateId(const plan_ma_msg_t *msg)
{
    const PlanMAMsg *proto = PROTO(msg);
    return proto->state_id();
}

void planShutdownProtobuf(void)
{
    google::protobuf::ShutdownProtobufLibrary();
}

plan_ma_msg_t *planMAMsgNew(int type, int subtype, int agent_id)
{
    int proto_type;
    PlanMAMsg *protobuf;

    protobuf = new PlanMAMsg;
    proto_type = (subtype << 4) | type;
    protobuf->set_type(proto_type);
    protobuf->set_agent_id(agent_id);

    if (type == PLAN_MA_MSG_TRACE_PATH){
        protobuf->set_initiator_agent_id(agent_id);

    }else if (type == PLAN_MA_MSG_SNAPSHOT
                && subtype == PLAN_MA_MSG_SNAPSHOT_INIT){
        uint64_t token = __sync_fetch_and_add(&snapshot_token_counter, 1);
        token = token << 32;
        token = token | (uint32_t)agent_id;
        protobuf->set_snapshot_token(token);
    }

    return (plan_ma_msg_t *)protobuf;
}

void planMAMsgDel(plan_ma_msg_t *msg)
{
    PlanMAMsg *proto = PROTO(msg);
    delete proto;
}

plan_ma_msg_t *planMAMsgClone(const plan_ma_msg_t *msg_in)
{
    const PlanMAMsg *proto_in = PROTO(msg_in);
    PlanMAMsg *proto;
    plan_ma_msg_t *msg;

    msg = planMAMsgNew(planMAMsgType(msg_in), planMAMsgSubType(msg_in),
                       planMAMsgAgent(msg_in));
    proto = PROTO(msg);
    *proto = *proto_in;
    return msg;
}

int planMAMsgType(const plan_ma_msg_t *msg)
{
    const PlanMAMsg *proto = PROTO(msg);
    return proto->type() & 0xf;
}

int planMAMsgSubType(const plan_ma_msg_t *msg)
{
    const PlanMAMsg *proto = PROTO(msg);
    return proto->type() >> 4;
}

int planMAMsgAgent(const plan_ma_msg_t *msg)
{
    const PlanMAMsg *proto = PROTO(msg);
    return proto->agent_id();
}

void *planMAMsgPacked(const plan_ma_msg_t *msg, size_t *size)
{
    const PlanMAMsg *proto = PROTO(msg);
    void *buf;

    *size = proto->ByteSize();
    buf = BOR_ALLOC_ARR(char, *size);
    proto->SerializeToArray(buf, *size);
    return buf;
}

plan_ma_msg_t *planMAMsgUnpacked(void *buf, size_t size)
{
    PlanMAMsg *proto = new PlanMAMsg;
    proto->ParseFromArray(buf, size);
    return (plan_ma_msg_t *)proto;
}


void planMAMsgTerminateSetAgent(plan_ma_msg_t *msg, int agent_id)
{
    PlanMAMsg *proto = PROTO(msg);
    proto->set_terminate_agent_id(agent_id);
}

int planMAMsgTerminateAgent(const plan_ma_msg_t *msg)
{
    const PlanMAMsg *proto = PROTO(msg);
    return proto->terminate_agent_id();
}

void planMAMsgPublicStateSetState(plan_ma_msg_t *msg,
                                  const void *statebuf,
                                  size_t statebuf_size,
                                  int state_id, int cost, int heur)
{
    PlanMAMsg *proto = PROTO(msg);
    proto->set_state(statebuf, statebuf_size);
    proto->set_state_id(state_id);
    proto->set_cost(cost);
    proto->set_heur(heur);
}

const void *planMAMsgPublicStateStateBuf(const plan_ma_msg_t *msg)
{
    const PlanMAMsg *proto = PROTO(msg);
    return proto->state().data();
}

int planMAMsgPublicStateStateId(const plan_ma_msg_t *msg)
{
    return stateId(msg);
}

int planMAMsgPublicStateCost(const plan_ma_msg_t *msg)
{
    const PlanMAMsg *proto = PROTO(msg);
    return proto->cost();
}

int planMAMsgPublicStateHeur(const plan_ma_msg_t *msg)
{
    const PlanMAMsg *proto = PROTO(msg);
    return proto->heur();
}

void planMAMsgTracePathSetStateId(plan_ma_msg_t *msg, int state_id)
{
    PlanMAMsg *proto = PROTO(msg);
    proto->set_state_id(state_id);
}

void planMAMsgTracePathAddPath(plan_ma_msg_t *msg, const plan_path_t *path)
{
    PlanMAMsg *proto = PROTO(msg);
    bor_list_t *item;
    const plan_path_op_t *p;

    for (item = path->prev; item != path; item = item->prev){
        p = BOR_LIST_ENTRY(item, plan_path_op_t, path);

        PlanMAMsgOp *op = proto->add_op();
        op->set_name(p->name);
        op->set_cost(p->cost);
    }
}

int planMAMsgTracePathStateId(const plan_ma_msg_t *msg)
{
    return stateId(msg);
}

void planMAMsgTracePathExtractPath(const plan_ma_msg_t *msg,
                                   plan_path_t *path)
{
    const PlanMAMsg *proto = PROTO(msg);
    int size;

    size = proto->op_size();
    for (int i = 0; i < size; ++i){
        const PlanMAMsgOp &op = proto->op(i);
        planPathPrepend2(path, op.name().c_str(), op.cost());
    }
}

int planMAMsgTracePathInitAgent(const plan_ma_msg_t *msg)
{
    const PlanMAMsg *proto = PROTO(msg);
    return proto->initiator_agent_id();
}


void planMAMsgSnapshotSetType(plan_ma_msg_t *msg, int type)
{
    PlanMAMsg *proto = PROTO(msg);
    proto->set_snapshot_type(type);
}

int planMAMsgSnapshotType(const plan_ma_msg_t *msg)
{
    const PlanMAMsg *proto = PROTO(msg);
    return proto->snapshot_type();
}

long planMAMsgSnapshotToken(const plan_ma_msg_t *msg)
{
    const PlanMAMsg *proto = PROTO(msg);
    return proto->snapshot_token();
}

plan_ma_msg_t *planMAMsgSnapshotNewMark(const plan_ma_msg_t *snapshot_init,
                                        int agent_id)
{
    plan_ma_msg_t *msg = planMAMsgNew(PLAN_MA_MSG_SNAPSHOT,
                                      PLAN_MA_MSG_SNAPSHOT_MARK, agent_id);
    PlanMAMsg *proto = PROTO(msg);
    proto->set_snapshot_token(planMAMsgSnapshotToken(snapshot_init));
    planMAMsgSnapshotSetType(msg, planMAMsgSnapshotType(snapshot_init));
    return msg;
}

plan_ma_msg_t *planMAMsgSnapshotNewResponse(const plan_ma_msg_t *sshot_init,
                                            int agent_id)
{
    plan_ma_msg_t *msg = planMAMsgNew(PLAN_MA_MSG_SNAPSHOT,
                                      PLAN_MA_MSG_SNAPSHOT_RESPONSE, agent_id);
    PlanMAMsg *proto = PROTO(msg);
    proto->set_snapshot_token(planMAMsgSnapshotToken(sshot_init));
    planMAMsgSnapshotSetType(msg, planMAMsgSnapshotType(sshot_init));
    return msg;
}

void planMAMsgSnapshotSetAck(plan_ma_msg_t *msg, int ack)
{
    PlanMAMsg *proto = PROTO(msg);
    proto->set_snapshot_ack(ack);
}

int planMAMsgSnapshotAck(const plan_ma_msg_t *msg)
{
    const PlanMAMsg *proto = PROTO(msg);
    return proto->snapshot_ack();
}


int planMAMsgHeurType(const plan_ma_msg_t *msg)
{
    int type = planMAMsgType(msg);
    int subtype = planMAMsgSubType(msg);

    if (type != PLAN_MA_MSG_HEUR)
        return PLAN_MA_MSG_HEUR_NONE;

    if ((subtype & 0x0f) == subtype)
        return PLAN_MA_MSG_HEUR_REQUEST;

    if ((subtype & 0xf0) == subtype)
        return PLAN_MA_MSG_HEUR_UPDATE;

    return PLAN_MA_MSG_HEUR_NONE;
}


void planMAMsgHeurFFSetRequest(plan_ma_msg_t *msg,
                               const int *init_state, int init_state_size,
                               int goal_op_id)
{
    PlanMAMsg *proto = PROTO(msg);
    planMAMsgSetStateFull(msg, init_state, init_state_size);
    proto->set_goal_op_id(goal_op_id);
}

void planMAMsgHeurFFSetResponse(plan_ma_msg_t *msg, int goal_op_id)
{
    PlanMAMsg *proto = PROTO(msg);
    proto->set_goal_op_id(goal_op_id);
}

int planMAMsgHeurFFOpId(const plan_ma_msg_t *msg)
{
    const PlanMAMsg *proto = PROTO(msg);
    return proto->goal_op_id();
}






void planMAMsgSetStateFull(plan_ma_msg_t *msg, const int *state, int size)
{
    PlanMAMsg *proto = PROTO(msg);

    for (int i = 0; i < size; ++i)
        proto->add_state_full(state[i]);
}

void planMAMsgSetStateFull2(plan_ma_msg_t *msg, const plan_state_t *state)
{
    PlanMAMsg *proto = PROTO(msg);

    int size = planStateSize(state);
    for (int i = 0; i < size; ++i)
        proto->add_state_full(planStateGet(state, i));
}

void planMAMsgStateFull(const plan_ma_msg_t *msg, plan_state_t *state)
{
    const PlanMAMsg *proto = PROTO(msg);
    int len = proto->state_full_size();
    for (int i = 0; i < len; ++i)
        planStateSet(state, i, proto->state_full(i));
}

plan_val_t planMAMsgStateFullVal(const plan_ma_msg_t *msg, plan_var_id_t var)
{
    const PlanMAMsg *proto = PROTO(msg);
    return proto->state_full(var);
}

void planMAMsgAddOp(plan_ma_msg_t *msg, int op_id, plan_cost_t cost,
                    int owner, plan_cost_t value)
{
    PlanMAMsg *proto = PROTO(msg);
    PlanMAMsgOp *op = proto->add_op();
    op->set_op_id(op_id);

    if (cost != PLAN_COST_INVALID)
        op->set_cost(cost);
    if (owner >= 0)
        op->set_owner(owner);
    if (value != PLAN_COST_INVALID)
        op->set_value(value);
}

int planMAMsgOpSize(const plan_ma_msg_t *msg)
{
    const PlanMAMsg *proto = PROTO(msg);
    return proto->op_size();
}

int planMAMsgOp(const plan_ma_msg_t *msg, int i,
                plan_cost_t *cost, int *owner, plan_cost_t *value)
{
    const PlanMAMsg *proto = PROTO(msg);
    const PlanMAMsgOp &op = proto->op(i);

    if (cost)
        *cost = op.cost();
    if (owner)
        *owner = op.owner();
    if (value)
        *value = op.value();
    return op.op_id();
}
