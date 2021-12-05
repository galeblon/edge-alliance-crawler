#include "max_flow.h"

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

int64_t ford_fulkerson(struct graph * g, struct vertex * source, struct vertex * target, int64_t k)
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

int64_t edmonds_karp(struct graph * g, struct vertex * source, struct vertex * target, int64_t k)
{
	int64_t max_flow = 0;

	struct vertex_queue * v_q = create_vertex_queue();

	struct vertex_queue_node * v_q_n_s = malloc(sizeof(struct vertex_queue_node));
	v_q_n_s->v = source;

	vertex_queue_push(v_q, v_q_n_s);


	struct vertex_queue_node * v_q_n;
	struct vertex_path_node * v_p_n;
	struct edge_ek_decorator * e_e_d;
	struct vertex_ek_decorator * v_e_d;
	struct vertex * v;
	struct edge* e;
	int64_t e_cf;


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


	// While there is a path, go
	int found_new_path = 1;
	while (found_new_path)
	{
		found_new_path = 0;

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
					v_q_n->v = v;

					e_e_d = (struct edge_ek_decorator *)e->decorator;
					e_cf = capacity(v, e, source, target, k) - e_e_d->f;
					if (e_cf > 0) {
						if (e->to == target)
						{
							found_new_path = 1;
							max_flow += e_cf;

							v_e_d = (struct vertex_ek_decorator *)v->decorator;
							v_p_n = v_e_d->next;
							while (v_p_n)
							{
								e_e_d = (struct edge_ek_decorator *)v_p_n->e->decorator;
								e_e_d->f += e_cf;
								v_p_n = v_p_n->next;
							}
						}

						v_p_n = v_e_d->next;
						v_e_d = (struct vertex_ek_decorator *)e->to->decorator;
						if (v_e_d->min_path_cf == 0)
						{
							v_e_d->min_path_cf = e_cf;
						}
						else
						{
							v_e_d->min_path_cf = (v_e_d->min_path_cf > e_cf) ? e_cf : v_e_d->min_path_cf;
						}

						v_e_d->next = v_p_n;
						v_p_n = malloc(sizeof(struct vertex_path_node));
						v_p_n->e = e;
						v_p_n->next = v_e_d->next;
						v_e_d->next = v_p_n;

						vertex_queue_push(v_q, v_q_n);
					}
				}
				e = e->next;
			}
		}

		while (v) {
			v_e_d = (struct vertex_ek_decorator *)v->decorator;
			v_e_d->visited = (v == source) ? 1 : 0;
			v_e_d->min_path_cf = 0;
			if (!v_e_d->next)
			{
				free(v_e_d->next);
				v_e_d->next = NULL;
			}

			v = v->next;
		}
	}
	

	// Remember: We leave the edge decorators.
	// They must be free()d later.
	v = g->v;
	while (v) {
		v_e_d = (struct vertex_ek_decorator *)v->decorator;
		if (!v_e_d->next)
		{
			free(v_e_d->next);
			v_e_d->next = NULL;
		}
		free(v_e_d);

		v = v->next;
	}

	free(v_q);

	return max_flow;
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
	}
}
