
#include <stdlib.h>
#include <stdbool.h>
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

void* data_create (int i) {
  int* tmp = xmalloc(sizeof(int));
  *tmp = i;
  return (void*)tmp;
}

int main() {

  void* data1 = data_create(2);
  void* data2 = data_create(3);
  void* data3 = data_create(5);
  queue_t Q1 = queue_new();
  enq(Q1, data1);
  enq(Q1, data2);
  enq(Q1, data3);
  assert(*(int*)queue_peek(Q1,1) == 3);
  assert(queue_size(Q1) == 3);
  queue_reverse(Q1);
  assert(*(int*)queue_peek(Q1,0) == 5);
  assert(*(int*)queue_peek(Q1,2) == 2);
  

  deq(Q1);
  assert(*(int*)queue_peek(Q1,0) == 3);
  deq(Q1);
  deq(Q1);
  assert(queue_size(Q1) == 0);

  queue_free(Q1, &free);
  free(data1);
  free(data2);
  free(data3);
  printf("All tests passed!\n");
  return 0;
}