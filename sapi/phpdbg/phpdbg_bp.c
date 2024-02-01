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

#include "zend.h"
#include "zend_hash.h"
#include "phpdbg.h"
#include "phpdbg_bp.h"
#include "phpdbg_utils.h"
#include "zend_globals.h"
#include "ext/standard/php_string.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg)

/* {{{ private api functions */
static inline phpdbg_breakbase_t *phpdbg_find_breakpoint_file(zend_op_array*);
static inline phpdbg_breakbase_t *phpdbg_find_breakpoint_symbol(zend_function*);
static inline phpdbg_breakbase_t *phpdbg_find_breakpoint_method(zend_op_array*);
static inline phpdbg_breakbase_t *phpdbg_find_breakpoint_opline(phpdbg_opline_ptr_t);
static inline phpdbg_breakbase_t *phpdbg_find_breakpoint_opcode(zend_uchar);
static inline phpdbg_breakbase_t *phpdbg_find_conditional_breakpoint(zend_execute_data *execute_data); /* }}} */

/*
* Note:
*	A break point must always set the correct id and type
*	A set breakpoint function must always map new points
*/
static inline void _phpdbg_break_mapping(int id, HashTable *table) /* {{{ */
{
	zend_hash_index_update_ptr(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP], id, table);
}
/* }}} */

#define PHPDBG_BREAK_MAPPING(id, table) _phpdbg_break_mapping(id, table)
#define PHPDBG_BREAK_UNMAPPING(id) \
	zend_hash_index_del(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP], (id))

#define PHPDBG_BREAK_INIT(b, t) do {\
	memset(&b, 0, sizeof(b)); \
	b.id = PHPDBG_G(bp_count)++; \
	b.type = t; \
	b.disabled = 0;\
	b.hits = 0; \
} while(0)

static void phpdbg_file_breaks_dtor(zval *data) /* {{{ */
{
	phpdbg_breakfile_t *bp = (phpdbg_breakfile_t*) Z_PTR_P(data);

	efree((char*)bp->filename);
	efree(bp);
} /* }}} */

static void phpdbg_class_breaks_dtor(zval *data) /* {{{ */
{
	phpdbg_breakmethod_t *bp = (phpdbg_breakmethod_t *) Z_PTR_P(data);

	efree((char*)bp->class_name);
	efree((char*)bp->func_name);
	efree(bp);
} /* }}} */

static void phpdbg_opline_class_breaks_dtor(zval *data) /* {{{ */
{
	zend_hash_destroy(Z_ARRVAL_P(data));
	efree(Z_ARRVAL_P(data));
} /* }}} */

static void phpdbg_opline_breaks_dtor(zval *data) /* {{{ */
{
	phpdbg_breakopline_t *bp = (phpdbg_breakopline_t *) Z_PTR_P(data);

	if (bp->class_name) {
		efree((char*)bp->class_name);
	}
	if (bp->func_name) {
		efree((char*)bp->func_name);
	}
	efree(bp);
} /* }}} */

PHPDBG_API void phpdbg_reset_breakpoints(void) /* {{{ */
{
	HashTable *table;

	ZEND_HASH_FOREACH_PTR(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP], table) {
		phpdbg_breakbase_t *brake;

		ZEND_HASH_FOREACH_PTR(table, brake) {
			brake->hits = 0;
		} ZEND_HASH_FOREACH_END();
	} ZEND_HASH_FOREACH_END();
} /* }}} */

PHPDBG_API void phpdbg_export_breakpoints(FILE *handle) /* {{{ */
{
	char *string;
	phpdbg_export_breakpoints_to_string(&string);
	fputs(string, handle);
}
/* }}} */

PHPDBG_API void phpdbg_export_breakpoints_to_string(char **str) /* {{{ */
{
	HashTable *table;
	zend_ulong id = 0L;

	*str = "";

	if (zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP])) {
		phpdbg_notice("Exporting %d breakpoints", zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP]));

		/* this only looks like magic, it isn't */
		ZEND_HASH_FOREACH_NUM_KEY_PTR(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP], id, table) {
			phpdbg_breakbase_t *brake;

			ZEND_HASH_FOREACH_PTR(table, brake) {
				if (brake->id == id) {
					char *new_str = NULL;

					switch (brake->type) {
						case PHPDBG_BREAK_FILE: {
							zend_string *filename = php_addcslashes_str(((phpdbg_breakfile_t*)brake)->filename, strlen(((phpdbg_breakfile_t*)brake)->filename), "\\\"\n", 3);
							phpdbg_asprintf(&new_str,
								"%sbreak \"%s\":"ZEND_ULONG_FMT"\n", *str,
								ZSTR_VAL(filename),
								((phpdbg_breakfile_t*)brake)->line);
							zend_string_release(filename);
						} break;

						case PHPDBG_BREAK_SYM: {
							phpdbg_asprintf(&new_str,
								"%sbreak %s\n", *str,
								((phpdbg_breaksymbol_t*)brake)->symbol);
						} break;

						case PHPDBG_BREAK_METHOD: {
							phpdbg_asprintf(&new_str,
								"%sbreak %s::%s\n", *str,
								((phpdbg_breakmethod_t*)brake)->class_name,
								((phpdbg_breakmethod_t*)brake)->func_name);
						} break;

						case PHPDBG_BREAK_METHOD_OPLINE: {
							phpdbg_asprintf(&new_str,
								"%sbreak %s::%s#"ZEND_ULONG_FMT"\n", *str,
								((phpdbg_breakopline_t*)brake)->class_name,
								((phpdbg_breakopline_t*)brake)->func_name,
								((phpdbg_breakopline_t*)brake)->opline_num);
						} break;

						case PHPDBG_BREAK_FUNCTION_OPLINE: {
							phpdbg_asprintf(&new_str,
								"%sbreak %s#"ZEND_ULONG_FMT"\n", *str,
								((phpdbg_breakopline_t*)brake)->func_name,
								((phpdbg_breakopline_t*)brake)->opline_num);
						} break;

						case PHPDBG_BREAK_FILE_OPLINE: {
							zend_string *filename = php_addcslashes_str(((phpdbg_breakopline_t*)brake)->class_name, strlen(((phpdbg_breakopline_t*)brake)->class_name), "\\\"\n", 3);
							phpdbg_asprintf(&new_str,
								"%sbreak \"%s\":#"ZEND_ULONG_FMT"\n", *str,
								ZSTR_VAL(filename),
								((phpdbg_breakopline_t*)brake)->opline_num);
							zend_string_release(filename);
						} break;

						case PHPDBG_BREAK_OPCODE: {
							phpdbg_asprintf(&new_str,
								"%sbreak %s\n", *str,
								((phpdbg_breakop_t*)brake)->name);
						} break;

						case PHPDBG_BREAK_COND: {
							phpdbg_breakcond_t *conditional = (phpdbg_breakcond_t*) brake;

							if (conditional->paramed) {
								switch (conditional->param.type) {
		                            case NUMERIC_FUNCTION_PARAM:
		                                phpdbg_asprintf(&new_str,
		                                    "%sbreak at %s#"ZEND_ULONG_FMT" if %s\n",
		                                    *str, conditional->param.str, conditional->param.num, conditional->code);
		                            break;

		                            case NUMERIC_METHOD_PARAM:
		                                phpdbg_asprintf(&new_str,
		                                    "%sbreak at %s::%s#"ZEND_ULONG_FMT" if %s\n",
		                                    *str, conditional->param.method.class, conditional->param.method.name, conditional->param.num, conditional->code);
		                            break;

		                            case ADDR_PARAM:
		                                phpdbg_asprintf(&new_str,
		                                    "%sbreak at 0X"ZEND_ULONG_FMT" if %s\n",
		                                    *str, conditional->param.addr, conditional->code);
		                            break;

									case STR_PARAM:
										phpdbg_asprintf(&new_str,
											"%sbreak at %s if %s\n", *str, conditional->param.str, conditional->code);
									break;

									case METHOD_PARAM:
										phpdbg_asprintf(&new_str,
											"%sbreak at %s::%s if %s\n", *str,
											conditional->param.method.class, conditional->param.method.name,
											conditional->code);
									break;

									case FILE_PARAM: {
										zend_string *filename = php_addcslashes_str(conditional->param.file.name, strlen(conditional->param.file.name), "\\\"\n", 3);
										phpdbg_asprintf(&new_str,
											"%sbreak at \"%s\":"ZEND_ULONG_FMT" if %s\n", *str,
											ZSTR_VAL(filename), conditional->param.file.line,
											conditional->code);
										zend_string_release(filename);
									} break;

									default: { /* do nothing */ } break;
								}
							} else {
								phpdbg_asprintf(&new_str, "%sbreak if %s\n", str, conditional->code);
							}
						} break;

						default: continue;
					}

					if ((*str)[0]) {
						free(*str);
					}
					*str = new_str;
				}
			} ZEND_HASH_FOREACH_END();
		} ZEND_HASH_FOREACH_END();
	}

	if ((*str) && !(*str)[0]) {
		*str = NULL;
	}
} /* }}} */

