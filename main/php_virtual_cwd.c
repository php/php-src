#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>

#include "php_virtual_cwd.h"

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_DECLARE_MODULE_GLOBALS(cwd);

cwd_state main_cwd_state; /* True global */

#ifndef ZEND_WIN32
#include <unistd.h>
#else
#include <direct.h>
#endif

#ifndef S_ISDIR
#define S_ISDIR(mode) ((mode) & _S_IFDIR)
#endif

#ifndef S_ISREG
#define S_ISREG(mode) ((mode) & _S_IFREG)
#endif

#ifdef ZEND_WIN32
#define strtok_r(a,b,c) strtok((a),(b))
#define IS_SLASH(c)	((c) == '/' || (c) == '\\')
#define DEFAULT_SLASH '\\'
#define TOKENIZER_STRING "/\\"

#define IS_ABSOLUTE_PATH(path, len) \
	(len >= 2 && isalpha(path[0]) && path[1] == ':')

#define COPY_WHEN_ABSOLUTE 2
	
static int php_check_dots(const char *element, int n) 
{
	while (n-- > 0) if (element[n] != '.') break;

	return (n != -1);
}
	
#define IS_DIRECTORY_UP(element, len) \
	(len >= 2 && !php_check_dots(element, len))

#define IS_DIRECTORY_CURRENT(element, len) \
	(len == 1 && ptr[0] == '.')


#else
#define IS_SLASH(c)	((c) == '/')
#define DEFAULT_SLASH '/'
#define TOKENIZER_STRING "/"
#endif


/* default macros */

#ifndef IS_ABSOLUTE_PATH	
#define IS_ABSOLUTE_PATH(path, len) \
	(IS_SLASH(path[0]))
#endif

#ifndef IS_DIRECTORY_UP
#define IS_DIRECTORY_UP(element, len) \
	(len == 2 && memcmp(element, "..", 2) == 0)
#endif

#ifndef IS_DIRECTORY_CURRENT
#define IS_DIRECTORY_CURRENT(element, len) \
	(len == 1 && ptr[0] == '.')
#endif

#ifndef COPY_WHEN_ABSOLUTE
#define COPY_WHEN_ABSOLUTE 0
#endif

/* define this to check semantics */
#define IS_DIR_OK(s) (1)
	
#ifndef IS_DIR_OK
#define IS_DIR_OK(state) (php_is_dir_ok(state) == 0)
#endif


#define CWD_STATE_COPY(d, s)				\
	(d)->cwd_length = (s)->cwd_length;		\
	(d)->cwd = (char *) malloc((s)->cwd_length+1);	\
	memcpy((d)->cwd, (s)->cwd, (s)->cwd_length+1);

#define CWD_STATE_FREE(s)			\
	free((s)->cwd);
	
static int php_is_dir_ok(const cwd_state *state) 
{
	struct stat buf;

	if (stat(state->cwd, &buf) == 0 && S_ISDIR(buf.st_mode))
		return (0);

	return (1);
}

static int php_is_file_ok(const cwd_state *state) 
{
	struct stat buf;

	if (stat(state->cwd, &buf) == 0 && S_ISREG(buf.st_mode))
		return (0);

	return (1);
}

static void cwd_globals_ctor(zend_cwd_globals *cwd_globals)
{
	CWD_STATE_COPY(&cwd_globals->cwd, &main_cwd_state);
}

static void cwd_globals_dtor(zend_cwd_globals *cwd_globals)
{
	CWD_STATE_FREE(&cwd_globals->cwd);
}

CWD_API void virtual_cwd_startup()
{
	char cwd[1024]; /* Should probably use system define here */
	char *result;

	result = getcwd(cwd, sizeof(cwd));	
	if (!result) {
		cwd[0] = '\0';
	}
	main_cwd_state.cwd = strdup(cwd);
	main_cwd_state.cwd_length = strlen(cwd);

	ZEND_INIT_MODULE_GLOBALS(cwd, cwd_globals_ctor, cwd_globals_dtor);
}

CWD_API void virtual_cwd_shutdown()
{
#ifndef ZTS
	cwd_globals_dtor(&cwd_globals);
#endif
	free(main_cwd_state.cwd); /* Don't use CWD_STATE_FREE because the non global states will probably use emalloc()/efree() */
}

CWD_API char *virtual_getcwd_ex(int *length)
{
	cwd_state *state;
	CWDLS_FETCH();

	state = &CWDG(cwd);

	if (state->cwd_length == 0) {
		char *retval;

		*length = 1;
		retval = (char *) malloc(2);
		retval[0] = DEFAULT_SLASH;
		retval[1] = '\0';	
		return retval;
	}

#ifdef ZEND_WIN32
	/* If we have something like C: */
	if (state->cwd_length == 2 && state->cwd[state->cwd_length-1] == ':') {
		char *retval;

		*length = state->cwd_length+1;
		retval = (char *) malloc(*length+1);
		memcpy(retval, state->cwd, *length);
		retval[*length-1] = DEFAULT_SLASH;
		retval[*length] = '\0';
		return retval;
	}
#endif
	*length = state->cwd_length;
	return strdup(state->cwd);
}


/* Same semantics as UNIX getcwd() */
CWD_API char *virtual_getcwd(char *buf, size_t size)
{
	size_t length;
	char *cwd;

	cwd = virtual_getcwd_ex(&length);

	if (buf == NULL) {
		return cwd;
	}
	if (length > size-1) {
		free(cwd);
		errno = ERANGE; /* Is this OK? */
		return NULL;
	}
	memcpy(buf, cwd, length+1);
	free(cwd);
	return buf;
}


