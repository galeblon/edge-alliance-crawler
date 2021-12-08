#ifndef GRAPH_GRAPH_H_
#define GRAPH_GRAPH_H_


#include<stdint.h>

typedef struct vertex * (*vertex_alloc_t)(void);
typedef struct edge * (*edge_alloc_t)(void);

struct edge {
    // Edge id in its from vertex
    uint64_t id;
    // Next edge in the Linked List
    struct edge * next;
    // Pointer to the vertex that the edge is leading to
    struct vertex * to;
};

struct vertex {
    // Vertex id in its graph
    uint64_t id;
    // Linked List of vertices
    struct vertex * next;
    // Id counter for the edges
    // Notice that this simple id algorithm is the limit on edges operations
    uint64_t id_tally;
    // Linked List of edges from this vertex
    struct edge * edge;
};

struct graph {
    // Number of vertices in a graph
    uint64_t n;
    // Id counter for the vertices
    // Notice that this simple id algorithm is the limit on vertex operations
    uint64_t id_tally;
    // Linked List of vertices
    struct vertex * v;
    // Vertex and edge allocators
    vertex_alloc_t new_vertex;
    edge_alloc_t new_edge;
};

void error(const char * msg);

struct graph new_graph(uint64_t nvertices, vertex_alloc_t vertex_alloc, edge_alloc_t edge_alloc);
struct graph parse_graph6(const char * buff, vertex_alloc_t vertex_alloc, edge_alloc_t edge_alloc);
void add_edge(struct graph * g, struct vertex * from, struct vertex * to);
struct vertex * get_vertex(struct graph * g, uint64_t id);
void delete_graph(struct graph * g);
void print_graph(struct graph * g);

void delete_edge(struct vertex * from, struct vertex * to);
uint64_t get_indegree(struct graph* g, struct vertex * v);
uint64_t get_outdegree(struct vertex * v);
uint64_t get_degree(struct vertex* v);
uint64_t get_degree_graph(struct graph* g);
int is_balanced(struct graph * g);
struct vertex * add_vertex(struct graph * g);
void isolate_vertex(struct graph* g, struct vertex* v);
void delete_vertex(struct graph* g, struct vertex* v);

// TODO edge capacities

#endif