PHPDBG_API void phpdbg_set_breakpoint_file(const char *path, size_t path_len, zend_ulong line_num) /* {{{ */
{
	php_stream_statbuf ssb;
	char realpath[MAXPATHLEN];
	const char *original_path = path;
	bool pending = 0;
	zend_string *path_str;

	HashTable *broken, *file_breaks = &PHPDBG_G(bp)[PHPDBG_BREAK_FILE];
	phpdbg_breakfile_t new_break;

	if (!path_len) {
		if (VCWD_REALPATH(path, realpath)) {
			path = realpath;
		}
	}
	path_len = strlen(path);

	phpdbg_debug("file path: %s, resolved path: %s, was compiled: %d\n", original_path, path, zend_hash_str_exists(&PHPDBG_G(file_sources), path, path_len));

	if (!zend_hash_str_exists(&PHPDBG_G(file_sources), path, path_len)) {
		if (php_stream_stat_path(path, &ssb) == FAILURE) {
			if (original_path[0] == '/') {
				phpdbg_error("Cannot stat %s, it does not exist", original_path);
				return;
			}

			file_breaks = &PHPDBG_G(bp)[PHPDBG_BREAK_FILE_PENDING];
			path = original_path;
			path_len = strlen(path);
			pending = 1;
		} else if (!(ssb.sb.st_mode & (S_IFREG|S_IFLNK))) {
			phpdbg_error("Cannot set breakpoint in %s, it is not a regular file", path);
			return;
		} else {
			phpdbg_debug("File exists, but not compiled\n");
		}
	}

	path_str = zend_string_init(path, path_len, 0);

	if (!(broken = zend_hash_find_ptr(file_breaks, path_str))) {
		HashTable breaks;
		zend_hash_init(&breaks, 8, NULL, phpdbg_file_breaks_dtor, 0);

		broken = zend_hash_add_mem(file_breaks, path_str, &breaks, sizeof(HashTable));
	}

	if (!zend_hash_index_exists(broken, line_num)) {
		PHPDBG_BREAK_INIT(new_break, PHPDBG_BREAK_FILE);
		new_break.filename = estrndup(path, path_len);
		new_break.line = line_num;

		zend_hash_index_update_mem(broken, line_num, &new_break, sizeof(phpdbg_breakfile_t));

		PHPDBG_BREAK_MAPPING(new_break.id, broken);

		if (pending) {
			zend_string *file;
			ZEND_HASH_MAP_FOREACH_STR_KEY(&PHPDBG_G(file_sources), file) {
				HashTable *fileht;

				phpdbg_debug("Compare against loaded %s\n", ZSTR_VAL(file));

				if (!(pending = ((fileht = phpdbg_resolve_pending_file_break_ex(ZSTR_VAL(file), ZSTR_LEN(file), path_str, broken)) == NULL))) {
					new_break = *(phpdbg_breakfile_t *) zend_hash_index_find_ptr(fileht, line_num);
					break;
				}
			} ZEND_HASH_FOREACH_END();
		}

		if (pending) {
			PHPDBG_G(flags) |= PHPDBG_HAS_PENDING_FILE_BP;

			phpdbg_notice("Pending breakpoint #%d added at %s:"ZEND_ULONG_FMT"", new_break.id, new_break.filename, new_break.line);
		} else {
			PHPDBG_G(flags) |= PHPDBG_HAS_FILE_BP;

			phpdbg_notice("Breakpoint #%d added at %s:"ZEND_ULONG_FMT"", new_break.id, new_break.filename, new_break.line);
		}
	} else {
		phpdbg_error("Breakpoint at %s:"ZEND_ULONG_FMT" exists", path, line_num);
	}

	zend_string_release(path_str);
} /* }}} */

PHPDBG_API HashTable *phpdbg_resolve_pending_file_break_ex(const char *file, uint32_t filelen, zend_string *cur, HashTable *fileht) /* {{{ */
{
	phpdbg_debug("file: %s, filelen: %u, cur: %s, curlen %u, pos: %c, memcmp: %d\n", file, filelen, ZSTR_VAL(cur), ZSTR_LEN(cur), filelen > ZSTR_LEN(cur) ? file[filelen - ZSTR_LEN(cur) - 1] : '?', filelen > ZSTR_LEN(cur) ? memcmp(file + filelen - ZSTR_LEN(cur), ZSTR_VAL(cur), ZSTR_LEN(cur)) : 0);

#ifdef _WIN32
# define WIN32_PATH_CHECK file[filelen - ZSTR_LEN(cur) - 1] == '\\'
#else
# define WIN32_PATH_CHECK 0
#endif

	if (((ZSTR_LEN(cur) < filelen && (file[filelen - ZSTR_LEN(cur) - 1] == '/' || WIN32_PATH_CHECK)) || filelen == ZSTR_LEN(cur)) && !memcmp(file + filelen - ZSTR_LEN(cur), ZSTR_VAL(cur), ZSTR_LEN(cur))) {
		phpdbg_breakfile_t *brake, new_brake;
		HashTable *master;

		PHPDBG_G(flags) |= PHPDBG_HAS_FILE_BP;

		if (!(master = zend_hash_str_find_ptr(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], file, filelen))) {
			HashTable new_ht;
			zend_hash_init(&new_ht, 8, NULL, phpdbg_file_breaks_dtor, 0);
			master = zend_hash_str_add_mem(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], file, filelen, &new_ht, sizeof(HashTable));
		}

		ZEND_HASH_FOREACH_PTR(fileht, brake) {
			new_brake = *brake;
			new_brake.filename = estrndup(file, filelen);
			PHPDBG_BREAK_UNMAPPING(brake->id);

			if (zend_hash_index_add_mem(master, brake->line, &new_brake, sizeof(phpdbg_breakfile_t))) {
				PHPDBG_BREAK_MAPPING(brake->id, master);
			}
		} ZEND_HASH_FOREACH_END();

		zend_hash_del(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_PENDING], cur);

		if (!zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_PENDING])) {
			PHPDBG_G(flags) &= ~PHPDBG_HAS_PENDING_FILE_BP;
		}

		phpdbg_debug("compiled file: %s, cur bp file: %s\n", file, ZSTR_VAL(cur));

		return master;
	}

	return NULL;
} /* }}} */

PHPDBG_API void phpdbg_resolve_pending_file_break(const char *file) /* {{{ */
{
	HashTable *fileht;
	uint32_t filelen = strlen(file);
	zend_string *cur;

	phpdbg_debug("was compiled: %s\n", file);

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_PENDING], cur, fileht) {
		phpdbg_debug("check bp: %s\n", ZSTR_VAL(cur));

		phpdbg_resolve_pending_file_break_ex(file, filelen, cur, fileht);
	} ZEND_HASH_FOREACH_END();
} /* }}} */

PHPDBG_API void phpdbg_set_breakpoint_symbol(const char *name, size_t name_len) /* {{{ */
{
	char *lcname;

	if (*name == '\\') {
		name++;
		name_len--;
	}

	lcname = zend_str_tolower_dup(name, name_len);

	if (!zend_hash_str_exists(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], name, name_len)) {
		phpdbg_breaksymbol_t new_break;

		PHPDBG_G(flags) |= PHPDBG_HAS_SYM_BP;

		PHPDBG_BREAK_INIT(new_break, PHPDBG_BREAK_SYM);
		new_break.symbol = estrndup(name, name_len);

		zend_hash_str_update_mem(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], lcname, name_len, &new_break, sizeof(phpdbg_breaksymbol_t));

		phpdbg_notice("Breakpoint #%d added at %s", new_break.id, new_break.symbol);

		PHPDBG_BREAK_MAPPING(new_break.id, &PHPDBG_G(bp)[PHPDBG_BREAK_SYM]);
	} else {
		phpdbg_error("Breakpoint exists at %s", name);
	}

	efree(lcname);
} /* }}} */

