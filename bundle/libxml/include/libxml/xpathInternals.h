/*
 * xpathInternals.c: internal interfaces for XML Path Language implementation
 *                   used to build new modules on top of XPath
 *
 * See COPYRIGHT for the status of this software
 *
 * Author: daniel@veillard.com
 */

#ifndef __XML_XPATH_INTERNALS_H__
#define __XML_XPATH_INTERNALS_H__

#include <libxml/xmlversion.h>
#include <libxml/xpath.h>

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 *									*
 *			Helpers						*
 *									*
 ************************************************************************/

/**
 * Many of these macros may later turn into functions. They
 * shouldn't be used in #ifdef's preprocessor instructions.
 */
/**
 * xmlXPathSetError:
 * @ctxt:  an XPath parser context
 * @err:  an xmlXPathError code
 *
 * Raises an error.
 */
#define xmlXPathSetError(ctxt, err)					\
    { xmlXPatherror((ctxt), __FILE__, __LINE__, (err));			\
      (ctxt)->error = (err); }

/**
 * xmlXPathSetArityError:
 * @ctxt:  an XPath parser context
 *
 * Raises an XPATH_INVALID_ARITY error.
 */
#define xmlXPathSetArityError(ctxt)					\
    xmlXPathSetError((ctxt), XPATH_INVALID_ARITY)

/**
 * xmlXPathSetTypeError:
 * @ctxt:  an XPath parser context
 *
 * Raises an XPATH_INVALID_TYPE error.
 */
#define xmlXPathSetTypeError(ctxt)					\
    xmlXPathSetError((ctxt), XPATH_INVALID_TYPE)

/**
 * xmlXPathGetError:
 * @ctxt:  an XPath parser context
 *
 * Get the error code of an XPath context.
 *
 * Returns the context error.
 */
#define xmlXPathGetError(ctxt)	  ((ctxt)->error)

/**
 * xmlXPathCheckError:
 * @ctxt:  an XPath parser context
 *
 * Check if an XPath error was raised.
 *
 * Returns true if an error has been raised, false otherwise.
 */
#define xmlXPathCheckError(ctxt)  ((ctxt)->error != XPATH_EXPRESSION_OK)

/**
 * xmlXPathGetDocument:
 * @ctxt:  an XPath parser context
 *
 * Get the document of an XPath context.
 *
 * Returns the context document.
 */
#define xmlXPathGetDocument(ctxt)	((ctxt)->context->doc)

/**
 * xmlXPathGetContextNode:
 * @ctxt: an XPath parser context
 *
 * Get the context node of an XPath context.
 *
 * Returns the context node.
 */
#define xmlXPathGetContextNode(ctxt)	((ctxt)->context->node)

int		xmlXPathPopBoolean	(xmlXPathParserContextPtr ctxt);
double		xmlXPathPopNumber	(xmlXPathParserContextPtr ctxt);
xmlChar *	xmlXPathPopString	(xmlXPathParserContextPtr ctxt);
xmlNodeSetPtr	xmlXPathPopNodeSet	(xmlXPathParserContextPtr ctxt);
void *		xmlXPathPopExternal	(xmlXPathParserContextPtr ctxt);

/**
 * xmlXPathReturnBoolean:
 * @ctxt:  an XPath parser context
 * @val:  a boolean
 *
 * Pushes the boolean @val on the context stack.
 */
#define xmlXPathReturnBoolean(ctxt, val)				\
    valuePush((ctxt), xmlXPathNewBoolean(val))

/**
 * xmlXPathReturnTrue:
 * @ctxt:  an XPath parser context
 *
 * Pushes true on the context stack.
 */
#define xmlXPathReturnTrue(ctxt)   xmlXPathReturnBoolean((ctxt), 1)

/**
 * xmlXPathReturnFalse:
 * @ctxt:  an XPath parser context
 *
 * Pushes false on the context stack.
 */
#define xmlXPathReturnFalse(ctxt)  xmlXPathReturnBoolean((ctxt), 0)

/**
 * xmlXPathReturnNumber:
 * @ctxt:  an XPath parser context
 * @val:  a double
 *
 * Pushes the double @val on the context stack.
 */
#define xmlXPathReturnNumber(ctxt, val)					\
    valuePush((ctxt), xmlXPathNewFloat(val))

/**
 * xmlXPathReturnString:
 * @ctxt:  an XPath parser context
 * @str:  a string
 *
 * Pushes the string @str on the context stack.
 */
