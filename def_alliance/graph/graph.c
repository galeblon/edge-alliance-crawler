#include<stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "graph.h"

static void error(const char * msg)
{
    fprintf(stderr, "ERROR: %s\n", msg);
    // TODO
}

struct graph new_graph(uint64_t nvertices)
{
    struct graph g = {.n = nvertices};

    if (g.n == 0) {
        return g;
    }

    g.v = malloc(sizeof(struct vertex));
    if (!g.v) {
        error("Memory allocation for vertex failed");
        exit(-1);
    }
    g.v->id = 0;
    g.v->edge = NULL;

    struct vertex * v = g.v;
    for (g.id_tally = 1; g.id_tally < g.n; g.id_tally++) {
        v->next = malloc(sizeof(struct vertex));
        if (!v->next) {
            error("Memory allocation for vertex failed");
            delete_graph(&g);
            exit(-2);
        }
        v = v->next;
        v->edge = NULL;
        v->id = g.id_tally;
    }
    v->next = NULL;

    return g;
}

struct vertex * get_vertex(struct graph * g, uint64_t id)
{
    struct vertex* v = g->v;
    while (v) {
        if (v->id == id) {
            return v;
        }
        v = v->next;
    }

    return NULL;
}

void add_edge(struct vertex * from, struct vertex * to)
{
    struct edge* e = NULL;

    if (from->edge) {
        struct edge* next = from->edge;

        while (next) {
            if (next->to == to)
                return; // edge already exists
            e = next;
            next = next->next;
        }

        e->next = malloc(sizeof(struct edge));
        e = e->next;
    }
    else {
        // There are no edges for this vertex yet
        from->edge = malloc(sizeof(struct edge));
        e = from->edge;
    }

    if (!e) {
        error("Memory allocation for edge failed");
        return;
    }

    e->next = NULL;
    e->to = to;
    e->id = from->id_tally++;
}

void delete_edge(struct vertex* from, struct vertex* to)
{
    struct edge* e = from->edge;

    if (e) {
        struct edge* next = e->next;
        if (e->to == to) {
            from->edge = next;
            free(e);
            return;
        }

        while (next) {
            if (next->to == to) {
                e->next = next->next;
                free(next);
                return;
            }
            e = next;
            next = next->next;
        }
    }
}

void delete_graph(struct graph * g)
{
    struct edge * e, * e_next;
    struct vertex* v, * v_next;

    if (g->v == NULL) {
        error("Vertices table cannot be freed, because it is not allocated");
        return;
    }

    v = g->v;
    while (v) {
        e = v->edge;
        while (e) {
            e_next = e->next;
            free(e);
            e = e_next;
        }

        v_next = v->next;
        free(v);
        v = v_next;
    }
}

void print_graph(struct graph * g)
{
    struct edge * e;
    struct vertex* v;

    printf("N=%llu\n", g->n);
    v = g->v;
    while (v) {
        printf("  %llu :", v->id);
        e = v->edge;
        while (e) {
            printf(" %llu", e->to->id);
            e = e->next;
        }
        printf(";\n");
        v = v->next;
    }
}

uint64_t get_indegree(struct graph* g, struct vertex * v)
{
    struct edge* e;
    struct vertex* v_checked;
    uint64_t deg = 0;

    v_checked = g->v;
    while (v_checked) {
        e = v_checked->edge;
        while (e) {
            if (e->to == v) {
                deg++;
            }
            e = e->next;
        }
        v_checked = v_checked->next;
    }

    return deg;
}

uint64_t get_outdegree(struct graph* g, struct vertex * v)
{
    uint64_t deg = 0;
    struct edge* e = v->edge;
    while (e != NULL) {
        deg++;
        e = e->next;
    }
    return deg;
}

uint64_t get_degree(struct graph * g, struct vertex * v)
{
    // As degree pertains to a simple graph and this is a digraph, this should be equal
    return get_outdegree(g, v);
}

uint64_t get_degree_graph(struct graph * g)
{
    struct vertex* v;
    uint64_t deg = 0, v_deg;

    v = g->v;
    while (v) {
        v_deg = get_degree(g, v);
        deg = v_deg > deg ? v_deg : deg;
    }

    return deg;
}

int is_balanced(struct graph * g)
{
    struct vertex* v;
    int balanced = 1;
    uint64_t v_indeg, v_outdeg;

    v = g->v;
    while (v) {
        v_indeg = get_indegree(g, v);
        v_outdeg = get_outdegree(g, v);
        balanced &= (v_indeg == v_outdeg);
    }

    return balanced;
}