PHPDBG_API void phpdbg_set_breakpoint_method(const char *class_name, const char *func_name) /* {{{ */
{
	HashTable class_breaks, *class_table;
	size_t class_len = strlen(class_name);
	size_t func_len = strlen(func_name);
	char *func_lcname, *class_lcname;

	if (*class_name == '\\') {
		class_name++;
		class_len--;
	}

	func_lcname = zend_str_tolower_dup(func_name, func_len);
	class_lcname = zend_str_tolower_dup(class_name, class_len);

	if (!(class_table = zend_hash_str_find_ptr(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD], class_lcname, class_len))) {
		zend_hash_init(&class_breaks, 8, NULL, phpdbg_class_breaks_dtor, 0);
		class_table = zend_hash_str_update_mem(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD], class_lcname, class_len, &class_breaks, sizeof(HashTable));
	}

	if (!zend_hash_str_exists(class_table, func_lcname, func_len)) {
		phpdbg_breakmethod_t new_break;

		PHPDBG_G(flags) |= PHPDBG_HAS_METHOD_BP;

		PHPDBG_BREAK_INIT(new_break, PHPDBG_BREAK_METHOD);
		new_break.class_name = estrndup(class_name, class_len);
		new_break.class_len = class_len;
		new_break.func_name = estrndup(func_name, func_len);
		new_break.func_len = func_len;

		zend_hash_str_update_mem(class_table, func_lcname, func_len, &new_break, sizeof(phpdbg_breakmethod_t));

		phpdbg_notice("Breakpoint #%d added at %s::%s", new_break.id, class_name, func_name);

		PHPDBG_BREAK_MAPPING(new_break.id, class_table);
	} else {
		phpdbg_error("Breakpoint exists at %s::%s", class_name, func_name);
	}

	efree(func_lcname);
	efree(class_lcname);
} /* }}} */

PHPDBG_API void phpdbg_set_breakpoint_opline(zend_ulong opline) /* {{{ */
{
	if (!zend_hash_index_exists(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], opline)) {
		phpdbg_breakline_t new_break;

		PHPDBG_G(flags) |= PHPDBG_HAS_OPLINE_BP;

		PHPDBG_BREAK_INIT(new_break, PHPDBG_BREAK_OPLINE);
		new_break.name = NULL;
		new_break.opline = opline;
		new_break.base = NULL;

		zend_hash_index_update_mem(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], opline, &new_break, sizeof(phpdbg_breakline_t));

		phpdbg_notice("Breakpoint #%d added at #"ZEND_ULONG_FMT, new_break.id, new_break.opline);
		PHPDBG_BREAK_MAPPING(new_break.id, &PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]);
	} else {
		phpdbg_error("Breakpoint exists at #"ZEND_ULONG_FMT, opline);
	}
} /* }}} */

PHPDBG_API int phpdbg_resolve_op_array_break(phpdbg_breakopline_t *brake, zend_op_array *op_array) /* {{{ */
{
	phpdbg_breakline_t opline_break;
	if (op_array->last <= brake->opline_num) {
		if (brake->class_name == NULL) {
			phpdbg_error("There are only %d oplines in function %s (breaking at opline "ZEND_ULONG_FMT" impossible)", op_array->last, brake->func_name, brake->opline_num);
		} else if (brake->func_name == NULL) {
			phpdbg_error("There are only %d oplines in file %s (breaking at opline "ZEND_ULONG_FMT" impossible)", op_array->last, brake->class_name, brake->opline_num);
		} else {
			phpdbg_error("There are only %d oplines in method %s::%s (breaking at opline "ZEND_ULONG_FMT" impossible)", op_array->last, brake->class_name, brake->func_name, brake->opline_num);
		}

		return FAILURE;
	}

	opline_break.disabled = 0;
	opline_break.hits = 0;
	opline_break.id = brake->id;
	opline_break.opline = brake->opline = (zend_ulong)(op_array->opcodes + brake->opline_num);
	opline_break.name = NULL;
	opline_break.base = brake;
	if (op_array->scope) {
		opline_break.type = PHPDBG_BREAK_METHOD_OPLINE;
	} else if (op_array->function_name) {
		opline_break.type = PHPDBG_BREAK_FUNCTION_OPLINE;
	} else {
		opline_break.type = PHPDBG_BREAK_FILE_OPLINE;
	}

	PHPDBG_G(flags) |= PHPDBG_HAS_OPLINE_BP;

	zend_hash_index_update_mem(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], opline_break.opline, &opline_break, sizeof(phpdbg_breakline_t));

	return SUCCESS;
} /* }}} */

PHPDBG_API void phpdbg_resolve_op_array_breaks(zend_op_array *op_array) /* {{{ */
{
	HashTable *func_table = &PHPDBG_G(bp)[PHPDBG_BREAK_FUNCTION_OPLINE];
	HashTable *oplines_table;
	phpdbg_breakopline_t *brake;

	if (op_array->scope != NULL && !(func_table = zend_hash_find_ptr(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD_OPLINE], op_array->scope->name))) {
		return;
	}

	if (op_array->function_name == NULL) {
		if (!(oplines_table = zend_hash_find_ptr(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_OPLINE], op_array->filename))) {
			return;
		}
	} else if (!op_array->function_name || !(oplines_table = zend_hash_find_ptr(func_table, op_array->function_name))) {
		return;
	}

	ZEND_HASH_MAP_FOREACH_PTR(oplines_table, brake) {
		if (phpdbg_resolve_op_array_break(brake, op_array) == SUCCESS) {
			phpdbg_breakline_t *opline_break;

			zend_hash_internal_pointer_end(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]);
			opline_break = zend_hash_get_current_data_ptr(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]);

			phpdbg_notice("Breakpoint #%d resolved at %s%s%s#"ZEND_ULONG_FMT" (opline #"ZEND_ULONG_FMT")",
				opline_break->id,
				brake->class_name ? brake->class_name : "",
				brake->class_name && brake->func_name ? "::" : "",
				brake->func_name ? brake->func_name : "",
				brake->opline_num,
				opline_break->opline);
		}
	} ZEND_HASH_FOREACH_END();
} /* }}} */

PHPDBG_API int phpdbg_resolve_opline_break(phpdbg_breakopline_t *new_break) /* {{{ */
{
	HashTable *func_table = EG(function_table);
	zend_function *func;

	if (new_break->func_name == NULL) {
		if (EG(current_execute_data) == NULL) {
			if (PHPDBG_G(ops) != NULL && !memcmp(PHPDBG_G(ops)->filename, new_break->class_name, new_break->class_len)) {
				if (phpdbg_resolve_op_array_break(new_break, PHPDBG_G(ops)) == SUCCESS) {
					return SUCCESS;
				} else {
					return 2;
				}
			}
			return FAILURE;
		} else {
			zend_execute_data *execute_data = EG(current_execute_data);
			do {
				if (ZEND_USER_CODE(execute_data->func->common.type)) {
					zend_op_array *op_array = &execute_data->func->op_array;
					if (op_array->function_name == NULL && op_array->scope == NULL && new_break->class_len == ZSTR_LEN(op_array->filename) && !memcmp(ZSTR_VAL(op_array->filename), new_break->class_name, new_break->class_len)) {
						if (phpdbg_resolve_op_array_break(new_break, op_array) == SUCCESS) {
							return SUCCESS;
						} else {
							return 2;
						}
					}
				}
			} while ((execute_data = execute_data->prev_execute_data) != NULL);
			return FAILURE;
		}
	}

	if (new_break->class_name != NULL) {
		zend_class_entry *ce;
		if (!(ce = zend_hash_str_find_ptr(EG(class_table), zend_str_tolower_dup(new_break->class_name, new_break->class_len), new_break->class_len))) {
			return FAILURE;
		}
		func_table = &ce->function_table;
	}

	if (!(func = zend_hash_str_find_ptr(func_table, zend_str_tolower_dup(new_break->func_name, new_break->func_len), new_break->func_len))) {
		if (new_break->class_name != NULL && new_break->func_name != NULL) {
			phpdbg_error("Method %s doesn't exist in class %s", new_break->func_name, new_break->class_name);
			return 2;
		}
		return FAILURE;
	}

	if (func->type != ZEND_USER_FUNCTION) {
		if (new_break->class_name == NULL) {
			phpdbg_error("%s is not a user defined function, no oplines exist", new_break->func_name);
		} else {
			phpdbg_error("%s::%s is not a user defined method, no oplines exist", new_break->class_name, new_break->func_name);
		}
		return 2;
	}

	if (phpdbg_resolve_op_array_break(new_break, &func->op_array) == FAILURE) {
		return 2;
	}

	return SUCCESS;
} /* }}} */

/* TODO ... method/function oplines need to be normalized (leading backslash, lowercase) and file oplines need to be resolved properly */

