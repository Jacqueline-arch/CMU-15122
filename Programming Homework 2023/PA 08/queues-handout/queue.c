#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "lib/contracts.h"
#include "lib/xalloc.h"
#include "queue.h"

typedef struct list_node list;
struct list_node {
  void* data;
  list* next;
};

typedef struct queue_header queue;
struct queue_header {
  list* front;
  list* back;
  size_t size;
};

bool is_inclusive_segment(list* start, list* end, size_t i) 
{
  if (start == NULL) {
    if (i == 0) return true;
    return false;
  }
  if (start == end) {
    if (i == 1) return true;
    return false;
  }
  ASSERT(start != NULL && start != end);
  return is_inclusive_segment(start->next, end, i-1);
}

bool is_queue(queue* Q) {
  return Q != NULL
      && is_inclusive_segment(Q->front, Q->back, Q->size);
}

void print_queue(queue* Q, print_data_fn* print_data) {
  list* node = Q->front;
  while (node != Q->back) {
    (*print_data)(node->data);
    node = node->next;
  }
  (*print_data)(node->data);
  return;
}

/* implementation of exported functions*/
queue* queue_new()
{
  queue* Q = xcalloc(1, sizeof(queue));
  Q->front = NULL;
  Q->back = NULL;
  ENSURES(is_queue(Q));
  return Q;
}

size_t queue_size(queue* Q)
{
  REQUIRES(is_queue(Q));
  return Q->size;
}

void enq(queue* Q, void* x)
{ 
  REQUIRES(is_queue(Q));
  list* node = xmalloc(sizeof(list));
  node->data = x;
  node->next = NULL;
  if (Q->front == NULL) {
    Q->front = node;
    Q->back = node;
  }
  else {
    ASSERT(Q->front!=NULL && Q->back!=NULL);
    Q->back->next = node;
    Q->back = node;
  }
  Q->size++;
  ENSURES(is_queue(Q));
}

void* deq(queue* Q)
{
  REQUIRES(is_queue(Q));
  list* node = Q->front;
  Q->front = Q->front->next;
  Q->size --;
  ENSURES(is_queue(Q));
  void* result = node->data;
  free(node);
  return result;
}

void* queue_peek(queue* Q, size_t i)
{
  REQUIRES(is_queue(Q));
  REQUIRES(i < Q->size);
  list* node = Q->front;
  while (i > 0) {
    node = node->next;
    i --;
  }
  ASSERT(i == 0);
  return node->data;
}

void queue_reverse(queue* Q)
{
  REQUIRES(is_queue(Q));
  if(Q->size == 0 || Q->size == 1) return;
  list* prev = Q->front;
  list* cur = Q->front->next;
  list* nxt = Q->front->next->next;
  ASSERT(cur->next == nxt);
  while(nxt != NULL) 
  {
    cur->next = prev;
    prev = cur;
    cur = nxt;
    nxt = nxt->next;
    ASSERT(cur->next == nxt);
  }
  ASSERT(nxt == NULL);
  cur->next = prev;
  Q->front->next = NULL;
  list* tmp = Q->front;
  Q->front = Q->back;
  Q->back = tmp;
  ENSURES(is_queue(Q));
}


bool queue_all(queue* Q, check_property_fn* P)
{
  REQUIRES(is_queue(Q)); 
  REQUIRES(P!=NULL);   
  list* node = Q->front;
  while (node != NULL) {
    if (! (*P)(node->data)) return false;
    node = node->next;
  }
  ASSERT(node == NULL);
  return true;
}

void* queue_iterate(queue* Q, void* base, iterate_fn* F)
{
  REQUIRES(is_queue(Q));
  REQUIRES(F!=NULL);
  list* tmp = Q->front;
  while (tmp != NULL) {
    base = (*F)(base, tmp->data);
    tmp = tmp->next;
  }
  ASSERT(tmp == NULL);
  return base;
}



void queue_free(queue* Q, free_fn *F)
{
  REQUIRES(is_queue);
  list* tmp = Q->front;
  while(tmp != NULL) {
    list* copy = tmp;
    if(F!=NULL) {
        (*F)(copy->data);
    }
    tmp = tmp->next;
    free(copy);
  }
  free(Q);
}