/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
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

#include "zend.h"

#include "php.h"
#include "phpdbg.h"
#include "phpdbg_opcode.h"
#include "phpdbg_utils.h"
#include "ext/standard/php_string.h"

/* FASYNC under Solaris */
#ifdef HAVE_SYS_FILE_H
# include <sys/file.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
# include "sys/ioctl.h"
# ifndef GWINSZ_IN_SYS_IOCTL
#  include <termios.h>
# endif
#endif

ZEND_EXTERN_MODULE_GLOBALS(phpdbg)

/* {{{ color structures */
const static phpdbg_color_t colors[] = {
	PHPDBG_COLOR_D("none",             "0;0"),

	PHPDBG_COLOR_D("white",            "0;64"),
	PHPDBG_COLOR_D("white-bold",       "1;64"),
	PHPDBG_COLOR_D("white-underline",  "4;64"),
	PHPDBG_COLOR_D("red",              "0;31"),
	PHPDBG_COLOR_D("red-bold",         "1;31"),
	PHPDBG_COLOR_D("red-underline",    "4;31"),
	PHPDBG_COLOR_D("green",            "0;32"),
	PHPDBG_COLOR_D("green-bold",       "1;32"),
	PHPDBG_COLOR_D("green-underline",  "4;32"),
	PHPDBG_COLOR_D("yellow",           "0;33"),
	PHPDBG_COLOR_D("yellow-bold",      "1;33"),
	PHPDBG_COLOR_D("yellow-underline", "4;33"),
	PHPDBG_COLOR_D("blue",             "0;34"),
	PHPDBG_COLOR_D("blue-bold",        "1;34"),
	PHPDBG_COLOR_D("blue-underline",   "4;34"),
	PHPDBG_COLOR_D("purple",           "0;35"),
	PHPDBG_COLOR_D("purple-bold",      "1;35"),
	PHPDBG_COLOR_D("purple-underline", "4;35"),
	PHPDBG_COLOR_D("cyan",             "0;36"),
	PHPDBG_COLOR_D("cyan-bold",        "1;36"),
	PHPDBG_COLOR_D("cyan-underline",   "4;36"),
	PHPDBG_COLOR_D("black",            "0;30"),
	PHPDBG_COLOR_D("black-bold",       "1;30"),
	PHPDBG_COLOR_D("black-underline",  "4;30"),
	PHPDBG_COLOR_END
}; /* }}} */

/* {{{ */
const static phpdbg_element_t elements[] = {
	PHPDBG_ELEMENT_D("prompt", PHPDBG_COLOR_PROMPT),
	PHPDBG_ELEMENT_D("error", PHPDBG_COLOR_ERROR),
	PHPDBG_ELEMENT_D("notice", PHPDBG_COLOR_NOTICE),
	PHPDBG_ELEMENT_END
}; /* }}} */

PHPDBG_API int phpdbg_is_numeric(const char *str) /* {{{ */
{
	if (!str)
		return 0;

	for (; *str; str++) {
		if (isspace(*str) || *str == '-') {
			continue;
		}
		return isdigit(*str);
	}
	return 0;
} /* }}} */

PHPDBG_API int phpdbg_is_empty(const char *str) /* {{{ */
{
	if (!str)
		return 1;

	for (; *str; str++) {
		if (isspace(*str)) {
			continue;
		}
		return 0;
	}
	return 1;
} /* }}} */

PHPDBG_API int phpdbg_is_addr(const char *str) /* {{{ */
{
	return str[0] && str[1] && memcmp(str, "0x", 2) == 0;
} /* }}} */

PHPDBG_API int phpdbg_is_class_method(const char *str, size_t len, char **class, char **method) /* {{{ */
{
	char *sep = NULL;

	if (strstr(str, "#") != NULL)
		return 0;

	if (strstr(str, " ") != NULL)
		return 0;

	sep = strstr(str, "::");

	if (!sep || sep == str || sep+2 == str+len-1) {
		return 0;
	}

	if (class != NULL) {

		if (str[0] == '\\') {
			str++;
			len--;
		}

		*class = estrndup(str, sep - str);
		(*class)[sep - str] = 0;
	}

	if (method != NULL) {
		*method = estrndup(sep+2, str + len - (sep + 2));
	}

	return 1;
} /* }}} */