PHPDBG_API void phpdbg_set_breakpoint_method_opline(const char *class, const char *method, zend_ulong opline) /* {{{ */
{
	phpdbg_breakopline_t new_break;
	HashTable class_breaks, *class_table;
	HashTable method_breaks, *method_table;

	PHPDBG_BREAK_INIT(new_break, PHPDBG_BREAK_METHOD_OPLINE);
	new_break.func_len = strlen(method);
	new_break.func_name = estrndup(method, new_break.func_len);
	new_break.class_len = strlen(class);
	new_break.class_name = estrndup(class, new_break.class_len);
	new_break.opline_num = opline;
	new_break.opline = 0;

	switch (phpdbg_resolve_opline_break(&new_break)) {
		case FAILURE:
			phpdbg_notice("Pending breakpoint #%d at %s::%s#"ZEND_ULONG_FMT, new_break.id, new_break.class_name, new_break.func_name, opline);
			break;

		case SUCCESS:
			phpdbg_notice("Breakpoint #%d added at %s::%s#"ZEND_ULONG_FMT, new_break.id, new_break.class_name, new_break.func_name, opline);
			break;

		case 2:
			return;
	}

	if (!(class_table = zend_hash_str_find_ptr(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD_OPLINE], new_break.class_name, new_break.class_len))) {
		zend_hash_init(&class_breaks, 8, NULL, phpdbg_opline_class_breaks_dtor, 0);
		class_table = zend_hash_str_update_mem(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD_OPLINE], new_break.class_name, new_break.class_len, &class_breaks, sizeof(HashTable));
	}

	if (!(method_table = zend_hash_str_find_ptr(class_table, new_break.func_name, new_break.func_len))) {
		zend_hash_init(&method_breaks, 8, NULL, phpdbg_opline_breaks_dtor, 0);
		method_table = zend_hash_str_update_mem(class_table, new_break.func_name, new_break.func_len, &method_breaks, sizeof(HashTable));
	}

	if (zend_hash_index_exists(method_table, opline)) {
		phpdbg_error("Breakpoint already exists for %s::%s#"ZEND_ULONG_FMT, new_break.class_name, new_break.func_name, opline);
		efree((char*)new_break.func_name);
		efree((char*)new_break.class_name);
		PHPDBG_G(bp_count)--;
		return;
	}

	PHPDBG_G(flags) |= PHPDBG_HAS_METHOD_OPLINE_BP;

	PHPDBG_BREAK_MAPPING(new_break.id, method_table);

	zend_hash_index_update_mem(method_table, opline, &new_break, sizeof(phpdbg_breakopline_t));
}
/* }}} */

PHPDBG_API void phpdbg_set_breakpoint_function_opline(const char *function, zend_ulong opline) /* {{{ */
{
	phpdbg_breakopline_t new_break;
	HashTable func_breaks, *func_table;

	PHPDBG_BREAK_INIT(new_break, PHPDBG_BREAK_FUNCTION_OPLINE);
	new_break.func_len = strlen(function);
	new_break.func_name = estrndup(function, new_break.func_len);
	new_break.class_len = 0;
	new_break.class_name = NULL;
	new_break.opline_num = opline;
	new_break.opline = 0;

	switch (phpdbg_resolve_opline_break(&new_break)) {
		case FAILURE:
			phpdbg_notice("Pending breakpoint #%d at %s#"ZEND_ULONG_FMT, new_break.id, new_break.func_name, opline);
			break;

		case SUCCESS:
			phpdbg_notice("Breakpoint #%d added at %s#"ZEND_ULONG_FMT, new_break.id, new_break.func_name, opline);
			break;

		case 2:
			return;
	}

	if (!(func_table = zend_hash_str_find_ptr(&PHPDBG_G(bp)[PHPDBG_BREAK_FUNCTION_OPLINE], new_break.func_name, new_break.func_len))) {
		zend_hash_init(&func_breaks, 8, NULL, phpdbg_opline_breaks_dtor, 0);
		func_table = zend_hash_str_update_mem(&PHPDBG_G(bp)[PHPDBG_BREAK_FUNCTION_OPLINE], new_break.func_name, new_break.func_len, &func_breaks, sizeof(HashTable));
	}

	if (zend_hash_index_exists(func_table, opline)) {
		phpdbg_error("Breakpoint already exists for %s#"ZEND_ULONG_FMT, new_break.func_name, opline);
		efree((char*)new_break.func_name);
		PHPDBG_G(bp_count)--;
		return;
	}

	PHPDBG_BREAK_MAPPING(new_break.id, func_table);

	PHPDBG_G(flags) |= PHPDBG_HAS_FUNCTION_OPLINE_BP;

	zend_hash_index_update_mem(func_table, opline, &new_break, sizeof(phpdbg_breakopline_t));
}
/* }}} */

PHPDBG_API void phpdbg_set_breakpoint_file_opline(const char *file, zend_ulong opline) /* {{{ */
{
	phpdbg_breakopline_t new_break;
	HashTable file_breaks, *file_table;

	PHPDBG_BREAK_INIT(new_break, PHPDBG_BREAK_FILE_OPLINE);
	new_break.func_len = 0;
	new_break.func_name = NULL;
	new_break.class_len = strlen(file);
	new_break.class_name = estrndup(file, new_break.class_len);
	new_break.opline_num = opline;
	new_break.opline = 0;

	switch (phpdbg_resolve_opline_break(&new_break)) {
		case FAILURE:
			phpdbg_notice("Pending breakpoint #%d at %s:"ZEND_ULONG_FMT, new_break.id, new_break.class_name, opline);
			break;

		case SUCCESS:
			phpdbg_notice("Breakpoint #%d added at %s:"ZEND_ULONG_FMT, new_break.id, new_break.class_name, opline);
			break;

		case 2:
			return;
	}

	if (!(file_table = zend_hash_str_find_ptr(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_OPLINE], new_break.class_name, new_break.class_len))) {
		zend_hash_init(&file_breaks, 8, NULL, phpdbg_opline_breaks_dtor, 0);
		file_table = zend_hash_str_update_mem(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_OPLINE], new_break.class_name, new_break.class_len, &file_breaks, sizeof(HashTable));
	}

	if (zend_hash_index_exists(file_table, opline)) {
		phpdbg_error("Breakpoint already exists for %s:"ZEND_ULONG_FMT, new_break.class_name, opline);
		efree((char*)new_break.class_name);
		PHPDBG_G(bp_count)--;
		return;
	}

	PHPDBG_BREAK_MAPPING(new_break.id, file_table);

	PHPDBG_G(flags) |= PHPDBG_HAS_FILE_OPLINE_BP;

	zend_hash_index_update_mem(file_table, opline, &new_break, sizeof(phpdbg_breakopline_t));
}
/* }}} */

PHPDBG_API void phpdbg_set_breakpoint_opcode(const char *name, size_t name_len) /* {{{ */
{
	phpdbg_breakop_t new_break;
	zend_ulong hash = zend_hash_func(name, name_len);

	if (zend_hash_index_exists(&PHPDBG_G(bp)[PHPDBG_BREAK_OPCODE], hash)) {
		phpdbg_error("Breakpoint exists for %s", name);
		return;
	}

	PHPDBG_BREAK_INIT(new_break, PHPDBG_BREAK_OPCODE);
	new_break.hash = hash;
	new_break.name = estrndup(name, name_len);

	zend_hash_index_update_mem(&PHPDBG_G(bp)[PHPDBG_BREAK_OPCODE], hash, &new_break, sizeof(phpdbg_breakop_t));

	PHPDBG_G(flags) |= PHPDBG_HAS_OPCODE_BP;

	phpdbg_notice("Breakpoint #%d added at %s", new_break.id, name);
	PHPDBG_BREAK_MAPPING(new_break.id, &PHPDBG_G(bp)[PHPDBG_BREAK_OPCODE]);
} /* }}} */

PHPDBG_API void phpdbg_set_breakpoint_opline_ex(phpdbg_opline_ptr_t opline) /* {{{ */
{
	if (!zend_hash_index_exists(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], (zend_ulong) opline)) {
		phpdbg_breakline_t new_break;

		PHPDBG_G(flags) |= PHPDBG_HAS_OPLINE_BP;

		PHPDBG_BREAK_INIT(new_break, PHPDBG_BREAK_OPLINE);
		new_break.opline = (zend_ulong) opline;
		new_break.base = NULL;

		zend_hash_index_update_mem(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], (zend_ulong) opline, &new_break, sizeof(phpdbg_breakline_t));

		phpdbg_notice("Breakpoint #%d added at #"ZEND_ULONG_FMT, new_break.id, new_break.opline);
		PHPDBG_BREAK_MAPPING(new_break.id, &PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]);
	} else {
		phpdbg_error("Breakpoint exists for opline #"ZEND_ULONG_FMT, (zend_ulong) opline);
	}
} /* }}} */

