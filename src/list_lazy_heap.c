#include <boruvka/alloc.h>
#include "plan/list_lazy_heap.h"

struct _heap_node_t {
    plan_cost_t cost;
    plan_state_id_t parent_state_id;
    plan_operator_t *op;
    bor_pairheap_node_t heap; /*!< Connector to an open list */
};
typedef struct _heap_node_t heap_node_t;

static int heapLessThan(const bor_pairheap_node_t *n1,
                        const bor_pairheap_node_t *n2,
                        void *data);

plan_list_lazy_heap_t *planListLazyHeapNew(void)
{
    return borPairHeapNew(heapLessThan, NULL);
}

void planListLazyHeapDel(plan_list_lazy_heap_t *l)
{
    planListLazyHeapClear(l);
    borPairHeapDel(l);
}

void planListLazyHeapPush(plan_list_lazy_heap_t *l,
                          plan_cost_t cost,
                          plan_state_id_t parent_state_id,
                          plan_operator_t *op)
{
    heap_node_t *n;

    n = BOR_ALLOC(heap_node_t);
    n->cost            = cost;
    n->parent_state_id = parent_state_id;
    n->op              = op;
    borPairHeapAdd(l, &n->heap);
}

int planListLazyHeapPop(plan_list_lazy_heap_t *l,
                        plan_state_id_t *parent_state_id,
                        plan_operator_t **op)
{
    bor_pairheap_node_t *heap_node;
    heap_node_t *n;

    if (borPairHeapEmpty(l))
        return -1;

    heap_node = borPairHeapExtractMin(l);
    n = bor_container_of(heap_node, heap_node_t, heap);

    *parent_state_id = n->parent_state_id;
    *op              = n->op;
    BOR_FREE(n);

    return 0;
}

void planListLazyHeapClear(plan_list_lazy_heap_t *l)
{
    // TODO: This can be done more efficiently, but requires changes in
    //       boruvka/pairheap
    bor_pairheap_node_t *heap_node;
    heap_node_t *n;

    while (!borPairHeapEmpty(l)){
        heap_node = borPairHeapExtractMin(l);
        n = bor_container_of(heap_node, heap_node_t, heap);
        BOR_FREE(n);
    }
}


static int heapLessThan(const bor_pairheap_node_t *_n1,
                        const bor_pairheap_node_t *_n2,
                        void *data)
{
    heap_node_t *n1 = bor_container_of(_n1, heap_node_t, heap);
    heap_node_t *n2 = bor_container_of(_n2, heap_node_t, heap);
    return n1->cost <= n2->cost;
}