PHPDBG_API char *phpdbg_resolve_path(const char *path) /* {{{ */
{
	char resolved_name[MAXPATHLEN];

	if (expand_filepath(path, resolved_name) == NULL) {
		return NULL;
	}

	return estrdup(resolved_name);
} /* }}} */

PHPDBG_API const char *phpdbg_current_file(void) /* {{{ */
{
	const char *file = zend_get_executed_filename();

	if (memcmp(file, "[no active file]", sizeof("[no active file]")) == 0) {
		return PHPDBG_G(exec);
	}

	return file;
} /* }}} */

PHPDBG_API const zend_function *phpdbg_get_function(const char *fname, const char *cname) /* {{{ */
{
	zend_function *func = NULL;
	zend_string *lfname = zend_string_init(fname, strlen(fname), 0);
	zend_string *tmp = zend_string_tolower(lfname);
	zend_string_release(lfname);
	lfname = tmp;

	if (cname) {
		zend_class_entry *ce;
		zend_string *lcname = zend_string_init(cname, strlen(cname), 0);
		tmp = zend_string_tolower(lcname);
		zend_string_release(lcname);
		lcname = tmp;
		ce = zend_lookup_class(lcname);

		zend_string_release(lcname);

		if (ce) {
			func = zend_hash_find_ptr(&ce->function_table, lfname);
		}
	} else {
		func = zend_hash_find_ptr(EG(function_table), lfname);
	}

	zend_string_release(lfname);
	return func;
} /* }}} */

PHPDBG_API char *phpdbg_trim(const char *str, size_t len, size_t *new_len) /* {{{ */
{
	const char *p = str;
	char *new = NULL;

	while (p && isspace(*p)) {
		++p;
		--len;
	}

	while (*p && isspace(*(p + len -1))) {
		--len;
	}

	if (len == 0) {
		new = estrndup("", sizeof(""));
		*new_len = 0;
	} else {
		new = estrndup(p, len);
		*(new + len) = '\0';

		if (new_len) {
			*new_len = len;
		}
	}

	return new;

} /* }}} */

PHPDBG_API const phpdbg_color_t *phpdbg_get_color(const char *name, size_t name_length) /* {{{ */
{
	const phpdbg_color_t *color = colors;

	while (color && color->name) {
		if (name_length == color->name_length &&
			memcmp(name, color->name, name_length) == SUCCESS) {
			phpdbg_debug("phpdbg_get_color(%s, %lu): %s", name, name_length, color->code);
			return color;
		}
		++color;
	}

	phpdbg_debug("phpdbg_get_color(%s, %lu): failed", name, name_length);

	return NULL;
} /* }}} */

PHPDBG_API void phpdbg_set_color(int element, const phpdbg_color_t *color) /* {{{ */
{
	PHPDBG_G(colors)[element] = color;
} /* }}} */

PHPDBG_API void phpdbg_set_color_ex(int element, const char *name, size_t name_length) /* {{{ */
{
	const phpdbg_color_t *color = phpdbg_get_color(name, name_length);

	if (color) {
		phpdbg_set_color(element, color);
	} else PHPDBG_G(colors)[element] = colors;
} /* }}} */

PHPDBG_API const phpdbg_color_t* phpdbg_get_colors(void) /* {{{ */
{
	return colors;
} /* }}} */

PHPDBG_API int phpdbg_get_element(const char *name, size_t len) {
	const phpdbg_element_t *element = elements;

	while (element && element->name) {
		if (len == element->name_length) {
			if (strncasecmp(name, element->name, len) == SUCCESS) {
				return element->id;
			}
		}
		element++;
	}

	return PHPDBG_COLOR_INVALID;
}

PHPDBG_API void phpdbg_set_prompt(const char *prompt) /* {{{ */
{
	/* free formatted prompt */
	if (PHPDBG_G(prompt)[1]) {
		free(PHPDBG_G(prompt)[1]);
		PHPDBG_G(prompt)[1] = NULL;
	}
	/* free old prompt */
	if (PHPDBG_G(prompt)[0]) {
		free(PHPDBG_G(prompt)[0]);
		PHPDBG_G(prompt)[0] = NULL;
	}

	/* copy new prompt */
	PHPDBG_G(prompt)[0] = strdup(prompt);
} /* }}} */