static inline void phpdbg_create_conditional_break(phpdbg_breakcond_t *brake, const phpdbg_param_t *param, const char *expr, size_t expr_len, zend_ulong hash) /* {{{ */
{
	phpdbg_breakcond_t new_break;
	uint32_t cops = CG(compiler_options);
	zend_string *bp_code;

	if (param) {
		switch (param->type) {
			case STR_PARAM:
			case NUMERIC_FUNCTION_PARAM:
			case METHOD_PARAM:
			case NUMERIC_METHOD_PARAM:
			case FILE_PARAM:
			case ADDR_PARAM:
				/* do nothing */
			break;

			default:
				phpdbg_error("Invalid parameter type for conditional breakpoint");
				return;
		}
	}

	PHPDBG_BREAK_INIT(new_break, PHPDBG_BREAK_COND);
	new_break.hash = hash;

	if (param) {
		new_break.paramed = 1;
		phpdbg_copy_param(
			param, &new_break.param);
	    if (new_break.param.type == FILE_PARAM ||
	        new_break.param.type == NUMERIC_FILE_PARAM) {
	        char realpath[MAXPATHLEN];

	        if (VCWD_REALPATH(new_break.param.file.name, realpath)) {
	            efree(new_break.param.file.name);

	            new_break.param.file.name = estrdup(realpath);
	        } else {
	            phpdbg_error("Invalid file for conditional break %s", new_break.param.file.name);
	            phpdbg_clear_param(&new_break.param);
	            return;
	        }
	    }
	} else {
		new_break.paramed = 0;
	}

	cops = CG(compiler_options);

	CG(compiler_options) = ZEND_COMPILE_DEFAULT_FOR_EVAL;

	new_break.code = estrndup(expr, expr_len);
	new_break.code_len = expr_len;

	bp_code = zend_string_concat3(
		"return ", sizeof("return ")-1, expr, expr_len, ";", sizeof(";")-1);
	new_break.ops = zend_compile_string(bp_code, "Conditional Breakpoint Code", ZEND_COMPILE_POSITION_AFTER_OPEN_TAG);
	zend_string_release(bp_code);

	if (new_break.ops) {
		brake = zend_hash_index_update_mem(&PHPDBG_G(bp)[PHPDBG_BREAK_COND], hash, &new_break, sizeof(phpdbg_breakcond_t));

		phpdbg_notice("Conditional breakpoint #%d added %s/%p", brake->id, brake->code, brake->ops);

		PHPDBG_G(flags) |= PHPDBG_HAS_COND_BP;
		PHPDBG_BREAK_MAPPING(new_break.id, &PHPDBG_G(bp)[PHPDBG_BREAK_COND]);
	} else {
		 phpdbg_error("Failed to compile code for expression %s", expr);
		 efree((char*)new_break.code);
		 PHPDBG_G(bp_count)--;
	}

	CG(compiler_options) = cops;
} /* }}} */

PHPDBG_API void phpdbg_set_breakpoint_expression(const char *expr, size_t expr_len) /* {{{ */
{
	zend_ulong expr_hash = zend_hash_func(expr, expr_len);
	phpdbg_breakcond_t new_break;

	if (!zend_hash_index_exists(&PHPDBG_G(bp)[PHPDBG_BREAK_COND], expr_hash)) {
		phpdbg_create_conditional_break(
			&new_break, NULL, expr, expr_len, expr_hash);
	} else {
		phpdbg_error("Conditional break %s exists", expr);
	}
} /* }}} */

PHPDBG_API void phpdbg_set_breakpoint_at(const phpdbg_param_t *param) /* {{{ */
{
	phpdbg_breakcond_t new_break;
	phpdbg_param_t *condition;
	zend_ulong hash = 0L;

	if (param->next) {
		condition = param->next;
		hash = zend_hash_func(condition->str, condition->len);

		if (!zend_hash_index_exists(&PHPDBG_G(bp)[PHPDBG_BREAK_COND], hash)) {
			phpdbg_create_conditional_break(&new_break, param, condition->str, condition->len, hash);
		} else {
			phpdbg_notice("Conditional break %s exists at the specified location", condition->str);
		}
	}

} /* }}} */

static inline phpdbg_breakbase_t *phpdbg_find_breakpoint_file(zend_op_array *op_array) /* {{{ */
{
	HashTable *breaks;
	phpdbg_breakbase_t *brake;

#if 0
	phpdbg_debug("Op at: %.*s %d\n", ZSTR_LEN(op_array->filename), ZSTR_VAL(op_array->filename), (*EG(opline_ptr))->lineno);
#endif

	/* NOTE: realpath resolution should have happened at compile time - no reason to do it here again */
	if (!(breaks = zend_hash_find_ptr(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], op_array->filename))) {
		return NULL;
	}

	if (EG(current_execute_data) && (brake = zend_hash_index_find_ptr(breaks, EG(current_execute_data)->opline->lineno))) {
		return brake;
	}

	return NULL;
} /* }}} */

static inline phpdbg_breakbase_t *phpdbg_find_breakpoint_symbol(zend_function *fbc) /* {{{ */
{
	zend_op_array *ops;

	if (fbc->type != ZEND_USER_FUNCTION) {
		return NULL;
	}

	ops = (zend_op_array *) fbc;

	if (ops->scope) {
		/* find method breaks here */
		return phpdbg_find_breakpoint_method(ops);
	}

	if (ops->function_name) {
		phpdbg_breakbase_t *brake;
		zend_string *fname = zend_string_tolower(ops->function_name);

		brake = zend_hash_find_ptr(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], fname);

		zend_string_release(fname);
		return brake;
	} else {
		return zend_hash_str_find_ptr(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], ZEND_STRL("main"));
	}
} /* }}} */

static inline phpdbg_breakbase_t *phpdbg_find_breakpoint_method(zend_op_array *ops) /* {{{ */
{
	HashTable *class_table;
	phpdbg_breakbase_t *brake = NULL;
	zend_string *class_lcname = zend_string_tolower(ops->scope->name);

	if ((class_table = zend_hash_find_ptr(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD], class_lcname))) {
		zend_string *lcname = zend_string_tolower(ops->function_name);

		brake = zend_hash_find_ptr(class_table, lcname);

		zend_string_release(lcname);
	}

	zend_string_release(class_lcname);
	return brake;
} /* }}} */

static inline phpdbg_breakbase_t *phpdbg_find_breakpoint_opline(phpdbg_opline_ptr_t opline) /* {{{ */
{
	phpdbg_breakline_t *brake;

	if ((brake = zend_hash_index_find_ptr(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], (zend_ulong) opline)) && brake->base) {
		return (phpdbg_breakbase_t *)brake->base;
	}

	return (phpdbg_breakbase_t *) brake;
} /* }}} */

static inline phpdbg_breakbase_t *phpdbg_find_breakpoint_opcode(zend_uchar opcode) /* {{{ */
{
	const char *opname = zend_get_opcode_name(opcode);

	if (!opname) {
		return NULL;
	}

	return zend_hash_index_find_ptr(&PHPDBG_G(bp)[PHPDBG_BREAK_OPCODE], zend_hash_func(opname, strlen(opname)));
} /* }}} */

static inline bool phpdbg_find_breakpoint_param(phpdbg_param_t *param, zend_execute_data *execute_data) /* {{{ */
{
	zend_function *function = execute_data->func;

	switch (param->type) {
		case NUMERIC_FUNCTION_PARAM:
		case STR_PARAM: {
			/* function breakpoint */

			if (function->type != ZEND_USER_FUNCTION) {
				return 0;
			}

			{
				const char *str = NULL;
				size_t len = 0L;
				zend_op_array *ops = (zend_op_array*)function;
				str = ops->function_name ? ZSTR_VAL(ops->function_name) : "main";
				len = ops->function_name ? ZSTR_LEN(ops->function_name) : strlen(str);

				if (len == param->len && memcmp(param->str, str, len) == SUCCESS) {
					return param->type == STR_PARAM || execute_data->opline - ops->opcodes == param->num;
				}
			}
		} break;

		case FILE_PARAM: {
			if (param->file.line == zend_get_executed_lineno()) {
				const char *str = zend_get_executed_filename();
				size_t lengths[2] = {strlen(param->file.name), strlen(str)};

				if (lengths[0] == lengths[1]) {
					return (memcmp(
						param->file.name, str, lengths[0]) == SUCCESS);
				}
			}
		} break;

		case NUMERIC_METHOD_PARAM:
		case METHOD_PARAM: {
			if (function->type != ZEND_USER_FUNCTION) {
				return 0;
			}

			{
				zend_op_array *ops = (zend_op_array*) function;

				if (ops->scope) {
					size_t lengths[2] = { strlen(param->method.class), ZSTR_LEN(ops->scope->name) };
					if (lengths[0] == lengths[1] && memcmp(param->method.class, ops->scope->name, lengths[0]) == SUCCESS) {
						lengths[0] = strlen(param->method.name);
						lengths[1] = ZSTR_LEN(ops->function_name);

						if (lengths[0] == lengths[1] && memcmp(param->method.name, ops->function_name, lengths[0]) == SUCCESS) {
							return param->type == METHOD_PARAM || (execute_data->opline - ops->opcodes) == param->num;
						}
					}
				}
			}
		} break;

		case ADDR_PARAM: {
			return ((zend_ulong)(phpdbg_opline_ptr_t)execute_data->opline == param->addr);
		} break;

		default: {
			/* do nothing */
		} break;
	}
	return 0;
} /* }}} */

