struct edge {
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
};

struct graph new_graph(uint64_t nvertices);
struct graph parse_graph6(const char * buff);
void add_edge(struct vertex * from, struct vertex * to);
struct vertex * get_vertex(struct graph * g, uint64_t id);
void delete_graph(struct graph * g);
void print_graph(struct graph * g);

void delete_edge(struct vertex * from, struct vertex * to);
uint64_t get_indegree(struct graph* g, struct vertex * v);
uint64_t get_outdegree(struct vertex * v);
uint64_t get_degree(struct vertex* v);
uint64_t get_degree(struct graph* g);
bool is_balanced(struct graph * g);
struct vertex * add_vertex(struct graph * g);
void isolate_vertex(struct graph* g, struct vertex* v);
void delete_vertex(struct graph* g, struct vertex* v);
