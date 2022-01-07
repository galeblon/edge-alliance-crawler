#ifndef GRAPH_GRAPH_H_
#define GRAPH_GRAPH_H_


#include<stdint.h>

struct edge {
    // Edge id in its from vertex
    uint64_t id;
    // Next edge in the Linked List
    struct edge * next;
    // Pointer to the vertex that the edge is leading to
    struct vertex * to;
    // The cursed C decorator, that surely has a more proper name
    // If a specific algorithm requires some additional fields, pack them in a struct and insert here to keep O(1) access times
    void * decorator;
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
    // The cursed C decorator, that surely has a more proper name
    // If a specific algorithm requires some additional fields, pack them in a struct and insert here to keep O(1) access times
    void * decorator;
};

struct graph {
    // Number of vertices in a graph
    uint64_t n;
    // Id counter for the vertices
    // Notice that this simple id algorithm is the limit on vertex operations
    uint64_t id_tally;
    // Linked List of vertices
    struct vertex * v;
    // The cursed C decorator, that surely has a more proper name
    // If a specific algorithm requires some additional fields, pack them in a struct and insert here to keep O(1) access times
    void * decorator;
};

struct graph new_graph(uint64_t nvertices);
struct graph parse_graph6(const char * buff);
void add_edge(struct vertex * from, struct vertex * to);
struct vertex * get_vertex(struct graph * g, uint64_t id);
void delete_graph(struct graph * g);
void print_graph(struct graph * g);

void delete_edge(struct vertex * from, struct vertex * to);
uint64_t get_indegree(struct graph* g, struct vertex * v);
uint64_t get_outdegree(struct graph* g, struct vertex * v);
uint64_t get_degree(struct graph* g, struct vertex* v);
uint64_t get_degree_graph(struct graph* g);
int is_balanced(struct graph * g);
struct vertex * add_vertex(struct graph * g);
struct vertex * add_vertex_unique(struct graph* g, uint64_t id);
void isolate_vertex(struct graph* g, struct vertex* v);
void delete_vertex(struct graph* g, struct vertex* v);

#endif