// Add vertex if given id is not already present, otherwise return existing vertex
struct vertex * add_vertex_unique(struct graph *g, uint64_t id) {
    struct vertex* v = NULL;

    if (g->v) {
        struct vertex* curr = g->v;
        while (curr) {
            if (curr->id == id) {
                return curr;
            }
            curr = curr->next;
        }

        struct vertex* next = g->v;

        g->v = malloc(sizeof(struct vertex));
        v = g->v;
        if (v == NULL)
            exit(-1);
        v->next = next;
    }
    else {
        // There are no vertices for this graph yet
        g->v = malloc(sizeof(struct vertex));
        v = g->v;
        if (v == NULL)
            exit(-1);
        v->next = NULL;
    }

    if (!g->v) {
        error("Memory allocation for vertex failed");
        return NULL;
    }

    v->edge = NULL;
    g->id_tally = g->id_tally <= id ? id + 1 : g->id_tally;
    v->id = id;

    g->n++;

    return v;
}

// Add vertex at the start of the vertex linked list. The list is not ordered by index.
struct vertex * add_vertex(struct graph * g)
{
    struct vertex * v = NULL;

    if (g->v) {
        struct vertex * next = g->v;

        g->v = malloc(sizeof(struct vertex));
        v = g->v;
        if (v == NULL)
            exit(-1);
        v->next = next;
    }
    else {
        // There are no vertices for this graph yet
        g->v = malloc(sizeof(struct vertex));
        v = g->v;
        if (v == NULL)
            exit(-1);
        v->next = NULL;
    }

    if (!g->v) {
        error("Memory allocation for vertex failed");
        return NULL;
    }

    v->edge = NULL;
    v->id = g->id_tally++;

    g->n++;

    return v;
}

//Makes vertex an isolated vertex, that is: deletes every edge related to it.
void isolate_vertex(struct graph * g, struct vertex * v)
{
    struct edge * e = v->edge, * e_next;
    while (e) {
        e_next = e->next;
        free(e);
        e = e_next;
    }

    struct vertex* v_checked = g->v;
    while (v_checked) {
        delete_edge(v_checked, v);
        v_checked = v_checked->next;
    }
}

void delete_vertex(struct graph * g, struct vertex * v)
{
    struct vertex * v_checked = g->v;

    if (v_checked) {
        struct vertex * next = v_checked->next;
        if (v_checked == v) {
            g->v = next;
            isolate_vertex(g, v_checked);
            free(v_checked);
            g->n--;
            return;
        }

        while (next) {
            if (next == v) {
                v_checked->next = next->next;
                isolate_vertex(g, next);
                free(next);
                g->n--;
                return;
            }
            v_checked = next;
            next = next->next;
        }
    }
}

// Calculates the number of vertices of the graph
// Returns the offset at which the adjacency matrix starts
int N_inverse(uint64_t * out, const char * bytes)
{
    int nbytes = 0;
    uint64_t multiplier = 1;
    uint64_t result = 0;

    if (bytes[0] != 126) {
        result = (uint64_t) (bytes[0] - 63);
        nbytes = 1;
    } else if (bytes[1] != 126) {
        // N is encoded on bytes 1:3
        for (int i = 3; i >= 1; i--) {
            result += (uint64_t)(bytes[i] - 63) * multiplier;
            multiplier *= 64;  // each byte/character contains 6 bits
        }
        nbytes = 4;
    } else {
        // N is encoded on bytes 2:8
        for (int i = 8; i >= 2; i--) {
            result += (uint64_t)(bytes[i] - 63) * multiplier;
            multiplier *= 64;  // each byte/character contains 6 bits
        }
        nbytes = 8;
    }

    *out = result;
    return nbytes;
}

struct graph parse_graph6(const char * buff)
{
    int len = strlen(buff);
    uint64_t nvertices = 0;
    int offset = N_inverse(&nvertices, buff);

    struct graph g = new_graph(nvertices);
    int k = 1;
    uint8_t x;
    const char * p = buff + offset;

    struct vertex* v_i, * v_j;
    for (int j = 1; j < nvertices; ++j) {
        for (int i = 0; i < j; ++i) {

            if (--k == 0) {
                k = 6;
                x = *(p++) - 63;

                if (--len < 0) {
                    error("Adjacency matrix truncated in graph");
                }
            }

            if (x & 32) {
                v_i = get_vertex(&g, i);
                v_j = get_vertex(&g, j);
                add_edge(v_i, v_j);
                add_edge(v_j, v_i);
            }

            x <<= 1;
        }
    }

    return g;
}
