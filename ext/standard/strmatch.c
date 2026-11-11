/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
  +----------------------------------------------------------------------+
  | Author: Artem Ukrainskiy <art.ukrainskiy@gmail.com>                    |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "php.h"
#include <limits.h>
#include <stddef.h>
#include "basic_functions.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_smart_str.h"
#include "strmatch_arginfo.h"

#define AC_MAX_STATES ((INT_MAX < INT32_MAX) ? INT_MAX : INT32_MAX)
#define AC_MATCH_FLAG UINT32_C(0x80000000)
#define AC_STATE_MASK UINT32_C(0x7fffffff)
#define AC_INITIAL_CAPACITY 16
#define AC_PRESIZE_LIMIT 4096
#define AC_DENSE_DEPTH 3
#define AC_DENSE_MIN_CHILDREN 8
#define AC_DENSE_FAILURE_WINDOW 4
#define AC_INITIAL_CANDIDATE_LIMIT 256
#define AC_SAMPLE_BYTES 256
#define AC_RUNS_SWITCH_PERCENT 12

typedef struct {
	int fail;
	int keyword;
	int output_link;

	union {
		uint32_t count;
		int longest;
	} output;
	uint32_t link;
	uint16_t child_count;
	uint16_t column;
} ac_build_state;

typedef struct {
	int keyword;
	int output_link;
	uint32_t output_count;
} ac_state;

typedef struct {
	int fail;
	uint32_t link;
	uint16_t count;
	uint16_t column;
} ac_sparse_state;

typedef struct {
	int target;
	uint32_t next;
	uint16_t column;
} ac_build_edge;

typedef struct {
	uint32_t target;
	uint16_t column;
} ac_edge;

typedef struct {
	uint32_t *transitions;
	ac_edge *edges;
	uint32_t dense_count;
	uint32_t dense_limit;
	uint32_t stride;
	ac_build_state *build_states;
	int *child_pool;
	uint32_t child_pool_used;
	uint32_t child_pool_size;
	ac_state *states;
	ac_sparse_state *sparse;
	int *longest;
	int state_count;
	int capacity;
	int alphabet_size;
	uint16_t alphabet[256];
	zend_string **keywords;
	zend_string **replacements;
	uint32_t keyword_count;
	bool initialized;
} ac_automaton;

typedef struct {
	ac_automaton automaton;

	zend_object std;
} matcher_object;

typedef struct {
	size_t offset;
	size_t length;
	int keyword;
} ac_hit;

typedef struct {
	size_t offset;
	int keyword;
} ac_candidate;

static zend_class_entry *str_matcher_ce;
static zend_class_entry *str_replacer_ce;
static zend_class_entry *str_matchresult_ce;

static zend_object_handlers matcher_handlers;
static uint32_t result_offset, result_length, result_keyword;

static inline matcher_object *matcher_from_object(zend_object *object)
{
	return (matcher_object *) ((char *) object - offsetof(matcher_object, std));
}

/* Consume AC_MATCH_FLAG before reusing an output target as a state ID. */
static zend_always_inline uint32_t ac_transition(const ac_automaton *automaton, uint32_t state, unsigned char byte,
		const bool runs)
{
	unsigned int column = automaton->alphabet[byte];

	if (runs && column == 0) {
		return 0;
	}

	if (EXPECTED(state < automaton->dense_limit)) {
		return automaton->transitions[(size_t) state + column];
	}
	if (column == 0) {
		return 0;
	}
	for (;;) {
		if (state < automaton->dense_limit) {
			return automaton->transitions[(size_t) state + column];
		}
		const ac_sparse_state *meta = &automaton->sparse[state - automaton->dense_limit];
		unsigned int count = meta->count;

		if (count == 1) {
			if (column == meta->column) {
				return meta->link;
			}
		} else if (count > 1) {
			const ac_edge *edges = automaton->edges + meta->link;

			for (unsigned int i = 0; i < count; i++) {
				if (edges[i].column == column) {
					return edges[i].target;
				}
			}
		}
		state = (uint32_t) meta->fail;
	}
}

/* True when sampled bytes rarely switch between key-alphabet bytes and other bytes. */
static bool ac_prefer_runs(const ac_automaton *automaton, const zend_string *subject)
{
	size_t length = MIN(ZSTR_LEN(subject), AC_SAMPLE_BYTES);
	const unsigned char *bytes = (const unsigned char *) ZSTR_VAL(subject);
	size_t switches = 0;

	for (size_t i = 1; i < length; i++) {
		switches += (automaton->alphabet[bytes[i]] == 0) != (automaton->alphabet[bytes[i - 1]] == 0);
	}
	return switches * 100 < length * AC_RUNS_SWITCH_PERCENT;
}

static zend_always_inline size_t ac_next_match_impl(const ac_automaton *automaton, const unsigned char *bytes,
		size_t i, size_t length, uint32_t *state, const bool runs)
{
	uint32_t current = *state;

	for (; i < length; i++) {
		current = ac_transition(automaton, current, bytes[i], runs);
		if (UNEXPECTED(current & AC_MATCH_FLAG)) {
			break;
		}
	}
	*state = current;
	return i;
}

/* Scans bytes[i, length) and returns the offset of the first byte that ends a match, or length. */
static zend_always_inline size_t ac_next_match(const ac_automaton *automaton, const unsigned char *bytes,
		size_t i, size_t length, uint32_t *state, bool runs)
{
	return runs ? ac_next_match_impl(automaton, bytes, i, length, state, true)
		: ac_next_match_impl(automaton, bytes, i, length, state, false);
}

static zend_always_inline size_t ac_previous_match_impl(const ac_automaton *automaton, const unsigned char *bytes,
		size_t i, uint32_t *state, const bool runs)
{
	uint32_t current = *state;

	while (i > 0) {
		i--;
		current = ac_transition(automaton, current, bytes[i], runs);
		if (UNEXPECTED(current & AC_MATCH_FLAG)) {
			*state = current;
			return i;
		}
	}
	*state = current;
	return SIZE_MAX;
}