PHPDBG_API const char *phpdbg_get_prompt(void) /* {{{ */
{
	/* find cached prompt */
	if (PHPDBG_G(prompt)[1]) {
		return PHPDBG_G(prompt)[1];
	}

	/* create cached prompt */
#ifndef HAVE_LIBEDIT
	/* TODO: libedit doesn't seems to support coloured prompt */
	if ((PHPDBG_G(flags) & PHPDBG_IS_COLOURED)) {
		ZEND_IGNORE_VALUE(asprintf(&PHPDBG_G(prompt)[1], "\033[%sm%s\033[0m ",
			PHPDBG_G(colors)[PHPDBG_COLOR_PROMPT]->code,
			PHPDBG_G(prompt)[0]));
	} else
#endif
	{
		ZEND_IGNORE_VALUE(asprintf(&PHPDBG_G(prompt)[1], "%s ", PHPDBG_G(prompt)[0]));
	}

	return PHPDBG_G(prompt)[1];
} /* }}} */

int phpdbg_rebuild_symtable(void) {
	if (!EG(current_execute_data) || !EG(current_execute_data)->func) {
		phpdbg_error("inactive", "type=\"op_array\"", "No active op array!");
		return FAILURE;
	}

	if (!zend_rebuild_symbol_table()) {
		phpdbg_error("inactive", "type=\"symbol_table\"", "No active symbol table!");
		return FAILURE;
	}

	return SUCCESS;
}

PHPDBG_API int phpdbg_get_terminal_width(void) /* {{{ */
{
	int columns;
#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
#elif defined(HAVE_SYS_IOCTL_H) && defined(TIOCGWINSZ)
	struct winsize w;

	columns = ioctl(fileno(stdout), TIOCGWINSZ, &w) == 0 ? w.ws_col : 80;
#else
	columns = 80;
#endif
	return columns;
} /* }}} */

PHPDBG_API int phpdbg_get_terminal_height(void) /* {{{ */
{
	int lines;
#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	lines = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#elif defined(HAVE_SYS_IOCTL_H) && defined(TIOCGWINSZ)
	struct winsize w;

	lines = ioctl(fileno(stdout), TIOCGWINSZ, &w) == 0 ? w.ws_row : 40;
#else
	lines = 40;
#endif
	return lines;
} /* }}} */

PHPDBG_API void phpdbg_set_async_io(int fd) {
#if !defined(_WIN32) && defined(FASYNC)
	int flags;
	fcntl(STDIN_FILENO, F_SETOWN, getpid());
	flags = fcntl(STDIN_FILENO, F_GETFL);
	fcntl(STDIN_FILENO, F_SETFL, flags | FASYNC);
#endif
}

int phpdbg_safe_class_lookup(const char *name, int name_length, zend_class_entry **ce) {
	if (PHPDBG_G(flags) & PHPDBG_IN_SIGNAL_HANDLER) {
		char *lc_name, *lc_free;
		int lc_length;

		if (name == NULL || !name_length) {
			return FAILURE;
		}

		lc_free = lc_name = emalloc(name_length + 1);
		zend_str_tolower_copy(lc_name, name, name_length);
		lc_length = name_length + 1;

		if (lc_name[0] == '\\') {
			lc_name += 1;
			lc_length -= 1;
		}

		phpdbg_try_access {
			*ce = zend_hash_str_find_ptr(EG(class_table), lc_name, lc_length);
		} phpdbg_catch_access {
			phpdbg_error("signalsegv", "class=\"%.*s\"", "Could not fetch class %.*s, invalid data source", name_length, name);
		} phpdbg_end_try_access();

		efree(lc_free);
	} else {
		zend_string *str_name = zend_string_init(name, name_length, 0);
		*ce = zend_lookup_class(str_name);
		efree(str_name);
	}

	return *ce ? SUCCESS : FAILURE;
}

char *phpdbg_get_property_key(char *key) {
	if (*key != 0) {
		return key;
	}
	return strchr(key + 1, 0) + 1;
}

