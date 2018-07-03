/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Nikita Popov <nikic@php.net>                                |
   |          Bob Weinand <bobwei9@hotmail.com>                           |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_GENERATORS_H
#define ZEND_GENERATORS_H

BEGIN_EXTERN_C()

extern ZEND_API zend_class_entry *zend_ce_generator;
extern ZEND_API zend_class_entry *zend_ce_ClosedGeneratorException;

typedef struct _zend_generator_node zend_generator_node;
typedef struct _zend_generator zend_generator;

/* The concept of `yield from` exposes problems when accessed at different levels of the chain of delegated generators. We need to be able to reference the currently executed Generator in all cases and still being able to access the return values of finished Generators.
 * The solution to this problem is a doubly-linked tree, which all Generators referenced in maintain a reference to. It should be impossible to avoid walking the tree in all cases. This way, we only need tree walks from leaf to root in case where some part of the `yield from` chain is passed to another `yield from`. (Update of leaf node pointer and list of multi-children nodes needed when leaf gets a child in direct path from leaf to root node.) But only in that case, which should be a fairly rare case (which is then possible, but not totally cheap).
 * The root of the tree is then the currently executed Generator. The subnodes of the tree (all except the root node) are all Generators which do `yield from`. Each node of the tree knows a pointer to one leaf descendant node. Each node with multiple children needs a list of all leaf descendant nodes paired with pointers to their respective child node. (The stack is determined by leaf node pointers) Nodes with only one child just don't need a list, there it is enough to just have a pointer to the child node. Further, leaf nodes store a pointer to the root node.
 * That way, when we advance any generator, we just need to look up a leaf node (which all have a reference to a root node). Then we can see at the root node whether current Generator is finished. If it isn't, all is fine and we can just continue. If the Generator finished, there will be two cases. Either it is a simple node with just one child, then go down to child node. Or it has multiple children and we now will remove the current leaf node from the list of nodes (unnecessary, is microoptimization) and go down to the child node whose reference was paired with current leaf node. Child node is then removed its parent reference and becomes new top node. Or the current node references the Generator we're currently executing, then we can continue from the YIELD_FROM opcode. When a node referenced as root node in a leaf node has a parent, then we go the way up until we find a root node without parent.
 * In case we go into a new `yield from` level, a node is created on top of current root and becomes the new root. Leaf node needs to be updated with new root node then.
 * When a Generator referenced by a node of the tree is added to `yield from`, that node now gets a list of children (we need to walk the descendants of that node and nodes of the tree of the other Generator down to the first multi-children node and copy all the leaf node pointers from there). In case there was no multi-children node (linear tree), we just add a pair (pointer to leaf node, pointer to child node), with the child node being in a direct path from leaf to this node.
 */

struct _zend_generator_node {
	zend_generator *parent; /* NULL for root */
	uint32_t children;
	union {
		HashTable *ht; /* if multiple children */
		struct { /* if one child */
			zend_generator *leaf;
			zend_generator *child;
		} single;
	} child;
	union {
		zend_generator *leaf; /* if > 0 children */
		zend_generator *root; /* if 0 children */
	} ptr;
};

struct _zend_generator {
	zend_object std;

	zend_object_iterator *iterator;

	/* The suspended execution context. */
	zend_execute_data *execute_data;

	/* Frozen call stack for "yield" used in context of other calls */
	zend_execute_data *frozen_call_stack;

	/* Current value */
	zval value;
	/* Current key */
	zval key;
	/* Return value */
	zval retval;
	/* Variable to put sent value into */
	zval *send_target;
	/* Largest used integer key for auto-incrementing keys */
	zend_long largest_used_integer_key;

	/* Values specified by "yield from" to yield from this generator.
	 * This is only used for arrays or non-generator Traversables.
	 * This zval also uses the u2 structure in the same way as
	 * by-value foreach. */
	zval values;

	/* Node of waiting generators when multiple "yield from" expressions
	 * are nested. */
	zend_generator_node node;

	/* Fake execute_data for stacktraces */
	zend_execute_data execute_fake;

	/* ZEND_GENERATOR_* flags */
	zend_uchar flags;

	zval *gc_buffer;
	uint32_t gc_buffer_size;
};

static const zend_uchar ZEND_GENERATOR_CURRENTLY_RUNNING = 0x1;
static const zend_uchar ZEND_GENERATOR_FORCED_CLOSE      = 0x2;
static const zend_uchar ZEND_GENERATOR_AT_FIRST_YIELD    = 0x4;
static const zend_uchar ZEND_GENERATOR_DO_INIT           = 0x8;

void zend_register_generator_ce(void);
ZEND_API void zend_generator_close(zend_generator *generator, zend_bool finished_execution);
ZEND_API void zend_generator_resume(zend_generator *generator);

ZEND_API void zend_generator_restore_call_stack(zend_generator *generator);
ZEND_API zend_execute_data* zend_generator_freeze_call_stack(zend_execute_data *execute_data);

void zend_generator_yield_from(zend_generator *generator, zend_generator *from);
ZEND_API zend_execute_data *zend_generator_check_placeholder_frame(zend_execute_data *ptr);

ZEND_API zend_generator *zend_generator_update_current(zend_generator *generator, zend_generator *leaf);
static zend_always_inline zend_generator *zend_generator_get_current(zend_generator *generator)
{
	zend_generator *leaf;
	zend_generator *root;

	if (EXPECTED(generator->node.parent == NULL)) {
		/* we're not in yield from mode */
		return generator;
	}

	leaf = generator->node.children ? generator->node.ptr.leaf : generator;
	root = leaf->node.ptr.root;

	if (EXPECTED(root->execute_data && root->node.parent == NULL)) {
		/* generator still running */
		return root;
	}

	return zend_generator_update_current(generator, leaf);
}

END_EXTERN_C()

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