/* returns 0 for ok, 1 for error */
static int virtual_file_ex(cwd_state *state, const char *path, verify_path_func verify_path)
{
	int path_length = strlen(path);
	char *ptr, *path_copy;
	char *tok = NULL;
	int ptr_length;
	cwd_state *old_state;
	int ret = 0;
	int copy_amount = -1;
	char *free_path;
	zend_bool is_absolute = 0;

	if (path_length == 0) 
		return (0);

	free_path = path_copy = estrndup(path, path_length);

	old_state = (cwd_state *) malloc(sizeof(cwd_state));
	CWD_STATE_COPY(old_state, state);

	if (IS_ABSOLUTE_PATH(path_copy, path_length)) {
		copy_amount = COPY_WHEN_ABSOLUTE;
		is_absolute = 1;
#ifdef ZEND_WIN32
	} else if(IS_SLASH(path_copy[0])) {
		copy_amount = 2;
#endif
	}

	if (copy_amount != -1) {
		state->cwd = (char *) realloc(state->cwd, copy_amount + 1);
		if (copy_amount) {
			if (is_absolute) {
				memcpy(state->cwd, path_copy, copy_amount);
			} else {
				memcpy(state->cwd, old_state->cwd, copy_amount);
			}
		}
		state->cwd[copy_amount] = '\0';
		state->cwd_length = copy_amount;
		path_copy += copy_amount;
	}


	ptr = strtok_r(path_copy, TOKENIZER_STRING, &tok);
	while (ptr) {
		ptr_length = strlen(ptr);

		if (IS_DIRECTORY_UP(ptr, ptr_length)) {
			char save;

			save = DEFAULT_SLASH;

#define PREVIOUS state->cwd[state->cwd_length - 1]

			while (IS_ABSOLUTE_PATH(state->cwd, state->cwd_length) &&
					!IS_SLASH(PREVIOUS)) {
				save = PREVIOUS;
				PREVIOUS = '\0';
				state->cwd_length--;
			}

			if (!IS_ABSOLUTE_PATH(state->cwd, state->cwd_length)) {
				state->cwd[state->cwd_length++] = save;
				state->cwd[state->cwd_length] = '\0';
			} else {
				PREVIOUS = '\0';
				state->cwd_length--;
			}
		} else if (!IS_DIRECTORY_CURRENT(ptr, ptr_length)) {
			state->cwd = (char *) realloc(state->cwd, state->cwd_length+ptr_length+1+1);
			state->cwd[state->cwd_length] = DEFAULT_SLASH;
			memcpy(&state->cwd[state->cwd_length+1], ptr, ptr_length+1);
			state->cwd_length += (ptr_length+1);
		}
		ptr = strtok_r(NULL, TOKENIZER_STRING, &tok);
	}

	if (verify_path && verify_path(state)) {
		CWD_STATE_FREE(state);

		*state = *old_state;

		ret = 1;
	} else {
		CWD_STATE_FREE(old_state);
	}
	
	free(old_state);
	
	efree(free_path);

	return (ret);
}

CWD_API int virtual_chdir(char *path)
{
	CWDLS_FETCH();

	return virtual_file_ex(&CWDG(cwd), path, php_is_dir_ok);
}

CWD_API int virtual_chdir_file(char *path)
{
	int length = strlen(path);

	if (length == 0) {
		return 1; /* Can't cd to empty string */
	}	
	while(--length >= 0 && !IS_SLASH(path[length])) {
	}

	if (length == -1) {
		return virtual_chdir(path);
	}
	path[length] = DEFAULT_SLASH;
	return virtual_chdir(path);
}


CWD_API int virtual_filepath(char *path, char **filepath)
{
	cwd_state new_state;
	int retval;
	CWDLS_FETCH();

	CWD_STATE_COPY(&new_state, &CWDG(cwd));

	retval = virtual_file_ex(&new_state, path, php_is_file_ok);
	*filepath = new_state.cwd;
	return retval;
}

CWD_API FILE *virtual_fopen(const char *path, const char *mode)
{
	cwd_state new_state;
	FILE *f;
	CWDLS_FETCH();

	CWD_STATE_COPY(&new_state, &CWDG(cwd));

	virtual_file_ex(&new_state, path, NULL);

	f = fopen(new_state.cwd, mode);
	CWD_STATE_FREE(&new_state);
	return f;
}

#if 0

main(void)
{
	cwd_state state;
	int length;

#ifndef ZEND_WIN32
	state.cwd = malloc(PATH_MAX + 1);
	state.cwd_length = PATH_MAX;

	while (getcwd(state.cwd, state.cwd_length) == NULL && errno == ERANGE) { 
		state.cwd_length <<= 1;
		state.cwd = realloc(state.cwd, state.cwd_length + 1);
	}
#else
	state.cwd = strdup("d:\\foo\\bar");
#endif
	state.cwd_length = strlen(state.cwd);

#define T(a) \
	printf("[%s] $ cd %s\n", virtual_getcwd_ex(&state, &length), a); \
	virtual_chdir(&state, strdup(a)); \
	printf("new path is %s\n", virtual_getcwd_ex(&state, &length));
	
	T("..")
	T("...")
	T("foo")
	T("../bar")
	T(".../slash/../dot")
	T("//baz")
	T("andi/././././././///bar")
	T("../andi/../bar")
	T("...foo")
	T("D:/flash/zone")
	T("../foo/bar/../baz")

	return 0;
}

#endif