#define xmlXPathReturnString(ctxt, str)					\
    valuePush((ctxt), xmlXPathWrapString(str))

/**
 * xmlXPathReturnEmptyString:
 * @ctxt:  an XPath parser context
 *
 * Pushes an empty string on the stack.
 */
#define xmlXPathReturnEmptyString(ctxt)					\
    valuePush((ctxt), xmlXPathNewCString(""))

/**
 * xmlXPathReturnNodeSet:
 * @ctxt:  an XPath parser context
 * @ns:  a node-set
 *
 * Pushes the node-set @ns on the context stack.
 */
#define xmlXPathReturnNodeSet(ctxt, ns)					\
    valuePush((ctxt), xmlXPathWrapNodeSet(ns))

/**
 * xmlXPathReturnEmptyNodeSet:
 * @ctxt:  an XPath parser context
 *
 * Pushes an empty node-set on the context stack.
 */
#define xmlXPathReturnEmptyNodeSet(ctxt)				\
    valuePush((ctxt), xmlXPathNewNodeSet(NULL))

/**
 * xmlXPathReturnExternal:
 * @ctxt:  an XPath parser context
 * @val:  user data
 *
 * Pushes user data on the context stack.
 */
#define xmlXPathReturnExternal(ctxt, val)				\
    valuePush((ctxt), xmlXPathWrapExternal(val))

/**
 * xmlXPathStackIsNodeSet:
 * @ctxt: an XPath parser context
 *
 * Check if the current value on the XPath stack is a node set or
 * an XSLT value tree.
 *
 * Returns true if the current object on the stack is a node-set.
 */
#define xmlXPathStackIsNodeSet(ctxt)					\
    (((ctxt)->value != NULL)						\
     && (((ctxt)->value->type == XPATH_NODESET)				\
         || ((ctxt)->value->type == XPATH_XSLT_TREE)))

/**
 * xmlXPathStackIsExternal:
 * @ctxt: an XPath parser context
 *
 * Checks if the current value on the XPath stack is an external
 * object.
 *
 * Returns true if the current object on the stack is an external
 * object.
 */
#define xmlXPathStackIsExternal(ctxt)					\
	((ctxt->value != NULL) && (ctxt->value->type == XPATH_USERS))

/**
 * xmlXPathEmptyNodeSet:
 * @ns:  a node-set
 *
 * Empties a node-set.
 */
#define xmlXPathEmptyNodeSet(ns)					\
    { while ((ns)->nodeNr > 0) (ns)->nodeTab[(ns)->nodeNr--] = NULL; }

/**
 * CHECK_ERROR:
 *
 * Macro to return from the function if an XPath error was detected.
 */
#define CHECK_ERROR							\
    if (ctxt->error != XPATH_EXPRESSION_OK) return

/**
 * CHECK_ERROR0:
 *
 * Macro to return 0 from the function if an XPath error was detected.
 */
#define CHECK_ERROR0							\
    if (ctxt->error != XPATH_EXPRESSION_OK) return(0)

/**
 * XP_ERROR:
 * @X:  the error code
 *
 * Macro to raise an XPath error and return.
 */
#define XP_ERROR(X)							\
    { xmlXPatherror(ctxt, __FILE__, __LINE__, X);			\
      ctxt->error = (X); return; }

/**
 * XP_ERROR0:
 * @X:  the error code
 *
 * Macro to raise an XPath error and return 0.
 */
#define XP_ERROR0(X)							\
    { xmlXPatherror(ctxt, __FILE__, __LINE__, X);			\
      ctxt->error = (X); return(0); }

/**
 * CHECK_TYPE:
 * @typeval:  the XPath type
 *
 * Macro to check that the value on top of the XPath stack is of a given
 * type.
 */
#define CHECK_TYPE(typeval)						\
    if ((ctxt->value == NULL) || (ctxt->value->type != typeval))	\
        XP_ERROR(XPATH_INVALID_TYPE)

/**
 * CHECK_TYPE0:
 * @typeval:  the XPath type
 *
 * Macro to check that the value on top of the XPath stack is of a given
 * type. Return(0) in case of failure
 */
#define CHECK_TYPE0(typeval)						\
    if ((ctxt->value == NULL) || (ctxt->value->type != typeval))	\
        XP_ERROR0(XPATH_INVALID_TYPE)

/**
 * CHECK_ARITY:
 * @x:  the number of expected args
 *
 * Macro to check that the number of args passed to an XPath function matches.
 */
