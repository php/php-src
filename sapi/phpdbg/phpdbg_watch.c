/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,	  |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

/* Some information for the reader...
 *
 * The main structure managing the direct observations is the watchpoint (phpdbg_watchpoint_t). There are several types of watchpoints currently:
 * WATCH_ON_BUCKET: a watchpoint on a Bucket element, used to monitor values inside HashTables (largely handled equivalently to WATCH_ON_ZVAL, it just monitors also for IS_UNDEF and key changes)
 * WATCH_ON_ZVAL: a watchpoint on a bare zval (&zend_reference.val, zval.value.indirect)
 * WATCH_ON_STR: a watchpoint on a zend_string (put on &ZSTR_LEN() in order to not watch refcount/hash)
 * WATCH_ON_HASHTABLE: a watchpoint on a HashTable (currently only used to observe size changes, put after flags in order to not watch refcount)
 * WATCH_ON_REFCOUNTED: a watchpoint on a zend_refcounted, observes the refcount and serves as reference pointer in the custom efree handler
 * WATCH_ON_HASHDATA: special watchpoint to watch for HT_GET_DATA_ADDR(ht) being efree()'d to be able to properly relocate Bucket watches
 *
 * Watch elements are either simple, recursive or implicit (PHPDBG_WATCH_* flags)
 * Simple means that a particular watchpoint was explicitly defined
 * Recursive watch elements are created recursively (recursive root flag is to distinguish the root element easily from its children recursive elements)
 * Implicit  watch elements are implicitly created on all ancestors of simple or recursive watch elements
 * Recursive and (simple or implicit) watch elements are mutually exclusive
 * Array/Object to distinguish watch elements on arrays
 *
 * Watch elements all contain a reference to a watchpoint (except if scheduled for recreation); a "watch" is a watch element created by the user with a specific id
 * Each watch has its independent structure of watch elements, watchpoints are responsible for managing collisions and preventing pointers being watched multiple times
 *
 * PHPDBG_G(watchpoint_tree) contains all watchpoints identified by the watch target address
 * PHPDBG_G(watch_HashTables) contains the addresses of parent_containers of watch elements
 * PHPDBG_G(watch_elements) contains all directly defined watch elements (i.e. those which have an individual id)
 * PHPDBG_G(watch_collisions) is indexed by a zend_refcounted * pointer (phpdbg_watchpoint_t.ref). It stores information about collisions (everything which contains a zend_refcounted * may be referenced by multiple watches)
 * PHPDBG_G(watch_free) is a set of pointers to watch for being freed (like HashTables referenced by phpdbg_watch_element.parent_container)
 * PHPDBG_G(watch_recreation) is the list of watch elements whose watchpoint has been removed (via efree() for example) and needs to be recreated
 * PHPDBG_G(watchlist_mem) is the list of unprotected memory pages; used to watch which pages need their PROT_WRITE attribute removed after checking
 *
 * Watching on addresses:
 * * Address and size are transformed into memory page aligned address and size
 * * mprotect() enables or disables them (depending on flags) - Windows has a transparent compatibility layer in phpdbg_win.c
 * * segfault handler stores the address of the page and marks it again as writable
 * * later watchpoints pointing inside these pages are compared against their current value and eventually reactivated (or deleted)
 *
 * Creating a watch:
 * * Implicit watch elements for each element in the hierarchy (starting from base, which typically is current symbol table) except the last one
 * * Create a watch element with either simple flag or recursive [+ root] flags
 * * If the element has recursive flag, create elements recursively for every referenced HashTable and zval
 *
 * Creating a watch element:
 * * For each watch element a related watchpoint is created, if there's none yet; add itself then into the list of parents of that watchpoint
 * * If the watch has a parent_container, add itself also into a phpdbg_watch_ht_info (inside PHPDBG_G(watch_HashTables)) [and creates it if not yet existing]
 *
 * Creation of watchpoints:
 * * Watchpoints create a watch collision for each refcounted or indirect on the zval (if type is WATCH_ON_BUCKET or WATCH_ON_ZVAL)
 * * Backs the current value of the watched pointer up
 * * Installs the watchpoint in PHPDBG_G(watchpoint_tree) and activates it (activation of a watchpoint = remove PROT_WRITE from the pages the watched pointer resides on)
 *
 * Watch collisions:
 * * Manages a watchpoint on the refcount (WATCH_ON_REFCOUNTED) or indirect zval (WATCH_ON_ZVAL)
 * * Guarantees that every pointer is watched at most once (by having a pointer to collision mapping in PHPDBG_G(watch_collisions), which have the unique watchpoints for the respective collision)
 * * Contains a list of parents, i.e. which watchpoints reference it (via watch->ref)
 * * If no watchpoint is referencing it anymore, the watch collision and its associated watchpoints (phpdbg_watch_collision.ref/reference) are removed
 *
 * Deleting a watch:
 * * Watches are stored by an id in PHPDBG_G(watch_elements); the associated watch element is then deleted
 * * Deletes all parent and children implicit watch elements
 *
 * Deleting a watch element:
 * * Removes itself from the parent list of the associated watchpoints; if that parent list is empty, also delete the watchpoint
 * * Removes itself from the related phpdbg_watch_ht_info if it has a parent_container
 *
 * Deleting a watchpoint:
 * * Remove itself from watch collisions this watchpoint participates in
 * * Removes the watchpoint from PHPDBG_G(watchpoint_tree) and deactivates it (deactivation of a watchpoint = add PROT_WRITE to the pages the watched pointer resides on)
 *
 * A watched pointer is efree()'d:
 * * Needs immediate action as we else may run into dereferencing a pointer into freed memory
 * * Deletes the associated watchpoint, and for each watch element, if recursive, all its children elements
 * * If the its watch elements are implicit, recursive roots or simple, they and all their children are dissociated from their watchpoints (i.e. removed from the watchpoint, if no other element is referencing it, it is deleted); adds these elements to PHPDBG_G(watch_recreation)
 *
 * Recreating watchpoints:
 * * Upon each opcode, PHPDBG_G(watch_recreation) is checked and all its elements are searched for whether the watch is still reachable via the tree given by its implicits
 * * In case they are not reachable, the watch is deleted (and thus all the related watch elements), else a new watchpoint is created for all the watch elements
 * * The old and new values of the watches are compared and shown if changed
 *
 * Comparing watchpoints:
 * * The old and new values of the watches are compared and shown if changed
 * * If changed, it is checked whether the refcounted/indirect changed and watch collisions removed or created accordingly
 * * If a zval/bucket watchpoint is recursive, watch elements are added or removed accordingly
 * * If an array watchpoint is recursive, new array watchpoints are added if there are new ones in the array
 * * If the watch (element with an id) is not reachable anymore due to changes in implicits, the watch is removed
 */

#include "zend.h"
#include "phpdbg.h"
#include "phpdbg_btree.h"
#include "phpdbg_watch.h"
#include "phpdbg_utils.h"
#include "phpdbg_prompt.h"
#ifndef _WIN32
# include <unistd.h>
# include <sys/mman.h>
#endif

ZEND_EXTERN_MODULE_GLOBALS(phpdbg)

const phpdbg_command_t phpdbg_watch_commands[] = {
	PHPDBG_COMMAND_D_EX(array,      "create watchpoint on an array", 'a', watch_array,     &phpdbg_prompt_commands[24], "s", 0),
	PHPDBG_COMMAND_D_EX(delete,     "delete watchpoint",             'd', watch_delete,    &phpdbg_prompt_commands[24], "n", 0),
	PHPDBG_COMMAND_D_EX(recursive,  "create recursive watchpoints",  'r', watch_recursive, &phpdbg_prompt_commands[24], "s", 0),
	PHPDBG_END_COMMAND
};

#define HT_FROM_ZVP(zvp) (Z_TYPE_P(zvp) == IS_OBJECT ? Z_OBJPROP_P(zvp) : Z_TYPE_P(zvp) == IS_ARRAY ? Z_ARRVAL_P(zvp) : NULL)

#define HT_WATCH_OFFSET (sizeof(zend_refcounted *) + sizeof(uint32_t)) /* we are not interested in gc and flags */
#define HT_PTR_HT(ptr) ((HashTable *) (((char *) (ptr)) - HT_WATCH_OFFSET))
#define HT_WATCH_HT(watch) HT_PTR_HT((watch)->addr.ptr)

