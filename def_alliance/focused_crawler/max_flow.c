#include "max_flow.h"
#include <stdio.h>

int64_t capacity(struct vertex * from, struct edge * e, struct vertex * source, struct vertex * target, int64_t k)
{
	if (from == source)
	{
		return INT64_MAX; // +inf
	}
	if (e->to == target)
	{
		return 1;
	}
	return k;
}

int64_t ford_fulkerson(struct graph * g, struct vertex_ek * source, struct vertex_ek * target, int64_t k)
{
	return edmonds_karp(g, source, target, k);
}

struct vertex_queue * create_vertex_queue()
{
	struct vertex_queue * v_q = malloc(sizeof(struct vertex_queue));

	v_q->first = NULL;
	v_q->last = NULL;

	return v_q;
}

void vertex_queue_push(struct vertex_queue * v_q, struct vertex_queue_node * v_q_n)
{
	if ((v_q->first == NULL) && (v_q->last == NULL))
	{
		v_q_n->prev = NULL;
		v_q_n->next = NULL;

		v_q->first = v_q_n;
		v_q->last = v_q_n;
	}
	else
	{
		v_q->last->next = v_q_n;

		v_q_n->prev = v_q->last;
		v_q_n->next = NULL;

		v_q->last = v_q_n;
	}
}

//You better remember to free() after pop()ing
struct vertex_queue_node* vertex_queue_pop(struct vertex_queue * v_q)
{
	struct vertex_queue_node* result = v_q->first;

	if ((v_q->first == NULL) && (v_q->last == NULL)) { }
	else if (v_q->first == v_q->last)
	{
		v_q->first = NULL;
		v_q->last = NULL;
	}
	else
	{
		v_q->first->next->prev = NULL;

		v_q->first = v_q->first->next;
	}

	return result;
}

void print_graph_ek(struct graph * g)
{
	struct edge_ek * e;
	struct vertex_ek * v;
	struct vertex_path_node* v_p_n;

	printf("N=%lu\n", g->n);
	v = (struct vertex_ek *)g->v;
	while (v) {
		printf("  V_id=%lu :\n", v->v.id);

		printf("    EK :\n");
		printf("      Vis=%d\n", v->visited);
		printf("      mcf=%lu\n", v->min_path_cf);
		printf("      PATH :\n");
		v_p_n = v->next;
		while (v_p_n) {
			printf("        E=(%lu, %lu) - f:%lu\n", e->from->v.id, v_p_n->e->to->id, e->f);
			v_p_n = v_p_n->next;
		}

		printf("    EDGES :\n");
		e = (struct edge_ek *)v->v.edge;
		while (e) {
			printf("      E_to=%lu\n", e->e.to->id);
			e = (struct edge_ek *)e->e.next;
		}
		printf(";\n");
		v = (struct vertex_ek *)v->v.next;
	}
}

int64_t edmonds_karp(struct graph * g, struct vertex_ek * source, struct vertex_ek * target, int64_t k)
{
	int64_t max_flow = 0;

	struct vertex_queue * v_q = create_vertex_queue();

	struct vertex_queue_node * v_q_n;
	struct vertex_path_node * v_p_n;
	struct vertex_ek * v;
	struct edge_ek * e;
	int64_t e_cf;


	// Initialize vertices
	v = (struct vertex_ek *)g->v;
	while (v) {
		v->visited = (v == source) ? 1 : 0;
		v->min_path_cf = 0;
		v->next = NULL;

		e = (struct edge_ek *)v->v.edge;

		while (e) {
			e->f = 0;
			e = (struct edge_ek *)e->e.next;
		}

		v = (struct vertex_ek *)v->next;
	}

	//print_graph_ek(g);

	// While there is a path, go
	int found_new_path = 1;
	while (found_new_path)
	{
		while (v_q->first)
		{
			free(vertex_queue_pop(v_q));
		}
		found_new_path = 0;
		v_q_n = malloc(sizeof(struct vertex_queue_node));
		v_q_n->v = (struct vertex *)source;
		vertex_queue_push(v_q, v_q_n);

		while (v_q->first)
		{
			v_q_n = vertex_queue_pop(v_q);
			v = (struct vertex_ek *)v_q_n->v;
			free(v_q_n);

			e = (struct edge_ek *)v->v.edge;
			while (e) {
                v = (struct vertex_ek *)e->e.to;
				if (!v->visited) {
					v_q_n = malloc(sizeof(struct vertex_queue_node));
					v_q_n->v = (struct vertex *)v;

					e->from = v;
					e_cf = capacity((struct vertex *)v, (struct edge *)e,
                            (struct vertex *)source, (struct vertex *)target, k) - e->f;
					if (e_cf > 0) {
						v->visited = 1;

						v_p_n = v->next;
						v->next = v_p_n;
						v_p_n = malloc(sizeof(struct vertex_path_node));
						v_p_n->e = (struct edge *)e;
						v_p_n->next = v->next;
						v->next = v_p_n;

						if (e->e.to == (struct vertex *)target)
						{
							found_new_path = 1;
							max_flow += e_cf;

                            v = (struct vertex_ek *)e->e.to;
							v_p_n = v->next;
							while (v_p_n)
							{
                                ((struct edge_ek *)v_p_n->e)->f += e_cf;
								v_p_n = v_p_n->next;
							}

							free(v_q_n);
						}
						else
						{
                            v = (struct vertex_ek *)e->e.to;
							if (v->min_path_cf == 0)
							{
								v->min_path_cf = e_cf;
							}
							else
							{
								v->min_path_cf = (v->min_path_cf > e_cf) ? e_cf : v->min_path_cf;
							}

							vertex_queue_push(v_q, v_q_n);
						}
					}
					else
					{
						free(v_q_n);
					}
				}

				if (found_new_path)
				{
					break;
				}

				e = (struct edge_ek *)e->e.next;
			}

			if (found_new_path)
			{
				break;
			}
		}

		//print_graph_ek(g);

		v = (struct vertex_ek *)g->v;
		while (v) {
			v->visited = (v == source) ? 1 : 0;
			v->min_path_cf = 0;
			if (v->next)
			{
				free(v->next);
				v->next = NULL;
			}

			v = (struct vertex_ek *)v->next;
		}
	}


	v = (struct vertex_ek *)g->v;
	while (v) {
		if (v->next)
		{
			free(v->next);
			v->next = NULL;
		}

		v = (struct vertex_ek *)v->next;
	}

	free(v_q);

	return max_flow;
}