static inline phpdbg_breakbase_t *phpdbg_find_conditional_breakpoint(zend_execute_data *execute_data) /* {{{ */
{
	phpdbg_breakcond_t *bp;
	int breakpoint = FAILURE;

	ZEND_HASH_MAP_FOREACH_PTR(&PHPDBG_G(bp)[PHPDBG_BREAK_COND], bp) {
		zval retval;
		const zend_op *orig_opline = EG(current_execute_data)->opline;
		zend_function *orig_func = EG(current_execute_data)->func;
		zval *orig_retval = EG(current_execute_data)->return_value;

		if (((phpdbg_breakbase_t*)bp)->disabled) {
			continue;
		}

		if (bp->paramed) {
			if (!phpdbg_find_breakpoint_param(&bp->param, execute_data)) {
				continue;
			}
		}

		EG(no_extensions) = 1;

		zend_rebuild_symbol_table();

		zend_try {
			PHPDBG_G(flags) |= PHPDBG_IN_COND_BP;
			zend_execute(bp->ops, &retval);
			if (zend_is_true(&retval)) {
				breakpoint = SUCCESS;
			}
 		} zend_end_try();

		EG(no_extensions) = 1;
		EG(current_execute_data)->opline = orig_opline;
		EG(current_execute_data)->func = orig_func;
		EG(current_execute_data)->return_value = orig_retval;
		PHPDBG_G(flags) &= ~PHPDBG_IN_COND_BP;

		if (breakpoint == SUCCESS) {
			break;
		}
	} ZEND_HASH_FOREACH_END();

	return (breakpoint == SUCCESS) ? ((phpdbg_breakbase_t *) bp) : NULL;
} /* }}} */

PHPDBG_API phpdbg_breakbase_t *phpdbg_find_breakpoint(zend_execute_data *execute_data) /* {{{ */
{
	phpdbg_breakbase_t *base = NULL;

	if (!(PHPDBG_G(flags) & PHPDBG_IS_BP_ENABLED)) {
		return NULL;
	}

	/* conditions cannot be executed by eval()'d code */
	if (!(PHPDBG_G(flags) & PHPDBG_IN_EVAL) &&
		(PHPDBG_G(flags) & PHPDBG_HAS_COND_BP) &&
		(base = phpdbg_find_conditional_breakpoint(execute_data))) {
		goto result;
	}

	if ((PHPDBG_G(flags) & PHPDBG_HAS_FILE_BP) && (base = phpdbg_find_breakpoint_file(&execute_data->func->op_array))) {
		goto result;
	}

	if (PHPDBG_G(flags) & (PHPDBG_HAS_METHOD_BP|PHPDBG_HAS_SYM_BP)) {
		zend_op_array *op_array = &execute_data->func->op_array;
		/* check we are at the beginning of the stack, but after argument RECV */
		if (execute_data->opline == op_array->opcodes + op_array->num_args + !!(op_array->fn_flags & ZEND_ACC_VARIADIC)) {
			if ((base = phpdbg_find_breakpoint_symbol(execute_data->func))) {
				goto result;
			}
		}
	}

	if ((PHPDBG_G(flags) & PHPDBG_HAS_OPLINE_BP) && (base = phpdbg_find_breakpoint_opline((phpdbg_opline_ptr_t) execute_data->opline))) {
		goto result;
	}

	if ((PHPDBG_G(flags) & PHPDBG_HAS_OPCODE_BP) && (base = phpdbg_find_breakpoint_opcode(execute_data->opline->opcode))) {
		goto result;
	}

	return NULL;

result:
	/* we return nothing for disable breakpoints */
	if (base->disabled) {
		return NULL;
	}

	return base;
} /* }}} */

PHPDBG_API void phpdbg_delete_breakpoint(zend_ulong num) /* {{{ */
{
	HashTable *table;
	phpdbg_breakbase_t *brake;
	zend_string *strkey;
	zend_ulong numkey;

	if ((brake = phpdbg_find_breakbase_ex(num, &table, &numkey, &strkey))) {
		int type = brake->type;
		char *name = NULL;
		size_t name_len = 0L;

		switch (type) {
			case PHPDBG_BREAK_FILE:
			case PHPDBG_BREAK_METHOD:
				if (zend_hash_num_elements(table) == 1) {
					name = estrdup(brake->name);
					name_len = strlen(name);
					if (zend_hash_num_elements(&PHPDBG_G(bp)[type]) == 1) {
						PHPDBG_G(flags) &= ~(1<<(brake->type+1));
					}
				}
			break;

			default: {
				if (zend_hash_num_elements(table) == 1) {
					PHPDBG_G(flags) &= ~(1<<(brake->type+1));
				}
			}
		}

		switch (type) {
			case PHPDBG_BREAK_FILE_OPLINE:
			case PHPDBG_BREAK_FUNCTION_OPLINE:
			case PHPDBG_BREAK_METHOD_OPLINE:
				if (zend_hash_num_elements(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]) == 1) {
					PHPDBG_G(flags) &= PHPDBG_HAS_OPLINE_BP;
				}
				zend_hash_index_del(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], ((phpdbg_breakopline_t *) brake)->opline);
		}

		if (strkey) {
			zend_hash_del(table, strkey);
		} else {
			zend_hash_index_del(table, numkey);
		}

		switch (type) {
			case PHPDBG_BREAK_FILE:
			case PHPDBG_BREAK_METHOD:
				if (name) {
					zend_hash_str_del(&PHPDBG_G(bp)[type], name, name_len);
					efree(name);
				}
			break;
		}

		phpdbg_notice("Deleted breakpoint #"ZEND_ULONG_FMT, num);
		PHPDBG_BREAK_UNMAPPING(num);
	} else {
		phpdbg_error("Failed to find breakpoint #"ZEND_ULONG_FMT, num);
	}
} /* }}} */

PHPDBG_API void phpdbg_clear_breakpoints(void) /* {{{ */
{
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_PENDING]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD_OPLINE]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_FUNCTION_OPLINE]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_OPLINE]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_OPCODE]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_COND]);
	zend_hash_clean(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP]);

	PHPDBG_G(flags) &= ~PHPDBG_BP_MASK;

	PHPDBG_G(bp_count) = 0;
} /* }}} */

PHPDBG_API void phpdbg_hit_breakpoint(phpdbg_breakbase_t *brake, bool output) /* {{{ */
{
	brake->hits++;

	if (output) {
		phpdbg_print_breakpoint(brake);
	}
} /* }}} */

