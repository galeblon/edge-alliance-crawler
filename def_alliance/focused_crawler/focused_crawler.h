#ifndef FOCUSED_CRAWLER_FOCUSED_CRAWLER_H_
#define FOCUSED_CRAWLER_FOCUSED_CRAWLER_H_

#include "../graph/graph.h"
#include <stdlib.h>

struct graph induce_graph_from_crawl(struct graph* g, struct vertex* seed[], int seed_size);
void focused_crawl(struct graph* g, struct vertex* seed_list[], struct vertex* s, struct vertex* t, uint64_t iterations, uint64_t k);

#endif /* FOCUSED_CRAWLER_FOCUSED_CRAWLER_H_ */