#define CHECK_ARITY(x)							\
    if (nargs != (x))							\
        XP_ERROR(XPATH_INVALID_ARITY);

/**
 * CAST_TO_STRING:
 *
 * Macro to try to cast the value on the top of the XPath stack to a string.
 */
#define CAST_TO_STRING							\
    if ((ctxt->value != NULL) && (ctxt->value->type != XPATH_STRING))	\
        xmlXPathStringFunction(ctxt, 1);

/**
 * CAST_TO_NUMBER:
 *
 * Macro to try to cast the value on the top of the XPath stack to a number.
 */
#define CAST_TO_NUMBER							\
    if ((ctxt->value != NULL) && (ctxt->value->type != XPATH_NUMBER))	\
        xmlXPathNumberFunction(ctxt, 1);

/**
 * CAST_TO_BOOLEAN:
 *
 * Macro to try to cast the value on the top of the XPath stack to a boolean.
 */
#define CAST_TO_BOOLEAN							\
    if ((ctxt->value != NULL) && (ctxt->value->type != XPATH_BOOLEAN))	\
        xmlXPathBooleanFunction(ctxt, 1);

/*
 * Variable Lookup forwarding.
 */
/**
 * xmlXPathVariableLookupFunc:
 * @ctxt:  an XPath context
 * @name:  name of the variable
 * @ns_uri:  the namespace name hosting this variable
 *
 * Prototype for callbacks used to plug variable lookup in the XPath
 * engine.
 *
 * Returns the XPath object value or NULL if not found.
 */
typedef xmlXPathObjectPtr (*xmlXPathVariableLookupFunc) (void *ctxt,
					 const xmlChar *name,
					 const xmlChar *ns_uri);

void	xmlXPathRegisterVariableLookup	(xmlXPathContextPtr ctxt,
					 xmlXPathVariableLookupFunc f,
					 void *data);

/*
 * Function Lookup forwarding.
 */
/**
 * xmlXPathFuncLookupFunc:
 * @ctxt:  an XPath context
 * @name:  name of the function
 * @ns_uri:  the namespace name hosting this function
 *
 * Prototype for callbacks used to plug function lookup in the XPath
 * engine.
 *
 * Returns the XPath function or NULL if not found.
 */
typedef xmlXPathFunction (*xmlXPathFuncLookupFunc) (void *ctxt,
					 const xmlChar *name,
					 const xmlChar *ns_uri);

void	xmlXPathRegisterFuncLookup	(xmlXPathContextPtr ctxt,
					 xmlXPathFuncLookupFunc f,
					 void *funcCtxt);

/*
 * Error reporting.
 */
void		xmlXPatherror	(xmlXPathParserContextPtr ctxt,
				 const char *file,
				 int line,
				 int no);

void		xmlXPathDebugDumpObject	(FILE *output,
					 xmlXPathObjectPtr cur,
					 int depth);
void		xmlXPathDebugDumpCompExpr(FILE *output,
					 xmlXPathCompExprPtr comp,
					 int depth);

/**
 * NodeSet handling.
 */
int		xmlXPathNodeSetContains		(xmlNodeSetPtr cur,
						 xmlNodePtr val);
xmlNodeSetPtr	xmlXPathDifference		(xmlNodeSetPtr nodes1,
						 xmlNodeSetPtr nodes2);
xmlNodeSetPtr	xmlXPathIntersection		(xmlNodeSetPtr nodes1,
						 xmlNodeSetPtr nodes2);

xmlNodeSetPtr	xmlXPathDistinctSorted		(xmlNodeSetPtr nodes);
xmlNodeSetPtr	xmlXPathDistinct		(xmlNodeSetPtr nodes);

int		xmlXPathHasSameNodes		(xmlNodeSetPtr nodes1,
						 xmlNodeSetPtr nodes2);

xmlNodeSetPtr	xmlXPathNodeLeadingSorted	(xmlNodeSetPtr nodes,
						 xmlNodePtr node);
xmlNodeSetPtr	xmlXPathLeadingSorted		(xmlNodeSetPtr nodes1,
						 xmlNodeSetPtr nodes2);
xmlNodeSetPtr	xmlXPathNodeLeading		(xmlNodeSetPtr nodes,
						 xmlNodePtr node);
xmlNodeSetPtr	xmlXPathLeading			(xmlNodeSetPtr nodes1,
						 xmlNodeSetPtr nodes2);

xmlNodeSetPtr	xmlXPathNodeTrailingSorted	(xmlNodeSetPtr nodes,
						 xmlNodePtr node);
