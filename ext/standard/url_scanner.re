
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YYCTYPE char
#define YYCURSOR state->crs
#define YYLIMIT state->end
#define YYMARKER state->ptr
#define YYFILL(n)

typedef enum {
	INITIAL,
	REF
} state;

typedef struct {
	state state;
	const char *crs;
	const char *end;
	const char *ptr;
	const char *start;
	char *target;
	size_t targetsize;
	const char *data;
} lexdata;

#define FINISH { catchup(state); goto finish; }

#define BEGIN(x) \
		switch(state->state) { \
			case INITIAL: \
				catchup(state); \
				break; \
			case REF: \
				screw_url(state); \
				break; \
		} \
		state->state = x; \
		state->start = state->crs; \
		goto nextiter

#define ATTACH(s, n) { \
	size_t _newlen = state->targetsize + n; \
	state->target = realloc(state->target, _newlen + 1); \
	memcpy(state->target + state->targetsize, s, n); \
	state->targetsize = _newlen; \
	state->target[_newlen] = '\0'; \
	}
	
#define URLLEN 512
	
static void screw_url(lexdata *state)
{
	char *url;
	int len;
	char buf[URLLEN];
	const char *p, *q;
	char c;

	/* search outer limits for URI */
	for(p = state->start; p < state->crs && (c = *p); p++)
		if(c != '"' && c != ' ') break;

	/*  
	 *  we look at q-1, because q points to the character behind the last
	 *  character we are going to copy and the decision is based on that last
	 *  character 
	 */

	for(q = state->crs; q > state->start && (c = *(q-1)); q--)
		if(c != '"' && c != ' ') break;

	/* attach beginning */
	
	ATTACH(state->start, p-state->start);
	
	/* copy old URI */
	len = q - p;
	url = malloc(len + 1);
	memcpy(url, p, len);
	url[len] = '\0';
	
	/* construct new URI */
	len = snprintf(buf, sizeof(buf), "%s%c%s", url,
			memchr(state->start, '?', len) ? '&' : '?',
			state->data);
	free(url);

	/* attach new URI */
	ATTACH(buf, len);
	
	/* attach rest */
	ATTACH(q, state->crs - q);
}

static void catchup(lexdata *state) 
{
	ATTACH(state->start, (state->crs - state->start));
}

/*!re2c
all = [\001-\377];
eof = [\000];
ws = [ \t\v\f];
A = [aA];
C = [cC];
E = [eE];
F = [fF];
H = [hH];
M = [mM];
R = [rR];
S = [sS];
*/

static void url_scanner(lexdata *state)
{
	while(state->crs < state->end) {
	
	switch(state->state) {
		case INITIAL: 
/*!re2c
  	"<" F R A M E ws+ S R C ws* "=" ws*		{ BEGIN(REF); }
  	"<" A ws+ H R E F ws* "="	ws*			{ BEGIN(REF); }
	(all\[<])+							{ BEGIN(INITIAL); }
	eof									{ FINISH; }
*/
			break;
		case REF: 
/*!re2c
    ["]? ws* (all\[> \t\v\f":#])+ ws* ["]? { BEGIN(INITIAL); }
    ["]? ws* (all\[> \t\v\f":#])+/[#]  { BEGIN(INITIAL); }
    ["]? ws* (all\[> \t\v\f"#])+ ws* ["]? { 
			/* don't modify absolute links */
			state->state = INITIAL; BEGIN(INITIAL); 
	}
*/
  			break;
	}
nextiter:
	}
finish:
}

char *url_adapt(const char *src, size_t srclen, const char *data, size_t *newlen)
{
	lexdata state;

	state.state = INITIAL;
	state.start = state.crs = src;
	state.end = src + srclen;
	state.ptr = NULL;
	state.target = NULL;
	state.targetsize = 0;
	state.data = data;

	url_scanner(&state);

	if(newlen) *newlen = state.targetsize;

	return state.target;
}
