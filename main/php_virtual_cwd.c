#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef PHP_WIN32
#include <unistd.h>
#endif

#ifndef S_ISDIR
#define S_ISDIR(mode) ((mode) & _S_IFDIR)
#endif

#ifndef S_ISREG
#define S_ISREG(mode) ((mode) & _S_IFREG)
#endif

#ifdef PHP_WIN32
typedef unsigned int uint;
#define strtok_r(a,b,c) strtok((a),(b))
#define IS_SLASH(c)	((c) == '/' || (c) == '\\')
#define DEFAULT_SLASH '\\'
#define TOKENIZER_STRING "/\\"

#define IS_ABSOLUTE_PATH(path, len) \
	(len >= 2 && isalpha(path[0]) && path[1] == ':')

#define COPY_WHEN_ABSOLUTE 2
	
static int php_check_dots(const char *element, uint n) 
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
	
typedef struct _cwd_state {
	char *cwd;
	uint cwd_length;
} cwd_state;

typedef int (*verify_path_func)(const cwd_state *);

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


char *virtual_getcwd(cwd_state *state, uint *length)
{
	if (state->cwd_length == 0) {
		char *retval;

		*length = 1;
		retval = (char *) malloc(2);
		retval[0] = DEFAULT_SLASH;
		retval[1] = '\0';	
		return retval;
	}

#ifdef PHP_WIN32
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

/* returns 0 for ok, 1 for error */
int virtual_file_ex(cwd_state *state, char *path, verify_path_func verify_path)
{
	uint path_length = strlen(path);
	char *ptr = path;
	char *tok = NULL;
	uint ptr_length;
	cwd_state *old_state;
	int ret = 0;
	uint copy_amount = -1;

	if (path_length == 0) 
		return (0);

	old_state = (cwd_state *) malloc(sizeof(*old_state));
	old_state->cwd = strdup(state->cwd);
	old_state->cwd_length = state->cwd_length;

	if (IS_ABSOLUTE_PATH(path, path_length)) {
		copy_amount = COPY_WHEN_ABSOLUTE;
#ifdef PHP_WIN32
	} else if(IS_SLASH(path[0])) {
		copy_amount = 2;
#endif
	}

	if (copy_amount != -1) {
		state->cwd = (char *) realloc(state->cwd, copy_amount + 1);
		if (copy_amount)
			memcpy(state->cwd, old_state->cwd, copy_amount);
		state->cwd[copy_amount] = '\0';
		state->cwd_length = copy_amount;
		path += copy_amount;
	}


	ptr = strtok_r(path, TOKENIZER_STRING, &tok);
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

	if (verify_path && !verify_path(state)) {
		free(state->cwd);

		*state = *old_state;

		ret = 1;
	} else
		free(old_state->cwd);
	
	free(old_state);
	
	return (ret);
}

int virtual_chdir(cwd_state *state, char *path)
{
	return virtual_file_ex(state, path, NULL); /* Use NULL right now instead of php_is_dir_ok */
}

int virtual_filepath(cwd_state *state, char *path, char **filepath)
{
	cwd_state new_state = *state;
	int retval;

	new_state.cwd = strdup(state->cwd);
	retval = virtual_file_ex(&new_state, path, php_is_file_ok);
	*filepath = new_state.cwd;
	return retval;
}

FILE *virtual_fopen(cwd_state *state, char *path, const char *mode)
{
	cwd_state new_state = *state;
	FILE *f;
	int retval;

	new_state.cwd = strdup(state->cwd);
	retval = virtual_file_ex(&new_state, path, php_is_file_ok);

	if (retval) {
		return NULL;
	}
	f = fopen(new_state.cwd, mode);
	free(new_state.cwd);
	return f;
}

main(void)
{
	cwd_state state;
	uint length;

#ifndef PHP_WIN32
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
	printf("[%s] $ cd %s\n", virtual_getcwd(&state, &length), a); \
	virtual_chdir(&state, strdup(a)); \
	printf("new path is %s\n", virtual_getcwd(&state, &length));
	
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
