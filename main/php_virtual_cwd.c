#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>

typedef unsigned int uint;

#if WINDOWS
#define IS_SLASH(c)	((c) == '/' || (c) == '\\')
#define DEFAULT_SLASH '\\'
#define TOKENIZER_STRING "/\\"

#define IS_ABSOLUTE_PATH(path, len) \
	(len >= 2 && isalpha(path[0]) && path[1] == ':')

static int php_check_dots(char *element, uint len) 
{
	uint n = len;

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

/* for now */
#define IS_DIR_OK(s) (1)
	
#ifndef IS_DIR_OK
#define IS_DIR_OK(state) (php_is_dir_ok(state) == 0)
#endif
	
typedef struct _cwd_state {
	char *cwd;
	uint cwd_length;
} cwd_state;

static int php_is_dir_ok(const cwd_state *state) 
{
	struct stat buf;

	if (stat(state->cwd, &buf) == 0 && S_ISDIR(buf.st_mode))
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

#if WINDOWS
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
int virtual_chdir(cwd_state *state, char *path)
{
	uint path_length = strlen(path);
	char *ptr = path;
	char *tok = NULL;
	uint ptr_length;
	cwd_state *old_state;

	if (path_length == 0) 
		return (0);

	old_state = (cwd_state *) malloc(sizeof(*old_state));
	old_state->cwd = strdup(state->cwd);
	old_state->cwd_length = state->cwd_length;
	
	if (IS_ABSOLUTE_PATH(path, path_length)) {
		state->cwd = (char *) malloc(1);
		state->cwd[0] = '\0';
		state->cwd_length = 0;
#if WINDOWS
	} else if(IS_SLASH(path[0])) {
		state->cwd = (char *) malloc(3);
		memcpy(state->cwd, old_state->cwd, 2);
		state->cwd[2] = '\0';
		state->cwd_length = 2;
#endif
	}


	ptr = strtok_r(path, TOKENIZER_STRING, &tok);
	while (ptr) {
		ptr_length = strlen(ptr);

		
		if (IS_DIRECTORY_UP(ptr, ptr_length)) {
			char save;

			save = '\0';

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

	if (!IS_DIR_OK(state)) {
		free(state->cwd);

		state->cwd = old_state->cwd;
		state->cwd_length = state->cwd_length;

		free(old_state);

		return (1);
	}
	
	return (0);
}

void main(void)
{
	cwd_state state;
	uint length;

#if !WINDOWS
	state.cwd = malloc(PATH_MAX + 1);
	state.cwd_length = PATH_MAX;

	while (getcwd(state.cwd, state.cwd_length) == NULL && errno == ERANGE) { 
		state.cwd_length <<= 1;
		state.cwd = realloc(state.cwd, state.cwd_length + 1);
	}
#else
	state.cwd = strdup("d:\\foo\\bar");
	state.cwd_length = strlen(state.cwd);
#endif

	printf("%s\n", virtual_getcwd(&state, &length));
	virtual_chdir(&state, strdup("/foo/barbara"));
	printf("%s\n", virtual_getcwd(&state, &length));
	virtual_chdir(&state, strdup("../andi"));
	printf("%s\n", virtual_getcwd(&state, &length));
	virtual_chdir(&state, strdup("../../..../.././.../foo/...../"));
	printf("%s\n", virtual_getcwd(&state, &length));
	virtual_chdir(&state, strdup("/////andi"));
	printf("%s\n", virtual_getcwd(&state, &length));
	virtual_chdir(&state, strdup("andi/././././././////bar"));
	printf("%s\n", virtual_getcwd(&state, &length));
	virtual_chdir(&state, strdup("../andi/../bar"));
	printf("%s\n", virtual_getcwd(&state, &length));
}