PHPDBG_API void phpdbg_print_breakpoint(phpdbg_breakbase_t *brake) /* {{{ */
{
	if (!brake)
		goto unknown;

	switch (brake->type) {
		case PHPDBG_BREAK_FILE: {
			phpdbg_notice("Breakpoint #%d at %s:"ZEND_ULONG_FMT", hits: "ZEND_ULONG_FMT"",
				((phpdbg_breakfile_t*)brake)->id,
				((phpdbg_breakfile_t*)brake)->filename,
				((phpdbg_breakfile_t*)brake)->line,
				((phpdbg_breakfile_t*)brake)->hits);
		} break;

		case PHPDBG_BREAK_SYM: {
			phpdbg_notice("Breakpoint #%d in %s() at %s:%u, hits: "ZEND_ULONG_FMT"",
				((phpdbg_breaksymbol_t*)brake)->id,
				((phpdbg_breaksymbol_t*)brake)->symbol,
				zend_get_executed_filename(),
				zend_get_executed_lineno(),
				((phpdbg_breakfile_t*)brake)->hits);
		} break;

		case PHPDBG_BREAK_OPLINE: {
			phpdbg_notice("Breakpoint #%d in #"ZEND_ULONG_FMT" at %s:%u, hits: "ZEND_ULONG_FMT"",
				((phpdbg_breakline_t*)brake)->id,
				((phpdbg_breakline_t*)brake)->opline,
				zend_get_executed_filename(),
				zend_get_executed_lineno(),
				((phpdbg_breakline_t*)brake)->hits);
		} break;

		case PHPDBG_BREAK_METHOD_OPLINE: {
			 phpdbg_notice("Breakpoint #%d in %s::%s()#"ZEND_ULONG_FMT" at %s:%u, hits: "ZEND_ULONG_FMT"",
				((phpdbg_breakopline_t*)brake)->id,
				((phpdbg_breakopline_t*)brake)->class_name,
				((phpdbg_breakopline_t*)brake)->func_name,
				((phpdbg_breakopline_t*)brake)->opline_num,
				zend_get_executed_filename(),
				zend_get_executed_lineno(),
				((phpdbg_breakopline_t*)brake)->hits);
		} break;

		case PHPDBG_BREAK_FUNCTION_OPLINE: {
			 phpdbg_notice("Breakpoint #%d in %s()#"ZEND_ULONG_FMT" at %s:%u, hits: "ZEND_ULONG_FMT"",
				((phpdbg_breakopline_t*)brake)->id,
				((phpdbg_breakopline_t*)brake)->func_name,
				((phpdbg_breakopline_t*)brake)->opline_num,
				zend_get_executed_filename(),
				zend_get_executed_lineno(),
				((phpdbg_breakopline_t*)brake)->hits);
		} break;

		case PHPDBG_BREAK_FILE_OPLINE: {
			 phpdbg_notice("Breakpoint #%d in #"ZEND_ULONG_FMT" at %s:%u, hits: "ZEND_ULONG_FMT"",
				((phpdbg_breakopline_t*)brake)->id,
				((phpdbg_breakopline_t*)brake)->opline_num,
				zend_get_executed_filename(),
				zend_get_executed_lineno(),
				((phpdbg_breakopline_t*)brake)->hits);
		} break;

		case PHPDBG_BREAK_OPCODE: {
			 phpdbg_notice("Breakpoint #%d in %s at %s:%u, hits: "ZEND_ULONG_FMT"",
				((phpdbg_breakop_t*)brake)->id,
				((phpdbg_breakop_t*)brake)->name,
				zend_get_executed_filename(),
				zend_get_executed_lineno(),
				((phpdbg_breakop_t*)brake)->hits);
		} break;

		case PHPDBG_BREAK_METHOD: {
			 phpdbg_notice("Breakpoint #%d in %s::%s() at %s:%u, hits: "ZEND_ULONG_FMT"",
				((phpdbg_breakmethod_t*)brake)->id,
				((phpdbg_breakmethod_t*)brake)->class_name,
				((phpdbg_breakmethod_t*)brake)->func_name,
				zend_get_executed_filename(),
				zend_get_executed_lineno(),
				((phpdbg_breakmethod_t*)brake)->hits);
		} break;

		case PHPDBG_BREAK_COND: {
			if (((phpdbg_breakcond_t*)brake)->paramed) {
				char *param;
				phpdbg_notice("Conditional breakpoint #%d: at %s if %s at %s:%u, hits: "ZEND_ULONG_FMT"",
					((phpdbg_breakcond_t*)brake)->id,
					phpdbg_param_tostring(&((phpdbg_breakcond_t*)brake)->param, &param),
					((phpdbg_breakcond_t*)brake)->code,
					zend_get_executed_filename(),
					zend_get_executed_lineno(),
					((phpdbg_breakcond_t*)brake)->hits);
				if (param)
					free(param);
			} else {
				phpdbg_notice("Conditional breakpoint #%d: on %s == true at %s:%u, hits: "ZEND_ULONG_FMT"",
					((phpdbg_breakcond_t*)brake)->id,
					((phpdbg_breakcond_t*)brake)->code,
					zend_get_executed_filename(),
					zend_get_executed_lineno(),
					((phpdbg_breakcond_t*)brake)->hits);
			}

		} break;

		default: {
unknown:
			phpdbg_notice("Unknown breakpoint at %s:%u",
				zend_get_executed_filename(),
				zend_get_executed_lineno());
		}
	}
} /* }}} */

PHPDBG_API void phpdbg_enable_breakpoint(zend_ulong id) /* {{{ */
{
	phpdbg_breakbase_t *brake = phpdbg_find_breakbase(id);

	if (brake) {
		brake->disabled = 0;
	}
} /* }}} */

PHPDBG_API void phpdbg_disable_breakpoint(zend_ulong id) /* {{{ */
{
	phpdbg_breakbase_t *brake = phpdbg_find_breakbase(id);

	if (brake) {
		brake->disabled = 1;
	}
} /* }}} */

PHPDBG_API void phpdbg_enable_breakpoints(void) /* {{{ */
{
	PHPDBG_G(flags) |= PHPDBG_IS_BP_ENABLED;
} /* }}} */

PHPDBG_API void phpdbg_disable_breakpoints(void) { /* {{{ */
	PHPDBG_G(flags) &= ~PHPDBG_IS_BP_ENABLED;
} /* }}} */

PHPDBG_API phpdbg_breakbase_t *phpdbg_find_breakbase(zend_ulong id) /* {{{ */
{
	HashTable *table;
	zend_string *strkey;
	zend_ulong numkey;

	return phpdbg_find_breakbase_ex(id, &table, &numkey, &strkey);
} /* }}} */

PHPDBG_API phpdbg_breakbase_t *phpdbg_find_breakbase_ex(zend_ulong id, HashTable **table, zend_ulong *numkey, zend_string **strkey) /* {{{ */
{
	if ((*table = zend_hash_index_find_ptr(&PHPDBG_G(bp)[PHPDBG_BREAK_MAP], id))) {
		phpdbg_breakbase_t *brake;

		ZEND_HASH_FOREACH_KEY_PTR(*table, *numkey, *strkey, brake) {
			if (brake->id == id) {
				return brake;
			}
		} ZEND_HASH_FOREACH_END();
	}

	return NULL;
} /* }}} */