/* ### PRINTING POINTER DIFFERENCES ### */
zend_bool phpdbg_check_watch_diff(phpdbg_watchtype type, void *oldPtr, void *newPtr) {
	switch (type) {
		case WATCH_ON_BUCKET:
			if (memcmp(&((Bucket *) oldPtr)->h, &((Bucket *) newPtr)->h, sizeof(Bucket) - sizeof(zval) /* key/val comparison */) != 0) {
				return 2;
			}
		case WATCH_ON_ZVAL:
			return memcmp(oldPtr, newPtr, sizeof(zend_value) + sizeof(uint32_t) /* value + typeinfo */) != 0;
		case WATCH_ON_HASHTABLE:
			return zend_hash_num_elements(HT_PTR_HT(oldPtr)) != zend_hash_num_elements(HT_PTR_HT(newPtr));
		case WATCH_ON_REFCOUNTED:
			return memcmp(oldPtr, newPtr, sizeof(uint32_t) /* no zend_refcounted metadata info */) != 0;
		case WATCH_ON_STR:
			return memcmp(oldPtr, newPtr, *(size_t *) oldPtr + XtOffsetOf(zend_string, val) - XtOffsetOf(zend_string, len)) != 0;
		case WATCH_ON_HASHDATA:
			ZEND_UNREACHABLE();
	}
	return 0;
}

void phpdbg_print_watch_diff(phpdbg_watchtype type, zend_string *name, void *oldPtr, void *newPtr) {
	int32_t elementDiff;

	PHPDBG_G(watchpoint_hit) = 1;

	phpdbg_notice("watchhit", "variable=\"%s\"", "Breaking on watchpoint %.*s", (int) ZSTR_LEN(name), ZSTR_VAL(name));
	phpdbg_xml("<watchdata %r>");

	switch (type) {
		case WATCH_ON_BUCKET:
		case WATCH_ON_ZVAL:
			if (Z_REFCOUNTED_P((zval *) oldPtr)) {
				phpdbg_writeln("watchvalue", "type=\"old\" inaccessible=\"inaccessible\"", "Old value inaccessible or destroyed");
			} else if (Z_TYPE_P((zval *) oldPtr) == IS_INDIRECT) {
				phpdbg_writeln("watchvalue", "type=\"old\" inaccessible=\"inaccessible\"", "Old value inaccessible or destroyed (was indirect)");
			} else {
				phpdbg_out("Old value: ");
				phpdbg_xml("<watchvalue %r type=\"old\">");
				zend_print_flat_zval_r((zval *) oldPtr);
				phpdbg_xml("</watchvalue>");
				phpdbg_out("\n");
			}

			while (Z_TYPE_P((zval *) newPtr) == IS_INDIRECT) {
				newPtr = Z_INDIRECT_P((zval *) newPtr);
			}

			phpdbg_out("New value%s: ", Z_ISREF_P((zval *) newPtr) ? " (reference)" : "");
			phpdbg_xml("<watchvalue %r%s type=\"new\">", Z_ISREF_P((zval *) newPtr) ? " reference=\"reference\"" : "");
			zend_print_flat_zval_r((zval *) newPtr);
			phpdbg_xml("</watchvalue>");
			phpdbg_out("\n");
			break;

		case WATCH_ON_HASHTABLE:
			elementDiff = zend_hash_num_elements(HT_PTR_HT(oldPtr)) - zend_hash_num_elements(HT_PTR_HT(newPtr));
			if (elementDiff > 0) {
				phpdbg_writeln("watchsize", "removed=\"%d\"", "%d elements were removed from the array", (int) elementDiff);
			} else if (elementDiff < 0) {
				phpdbg_writeln("watchsize", "added=\"%d\"", "%d elements were added to the array", (int) -elementDiff);
			}
			break;

		case WATCH_ON_REFCOUNTED:
			phpdbg_writeln("watchrefcount", "type=\"old\" refcount=\"%d\"", "Old refcount: %d", GC_REFCOUNT((zend_refcounted *) oldPtr));
			phpdbg_writeln("watchrefcount", "type=\"new\" refcount=\"%d\"", "New refcount: %d", GC_REFCOUNT((zend_refcounted *) newPtr));
			break;

		case WATCH_ON_STR:
			phpdbg_out("Old value: ");
			phpdbg_xml("<watchvalue %r type=\"old\">");
			zend_write((char *) oldPtr + XtOffsetOf(zend_string, val) - XtOffsetOf(zend_string, len), *(size_t *) oldPtr);
			phpdbg_xml("</watchvalue>");
			phpdbg_out("\n");

			phpdbg_out("New value: ");
			phpdbg_xml("<watchvalue %r type=\"new\">");
			zend_write((char *) newPtr + XtOffsetOf(zend_string, val) - XtOffsetOf(zend_string, len), *(size_t *) newPtr);
			phpdbg_xml("</watchvalue>");
			phpdbg_out("\n");
			break;

		case WATCH_ON_HASHDATA:
			ZEND_UNREACHABLE();
	}

	phpdbg_xml("</watchdata>");
}

/* ### LOW LEVEL WATCHPOINT HANDLING ### */
static phpdbg_watchpoint_t *phpdbg_check_for_watchpoint(void *addr) {
	phpdbg_watchpoint_t *watch;
	phpdbg_btree_result *result = phpdbg_btree_find_closest(&PHPDBG_G(watchpoint_tree), (zend_ulong) phpdbg_get_page_boundary(addr) + phpdbg_pagesize - 1);

	if (result == NULL) {
		return NULL;
	}

	watch = result->ptr;

	/* check if that addr is in a mprotect()'ed memory area */
	if ((char *) phpdbg_get_page_boundary(watch->addr.ptr) > (char *) addr || (char *) phpdbg_get_page_boundary(watch->addr.ptr) + phpdbg_get_total_page_size(watch->addr.ptr, watch->size) < (char *) addr) {
		/* failure */
		return NULL;
	}

	return watch;
}

static void phpdbg_change_watchpoint_access(phpdbg_watchpoint_t *watch, int access) {
	/* pagesize is assumed to be in the range of 2^x */
	mprotect(phpdbg_get_page_boundary(watch->addr.ptr), phpdbg_get_total_page_size(watch->addr.ptr, watch->size), access);
}

static inline void phpdbg_activate_watchpoint(phpdbg_watchpoint_t *watch) {
	phpdbg_change_watchpoint_access(watch, PROT_READ);
}

static inline void phpdbg_deactivate_watchpoint(phpdbg_watchpoint_t *watch) {
	phpdbg_change_watchpoint_access(watch, PROT_READ | PROT_WRITE);
}

