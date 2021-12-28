#ifndef FOCUSED_CRAWLER_FOCUSED_CRAWLER_H_
#define FOCUSED_CRAWLER_FOCUSED_CRAWLER_H_

#include "../graph/graph.h"
#include <stdlib.h>

struct graph induce_graph_from_crawl(struct graph* G, struct vertex* seed[], struct vertex* local_seed[], uint64_t seed_size, struct vertex** s, struct vertex** t);
struct vertex** focused_crawl(struct graph* g, struct vertex* seed_list[], uint64_t iterations, uint64_t* k);
struct vertex** max_flow_cut(struct graph* g, struct vertex* source, struct vertex* target, uint64_t k, uint64_t* c_size);

#endif /* FOCUSED_CRAWLER_FOCUSED_CRAWLER_H_ */
