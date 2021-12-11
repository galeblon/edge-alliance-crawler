#include "max_flow.h"

int64_t capacity(struct vertex * from, struct edge * e, struct vertex * source, struct vertex * target, uint64_t k)
{
	if (from == source)
	{
		return UINT64_MAX; // +inf
	}
	if (e->to == target)
	{
		return 1;
	}
	return k;
}

int64_t ford_fulkerson(struct graph * g, struct vertex * source, struct vertex * target, uint64_t k)
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
	struct edge* e;
	struct vertex* v;
	struct edge_ek_decorator* e_e_d;
	struct vertex_ek_decorator* v_e_d;
	struct vertex_path_node* v_p_n;

	printf("N=%lu\n", g->n);
	v = g->v;
	while (v) {
		printf("  V_id=%lu :\n", v->id);

		printf("    EK :\n");
		v_e_d = (struct vertex_ek_decorator*)v->decorator;
		printf("      Vis=%lu\n", v_e_d->visited);
		printf("      mcf=%lu\n", v_e_d->min_path_cf);
		printf("      PATH :\n");
		v_p_n = v_e_d->next;
		while (v_p_n) {
			e_e_d = (struct edge_ek_decorator *)v_p_n->e->decorator;
			printf("        E=(%lu, %lu) - f:%lu\n", e_e_d->from->id, v_p_n->e->to->id, e_e_d->f);
			v_p_n = v_p_n->next;
		}

		printf("    EDGES :\n");
		e = v->edge;
		while (e) {
			printf("      E_to=%lu\n", e->to->id);
			e = e->next;
		}
		printf(";\n");
		v = v->next;
	}
}