static int phpdbg_parse_variable_arg_wrapper(char *name, size_t len, char *keyname, size_t keylen, HashTable *parent, zval *zv, phpdbg_parse_var_func callback) {
	return callback(name, len, keyname, keylen, parent, zv);
}

PHPDBG_API int phpdbg_parse_variable(char *input, size_t len, HashTable *parent, size_t i, phpdbg_parse_var_func callback, zend_bool silent) {
	return phpdbg_parse_variable_with_arg(input, len, parent, i, (phpdbg_parse_var_with_arg_func) phpdbg_parse_variable_arg_wrapper, NULL, silent, callback);
}

PHPDBG_API int phpdbg_parse_variable_with_arg(char *input, size_t len, HashTable *parent, size_t i, phpdbg_parse_var_with_arg_func callback, phpdbg_parse_var_with_arg_func step_cb, zend_bool silent, void *arg) {
	int ret = FAILURE;
	zend_bool new_index = 1;
	char *last_index;
	size_t index_len = 0;
	zval *zv;

	if (len < 2 || *input != '$') {
		goto error;
	}

	while (i++ < len) {
		if (i == len) {
			new_index = 1;
		} else {
			switch (input[i]) {
				case '[':
					new_index = 1;
					break;
				case ']':
					break;
				case '>':
					if (last_index[index_len - 1] == '-') {
						new_index = 1;
						index_len--;
					}
					break;

				default:
					if (new_index) {
						last_index = input + i;
						new_index = 0;
					}
					if (input[i - 1] == ']') {
						goto error;
					}
					index_len++;
			}
		}

		if (new_index && index_len == 0) {
			zend_ulong numkey;
			zend_string *strkey;
			ZEND_HASH_FOREACH_KEY_PTR(parent, numkey, strkey, zv) {
				while (Z_TYPE_P(zv) == IS_INDIRECT) {
					zv = Z_INDIRECT_P(zv);
				}

				if (i == len || (i == len - 1 && input[len - 1] == ']')) {
					char *key, *propkey;
					size_t namelen, keylen;
					char *name;
					char *keyname = estrndup(last_index, index_len);
					if (strkey) {
						key = ZSTR_VAL(strkey);
						keylen = ZSTR_LEN(strkey);
					} else {
						keylen = spprintf(&key, 0, ZEND_ULONG_FMT, numkey);
					}
					propkey = phpdbg_get_property_key(key);
					name = emalloc(i + keylen + 2);
					namelen = sprintf(name, "%.*s%.*s%s", (int) i, input, (int) (keylen - (propkey - key)), propkey, input[len - 1] == ']'?"]":"");
					if (!strkey) {
						efree(key);
					}

					ret = callback(name, namelen, keyname, index_len, parent, zv, arg) == SUCCESS || ret == SUCCESS?SUCCESS:FAILURE;
				} else retry_ref: if (Z_TYPE_P(zv) == IS_OBJECT) {
					if (step_cb) {
						char *name = estrndup(input, i);
						char *keyname = estrndup(last_index, index_len);

						ret = step_cb(name, i, keyname, index_len, parent, zv, arg) == SUCCESS || ret == SUCCESS?SUCCESS:FAILURE;
					}

					phpdbg_parse_variable_with_arg(input, len, Z_OBJPROP_P(zv), i, callback, step_cb, silent, arg);
				} else if (Z_TYPE_P(zv) == IS_ARRAY) {
					if (step_cb) {
						char *name = estrndup(input, i);
						char *keyname = estrndup(last_index, index_len);

						ret = step_cb(name, i, keyname, index_len, parent, zv, arg) == SUCCESS || ret == SUCCESS?SUCCESS:FAILURE;
					}

					phpdbg_parse_variable_with_arg(input, len, Z_ARRVAL_P(zv), i, callback, step_cb, silent, arg);
				} else if (Z_ISREF_P(zv)) {
					if (step_cb) {
						char *name = estrndup(input, i);
						char *keyname = estrndup(last_index, index_len);

						ret = step_cb(name, i, keyname, index_len, parent, zv, arg) == SUCCESS || ret == SUCCESS?SUCCESS:FAILURE;
					}

					ZVAL_DEREF(zv);
					goto retry_ref;
				} else {
					/* Ignore silently */
				}
			} ZEND_HASH_FOREACH_END();
			return ret;
		} else if (new_index) {
			char last_chr = last_index[index_len];
			last_index[index_len] = 0;
			if (!(zv = zend_symtable_str_find(parent, last_index, index_len))) {
				if (!silent) {
					phpdbg_error("variable", "type=\"undefined\" variable=\"%.*s\"", "%.*s is undefined", (int) i, input);
				}
				return FAILURE;
			}
			while (Z_TYPE_P(zv) == IS_INDIRECT) {
				zv = Z_INDIRECT_P(zv);
			}

			last_index[index_len] = last_chr;
			if (i == len) {
				char *name = estrndup(input, i);
				char *keyname = estrndup(last_index, index_len);

				ret = callback(name, i, keyname, index_len, parent, zv, arg) == SUCCESS || ret == SUCCESS?SUCCESS:FAILURE;
			} else retry_ref_end: if (Z_TYPE_P(zv) == IS_OBJECT) {
				if (step_cb) {
					char *name = estrndup(input, i);
					char *keyname = estrndup(last_index, index_len);

					ret = step_cb(name, i, keyname, index_len, parent, zv, arg) == SUCCESS || ret == SUCCESS?SUCCESS:FAILURE;
				}

				parent = Z_OBJPROP_P(zv);
			} else if (Z_TYPE_P(zv) == IS_ARRAY) {
				if (step_cb) {
					char *name = estrndup(input, i);
					char *keyname = estrndup(last_index, index_len);

					ret = step_cb(name, i, keyname, index_len, parent, zv, arg) == SUCCESS || ret == SUCCESS?SUCCESS:FAILURE;
				}

				parent = Z_ARRVAL_P(zv);
			} else if (Z_ISREF_P(zv)) {
				if (step_cb) {
					char *name = estrndup(input, i);
					char *keyname = estrndup(last_index, index_len);

					ret = step_cb(name, i, keyname, index_len, parent, zv, arg) == SUCCESS || ret == SUCCESS?SUCCESS:FAILURE;
				}

				ZVAL_DEREF(zv);
				goto retry_ref_end;
			} else {
				phpdbg_error("variable", "type=\"notiterable\" variable=\"%.*s\"", "%.*s is nor an array nor an object", (int) (input[i] == '>' ? i - 1 : i), input);
				return FAILURE;
			}
			index_len = 0;
		}
	}

	return ret;
	error:
		phpdbg_error("variable", "type=\"invalidinput\"", "Malformed input");
		return FAILURE;
}