/* Scans bytes[0, i) backwards and returns the offset of the first byte that ends a match, or SIZE_MAX. */
static zend_always_inline size_t ac_previous_match(const ac_automaton *automaton, const unsigned char *bytes,
		size_t i, uint32_t *state, bool runs)
{
	return runs ? ac_previous_match_impl(automaton, bytes, i, state, true)
		: ac_previous_match_impl(automaton, bytes, i, state, false);
}

/* Converts an encoded state to the index of its output metadata. */
static zend_always_inline uint32_t ac_state_index(const ac_automaton *automaton, uint32_t state)
{
	return state < automaton->dense_limit ? state / automaton->stride
		: state - automaton->dense_limit + automaton->dense_count;
}

static int ac_add_state(ac_automaton *automaton)
{
	if (UNEXPECTED(automaton->state_count == AC_MAX_STATES)) {
		zend_throw_error(NULL, "Dictionary requires too many matching states");
		return -1;
	}
	if (automaton->state_count == automaton->capacity) {
		automaton->capacity = automaton->capacity == 0 ? AC_INITIAL_CAPACITY
			: (automaton->capacity > AC_MAX_STATES / 2 ? AC_MAX_STATES : automaton->capacity * 2);
		automaton->build_states = safe_erealloc(
			automaton->build_states, automaton->capacity, sizeof(ac_build_state), 0);
	}

	int state = automaton->state_count++;

	automaton->build_states[state].fail = 0;
	automaton->build_states[state].keyword = -1;
	automaton->build_states[state].output_link = -1;
	automaton->build_states[state].output.count = 0;
	return state;
}

static int ac_build_child(const ac_automaton *automaton, const ac_build_edge *edges, int state, int column)
{
	const ac_build_state *meta = &automaton->build_states[state];

	/* During the build, a nonzero column is the 1-based row of a direct child index. */
	if (meta->column) {
		return automaton->child_pool[(size_t) (meta->column - 1) * automaton->alphabet_size + column];
	}
	for (uint32_t edge = meta->link; edge != UINT32_MAX; edge = edges[edge].next) {
		if (edges[edge].column == column) {
			return edges[edge].target;
		}
	}
	return -1;
}

static int ac_build_step(const ac_automaton *automaton, const ac_build_edge *edges, int state, int column)
{
	int target;

	while ((target = ac_build_child(automaton, edges, state, column)) < 0 && state != 0) {
		state = automaton->build_states[state].fail;
	}
	return target < 0 ? 0 : target;
}

static int ac_add_build_state(ac_automaton *automaton)
{
	int state = ac_add_state(automaton);

	if (state >= 0) {
		automaton->build_states[state].link = UINT32_MAX;
		automaton->build_states[state].column = 0;
		automaton->build_states[state].child_count = 0;
	}
	return state;
}

static bool ac_use_dense_state(const ac_automaton *automaton, const int *depth, int state)
{
	return depth[state] <= AC_DENSE_DEPTH || automaton->build_states[state].child_count >= AC_DENSE_MIN_CHILDREN
		|| (depth[automaton->build_states[state].fail] > AC_DENSE_DEPTH
			&& depth[state] - depth[automaton->build_states[state].fail] <= AC_DENSE_FAILURE_WINDOW);
}

/* Build-only direct child index for one state: one pool slot per column, -1 for no child. */
static void ac_index_children(ac_automaton *automaton, const ac_build_edge *edges, int state)
{
	uint32_t width = (uint32_t) automaton->alphabet_size;

	if (automaton->child_pool_used / width >= UINT16_MAX) {
		return;
	}
	if (automaton->child_pool_size - automaton->child_pool_used < width) {
		automaton->child_pool_size = MAX(automaton->child_pool_size * 2, automaton->child_pool_used + width);
		automaton->child_pool = safe_erealloc(automaton->child_pool, automaton->child_pool_size, sizeof(int), 0);
	}
	uint32_t row = automaton->child_pool_used;

	automaton->child_pool_used += width;
	for (uint32_t column = 0; column < width; column++) {
		automaton->child_pool[row + column] = -1;
	}
	for (uint32_t e = automaton->build_states[state].link; e != UINT32_MAX; e = edges[e].next) {
		automaton->child_pool[row + edges[e].column] = edges[e].target;
	}
	automaton->build_states[state].column = (uint16_t) (row / width + 1);
}

