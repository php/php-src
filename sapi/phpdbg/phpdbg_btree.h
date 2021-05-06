/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#ifndef PHPDBG_BTREE_H
#define PHPDBG_BTREE_H

#include "zend.h"

typedef struct {
	zend_ulong idx;
	void *ptr;
} phpdbg_btree_result;

typedef union _phpdbg_btree_branch phpdbg_btree_branch;
union _phpdbg_btree_branch {
	phpdbg_btree_branch *branches[2];
	phpdbg_btree_result result;
};

typedef struct {
	zend_ulong count;
	zend_ulong depth;
	bool persistent;
	phpdbg_btree_branch *branch;
} phpdbg_btree;

typedef struct {
	phpdbg_btree *tree;
	zend_ulong cur;
	zend_ulong end;
} phpdbg_btree_position;

void phpdbg_btree_init(phpdbg_btree *tree, zend_ulong depth);
void phpdbg_btree_clean(phpdbg_btree *tree);
phpdbg_btree_result *phpdbg_btree_find(phpdbg_btree *tree, zend_ulong idx);
phpdbg_btree_result *phpdbg_btree_find_closest(phpdbg_btree *tree, zend_ulong idx);
phpdbg_btree_position phpdbg_btree_find_between(phpdbg_btree *tree, zend_ulong lower_idx, zend_ulong higher_idx);
phpdbg_btree_result *phpdbg_btree_next(phpdbg_btree_position *pos);
int phpdbg_btree_delete(phpdbg_btree *tree, zend_ulong idx);

#define PHPDBG_BTREE_INSERT 1
#define PHPDBG_BTREE_UPDATE 2
#define PHPDBG_BTREE_OVERWRITE (PHPDBG_BTREE_INSERT | PHPDBG_BTREE_UPDATE)

int phpdbg_btree_insert_or_update(phpdbg_btree *tree, zend_ulong idx, void *ptr, int flags);
#define phpdbg_btree_insert(tree, idx, ptr) phpdbg_btree_insert_or_update(tree, idx, ptr, PHPDBG_BTREE_INSERT)
#define phpdbg_btree_update(tree, idx, ptr) phpdbg_btree_insert_or_update(tree, idx, ptr, PHPDBG_BTREE_UPDATE)
#define phpdbg_btree_overwrite(tree, idx, ptr) phpdbg_btree_insert_or_update(tree, idx, ptr, PHPDBG_BTREE_OVERWRITE)


/* debugging functions */
void phpdbg_btree_branch_dump(phpdbg_btree_branch *branch, zend_ulong depth);
void phpdbg_btree_dump(phpdbg_btree *tree);

#endif