int phpdbg_is_auto_global(char *name, int len) {
	return zend_is_auto_global_str(name, len);
}

static int phpdbg_xml_array_element_dump(zval *zv, zend_string *key, zend_ulong num) {
	phpdbg_xml("<element");

	phpdbg_try_access {
		if (key) { /* string key */
			phpdbg_xml(" name=\"%.*s\"", (int) ZSTR_LEN(key), ZSTR_VAL(key));
		} else { /* numeric key */
			phpdbg_xml(" name=\"%ld\"", num);
		}
	} phpdbg_catch_access {
		phpdbg_xml(" severity=\"error\" ></element>");
		return 0;
	} phpdbg_end_try_access();

	phpdbg_xml(">");

	phpdbg_xml_var_dump(zv);

	phpdbg_xml("</element>");

	return 0;
}

static int phpdbg_xml_object_property_dump(zval *zv, zend_string *key, zend_ulong num) {
	phpdbg_xml("<property");

	phpdbg_try_access {
		if (key) { /* string key */
			const char *prop_name, *class_name;
			int unmangle = zend_unmangle_property_name(key, &class_name, &prop_name);

			if (class_name && unmangle == SUCCESS) {
				phpdbg_xml(" name=\"%s\"", prop_name);
				if (class_name[0] == '*') {
					phpdbg_xml(" protection=\"protected\"");
				} else {
					phpdbg_xml(" class=\"%s\" protection=\"private\"", class_name);
				}
			} else {
				phpdbg_xml(" name=\"%.*s\" protection=\"public\"", (int) ZSTR_LEN(key), ZSTR_VAL(key));
			}
		} else { /* numeric key */
			phpdbg_xml(" name=\"%ld\" protection=\"public\"", num);
		}
	} phpdbg_catch_access {
		phpdbg_xml(" severity=\"error\" ></property>");
		return 0;
	} phpdbg_end_try_access();

	phpdbg_xml(">");

	phpdbg_xml_var_dump(zv);

	phpdbg_xml("</property>");

	return 0;
}

