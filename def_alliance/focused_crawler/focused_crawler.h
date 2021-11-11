#ifndef FOCUSED_CRAWLER_FOCUSED_CRAWLER_H_
#define FOCUSED_CRAWLER_FOCUSED_CRAWLER_H_

#include "../graph/graph.h"
#include <stdlib.h>

void induce_graph_from_crawl(struct graph* g, struct vertex* seed[], int seed_size);
void focused_crawl(struct graph* g, struct vertex* seed_list[], struct vertex* s, struct vertex* t, uint64_t iterations, uint64_t k);
void focused_crawl_continue(struct graph* g, struct vertex* s, struct vertex* t, uint64_t iterations, char* checkpoint_file);
struct graph* max_flow_cut(struct graph* g, struct vertex* source, struct vertex* target);

#endif /* FOCUSED_CRAWLER_FOCUSED_CRAWLER_H_ */
