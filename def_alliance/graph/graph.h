struct edge {
    struct edge * next;
    uint64_t to;
};

struct vertex {
    uint64_t idx;
    struct edge * edge;
};

struct graph {
    uint64_t n;
    struct vertex * v;
};

struct graph new_graph(uint64_t nvertices);
struct graph parse_graph6(const char * buff);
void add_edge(struct graph * g, uint64_t from, uint64_t to);
struct vertex * get_vertex(struct graph * g, uint64_t index);
void delete_graph(struct graph * g);
void print_graph(struct graph * g);