#define COMMON (is_ref ? "&" : "")

PHPDBG_API void phpdbg_xml_var_dump(zval *zv) {
	HashTable *myht;
	zend_string *class_name, *key;
	zend_ulong num;
	zval *val;
	int (*element_dump_func)(zval *zv, zend_string *key, zend_ulong num);
	zend_bool is_ref = 0;

	int is_temp;

	phpdbg_try_access {
		is_ref = Z_ISREF_P(zv) && GC_REFCOUNT(Z_COUNTED_P(zv)) > 1;
		ZVAL_DEREF(zv);

		switch (Z_TYPE_P(zv)) {
			case IS_TRUE:
				phpdbg_xml("<bool refstatus=\"%s\" value=\"true\" />", COMMON);
				break;
			case IS_FALSE:
				phpdbg_xml("<bool refstatus=\"%s\" value=\"false\" />", COMMON);
				break;
			case IS_NULL:
				phpdbg_xml("<null refstatus=\"%s\" />", COMMON);
				break;
			case IS_LONG:
				phpdbg_xml("<int refstatus=\"%s\" value=\"" ZEND_LONG_FMT "\" />", COMMON, Z_LVAL_P(zv));
				break;
			case IS_DOUBLE:
				phpdbg_xml("<float refstatus=\"%s\" value=\"%.*G\" />", COMMON, (int) EG(precision), Z_DVAL_P(zv));
				break;
			case IS_STRING:
				phpdbg_xml("<string refstatus=\"%s\" length=\"%zd\" value=\"%.*s\" />", COMMON, Z_STRLEN_P(zv), (int) Z_STRLEN_P(zv), Z_STRVAL_P(zv));
				break;
			case IS_ARRAY:
				myht = Z_ARRVAL_P(zv);
				if (ZEND_HASH_APPLY_PROTECTION(myht) && ++myht->u.v.nApplyCount > 1) {
					phpdbg_xml("<recursion />");
					--myht->u.v.nApplyCount;
					break;
				}
				phpdbg_xml("<array refstatus=\"%s\" num=\"%d\">", COMMON, zend_hash_num_elements(myht));
				element_dump_func = phpdbg_xml_array_element_dump;
				is_temp = 0;
				goto head_done;
			case IS_OBJECT:
				myht = Z_OBJDEBUG_P(zv, is_temp);
				if (myht && ++myht->u.v.nApplyCount > 1) {
					phpdbg_xml("<recursion />");
					--myht->u.v.nApplyCount;
					break;
				}

				class_name = Z_OBJ_HANDLER_P(zv, get_class_name)(Z_OBJ_P(zv));
				phpdbg_xml("<object refstatus=\"%s\" class=\"%.*s\" id=\"%d\" num=\"%d\">", COMMON, (int) ZSTR_LEN(class_name), ZSTR_VAL(class_name), Z_OBJ_HANDLE_P(zv), myht ? zend_hash_num_elements(myht) : 0);
				zend_string_release(class_name);

				element_dump_func = phpdbg_xml_object_property_dump;
head_done:
				if (myht) {
					ZEND_HASH_FOREACH_KEY_VAL_IND(myht, num, key, val) {
						element_dump_func(val, key, num);
					} ZEND_HASH_FOREACH_END();
					zend_hash_apply_with_arguments(myht, (apply_func_args_t) element_dump_func, 0);
					--myht->u.v.nApplyCount;
					if (is_temp) {
						zend_hash_destroy(myht);
						efree(myht);
					}
				}
				if (Z_TYPE_P(zv) == IS_ARRAY) {
					phpdbg_xml("</array>");
				} else {
					phpdbg_xml("</object>");
				}
				break;
			case IS_RESOURCE: {
				const char *type_name = zend_rsrc_list_get_rsrc_type(Z_RES_P(zv));
				phpdbg_xml("<resource refstatus=\"%s\" id=\"%pd\" type=\"%s\" />", COMMON, Z_RES_P(zv)->handle, type_name ? type_name : "unknown");
				break;
			}
			default:
				break;
		}
	} phpdbg_end_try_access();
}