static zend_result ac_build_trie(ac_automaton *automaton, bool reversed, ac_build_edge **edges_out)
{
	int alphabet = 0;

	for (uint32_t k = 0; k < automaton->keyword_count; k++) {
		zend_string *word = automaton->keywords[k];

		for (size_t i = 0; i < ZSTR_LEN(word); i++) {
			unsigned char byte = (unsigned char) ZSTR_VAL(word)[i];
			if (automaton->alphabet[byte] == 0) {
				automaton->alphabet[byte] = (uint16_t) ++alphabet;
			}
		}
	}
	automaton->alphabet_size = alphabet + 1;

	/* Every key byte adds at most one state and one edge; larger builds grow by doubling. */
	size_t total = 0;

	for (uint32_t k = 0; k < automaton->keyword_count; k++) {
		total += ZSTR_LEN(automaton->keywords[k]);
	}
	automaton->capacity = (int) MIN(total + 1, (size_t) AC_PRESIZE_LIMIT);
	automaton->build_states = safe_emalloc(automaton->capacity, sizeof(ac_build_state), 0);
	if (ac_add_build_state(automaton) < 0) {
		return FAILURE;
	}
	if (reversed) {
		automaton->build_states[0].output.longest = -1;
	}
	uint32_t used = 0, capacity = (uint32_t) MIN(total, (size_t) AC_PRESIZE_LIMIT);
	ac_build_edge *build_edges = safe_emalloc(capacity, sizeof(ac_build_edge), 0);

	ac_index_children(automaton, build_edges, 0);

	for (uint32_t k = 0; k < automaton->keyword_count; k++) {
		zend_string *word = automaton->keywords[k];
		int state = 0;

		for (size_t i = 0; i < ZSTR_LEN(word); i++) {
			size_t offset = reversed ? ZSTR_LEN(word) - i - 1 : i;
			int column = automaton->alphabet[(unsigned char) ZSTR_VAL(word)[offset]];
			int target = ac_build_child(automaton, build_edges, state, column);

			if (target < 0) {
				target = ac_add_build_state(automaton);
				if (target < 0) {
					if (build_edges) {
						efree(build_edges);
					}
					return FAILURE;
				}
				if (used == capacity) {
					capacity = capacity == 0 ? AC_INITIAL_CAPACITY
						: (capacity > AC_MAX_STATES / 2 ? AC_MAX_STATES : capacity * 2);
					build_edges = safe_erealloc(build_edges, capacity, sizeof(ac_build_edge), 0);
				}
				build_edges[used].column = (uint16_t) column;
				build_edges[used].target = target;
				build_edges[used].next = automaton->build_states[state].link;
				automaton->build_states[state].link = used++;
				automaton->build_states[state].child_count++;
				uint16_t row = automaton->build_states[state].column;

				if (row) {
					automaton->child_pool[(size_t) (row - 1) * automaton->alphabet_size + column] = target;
				} else if (automaton->build_states[state].child_count == AC_DENSE_MIN_CHILDREN) {
					ac_index_children(automaton, build_edges, state);
				}
			}
			state = target;
		}
		automaton->build_states[state].keyword = (int) k;
	}
	automaton->build_states = safe_erealloc(automaton->build_states, automaton->state_count, sizeof(ac_build_state), 0);
	build_edges = safe_erealloc(build_edges, used, sizeof(ac_build_edge), 0);
	automaton->capacity = automaton->state_count;
	*edges_out = build_edges;
	return SUCCESS;
}

static void ac_build_failure_links(ac_automaton *automaton, const ac_build_edge *build_edges,
		int *queue, int *depth, bool reversed)
{
	int head = 0, tail = 0;

	for (uint32_t e = automaton->build_states[0].link; e != UINT32_MAX; e = build_edges[e].next) {
		int child = build_edges[e].target;

		queue[tail++] = child;
		depth[child] = 1;
	}
	while (head < tail) {
		int state = queue[head++];
		ac_build_state *meta = &automaton->build_states[state];
		int fail = meta->fail;

		meta->output_link = automaton->build_states[fail].keyword >= 0
			? fail : automaton->build_states[fail].output_link;
		if (reversed) {
			int longest = meta->keyword;
			int inherited = automaton->build_states[fail].output.longest;

			if (inherited >= 0 && (longest < 0
					|| ZSTR_LEN(automaton->keywords[inherited]) > ZSTR_LEN(automaton->keywords[longest]))) {
				longest = inherited;
			}
			meta->output.longest = longest;
		} else {
			meta->output.count = (meta->keyword >= 0 ? 1u : 0u) + automaton->build_states[fail].output.count;
		}
		for (uint32_t e = meta->link; e != UINT32_MAX; e = build_edges[e].next) {
			int child = build_edges[e].target;

			automaton->build_states[child].fail = ac_build_step(automaton, build_edges, fail, build_edges[e].column);
			depth[child] = depth[state] + 1;
			queue[tail++] = child;
		}
	}
	ZEND_ASSERT(tail == automaton->state_count - 1);
}

static int ac_map_states(const ac_automaton *automaton, const int *depth, int *state_map,
		size_t *edge_count)
{
	int dense_count = 0;
	*edge_count = 0;

	for (int state = 0; state < automaton->state_count; state++) {
		if (ac_use_dense_state(automaton, depth, state)) {
			state_map[state] = dense_count++;
		} else {
			state_map[state] = -1;
			if (automaton->build_states[state].child_count > 1) {
				*edge_count += automaton->build_states[state].child_count;
			}
		}
	}
	int next = dense_count;

	for (int state = 0; state < automaton->state_count; state++) {
		if (state_map[state] < 0) {
			state_map[state] = next++;
		}
	}

	return dense_count;
}

/* Cache keys use mapped sparse IDs minus dense_count; values are mapped targets. */
static int ac_resolve_transition(const ac_automaton *automaton, const ac_build_edge *build_edges,
		const int *state_map, int dense_count, int *cache, int state, int column)
{
	int first = state;
	int target;

	for (;;) {
		int mapped_state = state_map[state];

		if (mapped_state < dense_count) {
			target = automaton->transitions[(size_t) mapped_state * automaton->alphabet_size + column];
			break;
		}
		if (cache[mapped_state - dense_count] >= 0) {
			target = cache[mapped_state - dense_count];
			break;
		}
		int child = ac_build_child(automaton, build_edges, state, column);

		if (child >= 0) {
			target = state_map[child];
			cache[mapped_state - dense_count] = target;
			break;
		}
		state = automaton->build_states[state].fail;
	}
	while (first != state) {
		ZEND_ASSERT(state_map[first] >= dense_count);
		cache[state_map[first] - dense_count] = target;
		first = automaton->build_states[first].fail;
	}
	return target;
}

