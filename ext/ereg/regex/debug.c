#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>

#include "utils.h"
#include "regex2.h"
#include "debug.ih"

/*
 - regprint - print a regexp for debugging
 == void regprint(regex_t *r, FILE *d);
 */
void
regprint(r, d)
regex_t *r;
FILE *d;
{
	register struct re_guts *g = r->re_g;
	register int i;
	register int c;
	register int last;
	int nincat[NC];

	fprintf(d, "%ld states, %d categories", (long)g->nstates,
							g->ncategories);
	fprintf(d, ", first %ld last %ld", (long)g->firststate,
						(long)g->laststate);
	if (g->iflags&USEBOL)
		fprintf(d, ", USEBOL");
	if (g->iflags&USEEOL)
		fprintf(d, ", USEEOL");
	if (g->iflags&BAD)
		fprintf(d, ", BAD");
	if (g->nsub > 0)
		fprintf(d, ", nsub=%ld", (long)g->nsub);
	if (g->must != NULL)
		fprintf(d, ", must(%ld) `%*s'", (long)g->mlen, (int)g->mlen,
								g->must);
	if (g->backrefs)
		fprintf(d, ", backrefs");
	if (g->nplus > 0)
		fprintf(d, ", nplus %ld", (long)g->nplus);
	fprintf(d, "\n");
	s_print(g, d);
	for (i = 0; i < g->ncategories; i++) {
		nincat[i] = 0;
		for (c = CHAR_MIN; c <= CHAR_MAX; c++)
			if (g->categories[c] == i)
				nincat[i]++;
	}
	fprintf(d, "cc0#%d", nincat[0]);
	for (i = 1; i < g->ncategories; i++)
		if (nincat[i] == 1) {
			for (c = CHAR_MIN; c <= CHAR_MAX; c++)
				if (g->categories[c] == i)
					break;
			fprintf(d, ", %d=%s", i, regchar(c));
		}
	fprintf(d, "\n");
	for (i = 1; i < g->ncategories; i++)
		if (nincat[i] != 1) {
			fprintf(d, "cc%d\t", i);
			last = -1;
			for (c = CHAR_MIN; c <= CHAR_MAX+1; c++)	/* +1 does flush */
				if (c <= CHAR_MAX && g->categories[c] == i) {
					if (last < 0) {
						fprintf(d, "%s", regchar(c));
						last = c;
					}
				} else {
					if (last >= 0) {
						if (last != c-1)
							fprintf(d, "-%s",
								regchar(c-1));
						last = -1;
					}
				}
			fprintf(d, "\n");
		}
}

/*
 - s_print - print the strip for debugging
 == static void s_print(register struct re_guts *g, FILE *d);
 */
static void
s_print(g, d)
register struct re_guts *g;
FILE *d;
{
	register sop *s;
	register cset *cs;
	register int i;
	register int done = 0;
	register sop opnd;
	register int col = 0;
	register int last;
	register sopno offset = 2;
#	define	GAP()	{	if (offset % 5 == 0) { \
					if (col > 40) { \
						fprintf(d, "\n\t"); \
						col = 0; \
					} else { \
						fprintf(d, " "); \
						col++; \
					} \
				} else \
					col++; \
				offset++; \
			}

	if (OP(g->strip[0]) != OEND)
		fprintf(d, "missing initial OEND!\n");
	for (s = &g->strip[1]; !done; s++) {
		opnd = OPND(*s);
		switch (OP(*s)) {
		case OEND:
			fprintf(d, "\n");
			done = 1;
			break;
		case OCHAR:
			if (strchr("\\|()^$.[+*?{}!<> ", (char)opnd) != NULL)
				fprintf(d, "\\%c", (unsigned char)opnd);
			else
				fprintf(d, "%s", regchar((unsigned char)opnd));
			break;
		case OBOL:
			fprintf(d, "^");
			break;
		case OEOL:
			fprintf(d, "$");
			break;
		case OBOW:
			fprintf(d, "\\{");
			break;
		case OEOW:
			fprintf(d, "\\}");
			break;
		case OANY:
			fprintf(d, ".");
			break;
		case OANYOF:
			fprintf(d, "[(%ld)", (long)opnd);
			cs = &g->sets[opnd];
			last = -1;
			for (i = 0; i < g->csetsize+1; i++)	/* +1 flushes */
				if (CHIN(cs, i) && i < g->csetsize) {
					if (last < 0) {
						fprintf(d, "%s", regchar(i));
						last = i;
					}
				} else {
					if (last >= 0) {
						if (last != i-1)
							fprintf(d, "-%s",
								regchar(i-1));
						last = -1;
					}
				}
			fprintf(d, "]");
			break;
		case OBACK_:
			fprintf(d, "(\\<%ld>", (long)opnd);
			break;
		case O_BACK:
			fprintf(d, "<%ld>\\)", (long)opnd);
			break;
		case OPLUS_:
			fprintf(d, "(+");
			if (OP(*(s+opnd)) != O_PLUS)
				fprintf(d, "<%ld>", (long)opnd);
			break;
		case O_PLUS:
			if (OP(*(s-opnd)) != OPLUS_)
				fprintf(d, "<%ld>", (long)opnd);
			fprintf(d, "+)");
			break;
		case OQUEST_:
			fprintf(d, "(?");
			if (OP(*(s+opnd)) != O_QUEST)
				fprintf(d, "<%ld>", (long)opnd);
			break;
		case O_QUEST:
			if (OP(*(s-opnd)) != OQUEST_)
				fprintf(d, "<%ld>", (long)opnd);
			fprintf(d, "?)");
			break;
		case OLPAREN:
			fprintf(d, "((<%ld>", (long)opnd);
			break;
		case ORPAREN:
			fprintf(d, "<%ld>))", (long)opnd);
			break;
		case OCH_:
			fprintf(d, "<");
			if (OP(*(s+opnd)) != OOR2)
				fprintf(d, "<%ld>", (long)opnd);
			break;
		case OOR1:
			if (OP(*(s-opnd)) != OOR1 && OP(*(s-opnd)) != OCH_)
				fprintf(d, "<%ld>", (long)opnd);
			fprintf(d, "|");
			break;
		case OOR2:
			fprintf(d, "|");
			if (OP(*(s+opnd)) != OOR2 && OP(*(s+opnd)) != O_CH)
				fprintf(d, "<%ld>", (long)opnd);
			break;
		case O_CH:
			if (OP(*(s-opnd)) != OOR1)
				fprintf(d, "<%ld>", (long)opnd);
			fprintf(d, ">");
			break;
		default:
			fprintf(d, "!%ld(%ld)!", OP(*s), opnd);
			break;
		}
		if (!done)
			GAP();
	}
}

/*
 - regchar - make a character printable
 == static char *regchar(int ch);
 */
static unsigned char *			/* -> representation */
regchar(ch)
int ch;
{
	static unsigned char buf[10];

	if (isprint(ch) || ch == ' ')
		sprintf(buf, "%c", ch);
	else
		sprintf(buf, "\\%o", ch);
	return(buf);
}