xmlNodeSetPtr	xmlXPathTrailingSorted		(xmlNodeSetPtr nodes1,
						 xmlNodeSetPtr nodes2);
xmlNodeSetPtr	xmlXPathNodeTrailing		(xmlNodeSetPtr nodes,
						 xmlNodePtr node);
xmlNodeSetPtr	xmlXPathTrailing		(xmlNodeSetPtr nodes1,
						 xmlNodeSetPtr nodes2);


/**
 * Extending a context.
 */

int		   xmlXPathRegisterNs		(xmlXPathContextPtr ctxt,
						 const xmlChar *prefix,
						 const xmlChar *ns_uri);
const xmlChar *	   xmlXPathNsLookup		(xmlXPathContextPtr ctxt,
						 const xmlChar *prefix);
void		   xmlXPathRegisteredNsCleanup	(xmlXPathContextPtr ctxt);

int		   xmlXPathRegisterFunc		(xmlXPathContextPtr ctxt,
						 const xmlChar *name,
						 xmlXPathFunction f);
int		   xmlXPathRegisterFuncNS	(xmlXPathContextPtr ctxt,
						 const xmlChar *name,
						 const xmlChar *ns_uri,
						 xmlXPathFunction f);
int		   xmlXPathRegisterVariable	(xmlXPathContextPtr ctxt,
						 const xmlChar *name,
						 xmlXPathObjectPtr value);
int		   xmlXPathRegisterVariableNS	(xmlXPathContextPtr ctxt,
						 const xmlChar *name,
						 const xmlChar *ns_uri,
						 xmlXPathObjectPtr value);
xmlXPathFunction   xmlXPathFunctionLookup	(xmlXPathContextPtr ctxt,
						 const xmlChar *name);
xmlXPathFunction   xmlXPathFunctionLookupNS	(xmlXPathContextPtr ctxt,
						 const xmlChar *name,
						 const xmlChar *ns_uri);
void		   xmlXPathRegisteredFuncsCleanup(xmlXPathContextPtr ctxt);
xmlXPathObjectPtr  xmlXPathVariableLookup	(xmlXPathContextPtr ctxt,
						 const xmlChar *name);
xmlXPathObjectPtr  xmlXPathVariableLookupNS	(xmlXPathContextPtr ctxt,
						 const xmlChar *name,
						 const xmlChar *ns_uri);
void		   xmlXPathRegisteredVariablesCleanup(xmlXPathContextPtr ctxt);

/**
 * Utilities to extend XPath.
 */
xmlXPathParserContextPtr
		  xmlXPathNewParserContext	(const xmlChar *str,
			  			 xmlXPathContextPtr ctxt);
void		  xmlXPathFreeParserContext	(xmlXPathParserContextPtr ctxt);

/* TODO: remap to xmlXPathValuePop and Push. */
xmlXPathObjectPtr valuePop			(xmlXPathParserContextPtr ctxt);
int		  valuePush			(xmlXPathParserContextPtr ctxt,
					 	xmlXPathObjectPtr value);

xmlXPathObjectPtr xmlXPathNewString		(const xmlChar *val);
xmlXPathObjectPtr xmlXPathNewCString		(const char *val);
xmlXPathObjectPtr xmlXPathWrapString		(xmlChar *val);
xmlXPathObjectPtr xmlXPathWrapCString		(char * val);
xmlXPathObjectPtr xmlXPathNewFloat		(double val);
xmlXPathObjectPtr xmlXPathNewBoolean		(int val);
xmlXPathObjectPtr xmlXPathNewNodeSet		(xmlNodePtr val);
xmlXPathObjectPtr xmlXPathNewValueTree		(xmlNodePtr val);
void		  xmlXPathNodeSetAdd		(xmlNodeSetPtr cur,
						 xmlNodePtr val);
void              xmlXPathNodeSetAddUnique	(xmlNodeSetPtr cur,
						 xmlNodePtr val);
void		  xmlXPathNodeSetAddNs		(xmlNodeSetPtr cur, 
						 xmlNodePtr node, 
						 xmlNsPtr ns);
void              xmlXPathNodeSetSort		(xmlNodeSetPtr set);

void		  xmlXPathRoot			(xmlXPathParserContextPtr ctxt);
void		  xmlXPathEvalExpr		(xmlXPathParserContextPtr ctxt);
xmlChar *	  xmlXPathParseName		(xmlXPathParserContextPtr ctxt);
xmlChar *	  xmlXPathParseNCName		(xmlXPathParserContextPtr ctxt);