/* The current column is completed in BFS order before the cache is reused. */
static void ac_compile_dense(const ac_automaton *automaton, const ac_build_edge *build_edges,
		int *queue, const int *state_map, int dense_count, int *cache)
{
	int state_count = automaton->state_count;
	int dense_states = 0;

	/* Filtering preserves the BFS order of all dense rows. */
	for (int position = 0; position < state_count - 1; position++) {
		int state = queue[position];

		int mapped_state = state_map[state];

		if (mapped_state < dense_count) {
			queue[dense_states++] = state;
		}
	}
	ZEND_ASSERT(dense_states == dense_count - 1);

	/* Children first; UINT32_MAX marks a transition resolved through failure links below. */
	for (int position = -1; position < dense_states; position++) {
		int old = position < 0 ? 0 : queue[position];
		uint32_t *row = automaton->transitions + (size_t) state_map[old] * automaton->alphabet_size;

		row[0] = 0;
		for (int column = 1; column < automaton->alphabet_size; column++) {
			row[column] = old == 0 ? 0 : UINT32_MAX;
		}
		for (uint32_t e = automaton->build_states[old].link; e != UINT32_MAX; e = build_edges[e].next) {
			row[build_edges[e].column] = (uint32_t) state_map[build_edges[e].target];
		}
	}
	/* A row is complete when its failure row is complete; failure rows precede their states in BFS order. */
	bool *complete = ecalloc(dense_count, sizeof(bool));
	int pending = 0;

	complete[0] = true;
	for (int position = 0; position < dense_states; position++) {
		int old = queue[position];
		int fail = state_map[automaton->build_states[old].fail];

		if (fail >= dense_count || !complete[fail]) {
			queue[pending++] = old;
			continue;
		}
		uint32_t *row = automaton->transitions + (size_t) state_map[old] * automaton->alphabet_size;
		const uint32_t *fail_row = automaton->transitions + (size_t) fail * automaton->alphabet_size;

		for (int column = 1; column < automaton->alphabet_size; column++) {
			row[column] = row[column] == UINT32_MAX ? fail_row[column] : row[column];
		}
		complete[state_map[old]] = true;
	}
	efree(complete);

	/* Remaining rows: each column is completed in BFS order before the cache is reused. */
	for (int column = 1; pending && column < automaton->alphabet_size; column++) {
		for (int state = 0; state < state_count - dense_count; state++) {
			cache[state] = -1;
		}
		for (int position = 0; position < pending; position++) {
			int old = queue[position];
			uint32_t *cell = automaton->transitions + (size_t) state_map[old] * automaton->alphabet_size + column;

			if (*cell == UINT32_MAX) {
				*cell = (uint32_t) ac_resolve_transition(automaton, build_edges, state_map,
					dense_count, cache, automaton->build_states[old].fail, column);
			}
		}
	}
}

static void ac_pack_edges(ac_automaton *automaton, const ac_build_edge *build_edges,
		const int *state_map, int dense_count, ac_edge *edges, size_t edge_count)
{
	size_t edge = 0;

	for (int old = 0; old < automaton->state_count; old++) {
		int state = state_map[old];
		ac_build_state *meta = &automaton->build_states[old];

		if (state >= dense_count) {
			uint32_t first = meta->link;

			meta->link = (uint32_t) edge;
			for (uint32_t e = first; e != UINT32_MAX; e = build_edges[e].next) {
				if (meta->child_count == 1) {
					meta->column = build_edges[e].column;
					meta->link = (uint32_t) state_map[build_edges[e].target];
				} else {
					edges[edge].column = build_edges[e].column;
					edges[edge++].target = state_map[build_edges[e].target];
				}
			}
		}
		meta->fail = state_map[meta->fail];
		if (meta->output_link >= 0) {
			meta->output_link = state_map[meta->output_link];
		}
	}
	ZEND_ASSERT(edge == edge_count);
}

/* Copies build states to their mapped positions: dense states first, then sparse states. */
static void ac_pack_states(ac_automaton *automaton, const int *state_map, int dense_count, bool reversed)
{
	int sparse_count = automaton->state_count - dense_count;

	automaton->sparse = sparse_count ? safe_emalloc(sparse_count, sizeof(ac_sparse_state), 0) : NULL;
	if (reversed) {
		automaton->longest = safe_emalloc(automaton->state_count, sizeof(int), 0);
	} else {
		automaton->states = safe_emalloc(automaton->state_count, sizeof(ac_state), 0);
	}
	for (int old = 0; old < automaton->state_count; old++) {
		const ac_build_state *meta = &automaton->build_states[old];
		int state = state_map[old];

		if (state >= dense_count) {
			ac_sparse_state *info = &automaton->sparse[state - dense_count];

			info->fail = meta->fail;
			info->link = meta->link;
			info->count = meta->child_count;
			info->column = meta->column;
		}
		if (reversed) {
			automaton->longest[state] = meta->output.longest;
		} else {
			automaton->states[state].keyword = meta->keyword;
			automaton->states[state].output_link = meta->output_link;
			automaton->states[state].output_count = meta->output.count;
		}
	}
	efree(automaton->build_states);
	automaton->build_states = NULL;
	automaton->dense_count = (uint32_t) dense_count;
}

static zend_always_inline uint32_t ac_encode_state(const ac_automaton *automaton, uint32_t state)
{
	return state < automaton->dense_count ? state * automaton->stride
		: automaton->dense_limit + (state - automaton->dense_count);
}

/* Stored targets become dense row offsets, or dense_limit + sparse index, with AC_MATCH_FLAG on
 * targets that can report a match. */
static zend_result ac_encode_targets(ac_automaton *automaton, size_t edge_count, bool reversed, uint32_t *encoded)
{
	uint32_t state_count = (uint32_t) automaton->state_count;
	uint32_t sparse_count = state_count - automaton->dense_count;

	if (UNEXPECTED((uint64_t) automaton->dense_count * (uint32_t) automaton->alphabet_size + sparse_count
			> AC_STATE_MASK)) {
		zend_throw_error(NULL, "Dictionary requires too many matching states");
		return FAILURE;
	}
	automaton->stride = (uint32_t) automaton->alphabet_size;
	automaton->dense_limit = automaton->dense_count * automaton->stride;

	for (uint32_t state = 0; state < state_count; state++) {
		bool output = reversed ? automaton->longest[state] >= 0 : automaton->states[state].output_count != 0;

		encoded[state] = ac_encode_state(automaton, state) | (output ? AC_MATCH_FLAG : 0);
	}
	for (size_t i = 0; i < (size_t) automaton->dense_limit; i++) {
		automaton->transitions[i] = encoded[automaton->transitions[i]];
	}
	for (size_t i = 0; i < edge_count; i++) {
		automaton->edges[i].target = encoded[automaton->edges[i].target];
	}
	for (uint32_t i = 0; i < sparse_count; i++) {
		ac_sparse_state *meta = &automaton->sparse[i];

		if (meta->count == 1) {
			meta->link = encoded[meta->link];
		}
		meta->fail = (int) (encoded[meta->fail] & AC_STATE_MASK);
	}
	return SUCCESS;
}

