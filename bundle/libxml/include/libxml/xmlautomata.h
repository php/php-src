/*
 * automata.h : description of the API to build regexp automats
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <veillard@redhat.com>
 */

#ifndef __XML_AUTOMATA_H__
#define __XML_AUTOMATA_H__

#include <libxml/xmlversion.h>
#include <libxml/tree.h>

#ifdef LIBXML_AUTOMATA_ENABLED
#include <libxml/xmlregexp.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * xmlAutomataPtr:
 *
 * A libxml automata description, It can be compiled into a regexp
 */
typedef struct _xmlAutomata xmlAutomata;
typedef xmlAutomata *xmlAutomataPtr;

/**
 * xmlAutomataStatePtr:
 *
 * A state int the automata description,
 */
typedef struct _xmlAutomataState xmlAutomataState;
typedef xmlAutomataState *xmlAutomataStatePtr;

/*
 * Building API
 */
xmlAutomataPtr		xmlNewAutomata		(void);
void			xmlFreeAutomata		(xmlAutomataPtr am);

xmlAutomataStatePtr	xmlAutomataGetInitState	(xmlAutomataPtr am);
int			xmlAutomataSetFinalState(xmlAutomataPtr am,
						 xmlAutomataStatePtr state);
xmlAutomataStatePtr	xmlAutomataNewState	(xmlAutomataPtr am);
xmlAutomataStatePtr	xmlAutomataNewTransition(xmlAutomataPtr am,
						 xmlAutomataStatePtr from,
						 xmlAutomataStatePtr to,
						 const xmlChar *token,
						 void *data);
xmlAutomataStatePtr	xmlAutomataNewCountTrans(xmlAutomataPtr am,
						 xmlAutomataStatePtr from,
						 xmlAutomataStatePtr to,
						 const xmlChar *token,
						 int min,
						 int max,
						 void *data);
xmlAutomataStatePtr	xmlAutomataNewOnceTrans	(xmlAutomataPtr am,
						 xmlAutomataStatePtr from,
						 xmlAutomataStatePtr to,
						 const xmlChar *token,
						 int min,
						 int max,
						 void *data);
xmlAutomataStatePtr	xmlAutomataNewAllTrans	(xmlAutomataPtr am,
						 xmlAutomataStatePtr from,
						 xmlAutomataStatePtr to,
						 int lax);
xmlAutomataStatePtr	xmlAutomataNewEpsilon	(xmlAutomataPtr am,
						 xmlAutomataStatePtr from,
						 xmlAutomataStatePtr to);
xmlAutomataStatePtr	xmlAutomataNewCountedTrans(xmlAutomataPtr am,
						 xmlAutomataStatePtr from,
						 xmlAutomataStatePtr to,
						 int counter);
xmlAutomataStatePtr	xmlAutomataNewCounterTrans(xmlAutomataPtr am,
						 xmlAutomataStatePtr from,
						 xmlAutomataStatePtr to,
						 int counter);
int			xmlAutomataNewCounter	(xmlAutomataPtr am,
						 int min,
						 int max);

xmlRegexpPtr		xmlAutomataCompile	(xmlAutomataPtr am);
int	    		xmlAutomataIsDeterminist(xmlAutomataPtr am);

#ifdef __cplusplus
}
#endif 

#endif /* LIBXML_AUTOMATA_ENABLED */
#endif /* __XML_AUTOMATA_H__ */