/* Note that consecutive pages need to be merged in order to avoid watchpoints spanning page boundaries to have part of their data in the one page, part in the other page */
#ifdef _WIN32
int phpdbg_watchpoint_segfault_handler(void *addr) {
#else
int phpdbg_watchpoint_segfault_handler(siginfo_t *info, void *context) {
#endif

	void *page = phpdbg_get_page_boundary(
#ifdef _WIN32
		addr
#else
		info->si_addr
#endif
	);

	/* perhaps unnecessary, but check to be sure to not conflict with other segfault handlers */
	if (phpdbg_check_for_watchpoint(page) == NULL) {
		return FAILURE;
	}

	/* re-enable writing */
	mprotect(page, phpdbg_pagesize, PROT_READ | PROT_WRITE);

	zend_hash_index_add_empty_element(PHPDBG_G(watchlist_mem), (zend_ulong) page);

	return SUCCESS;
}

/* ### REGISTER WATCHPOINT ### To be used only by watch element and collision managers ### */
static inline void phpdbg_store_watchpoint_btree(phpdbg_watchpoint_t *watch) {
	phpdbg_btree_result *res;
	ZEND_ASSERT((res = phpdbg_btree_find(&PHPDBG_G(watchpoint_tree), (zend_ulong) watch->addr.ptr)) == NULL || res->ptr == watch);
	phpdbg_btree_insert(&PHPDBG_G(watchpoint_tree), (zend_ulong) watch->addr.ptr, watch);
}

static inline void phpdbg_remove_watchpoint_btree(phpdbg_watchpoint_t *watch) {
	phpdbg_btree_delete(&PHPDBG_G(watchpoint_tree), (zend_ulong) watch->addr.ptr);
}

/* ### SET WATCHPOINT ADDR ### To be used only by watch element and collision managers ### */
void phpdbg_set_addr_watchpoint(void *addr, size_t size, phpdbg_watchpoint_t *watch) {
	watch->addr.ptr = addr;
	watch->size = size;
	watch->ref = NULL;
	watch->coll = NULL;
	zend_hash_init(&watch->elements, 8, brml, NULL, 0);
}

void phpdbg_set_zval_watchpoint(zval *zv, phpdbg_watchpoint_t *watch) {
	phpdbg_set_addr_watchpoint(zv, sizeof(zval) - sizeof(uint32_t), watch);
	watch->type = WATCH_ON_ZVAL;
}

void phpdbg_set_bucket_watchpoint(Bucket *bucket, phpdbg_watchpoint_t *watch) {
	phpdbg_set_addr_watchpoint(bucket, sizeof(Bucket), watch);
	watch->type = WATCH_ON_BUCKET;
}

void phpdbg_set_ht_watchpoint(HashTable *ht, phpdbg_watchpoint_t *watch) {
	phpdbg_set_addr_watchpoint(((char *) ht) + HT_WATCH_OFFSET, sizeof(HashTable) - HT_WATCH_OFFSET, watch);
	watch->type = WATCH_ON_HASHTABLE;
}

void phpdbg_watch_backup_data(phpdbg_watchpoint_t *watch) {
	switch (watch->type) {
		case WATCH_ON_BUCKET:
		case WATCH_ON_ZVAL:
		case WATCH_ON_REFCOUNTED:
			memcpy(&watch->backup, watch->addr.ptr, watch->size);
			break;
		case WATCH_ON_STR:
			if (watch->backup.str) {
				zend_string_release(watch->backup.str);
			}
			watch->backup.str = zend_string_init((char *) watch->addr.ptr + XtOffsetOf(zend_string, val) - XtOffsetOf(zend_string, len), *(size_t *) watch->addr.ptr, 1);
			GC_MAKE_PERSISTENT_LOCAL(watch->backup.str);
			break;
		case WATCH_ON_HASHTABLE:
			memcpy((char *) &watch->backup + HT_WATCH_OFFSET, watch->addr.ptr, watch->size);
		case WATCH_ON_HASHDATA:
			break;
	}
}

/* ### MANAGE WATCH COLLISIONS ### To be used only by watch element manager and memory differ ### */
/* watch collisions are responsible for having only one watcher on a given refcounted/refval and having a mapping back to the parent zvals */
void phpdbg_delete_watch_collision(phpdbg_watchpoint_t *watch) {
	phpdbg_watch_collision *coll;
	if ((coll = zend_hash_index_find_ptr(&PHPDBG_G(watch_collisions), (zend_ulong) watch->ref))) {
		zend_hash_index_del(&coll->parents, (zend_ulong) watch);
		if (zend_hash_num_elements(&coll->parents) == 0) {
			phpdbg_deactivate_watchpoint(&coll->ref);
			phpdbg_remove_watchpoint_btree(&coll->ref);

			if (coll->ref.type == WATCH_ON_ZVAL) {
				phpdbg_delete_watch_collision(&coll->ref);
			} else if (coll->reference.addr.ptr) {
				phpdbg_deactivate_watchpoint(&coll->reference);
				phpdbg_remove_watchpoint_btree(&coll->reference);
				phpdbg_delete_watch_collision(&coll->reference);
				if (coll->reference.type == WATCH_ON_STR) {
					zend_string_release(coll->reference.backup.str);
				}
			}

			zend_hash_index_del(&PHPDBG_G(watch_collisions), (zend_ulong) watch->ref);
			zend_hash_destroy(&coll->parents);
			efree(coll);
		}
	}
}

void phpdbg_update_watch_ref(phpdbg_watchpoint_t *watch) {
	phpdbg_watch_collision *coll;

	ZEND_ASSERT(watch->type == WATCH_ON_ZVAL || watch->type == WATCH_ON_BUCKET);
	if (Z_REFCOUNTED_P(watch->addr.zv)) {
		if (Z_COUNTED_P(watch->addr.zv) == watch->ref) {
			return;
		}

		if (watch->ref != NULL) {
			phpdbg_delete_watch_collision(watch);
		}

		watch->ref = Z_COUNTED_P(watch->addr.zv);

		if (!(coll = zend_hash_index_find_ptr(&PHPDBG_G(watch_collisions), (zend_ulong) watch->ref))) {
			coll = emalloc(sizeof(*coll));
			coll->ref.type = WATCH_ON_REFCOUNTED;
			phpdbg_set_addr_watchpoint(Z_COUNTED_P(watch->addr.zv), sizeof(uint32_t), &coll->ref);
			coll->ref.coll = coll;
			phpdbg_store_watchpoint_btree(&coll->ref);
			phpdbg_activate_watchpoint(&coll->ref);
			phpdbg_watch_backup_data(&coll->ref);

			if (Z_ISREF_P(watch->addr.zv)) {
				phpdbg_set_zval_watchpoint(Z_REFVAL_P(watch->addr.zv), &coll->reference);
				coll->reference.coll = coll;
				phpdbg_update_watch_ref(&coll->reference);
				phpdbg_store_watchpoint_btree(&coll->reference);
				phpdbg_activate_watchpoint(&coll->reference);
				phpdbg_watch_backup_data(&coll->reference);
			} else if (Z_TYPE_P(watch->addr.zv) == IS_STRING) {
				coll->reference.type = WATCH_ON_STR;
				phpdbg_set_addr_watchpoint(&Z_STRLEN_P(watch->addr.zv), XtOffsetOf(zend_string, val) - XtOffsetOf(zend_string, len) + Z_STRLEN_P(watch->addr.zv) + 1, &coll->reference);
				coll->reference.coll = coll;
				phpdbg_store_watchpoint_btree(&coll->reference);
				phpdbg_activate_watchpoint(&coll->reference);
				coll->reference.backup.str = NULL;
				phpdbg_watch_backup_data(&coll->reference);
			} else {
				coll->reference.addr.ptr = NULL;
			}

			zend_hash_init(&coll->parents, 8, shitty stupid parameter, NULL, 0);
			zend_hash_index_add_ptr(&PHPDBG_G(watch_collisions), (zend_ulong) watch->ref, coll);
		}
		zend_hash_index_add_ptr(&coll->parents, (zend_long) watch, watch);
	} else if (Z_TYPE_P(watch->addr.zv) == IS_INDIRECT) {
		if ((zend_refcounted *) Z_INDIRECT_P(watch->addr.zv) == watch->ref) {
			return;
		}

		if (watch->ref != NULL) {
			phpdbg_delete_watch_collision(watch);
		}

		watch->ref = (zend_refcounted *) Z_INDIRECT_P(watch->addr.zv);

		if (!(coll = zend_hash_index_find_ptr(&PHPDBG_G(watch_collisions), (zend_ulong) watch->ref))) {
			coll = emalloc(sizeof(*coll));
			phpdbg_set_zval_watchpoint(Z_INDIRECT_P(watch->addr.zv), &coll->ref);
			coll->ref.coll = coll;
			phpdbg_update_watch_ref(&coll->ref);
			phpdbg_store_watchpoint_btree(&coll->ref);
			phpdbg_activate_watchpoint(&coll->ref);
			phpdbg_watch_backup_data(&coll->ref);

			zend_hash_init(&coll->parents, 8, shitty stupid parameter, NULL, 0);
			zend_hash_index_add_ptr(&PHPDBG_G(watch_collisions), (zend_ulong) watch->ref, coll);
		}
		zend_hash_index_add_ptr(&coll->parents, (zend_long) watch, watch);
	} else if (watch->ref) {
		phpdbg_delete_watch_collision(watch);
		watch->ref = NULL;
	}
}

/* ### MANAGE WATCH ELEMENTS ### */
/* watchpoints must be unique per element. Only one watchpoint may point to one element. But many elements may point to one watchpoint. */
void phpdbg_recurse_watch_element(phpdbg_watch_element *element);
void phpdbg_remove_watch_element_recursively(phpdbg_watch_element *element);
void phpdbg_free_watch_element(phpdbg_watch_element *element);
void phpdbg_remove_watchpoint(phpdbg_watchpoint_t *watch);
void phpdbg_watch_parent_ht(phpdbg_watch_element *element);

phpdbg_watch_element *phpdbg_add_watch_element(phpdbg_watchpoint_t *watch, phpdbg_watch_element *element) {
	phpdbg_btree_result *res;
	if ((res = phpdbg_btree_find(&PHPDBG_G(watchpoint_tree), (zend_ulong) watch->addr.ptr)) == NULL) {
		phpdbg_watchpoint_t *mem = emalloc(sizeof(*mem));
		*mem = *watch;
		watch = mem;
		phpdbg_store_watchpoint_btree(watch);
		if (watch->type == WATCH_ON_ZVAL || watch->type == WATCH_ON_BUCKET) {
			phpdbg_update_watch_ref(watch);
		}
		phpdbg_activate_watchpoint(watch);
		phpdbg_watch_backup_data(watch);
	} else {
		phpdbg_watch_element *old_element;
		watch = res->ptr;
		if ((old_element = zend_hash_find_ptr(&watch->elements, element->str))) {
			phpdbg_free_watch_element(element);
			return old_element;
		}
	}

	element->watch = watch;
	zend_hash_add_ptr(&watch->elements, element->str, element);

	if (element->flags & PHPDBG_WATCH_RECURSIVE) {
		phpdbg_recurse_watch_element(element);
	}

	return element;
}

phpdbg_watch_element *phpdbg_add_bucket_watch_element(Bucket *bucket, phpdbg_watch_element *element) {
	phpdbg_watchpoint_t watch;
	phpdbg_set_bucket_watchpoint(bucket, &watch);
	element = phpdbg_add_watch_element(&watch, element);
	phpdbg_watch_parent_ht(element);
	return element;
}

phpdbg_watch_element *phpdbg_add_ht_watch_element(zval *zv, phpdbg_watch_element *element) {
	phpdbg_watchpoint_t watch;
	HashTable *ht = HT_FROM_ZVP(zv);

	if (!ht) {
		return NULL;
	}

	element->flags |= Z_TYPE_P(zv) == IS_ARRAY ? PHPDBG_WATCH_ARRAY : PHPDBG_WATCH_OBJECT;
	phpdbg_set_ht_watchpoint(ht, &watch);
	return phpdbg_add_watch_element(&watch, element);
}

zend_bool phpdbg_is_recursively_watched(void *ptr, phpdbg_watch_element *element) {
	phpdbg_watch_element *next = element;
	do {
		element = next;
		if (element->watch->addr.ptr == ptr) {
			return 1;
		}
		next = element->parent;
	} while (!(element->flags & PHPDBG_WATCH_RECURSIVE_ROOT));

	return 0;
}

void phpdbg_add_recursive_watch_from_ht(phpdbg_watch_element *element, zend_long idx, zend_string *str, zval *zv) {
	phpdbg_watch_element *child;
	if (phpdbg_is_recursively_watched(zv, element)) {
		return;
	}

	child = emalloc(sizeof(*child));
	child->flags = PHPDBG_WATCH_RECURSIVE;
	if (str) {
		child->str = strpprintf(0, (element->flags & PHPDBG_WATCH_ARRAY) ? "%.*s[%s]" : "%.*s->%s", (int) ZSTR_LEN(element->str) - 2, ZSTR_VAL(element->str), phpdbg_get_property_key(ZSTR_VAL(str)));
	} else {
		child->str = strpprintf(0, (element->flags & PHPDBG_WATCH_ARRAY) ? "%.*s[" ZEND_LONG_FMT "]" : "%.*s->" ZEND_LONG_FMT, (int) ZSTR_LEN(element->str) - 2, ZSTR_VAL(element->str), idx);
	}
	if (!str) {
		str = zend_long_to_str(idx); // TODO: hack, use proper int handling for name in parent
	} else { str = zend_string_copy(str); }
	child->name_in_parent = str;
	child->parent = element;
	child->child = NULL;
	child->parent_container = HT_WATCH_HT(element->watch);
	zend_hash_add_ptr(&element->child_container, child->str, child);
	phpdbg_add_bucket_watch_element((Bucket *) zv, child);
}

void phpdbg_recurse_watch_element(phpdbg_watch_element *element) {
	phpdbg_watch_element *child;
	zval *zv;

	if (element->watch->type == WATCH_ON_ZVAL || element->watch->type == WATCH_ON_BUCKET) {
		zv = element->watch->addr.zv;
		while (Z_TYPE_P(zv) == IS_INDIRECT) {
			zv = Z_INDIRECT_P(zv);
		}
		ZVAL_DEREF(zv);

		if (element->child) {
			phpdbg_remove_watch_element_recursively(element->child);
		}

		if ((Z_TYPE_P(zv) != IS_ARRAY && Z_TYPE_P(zv) != IS_OBJECT)
		    || phpdbg_is_recursively_watched(HT_WATCH_OFFSET + (char *) HT_FROM_ZVP(zv), element)) {
			if (element->child) {
				phpdbg_free_watch_element(element->child);
				element->child = NULL;
			}
			return;
		}

		if (element->child) {
			child = element->child;
		} else {
			child = emalloc(sizeof(*child));
			child->flags = PHPDBG_WATCH_RECURSIVE;
			child->str = strpprintf(0, "%.*s[]", (int) ZSTR_LEN(element->str), ZSTR_VAL(element->str));
			child->name_in_parent = NULL;
			child->parent = element;
			child->child = NULL;
			element->child = child;
		}
		zend_hash_init(&child->child_container, 8, NULL, NULL, 0);
		phpdbg_add_ht_watch_element(zv, child);
	} else if (zend_hash_num_elements(&element->child_container) == 0) {
		zend_string *str;
		zend_long idx;

		ZEND_ASSERT(element->watch->type == WATCH_ON_HASHTABLE);
		ZEND_HASH_FOREACH_KEY_VAL(HT_WATCH_HT(element->watch), idx, str, zv) {
			phpdbg_add_recursive_watch_from_ht(element, idx, str, zv);
		} ZEND_HASH_FOREACH_END();
	}
}

void phpdbg_watch_parent_ht(phpdbg_watch_element *element) {
	if (element->watch->type == WATCH_ON_BUCKET) {
		phpdbg_btree_result *res;
		HashPosition pos;
		phpdbg_watch_ht_info *hti;
		ZEND_ASSERT(element->parent_container);
		if (!(res = phpdbg_btree_find(&PHPDBG_G(watch_HashTables), (zend_ulong) element->parent_container))) {
			hti = emalloc(sizeof(*hti));
			hti->ht = element->parent_container;

			zend_hash_init(&hti->watches, 0, grrrrr, ZVAL_PTR_DTOR, 0);
			phpdbg_btree_insert(&PHPDBG_G(watch_HashTables), (zend_ulong) hti->ht, hti);

			phpdbg_set_addr_watchpoint(HT_GET_DATA_ADDR(hti->ht), HT_HASH_SIZE(hti->ht->nTableMask), &hti->hash_watch);
			hti->hash_watch.type = WATCH_ON_HASHDATA;
			phpdbg_store_watchpoint_btree(&hti->hash_watch);
			phpdbg_activate_watchpoint(&hti->hash_watch);
		} else {
			hti = (phpdbg_watch_ht_info *) res->ptr;
		}

		zend_hash_internal_pointer_end_ex(hti->ht, &pos);
		hti->last = hti->ht->arData + pos;
		hti->last_str = hti->last->key;
		hti->last_idx = hti->last->h;

		zend_hash_add_ptr(&hti->watches, element->name_in_parent, element);
	}
}

void phpdbg_unwatch_parent_ht(phpdbg_watch_element *element) {
	if (element->watch->type == WATCH_ON_BUCKET) {
		phpdbg_btree_result *res = phpdbg_btree_find(&PHPDBG_G(watch_HashTables), (zend_ulong) element->parent_container);
		ZEND_ASSERT(element->parent_container);
		if (res) {
			phpdbg_watch_ht_info *hti = res->ptr;

			if (zend_hash_num_elements(&hti->watches) == 1) {
				zend_hash_destroy(&hti->watches);
				phpdbg_btree_delete(&PHPDBG_G(watch_HashTables), (zend_ulong) hti->ht);
				phpdbg_deactivate_watchpoint(&hti->hash_watch);
				phpdbg_remove_watchpoint_btree(&hti->hash_watch);
				efree(hti);
			} else {
				zend_hash_del(&hti->watches, element->name_in_parent);
			}
		}
	}
}

/* ### DE/QUEUE WATCH ELEMENTS ### to be used by watch element manager only */
/* implicit watchpoints may change (especially because of separation); elements updated by remove & re-add etc.; thus we need to wait a little bit (until next opcode) and then compare whether the watchpoint still exists and if not, remove it */

void phpdbg_dissociate_watch_element(phpdbg_watch_element *element, phpdbg_watch_element *until);
void phpdbg_free_watch_element_tree(phpdbg_watch_element *element);

void phpdbg_queue_element_for_recreation(phpdbg_watch_element *element) {
	/* store lowermost element */
	phpdbg_watch_element *prev;

	if ((prev = zend_hash_find_ptr(&PHPDBG_G(watch_recreation), element->str))) {
		phpdbg_watch_element *child = prev;
		do {
			if (child == element) {
				return;
			}
			child = child->child;
		} while (child);
	}
	zend_hash_update_ptr(&PHPDBG_G(watch_recreation), element->str, element);

	/* dissociate from watchpoint to avoid dangling memory watches */
	phpdbg_dissociate_watch_element(element, prev);

	if (!element->parent) {
		/* HERE BE DRAGONS; i.e. we assume HashTable is directly allocated via emalloc() ... (which *should be* the case for every user-accessible array and symbol tables) */
		zend_hash_index_add_empty_element(&PHPDBG_G(watch_free), (zend_ulong) element->parent_container);
	}
}

zend_bool phpdbg_try_readding_watch_element(zval *parent, phpdbg_watch_element *element) {
	zval *zv;
	HashTable *ht = HT_FROM_ZVP(parent);

	if (!ht) {
		return 0;
	} else if (element->flags & (PHPDBG_WATCH_ARRAY | PHPDBG_WATCH_OBJECT)) {
		char *htPtr = ((char *) ht) + HT_WATCH_OFFSET;
		char *oldPtr = ((char *) &element->backup.ht) + HT_WATCH_OFFSET;
		if (phpdbg_check_watch_diff(WATCH_ON_HASHTABLE, oldPtr, htPtr)) {
			phpdbg_print_watch_diff(WATCH_ON_HASHTABLE, element->str, oldPtr, htPtr);
		}

		phpdbg_add_ht_watch_element(parent, element);
	} else if ((zv = zend_symtable_find(ht, element->name_in_parent))) {
		if (element->flags & PHPDBG_WATCH_IMPLICIT) {
			zval *next = zv;

			while (Z_TYPE_P(next) == IS_INDIRECT) {
				next = Z_INDIRECT_P(next);
			}
			if (Z_ISREF_P(next)) {
				next = Z_REFVAL_P(next);
			}

			if (!phpdbg_try_readding_watch_element(next, element->child)) {
				return 0;
			}
		} else if (phpdbg_check_watch_diff(WATCH_ON_ZVAL, &element->backup.zv, zv)) {
			phpdbg_print_watch_diff(WATCH_ON_ZVAL, element->str, &element->backup.zv, zv);
		}

		element->parent_container = ht;
		phpdbg_add_bucket_watch_element((Bucket *) zv, element);
		phpdbg_watch_parent_ht(element);
	} else {
		return 0;
	}

	return 1;
}

void phpdbg_automatic_dequeue_free(phpdbg_watch_element *element) {
	phpdbg_watch_element *child = element;
	while (child->child && !(child->flags & PHPDBG_WATCH_RECURSIVE_ROOT)) {
		child = child->child;
	}
	PHPDBG_G(watchpoint_hit) = 1;
	if (zend_hash_index_del(&PHPDBG_G(watch_elements), child->id) == SUCCESS) {
		phpdbg_notice("watchdelete", "variable=\"%.*s\" recursive=\"%s\"", "%.*s has been removed, removing watchpoint%s", (int) ZSTR_LEN(child->str), ZSTR_VAL(child->str), (child->flags & PHPDBG_WATCH_RECURSIVE_ROOT) ? " recursively" : "");
	}
	phpdbg_free_watch_element_tree(element);
}

void phpdbg_dequeue_elements_for_recreation() {
	phpdbg_watch_element *element;

	ZEND_HASH_FOREACH_PTR(&PHPDBG_G(watch_recreation), element) {
		ZEND_ASSERT(element->flags & (PHPDBG_WATCH_IMPLICIT | PHPDBG_WATCH_RECURSIVE_ROOT | PHPDBG_WATCH_SIMPLE));
		if (element->parent || zend_hash_index_find(&PHPDBG_G(watch_free), (zend_ulong) element->parent_container)) {
			zval _zv, *zv = &_zv;
			if (element->parent) {
				ZEND_ASSERT(element->parent->watch->type == WATCH_ON_ZVAL || element->parent->watch->type == WATCH_ON_BUCKET);
				zv = element->parent->watch->addr.zv;
				while (Z_TYPE_P(zv) == IS_INDIRECT) {
					zv = Z_INDIRECT_P(zv);
				}
				ZVAL_DEREF(zv);
			} else {
				ZVAL_ARR(zv, element->parent_container);
			}
			if (!phpdbg_try_readding_watch_element(zv, element)) {
				phpdbg_automatic_dequeue_free(element);
			}
		} else {
			phpdbg_automatic_dequeue_free(element);
		}
	} ZEND_HASH_FOREACH_END();

	zend_hash_clean(&PHPDBG_G(watch_recreation));
	zend_hash_clean(&PHPDBG_G(watch_free));
}

/* ### WATCH ELEMENT DELETION ### only use phpdbg_remove_watch_element from the exterior */
void phpdbg_clean_watch_element(phpdbg_watch_element *element);

void phpdbg_free_watch_element(phpdbg_watch_element *element) {
	zend_string_release(element->str);
	if (element->name_in_parent) {
		zend_string_release(element->name_in_parent);
	}
	efree(element);
}

/* note: does *not* free the passed element, only clean */
void phpdbg_remove_watch_element_recursively(phpdbg_watch_element *element) {
	if (element->child) {
		phpdbg_remove_watch_element_recursively(element->child);
		phpdbg_free_watch_element(element->child);
		element->child = NULL;
	} else if (element->flags & (PHPDBG_WATCH_ARRAY | PHPDBG_WATCH_OBJECT)) {
		phpdbg_watch_element *child;
		ZEND_HASH_FOREACH_PTR(&element->child_container, child) {
			phpdbg_remove_watch_element_recursively(child);
			phpdbg_free_watch_element(child);
		} ZEND_HASH_FOREACH_END();
		zend_hash_destroy(&element->child_container);
	}

	phpdbg_clean_watch_element(element);
}

/* remove single watch (i.e. manual unset) or implicit removed */
void phpdbg_remove_watch_element(phpdbg_watch_element *element) {
	phpdbg_watch_element *parent = element->parent, *child = element->child;
	while (parent) {
		phpdbg_watch_element *cur = parent;
		parent = parent->parent;
		phpdbg_clean_watch_element(cur);
		phpdbg_free_watch_element(cur);
	}
	while (child) {
		phpdbg_watch_element *cur = child;
		child = child->child;
		if (cur->flags & PHPDBG_WATCH_RECURSIVE_ROOT) {
			phpdbg_remove_watch_element_recursively(cur);
			child = NULL;
		} else {
			phpdbg_clean_watch_element(cur);
		}
		phpdbg_free_watch_element(cur);
	}
	if (element->flags & PHPDBG_WATCH_RECURSIVE_ROOT) {
		phpdbg_remove_watch_element_recursively(element);
	} else {
		phpdbg_clean_watch_element(element);
	}
	zend_hash_index_del(&PHPDBG_G(watch_elements), element->id);
	phpdbg_free_watch_element(element);
}

void phpdbg_backup_watch_element(phpdbg_watch_element *element) {
	memcpy(&element->backup, &element->watch->backup, /* element->watch->size */ sizeof(element->backup));
}

/* until argument to prevent double remove of children elements */
void phpdbg_dissociate_watch_element(phpdbg_watch_element *element, phpdbg_watch_element *until) {
	phpdbg_watch_element *child = element;
	ZEND_ASSERT((element->flags & (PHPDBG_WATCH_RECURSIVE_ROOT | PHPDBG_WATCH_RECURSIVE)) != PHPDBG_WATCH_RECURSIVE);

	if (element->flags & PHPDBG_WATCH_RECURSIVE_ROOT) {
		phpdbg_backup_watch_element(element);
		phpdbg_remove_watch_element_recursively(element);
		return;
	}

	while (child->child != until) {
		child = child->child;
		if (child->flags & PHPDBG_WATCH_RECURSIVE_ROOT) {
			phpdbg_backup_watch_element(child);
			phpdbg_remove_watch_element_recursively(child);
			child->child = NULL;
			break;
		}
		if (child->child == NULL || (child->flags & PHPDBG_WATCH_RECURSIVE_ROOT)) {
			phpdbg_backup_watch_element(child);
		}
		phpdbg_clean_watch_element(child);
	}
	/* element needs to be removed last! */
	if (element->child == NULL) {
		phpdbg_backup_watch_element(element);
	}
	phpdbg_clean_watch_element(element);
}

/* unlike phpdbg_remove_watch_element this *only* frees and does not clean up element + children! Only use after previous cleanup (e.g. phpdbg_dissociate_watch_element) */
void phpdbg_free_watch_element_tree(phpdbg_watch_element *element) {
	phpdbg_watch_element *parent = element->parent, *child = element->child;
	while (parent) {
		phpdbg_watch_element *cur = parent;
		parent = parent->parent;
		phpdbg_clean_watch_element(cur);
		phpdbg_free_watch_element(cur);
	}
	while (child) {
		phpdbg_watch_element *cur = child;
		child = child->child;
		phpdbg_free_watch_element(cur);
	}
	phpdbg_free_watch_element(element);
}

void phpdbg_update_watch_element_watch(phpdbg_watch_element *element) {
	if (element->flags & PHPDBG_WATCH_IMPLICIT) {
		phpdbg_watch_element *child = element->child;
		while (child->flags & PHPDBG_WATCH_IMPLICIT) {
			child = child->child;
		}

		ZEND_ASSERT(element->watch->type == WATCH_ON_ZVAL || element->watch->type == WATCH_ON_BUCKET);
		phpdbg_queue_element_for_recreation(element);
	} else if (element->flags & (PHPDBG_WATCH_RECURSIVE_ROOT | PHPDBG_WATCH_SIMPLE)) {
		phpdbg_queue_element_for_recreation(element);
	} else if (element->flags & PHPDBG_WATCH_RECURSIVE) {
		phpdbg_remove_watch_element_recursively(element);
		if (element->parent->flags & (PHPDBG_WATCH_OBJECT | PHPDBG_WATCH_ARRAY)) {
			zend_hash_del(&element->parent->child_container, element->str);
		} else {
			element->parent->child = NULL;
		}
		phpdbg_free_watch_element(element);
	}
}

void phpdbg_update_watch_collision_elements(phpdbg_watchpoint_t *watch) {
	phpdbg_watchpoint_t *parent;
	phpdbg_watch_element *element;

	ZEND_HASH_FOREACH_PTR(&watch->coll->parents, parent) {
		if (parent->coll) {
			phpdbg_update_watch_collision_elements(parent);
		} else {
			ZEND_HASH_FOREACH_PTR(&parent->elements, element) {
				phpdbg_update_watch_element_watch(element);
			} ZEND_HASH_FOREACH_END();
		}
	} ZEND_HASH_FOREACH_END();
}

void phpdbg_remove_watchpoint(phpdbg_watchpoint_t *watch) {
	phpdbg_watch_element *element;

	phpdbg_deactivate_watchpoint(watch);
	phpdbg_remove_watchpoint_btree(watch);
	phpdbg_delete_watch_collision(watch);

	if (watch->coll) {
		phpdbg_update_watch_collision_elements(watch);
		return;
	}

	watch->elements.nNumOfElements++; /* dirty hack to avoid double free */
	ZEND_HASH_FOREACH_PTR(&watch->elements, element) {
		phpdbg_update_watch_element_watch(element);
	} ZEND_HASH_FOREACH_END();
	zend_hash_destroy(&watch->elements);

	efree(watch);
}

void phpdbg_clean_watch_element(phpdbg_watch_element *element) {
	HashTable *elements = &element->watch->elements;
	phpdbg_unwatch_parent_ht(element);
	zend_hash_del(elements, element->str);
	if (zend_hash_num_elements(elements) == 0) {
		phpdbg_remove_watchpoint(element->watch);
	}
}

/* TODO: compile a name of all hit watchpoints (ids ??) */
zend_string *phpdbg_watchpoint_change_collision_name(phpdbg_watchpoint_t *watch) {
	phpdbg_watchpoint_t *parent;
	phpdbg_watch_element *element;
	zend_string *name = NULL;
	if (watch->coll) {
		ZEND_HASH_FOREACH_PTR(&watch->coll->parents, parent) {
			if (name) {
				zend_string_release(name);
			}
			name = phpdbg_watchpoint_change_collision_name(parent);
		} ZEND_HASH_FOREACH_END();
		return name;
	}
	ZEND_HASH_FOREACH_PTR(&watch->elements, element) {
		if (element->flags & PHPDBG_WATCH_IMPLICIT) {
			if ((watch->type == WATCH_ON_ZVAL || watch->type == WATCH_ON_BUCKET) && Z_TYPE(watch->backup.zv) > IS_STRING) {
				phpdbg_update_watch_element_watch(element->child);
			}
			continue;
		}
		name = element->str;
	} ZEND_HASH_FOREACH_END();

	return name ? zend_string_copy(name) : NULL;
}

/* ### WATCHING FOR CHANGES ### */
/* TODO: enforce order: first parents, then children, in order to avoid false positives */
void phpdbg_check_watchpoint(phpdbg_watchpoint_t *watch) {
	zend_string *name = NULL;
	void *comparePtr;

	if (watch->type == WATCH_ON_HASHTABLE) {
		phpdbg_watch_element *element;
		zend_string *str;
		zend_long idx;
		zval *zv;
		ZEND_HASH_FOREACH_PTR(&watch->elements, element) {
			if (element->flags & PHPDBG_WATCH_RECURSIVE) {
				phpdbg_btree_result *res = phpdbg_btree_find(&PHPDBG_G(watch_HashTables), (zend_ulong) HT_WATCH_HT(watch));
				phpdbg_watch_ht_info *hti = res ? res->ptr : NULL;

				ZEND_HASH_REVERSE_FOREACH_KEY_VAL(HT_WATCH_HT(watch), idx, str, zv) {
					if (!str) {
						str = zend_long_to_str(idx); // TODO: hack, use proper int handling for name in parent
					} else {
						str = zend_string_copy(str);
					}
					if (hti && zend_hash_find(&hti->watches, str)) {
						zend_string_release(str);
						break;
					}
					ZEND_HASH_FOREACH_PTR(&watch->elements, element) {
						if (element->flags & PHPDBG_WATCH_RECURSIVE) {
							phpdbg_add_recursive_watch_from_ht(element, idx, str, zv);
						}
					} ZEND_HASH_FOREACH_END();
					phpdbg_notice("watchadd", "element=\"%.*s\"", "Element %.*s has been added to watchpoint", (int) ZSTR_LEN(str), ZSTR_VAL(str));
					zend_string_release(str);
					PHPDBG_G(watchpoint_hit) = 1;
				} ZEND_HASH_FOREACH_END();

				break;
			}
		} ZEND_HASH_FOREACH_END();
	}
	if (watch->type == WATCH_ON_HASHDATA) {
		return;
	}

	switch (watch->type) {
		case WATCH_ON_STR:
			comparePtr = &ZSTR_LEN(watch->backup.str);
			break;
		case WATCH_ON_HASHTABLE:
			comparePtr = (char *) &watch->backup.ht + HT_WATCH_OFFSET;
			break;
		default:
			comparePtr = &watch->backup;
	}
	if (!phpdbg_check_watch_diff(watch->type, comparePtr, watch->addr.ptr)) {
		return;
	}
	if (watch->type == WATCH_ON_REFCOUNTED && !(PHPDBG_G(flags) & PHPDBG_SHOW_REFCOUNTS)) {
		phpdbg_watch_backup_data(watch);
		return;
	}
	if (watch->type == WATCH_ON_BUCKET) {
		if (watch->backup.bucket.key != watch->addr.bucket->key || (watch->backup.bucket.key != NULL && watch->backup.bucket.h != watch->addr.bucket->h)) {
			phpdbg_watch_element *element = NULL;
			zval *new;

			ZEND_HASH_FOREACH_PTR(&watch->elements, element) {
				break;
			} ZEND_HASH_FOREACH_END();

			ZEND_ASSERT(element); /* elements must be non-empty */
			new = zend_symtable_find(element->parent_container, element->name_in_parent);

			if (!new) {
				/* dequeuing will take care of appropriate notification about removal */
				phpdbg_remove_watchpoint(watch);
				return;
			}

			phpdbg_deactivate_watchpoint(watch);
			phpdbg_remove_watchpoint_btree(watch);
			watch->addr.zv = new;
			phpdbg_store_watchpoint_btree(watch);
			phpdbg_activate_watchpoint(watch);

			if (!phpdbg_check_watch_diff(WATCH_ON_ZVAL, &watch->backup.bucket.val, watch->addr.ptr)) {
				phpdbg_watch_backup_data(watch);
				return;
			}
		} else if (Z_TYPE_P(watch->addr.zv) == IS_UNDEF) {
			/* dequeuing will take care of appropriate notification about removal */
			phpdbg_remove_watchpoint(watch);
			return;
		}
	}

	name = phpdbg_watchpoint_change_collision_name(watch);

	if (name) {
		phpdbg_print_watch_diff(watch->type, name, comparePtr, watch->addr.ptr);
		zend_string_release(name);
	}

	if (watch->type == WATCH_ON_ZVAL || watch->type == WATCH_ON_BUCKET) {
		phpdbg_watch_element *element;
		phpdbg_update_watch_ref(watch);
		ZEND_HASH_FOREACH_PTR(&watch->elements, element) {
			if (element->flags & PHPDBG_WATCH_RECURSIVE) {
				phpdbg_recurse_watch_element(element);
			}
		} ZEND_HASH_FOREACH_END();
	}

	phpdbg_watch_backup_data(watch);
}

void phpdbg_reenable_memory_watches(void) {
	zend_ulong page;
	phpdbg_btree_result *res;
	phpdbg_watchpoint_t *watch;

	ZEND_HASH_FOREACH_NUM_KEY(PHPDBG_G(watchlist_mem), page) {
		/* Disable writing again if there are any watchers on that page */
		res = phpdbg_btree_find_closest(&PHPDBG_G(watchpoint_tree), page + phpdbg_pagesize - 1);
		if (res) {
			watch = res->ptr;
			if ((char *) page < (char *) watch->addr.ptr + watch->size) {
				mprotect((void *) page, phpdbg_pagesize, PROT_READ);
			}
		}
	} ZEND_HASH_FOREACH_END();
	zend_hash_clean(PHPDBG_G(watchlist_mem));
}

int phpdbg_print_changed_zvals(void) {
	int ret;
	zend_ulong page;
	phpdbg_watchpoint_t *watch;
	phpdbg_btree_result *res;
	HashTable *mem_list = NULL;

	if (zend_hash_num_elements(&PHPDBG_G(watch_elements)) == 0) {
		return FAILURE;
	}

	if (zend_hash_num_elements(PHPDBG_G(watchlist_mem)) > 0) {
		/* we must not add elements to the hashtable while iterating over it (resize => read into freed memory) */
		mem_list = PHPDBG_G(watchlist_mem);
		PHPDBG_G(watchlist_mem) = PHPDBG_G(watchlist_mem_backup);

		ZEND_HASH_FOREACH_NUM_KEY(mem_list, page) {
			phpdbg_btree_position pos = phpdbg_btree_find_between(&PHPDBG_G(watchpoint_tree), page, page + phpdbg_pagesize);

			while ((res = phpdbg_btree_next(&pos))) {
				watch = res->ptr;
				phpdbg_check_watchpoint(watch);
			}
			if ((res = phpdbg_btree_find_closest(&PHPDBG_G(watchpoint_tree), page - 1))) {
				watch = res->ptr;
				if ((char *) page < (char *) watch->addr.ptr + watch->size) {
					phpdbg_check_watchpoint(watch);
				}
			}
		} ZEND_HASH_FOREACH_END();
	}

	phpdbg_dequeue_elements_for_recreation();

	phpdbg_reenable_memory_watches();

	if (mem_list) {
		PHPDBG_G(watchlist_mem) = mem_list;
		phpdbg_reenable_memory_watches();
	}

	ret = PHPDBG_G(watchpoint_hit) ? SUCCESS : FAILURE;
	PHPDBG_G(watchpoint_hit) = 0;

	return ret;
}

void phpdbg_watch_efree(void *ptr) {
	phpdbg_btree_result *result;

	/* only do expensive checks if there are any watches at all */
	if (zend_hash_num_elements(&PHPDBG_G(watch_elements))) {
		if ((result = phpdbg_btree_find(&PHPDBG_G(watchpoint_tree), (zend_ulong) ptr))) {
			phpdbg_watchpoint_t *watch = result->ptr;
			if (watch->type != WATCH_ON_HASHDATA) {
				phpdbg_remove_watchpoint(watch);
			} else {
				/* remove all linked watchpoints, they will be dissociated from their elements */
				phpdbg_watch_element *element;
				phpdbg_watch_ht_info *hti = (phpdbg_watch_ht_info *) watch;

				ZEND_HASH_FOREACH_PTR(&hti->watches, element) {
					zend_ulong num = zend_hash_num_elements(&hti->watches);
					phpdbg_remove_watchpoint(element->watch);
					if (num == 1) { /* prevent access into freed memory */
						break;
					}
				} ZEND_HASH_FOREACH_END();
			}
		}

		/* special case watchpoints as they aren't on ptr but on ptr + HT_WATCH_OFFSET */
		if ((result = phpdbg_btree_find(&PHPDBG_G(watchpoint_tree), HT_WATCH_OFFSET + (zend_ulong) ptr))) {
			phpdbg_watchpoint_t *watch = result->ptr;
			if (watch->type == WATCH_ON_HASHTABLE) {
				phpdbg_remove_watchpoint(watch);
			}
		}

		zend_hash_index_del(&PHPDBG_G(watch_free), (zend_ulong) ptr);
	}

	if (PHPDBG_G(original_free_function)) {
		PHPDBG_G(original_free_function)(ptr);
	}
}

/* ### USER API ### */
void phpdbg_list_watchpoints(void) {
	phpdbg_watch_element *element;

	phpdbg_xml("<watchlist %r>");

	ZEND_HASH_FOREACH_PTR(&PHPDBG_G(watch_elements), element) {
		phpdbg_writeln("watchvariable", "variable=\"%.*s\" on=\"%s\" type=\"%s\"", "%.*s (%s, %s)", (int) ZSTR_LEN(element->str), ZSTR_VAL(element->str), (element->flags & (PHPDBG_WATCH_ARRAY|PHPDBG_WATCH_OBJECT)) ? "array" : "variable", (element->flags & PHPDBG_WATCH_RECURSIVE) ? "recursive" : "simple");
	} ZEND_HASH_FOREACH_END();

	phpdbg_xml("</watchlist>");
}

static int phpdbg_create_simple_watchpoint(zval *zv, phpdbg_watch_element *element) {
	element->flags = PHPDBG_WATCH_SIMPLE;
	phpdbg_add_bucket_watch_element((Bucket *) zv, element);
	return SUCCESS;
}

static int phpdbg_create_array_watchpoint(zval *zv, phpdbg_watch_element *element) {
	phpdbg_watch_element *new;
	zend_string *str;
	zval *orig_zv = zv;

	ZVAL_DEREF(zv);
	if (Z_TYPE_P(zv) != IS_ARRAY && Z_TYPE_P(zv) != IS_OBJECT) {
		return FAILURE;
	}

	new = ecalloc(1, sizeof(phpdbg_watch_element));

	str = strpprintf(0, "%.*s[]", (int) ZSTR_LEN(element->str), ZSTR_VAL(element->str));
	zend_string_release(element->str);
	element->str = str;
	element->flags = PHPDBG_WATCH_IMPLICIT;
	phpdbg_add_bucket_watch_element((Bucket *) orig_zv, element);
	element->child = new;

	new->flags = PHPDBG_WATCH_SIMPLE;
	new->str = zend_string_copy(str);
	new->parent = element;
	phpdbg_add_ht_watch_element(zv, new);
	return SUCCESS;
}

static int phpdbg_create_recursive_watchpoint(zval *zv, phpdbg_watch_element *element) {
	element->flags = PHPDBG_WATCH_RECURSIVE | PHPDBG_WATCH_RECURSIVE_ROOT;
	element->child = NULL;
	phpdbg_add_bucket_watch_element((Bucket *) zv, element);
	return SUCCESS;
}

typedef struct { int (*callback)(zval *zv, phpdbg_watch_element *); zend_string *str; } phpdbg_watch_parse_struct;

static int phpdbg_watchpoint_parse_wrapper(char *name, size_t namelen, char *key, size_t keylen, HashTable *parent, zval *zv, phpdbg_watch_parse_struct *info) {
	int ret;
	phpdbg_watch_element *element = ecalloc(1, sizeof(phpdbg_watch_element));
	element->str = zend_string_init(name, namelen, 0);
	element->name_in_parent = zend_string_init(key, keylen, 0);
	element->parent_container = parent;
	element->parent = PHPDBG_G(watch_tmp);
	element->child = NULL;

	ret = info->callback(zv, element);

	efree(name);
	efree(key);

	if (ret != SUCCESS) {
		phpdbg_remove_watch_element(element);
	} else {
		if (PHPDBG_G(watch_tmp)) {
			PHPDBG_G(watch_tmp)->child = element;
		}

		if (element->child) {
			element = element->child;
		}

		/* work around missing API for extending an array with a new element, and getting its index */
		zend_hash_next_index_insert_ptr(&PHPDBG_G(watch_elements), element);
		element->id = PHPDBG_G(watch_elements).nNextFreeElement - 1;

		phpdbg_notice("watchadd", "index=\"%d\" variable=\"%.*s\"", "Added%s watchpoint #%u for %.*s", (element->flags & PHPDBG_WATCH_RECURSIVE_ROOT) ? " recursive" : "", element->id, (int) ZSTR_LEN(element->str), ZSTR_VAL(element->str));
	}

	PHPDBG_G(watch_tmp) = NULL;

	return ret;
}

PHPDBG_API int phpdbg_watchpoint_parse_input(char *input, size_t len, HashTable *parent, size_t i, phpdbg_watch_parse_struct *info, zend_bool silent) {
	return phpdbg_parse_variable_with_arg(input, len, parent, i, (phpdbg_parse_var_with_arg_func) phpdbg_watchpoint_parse_wrapper, NULL, 0, info);
}

static int phpdbg_watchpoint_parse_step(char *name, size_t namelen, char *key, size_t keylen, HashTable *parent, zval *zv, phpdbg_watch_parse_struct *info) {
	phpdbg_watch_element *element;

	/* do not install watch elements for references */
	if (PHPDBG_G(watch_tmp) && Z_ISREF_P(PHPDBG_G(watch_tmp)->watch->addr.zv) && Z_REFVAL_P(PHPDBG_G(watch_tmp)->watch->addr.zv) == zv) {
		efree(name);
		efree(key);
		return SUCCESS;
	}

	element = ecalloc(1, sizeof(phpdbg_watch_element));
	element->flags = PHPDBG_WATCH_IMPLICIT;
	element->str = zend_string_copy(info->str);
	element->name_in_parent = zend_string_init(key, keylen, 0);
	element->parent_container = parent;
	element->parent = PHPDBG_G(watch_tmp);
	element = phpdbg_add_bucket_watch_element((Bucket *) zv, element);

	efree(name);
	efree(key);

	if (PHPDBG_G(watch_tmp)) {
		PHPDBG_G(watch_tmp)->child = element;
	}
	PHPDBG_G(watch_tmp) = element;

	return SUCCESS;
}

static int phpdbg_watchpoint_parse_symtables(char *input, size_t len, int (*callback)(zval *, phpdbg_watch_element *)) {
	zend_class_entry *scope = zend_get_executed_scope();
	phpdbg_watch_parse_struct info;
	int ret;

	if (scope && len >= 5 && !memcmp("$this", input, 5)) {
		zend_hash_str_add(EG(current_execute_data)->symbol_table, ZEND_STRL("this"), &EG(current_execute_data)->This);
	}

	if (callback == phpdbg_create_array_watchpoint) {
		info.str = strpprintf(0, "%.*s[]", (int) len, input);
	} else {
		info.str = zend_string_init(input, len, 0);
	}
	info.callback = callback;

	if (phpdbg_is_auto_global(input, len) && phpdbg_watchpoint_parse_input(input, len, &EG(symbol_table), 0, &info, 1) != FAILURE) {
		zend_string_release(info.str);
		return SUCCESS;
	}

	ret = phpdbg_parse_variable_with_arg(input, len, EG(current_execute_data)->symbol_table, 0, (phpdbg_parse_var_with_arg_func) phpdbg_watchpoint_parse_wrapper, (phpdbg_parse_var_with_arg_func) phpdbg_watchpoint_parse_step, 0, &info);

	zend_string_release(info.str);
	return ret;
}

PHPDBG_WATCH(delete) /* {{{ */
{
	phpdbg_watch_element *element;
	switch (param->type) {
		case NUMERIC_PARAM:
			if ((element = zend_hash_index_find_ptr(&PHPDBG_G(watch_elements), param->num))) {
				phpdbg_remove_watch_element(element);
				phpdbg_notice("watchdelete", "variable=\"%.*s\"", "Removed watchpoint %d", (int) param->num);
			} else {
				phpdbg_error("watchdelete", "type=\"nowatch\"", "Nothing was deleted, no corresponding watchpoint found");
			}
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

int phpdbg_create_var_watchpoint(char *input, size_t len) {
	if (phpdbg_rebuild_symtable() == FAILURE) {
		return FAILURE;
	}

	return phpdbg_watchpoint_parse_symtables(input, len, phpdbg_create_simple_watchpoint);
}

PHPDBG_WATCH(recursive) /* {{{ */
{
	if (phpdbg_rebuild_symtable() == FAILURE) {
		return SUCCESS;
	}

	switch (param->type) {
		case STR_PARAM:
			phpdbg_watchpoint_parse_symtables(param->str, param->len, phpdbg_create_recursive_watchpoint);
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_WATCH(array) /* {{{ */
{
	if (phpdbg_rebuild_symtable() == FAILURE) {
		return SUCCESS;
	}

	switch (param->type) {
		case STR_PARAM:
			phpdbg_watchpoint_parse_symtables(param->str, param->len, phpdbg_create_array_watchpoint);
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */


void phpdbg_setup_watchpoints(void) {
#if defined(_SC_PAGE_SIZE)
	phpdbg_pagesize = sysconf(_SC_PAGE_SIZE);
#elif defined(_SC_PAGESIZE)
	phpdbg_pagesize = sysconf(_SC_PAGESIZE);
#elif defined(_SC_NUTC_OS_PAGESIZE)
	phpdbg_pagesize = sysconf(_SC_NUTC_OS_PAGESIZE);
#else
	phpdbg_pagesize = 4096; /* common pagesize */
#endif

	phpdbg_btree_init(&PHPDBG_G(watchpoint_tree), sizeof(void *) * 8);
	phpdbg_btree_init(&PHPDBG_G(watch_HashTables), sizeof(void *) * 8);
	zend_hash_init(&PHPDBG_G(watch_elements), 8, NULL, NULL, 0);
	zend_hash_init(&PHPDBG_G(watch_collisions), 8, NULL, NULL, 0);
	zend_hash_init(&PHPDBG_G(watch_recreation), 8, NULL, NULL, 0);
	zend_hash_init(&PHPDBG_G(watch_free), 8, NULL, NULL, 0);

	/* put these on a separate page, to avoid conflicts with other memory */
	PHPDBG_G(watchlist_mem) = malloc(phpdbg_pagesize > sizeof(HashTable) ? phpdbg_pagesize : sizeof(HashTable));
	zend_hash_init(PHPDBG_G(watchlist_mem), phpdbg_pagesize / (sizeof(Bucket) + sizeof(uint32_t)), NULL, NULL, 1);
	PHPDBG_G(watchlist_mem_backup) = malloc(phpdbg_pagesize > sizeof(HashTable) ? phpdbg_pagesize : sizeof(HashTable));
	zend_hash_init(PHPDBG_G(watchlist_mem_backup), phpdbg_pagesize / (sizeof(Bucket) + sizeof(uint32_t)), NULL, NULL, 1);

	PHPDBG_G(watch_tmp) = NULL;
}

void phpdbg_destroy_watchpoints(void) {
	phpdbg_watch_element *element;
	phpdbg_btree_position pos;
	phpdbg_btree_result *res;

	/* unconditionally free all remaining elements to avoid memory leaks */
	ZEND_HASH_FOREACH_PTR(&PHPDBG_G(watch_recreation), element) {
		phpdbg_automatic_dequeue_free(element);
	} ZEND_HASH_FOREACH_END();

	/* upon fatal errors etc. (i.e. CG(unclean_shutdown) == 1), some watchpoints may still be active. Ensure memory is not watched anymore for next run. Do not care about memory freeing here, shutdown is unclean and near anyway. */
	pos = phpdbg_btree_find_between(&PHPDBG_G(watchpoint_tree), 0, -1);
	while ((res = phpdbg_btree_next(&pos))) {
		phpdbg_deactivate_watchpoint(res->ptr);
	}

	zend_hash_destroy(&PHPDBG_G(watch_elements)); PHPDBG_G(watch_elements).nNumOfElements = 0; /* phpdbg_watch_efree() is checking against this arrays size */
	zend_hash_destroy(&PHPDBG_G(watch_recreation));
	zend_hash_destroy(&PHPDBG_G(watch_free));
	zend_hash_destroy(&PHPDBG_G(watch_collisions));
	zend_hash_destroy(PHPDBG_G(watchlist_mem));
	free(PHPDBG_G(watchlist_mem));
	zend_hash_destroy(PHPDBG_G(watchlist_mem_backup));
	free(PHPDBG_G(watchlist_mem_backup));
}

void phpdbg_purge_watchpoint_tree(void) {
	phpdbg_btree_position pos;
	phpdbg_btree_result *res;

	pos = phpdbg_btree_find_between(&PHPDBG_G(watchpoint_tree), 0, -1);
	while ((res = phpdbg_btree_next(&pos))) {
		phpdbg_deactivate_watchpoint(res->ptr);
	}
}