PHPDBG_API void phpdbg_print_breakpoints(zend_ulong type) /* {{{ */
{
	switch (type) {
		case PHPDBG_BREAK_SYM: if ((PHPDBG_G(flags) & PHPDBG_HAS_SYM_BP)) {
			phpdbg_breaksymbol_t *brake;

			phpdbg_out(SEPARATE "\n");
			phpdbg_out("Function Breakpoints:\n");
			ZEND_HASH_MAP_FOREACH_PTR(&PHPDBG_G(bp)[PHPDBG_BREAK_SYM], brake) {
				phpdbg_writeln("#%d\t\t%s%s",
					brake->id, brake->symbol,
					((phpdbg_breakbase_t *) brake)->disabled ? " [disabled]" : "");
			} ZEND_HASH_FOREACH_END();
		} break;

		case PHPDBG_BREAK_METHOD: if ((PHPDBG_G(flags) & PHPDBG_HAS_METHOD_BP)) {
			HashTable *class_table;

			phpdbg_out(SEPARATE "\n");
			phpdbg_out("Method Breakpoints:\n");
			ZEND_HASH_MAP_FOREACH_PTR(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD], class_table) {
				phpdbg_breakmethod_t *brake;

				ZEND_HASH_MAP_FOREACH_PTR(class_table, brake) {
					phpdbg_writeln("#%d\t\t%s::%s%s",
						brake->id, brake->class_name, brake->func_name,
						((phpdbg_breakbase_t *) brake)->disabled ? " [disabled]" : "");
				} ZEND_HASH_FOREACH_END();
			} ZEND_HASH_FOREACH_END();
		} break;

		case PHPDBG_BREAK_FILE: if ((PHPDBG_G(flags) & PHPDBG_HAS_FILE_BP)) {
			HashTable *points;

			phpdbg_out(SEPARATE "\n");
			phpdbg_out("File Breakpoints:\n");
			ZEND_HASH_MAP_FOREACH_PTR(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE], points) {
				phpdbg_breakfile_t *brake;

				ZEND_HASH_MAP_FOREACH_PTR(points, brake) {
					phpdbg_writeln("#%d\t\t%s:"ZEND_ULONG_FMT"%s",
						brake->id, brake->filename, brake->line,
						((phpdbg_breakbase_t *) brake)->disabled ? " [disabled]" : "");
				} ZEND_HASH_FOREACH_END();
			} ZEND_HASH_FOREACH_END();
		}  if ((PHPDBG_G(flags) & PHPDBG_HAS_PENDING_FILE_BP)) {
			HashTable *points;

			phpdbg_out(SEPARATE "\n");
			phpdbg_out("Pending File Breakpoints:\n");
			ZEND_HASH_MAP_FOREACH_PTR(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_PENDING], points) {
				phpdbg_breakfile_t *brake;

				ZEND_HASH_MAP_FOREACH_PTR(points, brake) {
					phpdbg_writeln("#%d\t\t%s:"ZEND_ULONG_FMT"%s",
						brake->id, brake->filename, brake->line,
						((phpdbg_breakbase_t *) brake)->disabled ? " [disabled]" : "");
				} ZEND_HASH_FOREACH_END();
			} ZEND_HASH_FOREACH_END();
		} break;

		case PHPDBG_BREAK_OPLINE: if ((PHPDBG_G(flags) & PHPDBG_HAS_OPLINE_BP)) {
			phpdbg_breakline_t *brake;

			phpdbg_out(SEPARATE "\n");
			phpdbg_out("Opline Breakpoints:\n");
			ZEND_HASH_MAP_FOREACH_PTR(&PHPDBG_G(bp)[PHPDBG_BREAK_OPLINE], brake) {
				const char *type;
				switch (brake->type) {
					case PHPDBG_BREAK_METHOD_OPLINE:
						type = "method";
						goto print_opline;
					case PHPDBG_BREAK_FUNCTION_OPLINE:
						type = "function";
						goto print_opline;
					case PHPDBG_BREAK_FILE_OPLINE:
						type = "method";

					print_opline: {
						if (brake->type == PHPDBG_BREAK_METHOD_OPLINE) {
							type = "method";
						} else if (brake->type == PHPDBG_BREAK_FUNCTION_OPLINE) {
							type = "function";
						} else if (brake->type == PHPDBG_BREAK_FILE_OPLINE) {
							type = "file";
						}

						phpdbg_writeln("#%d\t\t#"ZEND_ULONG_FMT"\t\t(%s breakpoint)%s",
							brake->id, brake->opline, type,
							((phpdbg_breakbase_t *) brake)->disabled ? " [disabled]" : "");
					} break;

					default:
						phpdbg_writeln("#%d\t\t#"ZEND_ULONG_FMT"%s",
							brake->id, brake->opline,
							((phpdbg_breakbase_t *) brake)->disabled ? " [disabled]" : "");
						break;
				}
			} ZEND_HASH_FOREACH_END();
		} break;

		case PHPDBG_BREAK_METHOD_OPLINE: if ((PHPDBG_G(flags) & PHPDBG_HAS_METHOD_OPLINE_BP)) {
			HashTable *class_table, *method_table;

			phpdbg_out(SEPARATE "\n");
			phpdbg_out("Method opline Breakpoints:\n");
			ZEND_HASH_MAP_FOREACH_PTR(&PHPDBG_G(bp)[PHPDBG_BREAK_METHOD_OPLINE], class_table) {
				ZEND_HASH_MAP_FOREACH_PTR(class_table, method_table) {
					phpdbg_breakopline_t *brake;

					ZEND_HASH_MAP_FOREACH_PTR(method_table, brake) {
						phpdbg_writeln("#%d\t\t%s::%s opline "ZEND_ULONG_FMT"%s",
							brake->id, brake->class_name, brake->func_name, brake->opline_num,
							((phpdbg_breakbase_t *) brake)->disabled ? " [disabled]" : "");
					} ZEND_HASH_FOREACH_END();
				} ZEND_HASH_FOREACH_END();
			} ZEND_HASH_FOREACH_END();
		} break;

		case PHPDBG_BREAK_FUNCTION_OPLINE: if ((PHPDBG_G(flags) & PHPDBG_HAS_FUNCTION_OPLINE_BP)) {
			HashTable *function_table;

			phpdbg_out(SEPARATE "\n");
			phpdbg_out("Function opline Breakpoints:\n");
			ZEND_HASH_MAP_FOREACH_PTR(&PHPDBG_G(bp)[PHPDBG_BREAK_FUNCTION_OPLINE], function_table) {
				phpdbg_breakopline_t *brake;

				ZEND_HASH_MAP_FOREACH_PTR(function_table, brake) {
					phpdbg_writeln("#%d\t\t%s opline "ZEND_ULONG_FMT"%s",
						brake->id, brake->func_name, brake->opline_num,
						((phpdbg_breakbase_t *) brake)->disabled ? " [disabled]" : "");
				} ZEND_HASH_FOREACH_END();
			} ZEND_HASH_FOREACH_END();
		} break;

		case PHPDBG_BREAK_FILE_OPLINE: if ((PHPDBG_G(flags) & PHPDBG_HAS_FILE_OPLINE_BP)) {
			HashTable *file_table;

			phpdbg_out(SEPARATE "\n");
			phpdbg_out("File opline Breakpoints:\n");
			ZEND_HASH_MAP_FOREACH_PTR(&PHPDBG_G(bp)[PHPDBG_BREAK_FILE_OPLINE], file_table) {
				phpdbg_breakopline_t *brake;

				ZEND_HASH_MAP_FOREACH_PTR(file_table, brake) {
					phpdbg_writeln("#%d\t\t%s opline "ZEND_ULONG_FMT"%s",
						brake->id, brake->class_name, brake->opline_num,
						((phpdbg_breakbase_t *) brake)->disabled ? " [disabled]" : "");
				} ZEND_HASH_FOREACH_END();
			} ZEND_HASH_FOREACH_END();
		} break;

		case PHPDBG_BREAK_COND: if ((PHPDBG_G(flags) & PHPDBG_HAS_COND_BP)) {
			phpdbg_breakcond_t *brake;

			phpdbg_out(SEPARATE "\n");
			phpdbg_out("Conditional Breakpoints:\n");
			ZEND_HASH_MAP_FOREACH_PTR(&PHPDBG_G(bp)[PHPDBG_BREAK_COND], brake) {
				if (brake->paramed) {
					switch (brake->param.type) {
						case STR_PARAM:
							phpdbg_writeln("#%d\t\tat %s if %s%s",
				 				brake->id, brake->param.str, brake->code,
				 				((phpdbg_breakbase_t *) brake)->disabled ? " [disabled]" : "");
						break;

						case NUMERIC_FUNCTION_PARAM:
							phpdbg_writeln("#%d\t\tat %s#"ZEND_ULONG_FMT" if %s%s",
				 				brake->id, brake->param.str, brake->param.num, brake->code,
				 				((phpdbg_breakbase_t *) brake)->disabled ? " [disabled]" : "");
						break;

						case METHOD_PARAM:
							phpdbg_writeln("#%d\t\tat %s::%s if %s%s",
				 				brake->id, brake->param.method.class, brake->param.method.name, brake->code,
				 				((phpdbg_breakbase_t*)brake)->disabled ? " [disabled]" : "");
						break;

						case NUMERIC_METHOD_PARAM:
							phpdbg_writeln("#%d\t\tat %s::%s#"ZEND_ULONG_FMT" if %s%s",
				 				brake->id, brake->param.method.class, brake->param.method.name, brake->param.num, brake->code,
				 				((phpdbg_breakbase_t *) brake)->disabled ? " [disabled]" : "");
						break;

						case FILE_PARAM:
							phpdbg_writeln("#%d\t\tat %s:"ZEND_ULONG_FMT" if %s%s",
				 				brake->id, brake->param.file.name, brake->param.file.line, brake->code,
				 				((phpdbg_breakbase_t *) brake)->disabled ? " [disabled]" : "");
						break;

						case ADDR_PARAM:
							phpdbg_writeln("#%d\t\tat #"ZEND_ULONG_FMT" if %s%s",
				 				brake->id, brake->param.addr, brake->code,
				 				((phpdbg_breakbase_t *) brake)->disabled ? " [disabled]" : "");
						break;

						default:
							phpdbg_error("Invalid parameter type for conditional breakpoint");
						return;
					}
				} else {
					phpdbg_writeln("#%d\t\tif %s%s",
				 		brake->id, brake->code,
				 		((phpdbg_breakbase_t *) brake)->disabled ? " [disabled]" : "");
				}
			} ZEND_HASH_FOREACH_END();
		} break;

		case PHPDBG_BREAK_OPCODE: if (PHPDBG_G(flags) & PHPDBG_HAS_OPCODE_BP) {
			phpdbg_breakop_t *brake;

			phpdbg_out(SEPARATE "\n");
			phpdbg_out("Opcode Breakpoints:\n");
			ZEND_HASH_MAP_FOREACH_PTR(&PHPDBG_G(bp)[PHPDBG_BREAK_OPCODE], brake) {
				phpdbg_writeln("#%d\t\t%s%s",
					brake->id, brake->name,
					((phpdbg_breakbase_t *) brake)->disabled ? " [disabled]" : "");
			} ZEND_HASH_FOREACH_END();
		} break;
	}
} /* }}} */