static zend_result ac_build(ac_automaton *automaton, bool reversed)
{
	if (automaton->keyword_count == 0) {
		return SUCCESS;
	}

	ac_build_edge *build_edges = NULL;

	if (ac_build_trie(automaton, reversed, &build_edges) == FAILURE) {
		return FAILURE;
	}

	int *queue = safe_emalloc(automaton->state_count, sizeof(int), 0);
	int *depth = ecalloc(automaton->state_count, sizeof(int));
	int *state_map = safe_emalloc(automaton->state_count, sizeof(int), 0);
	size_t edge_count;

	ac_build_failure_links(automaton, build_edges, queue, depth, reversed);
	int dense_count = ac_map_states(automaton, depth, state_map, &edge_count);

	automaton->transitions = safe_emalloc(dense_count, automaton->alphabet_size * sizeof(uint32_t), 0);
	automaton->edges = edge_count ? safe_emalloc(edge_count, sizeof(ac_edge), 0) : NULL;
	/* Column caching reuses the completed depth workspace. */
	ac_compile_dense(automaton, build_edges, queue, state_map, dense_count, depth);
	efree(queue);
	efree(depth);

	efree(automaton->child_pool);
	automaton->child_pool = NULL;
	ac_pack_edges(automaton, build_edges, state_map, dense_count, automaton->edges, edge_count);
	efree(build_edges);
	ac_pack_states(automaton, state_map, dense_count, reversed);

	/* The state map is no longer needed; its slots hold the encoded targets. */
	zend_result result = ac_encode_targets(automaton, edge_count, reversed, (uint32_t *) state_map);

	efree(state_map);
	return result;
}

static void ac_destroy(ac_automaton *automaton)
{
	if (automaton->child_pool) {
		efree(automaton->child_pool);
	}
	for (uint32_t i = 0; i < automaton->keyword_count; i++) {
		zend_string_release(automaton->keywords[i]);
		if (automaton->replacements) {
			zend_string_release(automaton->replacements[i]);
		}
	}
	if (automaton->keywords) {
		efree(automaton->keywords);
	}
	if (automaton->replacements) {
		efree(automaton->replacements);
	}
	if (automaton->transitions) {
		efree(automaton->transitions);
	}
	if (automaton->edges) {
		efree(automaton->edges);
	}
	if (automaton->build_states) {
		efree(automaton->build_states);
	}
	if (automaton->sparse) {
		efree(automaton->sparse);
	}
	if (automaton->longest) {
		efree(automaton->longest);
	}
	if (automaton->states) {
		efree(automaton->states);
	}
}

static zend_object *matcher_create(zend_class_entry *ce)
{
	matcher_object *object = zend_object_alloc(sizeof(matcher_object), ce);
	zend_object_std_init(&object->std, ce);
	object_properties_init(&object->std, ce);
	return &object->std;
}

static void matcher_free(zend_object *object)
{
	ac_destroy(&matcher_from_object(object)->automaton);
	zend_object_std_dtor(object);
}

static int matcher_compare(zval *left, zval *right)
{
	ZEND_COMPARE_OBJECTS_FALLBACK(left, right);

	if (Z_OBJ_P(left) == Z_OBJ_P(right)) {
		return 0;
	}
	if (Z_OBJCE_P(left) != Z_OBJCE_P(right)) {
		return ZEND_UNCOMPARABLE;
	}

	const ac_automaton *a = &matcher_from_object(Z_OBJ_P(left))->automaton;
	const ac_automaton *b = &matcher_from_object(Z_OBJ_P(right))->automaton;
	if (!a->initialized || !b->initialized || a->keyword_count != b->keyword_count) {
		return ZEND_UNCOMPARABLE;
	}

	uint32_t i;
	for (i = 0; i < a->keyword_count; i++) {
		if (!zend_string_equals(a->keywords[i], b->keywords[i])) {
			break;
		}
		if (a->replacements && !zend_string_equals(a->replacements[i], b->replacements[i])) {
			return ZEND_UNCOMPARABLE;
		}
	}
	if (i == a->keyword_count) {
		return 0;
	}

	/* Keys are unique, so equal counts and containment mean the same key set or mapping. */
	HashTable mapping;
	zend_hash_init(&mapping, b->keyword_count, NULL, NULL, false);
	for (i = 0; i < b->keyword_count; i++) {
		zend_hash_add_new_ptr(&mapping, b->keywords[i], b->replacements ? b->replacements[i] : b->keywords[i]);
	}
	bool equal = true;
	for (i = 0; i < a->keyword_count; i++) {
		zend_string *value = zend_hash_find_ptr(&mapping, a->keywords[i]);
		if (!value || (a->replacements && !zend_string_equals(a->replacements[i], value))) {
			equal = false;
			break;
		}
	}
	zend_hash_destroy(&mapping);
	return equal ? 0 : ZEND_UNCOMPARABLE;
}

static zend_result matcher_check_uninitialized(ac_automaton *automaton, zend_class_entry *ce)
{
	if (UNEXPECTED(automaton->initialized)) {
		zend_throw_error(NULL, "%pS is already initialized", ce->name);
		return FAILURE;
	}
	return SUCCESS;
}

