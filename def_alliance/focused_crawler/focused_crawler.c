#include "focused_crawler.h"

void extend_list_by_metric_value(
		struct graph* g,
		struct vertex* start_v,
		struct vertex* expanded_list[],
		int* expanded_len,
		struct vertex* feasible_list[],
		int feasible_len,
		uint64_t(*metric)(struct graph*, struct vertex*),
		int metric_seeked_value)
{
	for(uint64_t feasible_vertex_index = 0; feasible_vertex_index < feasible_len; feasible_vertex_index++ ) {
		struct vertex* feasible_vertex = feasible_list[feasible_vertex_index];
		int metric_value = (*metric)(g, feasible_vertex);
		if(metric_value == metric_seeked_value) {
			int skip_vertex = 0;
			for(uint64_t expanded_index=0; expanded_index<expanded_len; expanded_index++) {
				if(feasible_vertex == expanded_list[expanded_index]) {
					skip_vertex = 1;
					break;
				}
			}
			if(skip_vertex)
				continue;
			// TODO set capacity to +inf, or during max_flow_cut we may assume all edges starting
			// from virtual start have +inf  capacity
			add_edge(start_v, feasible_vertex);
			// Realloc every time, will probably have to change to a list or something
			expanded_list = realloc(expanded_list, ++(*expanded_len)*sizeof(struct vertex*));
			expanded_list[*expanded_len-1] = feasible_vertex;
		}
	}
}

// Don't use for metrics with possible negative value
int find_metric_max_value(
		struct graph* g,
		uint64_t(*metric)(struct graph*, struct vertex*),
		struct vertex* v_list[],
		int v_len,
		struct vertex* v_skip_list[],
		int skip_len)
{
	int metric_max_value = 0;
	for(uint64_t vertex_index = 0; vertex_index<v_len; vertex_index++) {
		int skip_vertex = 0;
		struct vertex* vertex = get_vertex(g, vertex_index);
		for(uint64_t skip_vertex_index = 0; skip_vertex_index<skip_len; skip_vertex_index++) {
			if(vertex == v_skip_list[skip_vertex_index]) {
				skip_vertex = 1;
				break;
			}
		}
		if(skip_vertex)
			continue;
		int metric_value = (*metric)(g, vertex);
		if(metric_value > metric_max_value)
			metric_max_value = metric_value;
	}
	return metric_max_value;
}

// TODO change add_vertex to skip adding when vertex with given id exists
struct vertex* expand_graph(struct graph* g, struct vertex* curr_v_G, int depth=0) {
	struct vertex* v = add_vertex_unique(g, curr_v_G->id);
	if(depth == 2)
		return v;
	struct edge* next = curr_v_G->edge;
	while (next) {
		struct vertex* to = add_vertex_unique(g, next->to);
		add_edge(v, to);
		expand_graph(g, next->to, depth+1);
		next = next->next;
	}
	return v;
}

struct graph induce_graph_from_crawl(struct graph* G, struct vertex* seed[], struct vertex* local_seed[], int seed_size)
{
	struct graph g = new_graph(seed_size);
	struct vertex* virtual_source = add_vertex(g);
	struct vertex* virtual_sink = add_vertex(g);

	for(uint seed_index=0; seed_index<seed_size; seed_index++) {
		struct vertex* v = expand_graph(g, seed[seed_index]);
		local_seed[seed_index] = v;
		add_edge(virtual_source, v);
	}

	for(uint i = 0; i<g->n; i++) {
		struct vertex* v = get_vertex(g, i);
		if(v == virtual_sink || v == virtual_source)
			continue;
		if(get_outdegree(v) <= 1) {
			add_edge(v, virtual_sink);
		}
	}

	return g;
}

//TODO
struct vertex** max_flow_cut(struct graph* g, struct vertex* source, struct vertex* target, int* c_size) {
	*c_size = 0;
	return NULL;
}

// G is assumed to be a giant graph resulting from extensive web crawling
void focused_crawl(struct graph* G, struct vertex* seed_list[], struct vertex* s, struct vertex* t, uint64_t iterations, uint64_t k)
{
	// seed_list refers to vertices from global G graph, translate them to smaller g graph
	struct vertex* local_seed_list[] = malloc(sizeof(struct vertex*)*k);
	struct graph g = induce_graph_from_crawl(G, seed_list, local_seed_list, k);
	for (uint64_t current_iteration = 0; current_iteration < iterations; current_iteration++) {
		int c_len;
		struct vertex* c_list[] = max_flow_cut(g, s, t, &c_len);

		int max_indegree = find_metric_max_value(g, get_indegree, c_list, c_len, local_seed_list, k);
		extend_list_by_metric_value(g, local_seed_list, &k, c_list, c_len, get_indegree, max_indegree);

		int max_outdegree = find_metric_max_value(g, get_outdegree, c_list, c_len, local_seed_list, k);
		extend_list_by_metric_value(g, local_seed_list, &k, c_list, c_len, get_outdegree, max_outdegree);

		// TODO update global seed list from local seed list
		// local_seed_list -> seed_list

		// Re-crawl G and reinduce the graph
		delete_graph(&g);
		free(local_seed_list);
		g = induce_graph_from_crawl(G, seed_list, local_seed_list, k);
	}
}