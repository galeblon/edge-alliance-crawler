#include "max_flow.h"

int capacity(vertex* from, edge* e, vertex* source, vertex* target, int64_t k)
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

int ford_fulkerson(graph* g, vertex* source, vertex* target, int64_t k)
{
	return edmonds_karp(g, source, target, k);
}

void vertex_queue_push(struct vertex_queue * v_q, struct vertex_queue_node * v_q_n)
{
	if ((v_q->next == NULL) && (v_q->last == NULL))
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

	if ((v_q->next == NULL) && (v_q->last == NULL)) { }
	else if (v_q->next == v_q->last)
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

int edmonds_karp(struct graph * g, struct vertex * source, struct vertex * target, int64_t k)
{
	int_64_t max_flow = 0;

	struct vertex_queue v_q;

	struct vertex_queue_node * v_q_n_s = malloc(sizeof(struct vertex_queue_node));
	v_q_n_s->v = source;

	vertex_queue_push(&v_q, v_q_n_s);


	struct vertex_queue_node * v_q_n;
	struct vertex_path_node * v_p_n;
	struct edge_ek_decorator * e_e_d;
	struct vertex_ek_decorator * v_e_d;
	struct vertex * v;
	struct edge* e;


	// Add decorators
	v = g->v;

	while (v) {
		v_e_d = malloc(sizeof(vertex_ek_decorator));
		v->decorator = (void *)v_e_d;
		v_e_d->visited = (v == source) ? 1 : 0;
		v_e_d->min_path_cf = 0;
		v_e_d->next = NULL;

		e = v->edge;

		while (e) {
			e_e_d = malloc(sizeof(edge_ek_decorator));
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

		while (v_q.first)
		{
			v_q_n = vertex_queue_pop(&v_q);
			v = v_q_n->v;
			free(v_q_n);

			e = v->edge;
			while (e) {
				if (!e->to->decorator->visited) {
					v_q_n = malloc(sizeof(struct vertex_queue_node));
					v_q_n->v = v;

					e_e_d = (struct edge_ek_decorator*)e->decorator;
					e_cf = capacity(v, e, source, target, k) - e_e_d->f;
					if (e_cf > 0) {
						if (e->to == target)
						{
							found_new_path = 1;
							max_flow += e_cf;

							v_e_d = (struct vertex_ek_decorator*)v->decorator;
							v_p_n = v_e_d->next;
							while (v_p_n)
							{
								e_e_d = (struct edge_ek_decorator*)v_p_n->e->decorator;
								e_e_d->f += e_cf;
								v_p_n = v_p_n->next;
							}
						}

						v_p_n = v_e_d->next;
						v_e_d = (struct vertex_ek_decorator*)e->to->decorator;
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

						vertex_queue_push(&v_q, v_q_n);
					}
				}
				e = e->next;
			}
		}

		//TODO: cleanup, free() all v_p_n
	}
	

	//TODO: cleanup, free() all decorators

	return max_flow;
}
