/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2003 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Author: Harald Radi <harald.radi@nme.at>                             |
   +----------------------------------------------------------------------+
*/


#ifndef ZEND_INI_ENTRY_H
#define ZEND_INI_ENTRY_H

#define ZEND_INI_MH(name) int name(zend_ini_entry *entry, char *new_value, uint new_value_length, void *mh_arg1, void *mh_arg2, void *mh_arg3, int stage TSRMLS_DC)
#define ZEND_INI_DISP(name) void name(zend_ini_entry *ini_entry, int type)

typedef struct _zend_ini_entry zend_ini_entry;

struct _zend_ini_entry {
	int module_number;
	int modifyable;
	char *name;
	uint name_length;
	ZEND_INI_MH((*on_modify));
	void *mh_arg1;
	void *mh_arg2;
	void *mh_arg3;

	char *value;
	uint value_length;

	char *orig_value;
	uint orig_value_length;
	int modified;

	void (*displayer)(zend_ini_entry *ini_entry, int type);
};

#endif /* ZEND_INI_ENTRY_H */