PHP_METHOD(Str_Matcher, __construct)
{
	HashTable *keywords;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(keywords)
	ZEND_PARSE_PARAMETERS_END();

	ac_automaton *automaton = &matcher_from_object(Z_OBJ_P(ZEND_THIS))->automaton;

	if (matcher_check_uninitialized(automaton, Z_OBJCE_P(ZEND_THIS)) == FAILURE) {
		RETURN_THROWS();
	}

	ac_automaton prepared = {0};
	uint32_t count = zend_hash_num_elements(keywords);

	if (count) {
		prepared.keywords = safe_emalloc(count, sizeof(zend_string *), 0);
	}
	HashTable seen;

	zend_hash_init(&seen, count, NULL, NULL, false);
	zval *entry;

	ZEND_HASH_FOREACH_VAL(keywords, entry) {
		ZVAL_DEREF(entry);
		if (Z_TYPE_P(entry) != IS_STRING) {
			zend_argument_type_error(1, "must be an array of strings");
			goto failure;
		}
		if (Z_STRLEN_P(entry) == 0) {
			zend_argument_value_error(1, "must not contain an empty keyword");
			goto failure;
		}
		if (zend_hash_add_empty_element(&seen, Z_STR_P(entry))) {
			prepared.keywords[prepared.keyword_count++] = zend_string_copy(Z_STR_P(entry));
		}
	} ZEND_HASH_FOREACH_END();
	zend_hash_destroy(&seen);
	if (prepared.keyword_count < count) {
		prepared.keywords = safe_erealloc(prepared.keywords, prepared.keyword_count, sizeof(zend_string *), 0);
	}

	if (ac_build(&prepared, false) == FAILURE) {
		ac_destroy(&prepared);
		RETURN_THROWS();
	}
	prepared.initialized = true;
	*automaton = prepared;
	return;

failure:
	zend_hash_destroy(&seen);
	ac_destroy(&prepared);
	RETURN_THROWS();
}

PHP_METHOD(Str_Matcher, matches)
{
	zend_string *subject;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(subject)
	ZEND_PARSE_PARAMETERS_END();

	ac_automaton *automaton = &matcher_from_object(Z_OBJ_P(ZEND_THIS))->automaton;

	if (automaton->keyword_count == 0) {
		RETURN_FALSE;
	}
	uint32_t state = 0;

	RETURN_BOOL(ac_next_match(automaton, (const unsigned char *) ZSTR_VAL(subject), 0, ZSTR_LEN(subject), &state,
		ac_prefer_runs(automaton, subject)) < ZSTR_LEN(subject));
}

static zend_always_inline bool ac_count_add(zend_long *count, uint32_t increment)
{
	if (UNEXPECTED(increment > ZEND_LONG_MAX - *count)) {
		return false;
	}
	*count += increment;
	return true;
}

PHP_METHOD(Str_Matcher, countMatches)
{
	zend_string *subject;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(subject)
	ZEND_PARSE_PARAMETERS_END();

	ac_automaton *automaton = &matcher_from_object(Z_OBJ_P(ZEND_THIS))->automaton;
	zend_long count = 0;

	if (automaton->keyword_count == 0) {
		RETURN_LONG(0);
	}
	uint32_t state = 0;
	const unsigned char *bytes = (const unsigned char *) ZSTR_VAL(subject);
	size_t length = ZSTR_LEN(subject);
	bool runs = ac_prefer_runs(automaton, subject);

	for (size_t i = ac_next_match(automaton, bytes, 0, length, &state, runs); i < length;
			i = ac_next_match(automaton, bytes, i + 1, length, &state, runs)) {
		state &= AC_STATE_MASK;
		if (UNEXPECTED(!ac_count_add(&count, automaton->states[ac_state_index(automaton, state)].output_count))) {
			zend_throw_error(NULL, "Match count exceeds PHP_INT_MAX");
			RETURN_THROWS();
		}
	}
	RETURN_LONG(count);
}

static int ac_hit_compare(const void *a, const void *b)
{
	const ac_hit *left = a, *right = b;

	if (left->offset != right->offset) {
		return left->offset < right->offset ? -1 : 1;
	}
	return (left->length > right->length) - (left->length < right->length);
}

static zend_result ac_record_hit(ac_hit **hits, size_t *count, size_t *capacity,
		size_t offset, int keyword, zend_string *word)
{
	if (UNEXPECTED(*count == HT_MAX_SIZE)) {
		zend_throw_error(NULL, "Match result exceeds the maximum array size");
		return FAILURE;
	}
	if (*count == *capacity) {
		*capacity = *capacity ? *capacity * 2 : AC_INITIAL_CAPACITY;
		*hits = safe_erealloc(*hits, *capacity, sizeof(ac_hit), 0);
	}
	(*hits)[*count].offset = offset;
	(*hits)[*count].length = ZSTR_LEN(word);
	(*hits)[*count].keyword = keyword;
	(*count)++;
	return SUCCESS;
}

