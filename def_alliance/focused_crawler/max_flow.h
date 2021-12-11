#ifndef MAX_FLOW_ALGORITHMS_H_
#define MAX_FLOW_ALGORITHMS_H_

#include "../graph/graph.h"
#include <stdlib.h>

struct vertex_queue_node {
	//Vertex
	struct vertex * v;
	//Next node
	struct vertex_queue_node* next;
	//Previous node
	struct vertex_queue_node* prev;
};

struct vertex_queue {
	//Start of Doubly Linked List - take out here
	struct vertex_queue_node * first;
	//End - put in here
	struct vertex_queue_node * last;
};

struct edge_ek_decorator {
	struct vertex * from;
	uint64_t f;
};

// Goes from the current node to the source
struct vertex_path_node {
	struct edge * e;
	struct vertex_path_node * next;
};

struct vertex_ek_decorator {
	int visited;
	int64_t min_path_cf;
	// Path taken to get to this vertex - Linked List
	struct vertex_path_node * next;
};

int64_t capacity(struct vertex * from, struct edge * e, struct vertex * source, struct vertex * target, int64_t k);

int64_t ford_fulkerson(struct graph * g, struct vertex * source, struct vertex * target, int64_t k);

int64_t edmonds_karp(struct graph* g, struct vertex* source, struct vertex* target, int64_t k);
void free_edge_ek_decorators(struct graph * g);
void free_vertex_ek_decorators(struct graph* g);

struct vertex** source_component_min_cut_after_max_flow(struct graph* g, struct vertex* source, struct vertex* target, uint64_t k, uint64_t* source_component_size);

struct graph* max_flow_cut(struct graph* g, struct vertex* source, struct vertex* target);

#endif /* MAX_FLOW_ALGORITHMS_H_ */
