#include <boruvka/alloc.h>

#include "plan/ma_comm.h"
#include "ma_msg.pb.h"

plan_ma_msg_t *planMAMsgNew(void)
{
    PlanMAMsg *msg;
    msg = new PlanMAMsg;
    return msg;
}

void planMAMsgDel(plan_ma_msg_t *_msg)
{
    PlanMAMsg *msg = static_cast<PlanMAMsg *>(_msg);
    delete msg;
}

void *planMAMsgPacked(const plan_ma_msg_t *_msg, size_t *size)
{
    const PlanMAMsg *msg = static_cast<const PlanMAMsg *>(_msg);
    void *buf;

    *size = msg->ByteSize();
    buf = BOR_ALLOC_ARR(char, *size);
    msg->SerializeToArray(buf, *size);
    return buf;
}

plan_ma_msg_t *planMAMsgUnpacked(void *buf, size_t size)
{
    PlanMAMsg *msg = new PlanMAMsg;
    msg->ParseFromArray(buf, size);
    return msg;
}

void planMAMsgSetPublicState(plan_ma_msg_t *_msg, int agent_id,
                             const void *state, size_t state_size,
                             int cost, int heuristic)
{
    PlanMAMsg *msg = static_cast<PlanMAMsg *>(_msg);
    PlanMAMsgPublicState *public_state;

    msg->set_type(PlanMAMsg::PUBLIC_STATE);
    public_state = msg->mutable_public_state();
    public_state->set_agent_id(agent_id);
    public_state->set_state(state, state_size);
    public_state->set_cost(cost);
    public_state->set_heuristic(heuristic);
}

int planMAMsgIsPublicState(const plan_ma_msg_t *_msg)
{
    const PlanMAMsg *msg = static_cast<const PlanMAMsg *>(_msg);
    return msg->type() == PlanMAMsg::PUBLIC_STATE;
}

void planMAMsgGetPublicState(const plan_ma_msg_t *_msg, int *agent_id,
                             void *state, size_t state_size,
                             int *cost, int *heuristic)
{
    const PlanMAMsg *msg = static_cast<const PlanMAMsg *>(_msg);
    size_t st_size;

    const PlanMAMsgPublicState &public_state = msg->public_state();
    *agent_id = public_state.agent_id();

    st_size = BOR_MIN(state_size, public_state.state().size());
    memcpy(state, public_state.state().data(), st_size);

    *cost      = public_state.cost();
    *heuristic = public_state.heuristic();
}

