#ifndef FOCUSED_CRAWLER_FOCUSED_CRAWLER_H_
#define FOCUSED_CRAWLER_FOCUSED_CRAWLER_H_

#include "../graph/graph.h"
#include <stdlib.h>

struct graph induce_graph_from_crawl(struct graph* G, struct vertex* seed[], struct vertex* local_seed[], uint64_t seed_size);
void focused_crawl(struct graph* g, struct vertex* seed_list[], struct vertex* s, struct vertex* t, uint64_t iterations, uint64_t k);
void focused_crawl_continue(struct graph* g, struct vertex* s, struct vertex* t, uint64_t iterations, char* checkpoint_file);
struct vertex** max_flow_cut(struct graph* g, struct vertex* source, struct vertex* target, uint64_t k, uint64_t* c_size);

#endif /* FOCUSED_CRAWLER_FOCUSED_CRAWLER_H_ */