int64_t edmonds_karp(struct graph * g, struct vertex * source, struct vertex * target, uint64_t k)
{
	int64_t max_flow = 0;

	struct vertex_queue * v_q = create_vertex_queue();

	struct vertex_queue_node * v_q_n;
	struct vertex_path_node * v_p_n;
	struct edge_ek_decorator * e_e_d;
	struct vertex_ek_decorator * v_e_d;
	struct vertex * v;
	struct edge* e;
	uint64_t e_cf;


	// Add decorators
	v = g->v;
	while (v) {
		v_e_d = malloc(sizeof(struct vertex_ek_decorator));
		v->decorator = (void *)v_e_d;
		v_e_d->visited = (v == source) ? 1 : 0;
		v_e_d->min_path_cf = 0;
		v_e_d->next = NULL;

		e = v->edge;

		while (e) {
			e_e_d = malloc(sizeof(struct edge_ek_decorator));
			e->decorator = (void *)e_e_d;
			e_e_d->f = 0;

			e = e->next;
		}

		v = v->next;
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
		v_q_n->v = source;
		vertex_queue_push(v_q, v_q_n);

		while (v_q->first)
		{
			v_q_n = vertex_queue_pop(v_q);
			v = v_q_n->v;
			free(v_q_n);

			e = v->edge;
			while (e) {
				v_e_d = (struct vertex_ek_decorator*)e->to->decorator;
				if (!v_e_d->visited) {
					v_q_n = malloc(sizeof(struct vertex_queue_node));
					v_q_n->v = e->to;

					e_e_d = (struct edge_ek_decorator *)e->decorator;
					e_e_d->from = v;
					e_cf = capacity(v, e, source, target, k) - e_e_d->f;
					if (e_cf > 0) {
						v_e_d->visited = 1;

						v_e_d = (struct vertex_ek_decorator*)v->decorator;
						v_p_n = v_e_d->next;
						v_e_d = (struct vertex_ek_decorator*)e->to->decorator;
						v_e_d->next = v_p_n;
						v_p_n = malloc(sizeof(struct vertex_path_node));
						v_p_n->e = e;
						v_p_n->next = v_e_d->next;
						v_e_d->next = v_p_n;

						if (e->to == target)
						{
							found_new_path = 1;
							max_flow += e_cf;

							v_e_d = (struct vertex_ek_decorator *)e->to->decorator;
							v_p_n = v_e_d->next;
							while (v_p_n)
							{
								e_e_d = (struct edge_ek_decorator *)v_p_n->e->decorator;
								e_e_d->f += e_cf;
								v_p_n = v_p_n->next;
							}

							free(v_q_n);
						}
						else
						{
							v_e_d = (struct vertex_ek_decorator*)e->to->decorator;
							if (v_e_d->min_path_cf == 0)
							{
								v_e_d->min_path_cf = e_cf;
							}
							else
							{
								v_e_d->min_path_cf = (v_e_d->min_path_cf > e_cf) ? e_cf : v_e_d->min_path_cf;
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

				e = e->next;
			}

			if (found_new_path)
			{
				break;
			}
		}

		//print_graph_ek(g);

		v = g->v;
		while (v) {
			v_e_d = (struct vertex_ek_decorator *)v->decorator;
			v_e_d->visited = (v == source) ? 1 : 0;
			v_e_d->min_path_cf = 0;
			if (v_e_d->next)
			{
				free(v_e_d->next);
				v_e_d->next = NULL;
			}

			v = v->next;
		}
	}
	
	// Remember: We leave the decorators.
	// They must be free()d later.
	free(v_q);

	return max_flow;
}

struct vertex * * source_component_min_cut_after_max_flow(struct graph * g, struct vertex * source, struct vertex* target, uint64_t k, uint64_t * source_component_size)
{
	struct vertex_queue* v_q = create_vertex_queue();

	uint64_t index = 0, size = 1;
	struct vertex** result = malloc(size * sizeof(struct vertex*));
	struct edge * e;
	struct vertex * v;
	struct vertex_ek_decorator* v_e_d;
	struct edge_ek_decorator* e_e_d;
	struct vertex_queue_node* v_q_n = malloc(sizeof(struct vertex_queue_node));
	v_q_n->v = source;
	vertex_queue_push(v_q, v_q_n);

	// Set up the decorators for BFS
	v = g->v;
	while (v) {
		v_e_d = (struct vertex_ek_decorator*)v->decorator;
		v_e_d->visited = (v == source) ? 1 : 0;
		v = v->next;
	}

	while (v_q->first)
	{
		v_q_n = vertex_queue_pop(v_q);
		v = v_q_n->v;
		free(v_q_n);

		if (index >= size)
		{
			size *= 2;
			result = realloc(result, size * sizeof(struct vertex*));
		}
		result[index] = v;
		index++;

		e = v->edge;
		while (e) {
			v_e_d = (struct vertex_ek_decorator*)e->to->decorator;
			e_e_d = (struct edge_ek_decorator*)e->decorator;
			if (
				!v_e_d->visited									// BFS condition
				&& e_e_d->f < capacity(v, e, source, target, k) // max flow -> min cut condition
				&& e->to != target								// s-t cut condition
			)
			{
				v_e_d->visited = 1;
				v_q_n = malloc(sizeof(struct vertex_queue_node));
				v_q_n->v = e->to;
				vertex_queue_push(v_q, v_q_n);
			}

			e = e->next;
		}
	}

	result = realloc(result, index * sizeof(struct vertex*));
	*source_component_size = index;
	return result;
}

void free_vertex_ek_decorators(struct graph* g)
{
	struct edge* e;
	struct vertex* v;
	struct vertex_ek_decorator * v_e_d;

	v = g->v;
	while (v) {
		v_e_d = (struct vertex_ek_decorator*)v->decorator;
		if (v_e_d->next)
		{
			free(v_e_d->next);
			v_e_d->next = NULL;
		}
		free(v->decorator);

		v = v->next;
	}
}

void free_edge_ek_decorators(struct graph * g)
{
	struct edge* e;
	struct vertex* v_checked;

	v_checked = g->v;
	while (v_checked) {
		e = v_checked->edge;
		while (e) {
			free(e->decorator);
			e->decorator = NULL;

			e = e->next;
		}
		v_checked = v_checked->next;
	}
}
