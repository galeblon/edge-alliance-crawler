#include "focused_crawler.h"

// TODO skim the graph further (only keep seed and depth 2 of original crawl snapshot)
void induce_graph_from_crawl(struct graph* g, struct vertex* seed[], int seed_size)
{
	struct vertex* virtual_source = add_vertex(g);
	struct vertex* virtual_sink = add_vertex(g);
	for(uint i = 0; i<seed_size; i++) {
		add_edge(virtual_source, seed[i]);
	}

	for(uint i = 0; i<g->n; i++) {
		struct vertex* v = get_vertex(g, i);
		if(v == virtual_sink)
			continue;
		if(get_outdegree(v) <= 1) {
			add_edge(v, virtual_sink);
		}
	}
}

//TODO
struct graph* max_flow_cut(struct graph* g, struct vertex* source, struct vertex* target) {
	return NULL;
}

void focused_crawl(struct graph* g, struct vertex* seed_list[], struct vertex* s, struct vertex* t, uint64_t iterations, uint64_t k)
{
	for (uint64_t current_iteration = 0; current_iteration < iterations; current_iteration++) {
		struct graph* C = max_flow_cut(g, s, t);
		int max_indegree = 0;
		for(uint64_t c_vertex = 0; c_vertex<C->n; c_vertex++) {
			int skip_vertex = 0;
			struct vertex* c_v = get_vertex(C, c_vertex);
			for(uint64_t seed_vertex = 0; seed_vertex<k; seed_vertex++) {
				if(c_v == seed_list[seed_vertex]) {
					skip_vertex = 1;
					break;
				}
			}
			if(skip_vertex)
				continue;
			int in_degree = get_indegree(g, c_v);
			if(in_degree >= max_indegree)
				max_indegree = in_degree;
		}
		for(uint64_t c_vertex = 0; c_vertex<C->n; c_vertex++) {
			struct vertex* c_v = get_vertex(C, c_vertex);
			int in_degree = get_indegree(g, c_v);
			if(in_degree == max_indegree) {
				// TODO
				// add c_v to seed_list
				// add edge (s, cv) with +inf capacity
			}
		}
		int max_outdegree = 0;
		for(uint64_t c_vertex = 0; c_vertex<C->n; c_vertex++) {
			int skip_vertex = 0;
			struct vertex* c_v = get_vertex(C, c_vertex);
			for(uint64_t seed_vertex = 0; seed_vertex<k; seed_vertex++) {
				if(c_v == seed_list[seed_vertex]) {
					skip_vertex = 1;
					break;
				}
			}
			if(skip_vertex)
				continue;
			int out_degree = get_outdegree(g, c_v);
			if(out_degree >= max_outdegree)
				max_outdegree = out_degree;
		}
		for(uint64_t c_vertex = 0; c_vertex<C->n; c_vertex++) {
			struct vertex* c_v = get_vertex(C, c_vertex);
			int out_degree = get_outdegree(g, c_v);
			if(out_degree == max_outdegree) {
				// TODO
				// add c_v to seed_list
				// add edge (s, cv) with +inf capacity
			}
		}

		// TODO
		// Re-crawl G and reinduce the graph
	}
}