PHP_METHOD(Str_Matcher, getMatchedKeywords)
{
	zend_string *subject;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(subject)
	ZEND_PARSE_PARAMETERS_END();

	ac_automaton *automaton = &matcher_from_object(Z_OBJ_P(ZEND_THIS))->automaton;

	array_init(return_value);
	if (automaton->keyword_count == 0 || ZSTR_LEN(subject) == 0) {
		return;
	}
	bool *seen = ecalloc(automaton->keyword_count, sizeof(bool));
	ac_hit *first = NULL;
	size_t count = 0, capacity = 0;
	uint32_t state = 0;
	const unsigned char *bytes = (const unsigned char *) ZSTR_VAL(subject);
	size_t length = ZSTR_LEN(subject);
	bool runs = ac_prefer_runs(automaton, subject);

	for (size_t i = ac_next_match(automaton, bytes, 0, length, &state, runs); i < length;
			i = ac_next_match(automaton, bytes, i + 1, length, &state, runs)) {
		state &= AC_STATE_MASK;
		uint32_t index = ac_state_index(automaton, state);
		int output = automaton->states[index].keyword >= 0 ? (int) index : automaton->states[index].output_link;

		while (output >= 0) {
			int keyword = automaton->states[output].keyword;
			/* A visited terminal and all its output ancestors have already been recorded. */
			if (seen[keyword]) {
				break;
			}
			seen[keyword] = true;
			zend_string *word = automaton->keywords[keyword];

			if (ac_record_hit(&first, &count, &capacity, i + 1 - ZSTR_LEN(word), keyword, word) == FAILURE) {
				efree(seen);
				if (first) {
					efree(first);
				}
				RETURN_THROWS();
			}
			output = automaton->states[output].output_link;
		}
		if (count == automaton->keyword_count) {
			break;
		}
	}
	efree(seen);
	if (count > 1) {
		qsort(first, count, sizeof(ac_hit), ac_hit_compare);
	}
	for (size_t i = 0; i < count; i++) {
		add_next_index_str(return_value, zend_string_copy(automaton->keywords[first[i].keyword]));
	}
	if (first) {
		efree(first);
	}
}

PHP_METHOD(Str_Matcher, matchAll)
{
	zend_string *subject;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(subject)
	ZEND_PARSE_PARAMETERS_END();

	ac_automaton *automaton = &matcher_from_object(Z_OBJ_P(ZEND_THIS))->automaton;

	array_init(return_value);
	if (automaton->keyword_count == 0 || ZSTR_LEN(subject) == 0) {
		return;
	}
	ac_hit *hits = NULL;
	size_t count = 0, capacity = 0;
	uint32_t state = 0;
	const unsigned char *bytes = (const unsigned char *) ZSTR_VAL(subject);
	size_t length = ZSTR_LEN(subject);
	bool runs = ac_prefer_runs(automaton, subject);

	for (size_t i = ac_next_match(automaton, bytes, 0, length, &state, runs); i < length;
			i = ac_next_match(automaton, bytes, i + 1, length, &state, runs)) {
		state &= AC_STATE_MASK;
		uint32_t index = ac_state_index(automaton, state);
		int output = automaton->states[index].keyword >= 0 ? (int) index : automaton->states[index].output_link;

		while (output >= 0) {
			int keyword = automaton->states[output].keyword;
			zend_string *word = automaton->keywords[keyword];

			if (ac_record_hit(&hits, &count, &capacity, i + 1 - ZSTR_LEN(word), keyword, word) == FAILURE) {
				if (hits) {
					efree(hits);
				}
				RETURN_THROWS();
			}
			output = automaton->states[output].output_link;
		}
	}
	if (count > 1) {
		qsort(hits, count, sizeof(ac_hit), ac_hit_compare);
	}
	if (count > 0 && UNEXPECTED(hits[count - 1].offset > (size_t) ZEND_LONG_MAX)) {
		efree(hits);
		zend_throw_error(NULL, "Match offset exceeds PHP_INT_MAX");
		RETURN_THROWS();
	}
	for (size_t i = 0; i < count; i++) {
		zval result;

		object_init_ex(&result, str_matchresult_ce);
		zend_object *object = Z_OBJ(result);
		ZVAL_LONG(OBJ_PROP(object, result_offset), (zend_long) hits[i].offset);
		ZVAL_LONG(OBJ_PROP(object, result_length), (zend_long) hits[i].length);
		ZVAL_STR(OBJ_PROP(object, result_keyword), zend_string_copy(automaton->keywords[hits[i].keyword]));
		add_next_index_zval(return_value, &result);
	}
	if (hits) {
		efree(hits);
	}
}

PHP_METHOD(Str_Matcher, getKeywords)
{
	ZEND_PARSE_PARAMETERS_NONE();
	ac_automaton *automaton = &matcher_from_object(Z_OBJ_P(ZEND_THIS))->automaton;

	array_init_size(return_value, automaton->keyword_count);
	for (uint32_t i = 0; i < automaton->keyword_count; i++) {
		add_next_index_str(return_value, zend_string_copy(automaton->keywords[i]));
	}
}

PHP_METHOD(Str_MatchResult, __construct)
{
	zend_throw_error(NULL, "Cannot directly construct Str\\MatchResult, use Str\\Matcher::matchAll() instead");
	RETURN_THROWS();
}

PHP_METHOD(Str_Replacer, __construct)
{
	HashTable *replacements;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(replacements)
	ZEND_PARSE_PARAMETERS_END();

	ac_automaton *automaton = &matcher_from_object(Z_OBJ_P(ZEND_THIS))->automaton;

	if (matcher_check_uninitialized(automaton, Z_OBJCE_P(ZEND_THIS)) == FAILURE) {
		RETURN_THROWS();
	}
	ac_automaton prepared = {0};
	uint32_t count = zend_hash_num_elements(replacements);

	if (count) {
		prepared.keywords = safe_emalloc(count, sizeof(zend_string *), 0);
		prepared.replacements = safe_emalloc(count, sizeof(zend_string *), 0);
	}
	zend_string *key;
	zend_ulong index;
	zval *value;

	ZEND_HASH_FOREACH_KEY_VAL(replacements, index, key, value) {
		ZVAL_DEREF(value);
		if (Z_TYPE_P(value) != IS_STRING) {
			zend_argument_type_error(1, "must be an array of string replacements");
			goto failure;
		}
		if (key && ZSTR_LEN(key) == 0) {
			zend_argument_value_error(1, "must not contain an empty keyword");
			goto failure;
		}
		prepared.keywords[prepared.keyword_count] = key ? zend_string_copy(key) : zend_long_to_str((zend_long) index);
		prepared.replacements[prepared.keyword_count] = zend_string_copy(Z_STR_P(value));
		prepared.keyword_count++;
	} ZEND_HASH_FOREACH_END();

	if (ac_build(&prepared, true) == FAILURE) {
		goto failure;
	}
	prepared.initialized = true;
	*automaton = prepared;
	return;

failure:
	ac_destroy(&prepared);
	RETURN_THROWS();
}