PHPDBG_API zend_bool phpdbg_check_caught_ex(zend_execute_data *execute_data, zend_object *exception) {
	const zend_op *op;
	zend_op *cur;
	uint32_t op_num, i;
	zend_op_array *op_array = &execute_data->func->op_array;

	if (execute_data->opline >= EG(exception_op) && execute_data->opline < EG(exception_op) + 3) {
		op = EG(opline_before_exception);
	} else {
		op = execute_data->opline;
	}

	op_num = op - op_array->opcodes;

	for (i = 0; i < op_array->last_try_catch && op_array->try_catch_array[i].try_op <= op_num; i++) {
		uint32_t catch = op_array->try_catch_array[i].catch_op, finally = op_array->try_catch_array[i].finally_op;
		if (op_num <= catch || op_num <= finally) {
			if (finally) {
				return 1;
			}

			do {
				zend_class_entry *ce;
				cur = &op_array->opcodes[catch];

				if (!(ce = CACHED_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(cur->op1))))) {
					ce = zend_fetch_class_by_name(Z_STR_P(EX_CONSTANT(cur->op1)), EX_CONSTANT(cur->op1) + 1, ZEND_FETCH_CLASS_NO_AUTOLOAD);
					CACHE_PTR(Z_CACHE_SLOT_P(EX_CONSTANT(cur->op1)), ce);
				}

				if (ce == exception->ce || (ce && instanceof_function(exception->ce, ce))) {
					return 1;
				}

				catch = cur->extended_value;
			} while (!cur->result.num);

			return 0;
		}
	}

	return op->opcode == ZEND_CATCH;
}

char *phpdbg_short_zval_print(zval *zv, int maxlen) /* {{{ */
{
	char *decode = NULL;

	switch (Z_TYPE_P(zv)) {
		case IS_UNDEF:
			decode = estrdup("");
			break;
		case IS_NULL:
			decode = estrdup("null");
			break;
		case IS_FALSE:
			decode = estrdup("false");
			break;
		case IS_TRUE:
			decode = estrdup("true");
			break;
		case IS_LONG:
			spprintf(&decode, 0, ZEND_LONG_FMT, Z_LVAL_P(zv));
			break;
		case IS_DOUBLE:
			spprintf(&decode, 0, "%.*G", 14, Z_DVAL_P(zv));
			break;
		case IS_STRING: {
			int i;
			zend_string *str = php_addcslashes(Z_STR_P(zv), 0, "\\\"", 2);
			for (i = 0; i < ZSTR_LEN(str); i++) {
				if (ZSTR_VAL(str)[i] < 32) {
					ZSTR_VAL(str)[i] = ' ';
				}
			}
			spprintf(&decode, 0, "\"%.*s\"%c",
				ZSTR_LEN(str) <= maxlen - 2 ? (int) ZSTR_LEN(str) : (maxlen - 3),
				ZSTR_VAL(str), ZSTR_LEN(str) <= maxlen - 2 ? 0 : '+');
			zend_string_release(str);
			} break;
		case IS_RESOURCE:
			spprintf(&decode, 0, "Rsrc #%d", Z_RES_HANDLE_P(zv));
			break;
		case IS_ARRAY:
			spprintf(&decode, 0, "array(%d)", zend_hash_num_elements(Z_ARR_P(zv)));
			break;
		case IS_OBJECT: {
			zend_string *str = Z_OBJCE_P(zv)->name;
			spprintf(&decode, 0, "%.*s%c",
				ZSTR_LEN(str) <= maxlen ? (int) ZSTR_LEN(str) : maxlen - 1,
				ZSTR_VAL(str), ZSTR_LEN(str) <= maxlen ? 0 : '+');
			break;
		}
		case IS_CONSTANT:
			decode = estrdup("<constant>");
			break;
		case IS_CONSTANT_AST:
			decode = estrdup("<ast>");
			break;
		default:
			spprintf(&decode, 0, "unknown type: %d", Z_TYPE_P(zv));
			break;
	}

	return decode;
} /* }}} */
