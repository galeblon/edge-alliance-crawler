#include"focused_crawler.h"
#include"max_flow.h"
#include"../graph/graph.h"
#include<assert.h>

struct vertex** extend_list_by_metric_value(
		struct graph* g,
		struct vertex* start_v,
		struct vertex** expanded_list,
		uint64_t* expanded_len,
		struct vertex* feasible_list[],
		uint64_t feasible_len,
		uint64_t(*metric)(struct graph*, struct vertex*),
		uint64_t metric_seeked_value,
		struct vertex* s,
		struct vertex* t)
{
	for(uint64_t feasible_vertex_index = 0; feasible_vertex_index < feasible_len; feasible_vertex_index++ ) {
		struct vertex* feasible_vertex = feasible_list[feasible_vertex_index];
		uint64_t metric_value = (*metric)(g, feasible_vertex);
		if(metric_value == metric_seeked_value) {
			int skip_vertex = 0;
			for(uint64_t expanded_index=0; expanded_index<*expanded_len; expanded_index++) {
				if(feasible_vertex == expanded_list[expanded_index]) {
					skip_vertex = 1;
					break;
				}
			}
			if(skip_vertex || feasible_vertex == s || feasible_vertex == t)
				continue;
			add_edge(start_v, feasible_vertex);
			// Realloc every time, will probably have to change to a list or something
			struct vertex** tmp = realloc(expanded_list, (size_t)(++(*expanded_len)) * sizeof(struct vertex*));
			if (tmp == NULL)
				exit(-1);
			expanded_list = tmp;
			expanded_list[*expanded_len-1] = feasible_vertex;
		}
	}
	return expanded_list;
}

// Don't use for metrics with possible negative value
uint64_t find_metric_max_value(
		struct graph* g,
		uint64_t(*metric)(struct graph*, struct vertex*),
		struct vertex* v_list[],
		uint64_t v_len,
		struct vertex* v_skip_list[],
		uint64_t skip_len,
		struct vertex* s,
		struct vertex* t)
{
	uint64_t metric_max_value = 0;
	struct vertex* vertex = g->v;
	for(uint64_t vertex_index = 0; vertex_index<v_len; vertex_index++) {
		int skip_vertex = 0;
		struct vertex* vertex = v_list[vertex_index];
		for(uint64_t skip_vertex_index = 0; skip_vertex_index<skip_len; skip_vertex_index++) {
			if(vertex == v_skip_list[skip_vertex_index]) {
				skip_vertex = 1;
				break;
			}
		}
		if(skip_vertex)
			continue;
		uint64_t metric_value = (*metric)(g, vertex);
		if(metric_value > metric_max_value)
			metric_max_value = metric_value;
	}
	return metric_max_value;
}

struct vertex* expand_graph(struct graph* g, struct vertex* curr_v_G, int depth) {
	struct vertex* v = add_vertex_unique(g, curr_v_G->id);
	if(depth == 2)
		return v;
	struct edge* next = curr_v_G->edge;
	while (next) {
		struct vertex* to = add_vertex_unique(g, next->to->id);
		add_edge(v, to);
		expand_graph(g, next->to, depth+1);
		next = next->next;
	}
	return v;
}

struct graph induce_graph_from_crawl(struct graph* G, struct vertex* seed[], struct vertex* local_seed[], uint64_t seed_size, struct vertex** s, struct vertex** t)
{
	struct graph g = new_graph(0);

	// Local copy of induced graph
	for(uint64_t seed_index=0; seed_index<seed_size; seed_index++) {
		struct vertex* v = expand_graph(&g, get_vertex(G, seed[seed_index]->id), 0);
		local_seed[seed_index] = v;
	}

	struct vertex* virtual_source = add_vertex(&g);
	struct vertex* virtual_sink = add_vertex(&g);

	// Connect seed to source
	for(uint64_t seed_index = 0; seed_index < seed_size; seed_index++) {
		add_edge(virtual_source, local_seed[seed_index]);
	}

	// Connect last layer to sink
	struct vertex* v = g.v;
	while (v) {
		if (v == virtual_sink || v == virtual_source) {
			v = v->next;
			continue;
		}
		if (get_outdegree(&g, v) == 0) {
			add_edge(v, virtual_sink);
		}
		v = v->next;
	}

	*s = virtual_source;
	*t = virtual_sink;
	return g;
}

struct vertex** max_flow_cut(struct graph* g, struct vertex* source, struct vertex* target, uint64_t k, uint64_t * c_size) {
	struct vertex** c_list = source_component_min_cut_after_max_flow(g, source, target, k, c_size);
	free_edge_ek_decorators(g);
	free_vertex_ek_decorators(g);
	return c_list;
}

// TODO save to file updated seed each iteration
// G is assumed to be a giant graph resulting from extensive web crawling
struct vertex** focused_crawl(struct graph* G, struct vertex* seed_list[], uint64_t iterations, uint64_t* k)
{
	// seed_list refers to vertices from global G graph, translate them to smaller g graph
	struct vertex** local_seed_list = malloc(sizeof(struct vertex*)*(size_t)*k);
	if (local_seed_list == NULL)
		exit(-1);
	struct vertex** local_seed_tmp = NULL;
	struct vertex* s = NULL;
	struct vertex* t = NULL;
	struct graph g = induce_graph_from_crawl(G, seed_list, local_seed_list, *k, &s, &t);
	struct graph g_tmp;

	for (uint64_t current_iteration = 0; current_iteration < iterations; current_iteration++) {
		uint64_t c_len;
		uint64_t flow = edmonds_karp(&g, s, t, *k);
		struct vertex** c_list = max_flow_cut(&g, s, t, *k, &c_len);

		uint64_t max_indegree = find_metric_max_value(&g, get_indegree, c_list, c_len, local_seed_list, *k, s, t);
		local_seed_list = extend_list_by_metric_value(&g, s, local_seed_list, k, c_list, c_len, get_indegree, max_indegree, s, t);

		uint64_t max_outdegree = find_metric_max_value(&g, get_outdegree, c_list, c_len, local_seed_list, *k, s, t);
		local_seed_list = extend_list_by_metric_value(&g, s, local_seed_list, k, c_list, c_len, get_outdegree, max_outdegree, s, t);

		// Re-crawl G and reinduce the graph
		local_seed_tmp = malloc(sizeof(struct vertex*) * (size_t)*k);
		if (local_seed_tmp == NULL)
			exit(-1);
		g_tmp = induce_graph_from_crawl(G, local_seed_list, local_seed_tmp, *k, &s, &t);
		delete_graph(&g);
		free(local_seed_list);
		g = g_tmp;
		local_seed_list = local_seed_tmp;
		
	}

	return local_seed_list;
}