/* The reversed automaton gives the longest keyword at each original start position. */
PHP_METHOD(Str_Replacer, replace)
{
	zend_string *subject;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(subject)
	ZEND_PARSE_PARAMETERS_END();

	ac_automaton *automaton = &matcher_from_object(Z_OBJ_P(ZEND_THIS))->automaton;
	int *best = NULL;
	ac_candidate *matches = NULL;
	size_t match_count = 0, capacity = 0, limit = 0, candidates = 0;

	if (automaton->keyword_count) {
		uint32_t state = 0;
		size_t i = ZSTR_LEN(subject);
		const unsigned char *bytes = (const unsigned char *) ZSTR_VAL(subject);
		bool runs = ac_prefer_runs(automaton, subject);

		for (;;) {
			i = ac_previous_match(automaton, bytes, i, &state, runs);
			if (i == SIZE_MAX) {
				i = 0;
				break;
			}
			state &= AC_STATE_MASK;
			if (candidates == 0) {
				candidates = i + 1;
				limit = MIN(AC_INITIAL_CANDIDATE_LIMIT, candidates / sizeof(ac_candidate) * sizeof(int));
			}
			if (match_count == capacity) {
				if (capacity == limit && capacity >= AC_INITIAL_CANDIDATE_LIMIT) {
					size_t sparse_limit = candidates / (4 * sizeof(ac_candidate));

					if (capacity < sparse_limit
							&& match_count <= (candidates - i) / (4 * sizeof(ac_candidate))) {
						limit = MIN(sparse_limit, capacity * 2);
					}
				}
				if (capacity == limit) {
					best = safe_emalloc(candidates, sizeof(int), 0);
					for (size_t j = i + 1; j < candidates; j++) {
						best[j] = -1;
					}
					for (size_t j = 0; j < match_count; j++) {
						best[matches[j].offset] = matches[j].keyword;
					}
					if (matches) {
						efree(matches);
						matches = NULL;
					}
					best[i] = automaton->longest[ac_state_index(automaton, state)];
					break;
				}
				capacity = MIN(limit, capacity ? capacity * 2 : 16);
				matches = safe_erealloc(matches, capacity, sizeof(ac_candidate), 0);
			}
			matches[match_count].offset = i;
			matches[match_count++].keyword = automaton->longest[ac_state_index(automaton, state)];
		}
		while (i > 0) {
			size_t match = ac_previous_match(automaton, bytes, i, &state, runs);
			size_t first = match == SIZE_MAX ? 0 : match + 1;

			memset(best + first, 0xff, (i - first) * sizeof(int));
			if (match == SIZE_MAX) {
				break;
			}
			state &= AC_STATE_MASK;
			best[match] = automaton->longest[ac_state_index(automaton, state)];
			i = match;
		}
	}
	if (candidates == 0) {
		RETURN_STR_COPY(subject);
	}

	smart_str output = {0};
	size_t position = 0, literal = 0;

	while (best ? position < candidates : match_count > 0) {
		int keyword;

		if (best) {
			keyword = best[position];
			if (keyword < 0) {
				position++;
				continue;
			}
		} else {
			const ac_candidate *match = &matches[--match_count];

			if (match->offset < position) {
				continue;
			}
			position = match->offset;
			keyword = match->keyword;
		}
		if (position > literal) {
			smart_str_appendl(&output, ZSTR_VAL(subject) + literal, position - literal);
		}
		smart_str_append(&output, automaton->replacements[keyword]);
		position += ZSTR_LEN(automaton->keywords[keyword]);
		literal = position;
	}
	if (ZSTR_LEN(subject) > literal) {
		smart_str_appendl(&output, ZSTR_VAL(subject) + literal, ZSTR_LEN(subject) - literal);
	}
	if (best) {
		efree(best);
	} else {
		efree(matches);
	}
	smart_str_0(&output);
	RETURN_STR(output.s ? output.s : ZSTR_EMPTY_ALLOC());
}

PHP_METHOD(Str_Replacer, getReplacements)
{
	ZEND_PARSE_PARAMETERS_NONE();
	ac_automaton *automaton = &matcher_from_object(Z_OBJ_P(ZEND_THIS))->automaton;

	array_init_size(return_value, automaton->keyword_count);
	for (uint32_t i = 0; i < automaton->keyword_count; i++) {
		zval replacement;

		ZVAL_STR(&replacement, zend_string_copy(automaton->replacements[i]));
		zend_symtable_update(Z_ARRVAL_P(return_value), automaton->keywords[i], &replacement);
	}
}

PHP_MINIT_FUNCTION(strmatch)
{
	str_matchresult_ce = register_class_Str_MatchResult();
	result_offset = ((zend_property_info *) zend_hash_str_find_ptr(
		&str_matchresult_ce->properties_info, "offset", strlen("offset")))->offset;
	result_length = ((zend_property_info *) zend_hash_str_find_ptr(
		&str_matchresult_ce->properties_info, "length", strlen("length")))->offset;
	result_keyword = ((zend_property_info *) zend_hash_str_find_ptr(
		&str_matchresult_ce->properties_info, "keyword", strlen("keyword")))->offset;

	memcpy(&matcher_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	matcher_handlers.offset = offsetof(matcher_object, std);
	matcher_handlers.free_obj = matcher_free;
	matcher_handlers.clone_obj = NULL;
	matcher_handlers.compare = matcher_compare;

	str_matcher_ce = register_class_Str_Matcher();
	str_matcher_ce->create_object = matcher_create;
	str_matcher_ce->default_object_handlers = &matcher_handlers;
	str_replacer_ce = register_class_Str_Replacer();
	str_replacer_ce->create_object = matcher_create;
	str_replacer_ce->default_object_handlers = &matcher_handlers;
	return SUCCESS;
}