/*
 * Existing functions.
 */
double xmlXPathStringEvalNumber(const xmlChar *str);
int xmlXPathEvaluatePredicateResult(xmlXPathParserContextPtr ctxt, 
                                    xmlXPathObjectPtr res);
void xmlXPathRegisterAllFunctions(xmlXPathContextPtr ctxt);
xmlNodeSetPtr xmlXPathNodeSetMerge(xmlNodeSetPtr val1, xmlNodeSetPtr val2);
void xmlXPathNodeSetDel(xmlNodeSetPtr cur, xmlNodePtr val);
void xmlXPathNodeSetRemove(xmlNodeSetPtr cur, int val);
xmlXPathObjectPtr xmlXPathNewNodeSetList(xmlNodeSetPtr val);
xmlXPathObjectPtr xmlXPathWrapNodeSet(xmlNodeSetPtr val);
xmlXPathObjectPtr xmlXPathWrapExternal(void *val);

int xmlXPathEqualValues(xmlXPathParserContextPtr ctxt);
int xmlXPathNotEqualValues(xmlXPathParserContextPtr ctxt);
int xmlXPathCompareValues(xmlXPathParserContextPtr ctxt, int inf, int strict);
void xmlXPathValueFlipSign(xmlXPathParserContextPtr ctxt);
void xmlXPathAddValues(xmlXPathParserContextPtr ctxt);
void xmlXPathSubValues(xmlXPathParserContextPtr ctxt);
void xmlXPathMultValues(xmlXPathParserContextPtr ctxt);
void xmlXPathDivValues(xmlXPathParserContextPtr ctxt);
void xmlXPathModValues(xmlXPathParserContextPtr ctxt);

int xmlXPathIsNodeType(const xmlChar *name);

/*
 * Some of the axis navigation routines.
 */
xmlNodePtr xmlXPathNextSelf(xmlXPathParserContextPtr ctxt,
			xmlNodePtr cur);
xmlNodePtr xmlXPathNextChild(xmlXPathParserContextPtr ctxt,
			xmlNodePtr cur);
xmlNodePtr xmlXPathNextDescendant(xmlXPathParserContextPtr ctxt,
			xmlNodePtr cur);
xmlNodePtr xmlXPathNextDescendantOrSelf(xmlXPathParserContextPtr ctxt,
			xmlNodePtr cur);
xmlNodePtr xmlXPathNextParent(xmlXPathParserContextPtr ctxt,
			xmlNodePtr cur);
xmlNodePtr xmlXPathNextAncestorOrSelf(xmlXPathParserContextPtr ctxt,
			xmlNodePtr cur);
xmlNodePtr xmlXPathNextFollowingSibling(xmlXPathParserContextPtr ctxt,
			xmlNodePtr cur);
xmlNodePtr xmlXPathNextFollowing(xmlXPathParserContextPtr ctxt,
			xmlNodePtr cur);
xmlNodePtr xmlXPathNextNamespace(xmlXPathParserContextPtr ctxt,
			xmlNodePtr cur);
xmlNodePtr xmlXPathNextAttribute(xmlXPathParserContextPtr ctxt,
			xmlNodePtr cur);
xmlNodePtr xmlXPathNextPreceding(xmlXPathParserContextPtr ctxt,
			xmlNodePtr cur);
xmlNodePtr xmlXPathNextAncestor(xmlXPathParserContextPtr ctxt,
			xmlNodePtr cur);
xmlNodePtr xmlXPathNextPrecedingSibling(xmlXPathParserContextPtr ctxt,
			xmlNodePtr cur);
/*
 * The official core of XPath functions.
 */
void xmlXPathLastFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathPositionFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathCountFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathIdFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathLocalNameFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathNamespaceURIFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathStringFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathStringLengthFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathConcatFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathContainsFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathStartsWithFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathSubstringFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathSubstringBeforeFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathSubstringAfterFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathNormalizeFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathTranslateFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathNotFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathTrueFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathFalseFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathLangFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathNumberFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathSumFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathFloorFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathCeilingFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathRoundFunction(xmlXPathParserContextPtr ctxt, int nargs);
void xmlXPathBooleanFunction(xmlXPathParserContextPtr ctxt, int nargs);

/**
 * Really internal functions
 */
void xmlXPathNodeSetFreeNs(xmlNsPtr ns);
 
#ifdef __cplusplus
}
#endif
#endif /* ! __XML_XPATH_INTERNALS_H__ */
