#include <string.h>
#include <stdio.h>

typedef unsigned int uint;

#if WINDOWS
# define IS_SLASH(c)	((c) == '/' || (c) == '\\')
# define DEFAULT_SLASH '\\'
# define TOKENIZER_STRING "/\\"
#else
# define IS_SLASH(c)	((c) == '/')
# define DEFAULT_SLASH '/'
# define TOKENIZER_STRING "/"
#endif



typedef struct _cwd_state {
	char *cwd;
	uint cwd_length;
} cwd_state;

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

void virtual_chdir(cwd_state *state, char *path)
{
	uint path_length = strlen(path);
	char *ptr = path;
	uint ptr_length;

	if (path_length == 0) {
		return;
	}
#if WINDOWS
	/* Full path includes anything starting with C: */
	if (IS_SLASH(*ptr) || (path_length >= 2 && path[1] == ':'))
#else
	if (IS_SLASH(*ptr))
#endif
	{
		state->cwd = (char *) realloc(state->cwd, 1);
		state->cwd[0] = '\0';
		state->cwd_length = 0;
	}	

	ptr = strtok(path, TOKENIZER_STRING);
	while(ptr) {
		ptr_length = strlen(ptr);	
		if (ptr_length == 2 && !memcmp(ptr, "..", 2)) {
			while (!IS_SLASH(state->cwd[state->cwd_length-1])) {
				state->cwd[--(state->cwd_length)] = '\0';
			}
			state->cwd[--(state->cwd_length)] = '\0';
		} else if (ptr_length == 1 && *ptr == '.') {
			/* Do nothing */
		} else {
			state->cwd = (char *) realloc(state->cwd, state->cwd_length+ptr_length+1+1);
			state->cwd[state->cwd_length] = DEFAULT_SLASH;
			memcpy(&state->cwd[state->cwd_length+1], ptr, ptr_length+1);
			state->cwd_length += (ptr_length+1);
		}
		ptr = strtok(NULL, TOKENIZER_STRING);
	}
}

main()
{
	cwd_state state;
	uint length;
	
	state.cwd = strdup("");
	state.cwd_length = 0;

	printf("%s\n", virtual_getcwd(&state, &length));
	virtual_chdir(&state, strdup("/foo/barbara"));
	printf("%s\n", virtual_getcwd(&state, &length));
	virtual_chdir(&state, strdup("../andi"));
	printf("%s\n", virtual_getcwd(&state, &length));
	virtual_chdir(&state, strdup("/////andi"));
	printf("%s\n", virtual_getcwd(&state, &length));
	virtual_chdir(&state, strdup("andi/././././././////bar"));
	printf("%s\n", virtual_getcwd(&state, &length));
	virtual_chdir(&state, strdup("../andi/../bar"));
	printf("%s\n", virtual_getcwd(&state, &length));
}
