/*
 * xpath.c: XML Path Language implementation
 *          XPath is a language for addressing parts of an XML document,
 *          designed to be used by both XSLT and XPointer
 *
 * Reference: W3C Recommendation 16 November 1999
 *     http://www.w3.org/TR/1999/REC-xpath-19991116
 * Public reference:
 *     http://www.w3.org/TR/xpath
 *
 * See Copyright for the status of this software
 *
 * Author: daniel@veillard.com
 *
 */

#define IN_LIBXML
#include "libxml.h"
#ifdef LIBXML_XPATH_ENABLED

#include <string.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_MATH_H
#include <math.h>
#endif
#ifdef HAVE_FLOAT_H
#include <float.h>
#endif
#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#include <libxml/xmlmemory.h>
#include <libxml/tree.h>
#include <libxml/valid.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/parserInternals.h>
#include <libxml/hash.h>
#ifdef LIBXML_XPTR_ENABLED
#include <libxml/xpointer.h>
#endif
#ifdef LIBXML_DEBUG_ENABLED
#include <libxml/debugXML.h>
#endif
#include <libxml/xmlerror.h>
#include <libxml/threads.h>
#include <libxml/globals.h>

/* #define DEBUG */
/* #define DEBUG_STEP */
/* #define DEBUG_STEP_NTH */
/* #define DEBUG_EXPR */
/* #define DEBUG_EVAL_COUNTS */

static xmlNs xmlXPathXMLNamespaceStruct = {
    NULL,
    XML_NAMESPACE_DECL,
    XML_XML_NAMESPACE,
    BAD_CAST "xml",
    NULL
};
static xmlNsPtr xmlXPathXMLNamespace = &xmlXPathXMLNamespaceStruct;
#ifndef LIBXML_THREAD_ENABLED
/* 
 * Optimizer is disabled only when threaded apps are detected while
 * the library ain't compiled for thread safety.
 */
static int xmlXPathDisableOptimizer = 0;
#endif

/************************************************************************
 * 									*
 * 			Floating point stuff				*
 * 									*
 ************************************************************************/

#ifndef TRIO_REPLACE_STDIO
#define TRIO_PUBLIC static
#endif
#include "trionan.c"

/*
 * The lack of portability of this section of the libc is annoying !
 */
double xmlXPathNAN = 0;
double xmlXPathPINF = 1;
double xmlXPathNINF = -1;
double xmlXPathNZERO = 0;
static int xmlXPathInitialized = 0;

/**
 * xmlXPathInit:
 *
 * Initialize the XPath environment
 */
void
xmlXPathInit(void) {
    if (xmlXPathInitialized) return;

    xmlXPathPINF = trio_pinf();
    xmlXPathNINF = trio_ninf();
    xmlXPathNAN = trio_nan();
    xmlXPathNZERO = trio_nzero();

    xmlXPathInitialized = 1;
}

/**
 * xmlXPathIsNaN:
 * @val:  a double value
 *
 * Provides a portable isnan() function to detect whether a double
 * is a NotaNumber. Based on trio code
 * http://sourceforge.net/projects/ctrio/
 * 
 * Returns 1 if the value is a NaN, 0 otherwise
 */
int
xmlXPathIsNaN(double val) {
    return(trio_isnan(val));
}

/**
 * xmlXPathIsInf:
 * @val:  a double value
 *
 * Provides a portable isinf() function to detect whether a double
 * is a +Infinite or -Infinite. Based on trio code
 * http://sourceforge.net/projects/ctrio/
 * 
 * Returns 1 vi the value is +Infinite, -1 if -Infinite, 0 otherwise
 */
int
xmlXPathIsInf(double val) {
    return(trio_isinf(val));
}

/**
 * xmlXPathGetSign:
 * @val:  a double value
 *
 * Provides a portable function to detect the sign of a double
 * Modified from trio code
 * http://sourceforge.net/projects/ctrio/
 * 
 * Returns 1 if the value is Negative, 0 if positive
 */
static int
xmlXPathGetSign(double val) {
    return(trio_signbit(val));
}


/************************************************************************
 * 									*
 * 			Parser Types					*
 * 									*
 ************************************************************************/

/*
 * Types are private:
 */

typedef enum {
    XPATH_OP_END=0,
    XPATH_OP_AND,
    XPATH_OP_OR,
    XPATH_OP_EQUAL,
    XPATH_OP_CMP,
    XPATH_OP_PLUS,
    XPATH_OP_MULT,
    XPATH_OP_UNION,
    XPATH_OP_ROOT,
    XPATH_OP_NODE,
    XPATH_OP_RESET,
    XPATH_OP_COLLECT,
    XPATH_OP_VALUE,
    XPATH_OP_VARIABLE,
    XPATH_OP_FUNCTION,
    XPATH_OP_ARG,
    XPATH_OP_PREDICATE,
    XPATH_OP_FILTER,
    XPATH_OP_SORT
#ifdef LIBXML_XPTR_ENABLED
    ,XPATH_OP_RANGETO
#endif
} xmlXPathOp;

typedef enum {
    AXIS_ANCESTOR = 1,
    AXIS_ANCESTOR_OR_SELF,
    AXIS_ATTRIBUTE,
    AXIS_CHILD,
    AXIS_DESCENDANT,
    AXIS_DESCENDANT_OR_SELF,
    AXIS_FOLLOWING,
    AXIS_FOLLOWING_SIBLING,
    AXIS_NAMESPACE,
    AXIS_PARENT,
    AXIS_PRECEDING,
    AXIS_PRECEDING_SIBLING,
    AXIS_SELF
} xmlXPathAxisVal;

typedef enum {
    NODE_TEST_NONE = 0,
    NODE_TEST_TYPE = 1,
    NODE_TEST_PI = 2,
    NODE_TEST_ALL = 3,
    NODE_TEST_NS = 4,
    NODE_TEST_NAME = 5
} xmlXPathTestVal;

typedef enum {
    NODE_TYPE_NODE = 0,
    NODE_TYPE_COMMENT = XML_COMMENT_NODE,
    NODE_TYPE_TEXT = XML_TEXT_NODE,
    NODE_TYPE_PI = XML_PI_NODE
} xmlXPathTypeVal;


typedef struct _xmlXPathStepOp xmlXPathStepOp;
typedef xmlXPathStepOp *xmlXPathStepOpPtr;
struct _xmlXPathStepOp {
    xmlXPathOp op;
    int ch1;
    int ch2;
    int value;
    int value2;
    int value3;
    void *value4;
    void *value5;
    void *cache;
    void *cacheURI;
};

struct _xmlXPathCompExpr {
    int nbStep;
    int maxStep;
    xmlXPathStepOp *steps;        /* ops for computation */
    int last;
    xmlChar *expr;
#ifdef DEBUG_EVAL_COUNTS
    int nb;
    xmlChar *string;
#endif
};

/************************************************************************
 * 									*
 * 			Parser Type functions 				*
 * 									*
 ************************************************************************/

/**
 * xmlXPathNewCompExpr:
 *
 * Create a new Xpath component
 *
 * Returns the newly allocated xmlXPathCompExprPtr or NULL in case of error
 */
static xmlXPathCompExprPtr
xmlXPathNewCompExpr(void) {
    xmlXPathCompExprPtr cur;

    cur = (xmlXPathCompExprPtr) xmlMalloc(sizeof(xmlXPathCompExpr));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlXPathNewCompExpr : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlXPathCompExpr));
    cur->maxStep = 10;
    cur->nbStep = 0;
    cur->steps = (xmlXPathStepOp *) xmlMalloc(cur->maxStep *
	                                   sizeof(xmlXPathStepOp));
    if (cur->steps == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlXPathNewCompExpr : malloc failed\n");
	xmlFree(cur);
	return(NULL);
    }
    memset(cur->steps, 0, cur->maxStep * sizeof(xmlXPathStepOp));
    cur->last = -1;
#ifdef DEBUG_EVAL_COUNTS
    cur->nb = 0;
#endif
    return(cur);
}

/**
 * xmlXPathFreeCompExpr:
 * @comp:  an XPATH comp
 *
 * Free up the memory allocated by @comp
 */
void
xmlXPathFreeCompExpr(xmlXPathCompExprPtr comp)
{
    xmlXPathStepOpPtr op;
    int i;

    if (comp == NULL)
        return;
    for (i = 0; i < comp->nbStep; i++) {
        op = &comp->steps[i];
        if (op->value4 != NULL) {
            if (op->op == XPATH_OP_VALUE)
                xmlXPathFreeObject(op->value4);
            else
                xmlFree(op->value4);
        }
        if (op->value5 != NULL)
            xmlFree(op->value5);
    }
    if (comp->steps != NULL) {
        xmlFree(comp->steps);
    }
#ifdef DEBUG_EVAL_COUNTS
    if (comp->string != NULL) {
        xmlFree(comp->string);
    }
#endif
    if (comp->expr != NULL) {
        xmlFree(comp->expr);
    }

    xmlFree(comp);
}

/**
 * xmlXPathCompExprAdd:
 * @comp:  the compiled expression
 * @ch1: first child index
 * @ch2: second child index
 * @op:  an op
 * @value:  the first int value
 * @value2:  the second int value
 * @value3:  the third int value
 * @value4:  the first string value
 * @value5:  the second string value
 *
 * Add an step to an XPath Compiled Expression
 *
 * Returns -1 in case of failure, the index otherwise
 */
static int
xmlXPathCompExprAdd(xmlXPathCompExprPtr comp, int ch1, int ch2,
   xmlXPathOp op, int value,
   int value2, int value3, void *value4, void *value5) {
    if (comp->nbStep >= comp->maxStep) {
	xmlXPathStepOp *real;

	comp->maxStep *= 2;
	real = (xmlXPathStepOp *) xmlRealloc(comp->steps,
		                      comp->maxStep * sizeof(xmlXPathStepOp));
	if (real == NULL) {
	    comp->maxStep /= 2;
	    xmlGenericError(xmlGenericErrorContext,
		    "xmlXPathCompExprAdd : realloc failed\n");
	    return(-1);
	}
	comp->steps = real;
    }
    comp->last = comp->nbStep;
    comp->steps[comp->nbStep].ch1 = ch1;
    comp->steps[comp->nbStep].ch2 = ch2;
    comp->steps[comp->nbStep].op = op;
    comp->steps[comp->nbStep].value = value;
    comp->steps[comp->nbStep].value2 = value2;
    comp->steps[comp->nbStep].value3 = value3;
    comp->steps[comp->nbStep].value4 = value4;
    comp->steps[comp->nbStep].value5 = value5;
    comp->steps[comp->nbStep].cache = NULL;
    return(comp->nbStep++);
}

/**
 * xmlXPathCompSwap:
 * @comp:  the compiled expression
 * @op: operation index
 *
 * Swaps 2 operations in the compiled expression
 */
static void
xmlXPathCompSwap(xmlXPathStepOpPtr op) {
    int tmp;

#ifndef LIBXML_THREAD_ENABLED
    /*
     * Since this manipulates possibly shared variables, this is
     * disable if one detects that the library is used in a multithreaded
     * application
     */
    if (xmlXPathDisableOptimizer)
	return;
#endif

    tmp = op->ch1;
    op->ch1 = op->ch2;
    op->ch2 = tmp;
}

#define PUSH_FULL_EXPR(op, op1, op2, val, val2, val3, val4, val5)	\
    xmlXPathCompExprAdd(ctxt->comp, (op1), (op2),			\
	                (op), (val), (val2), (val3), (val4), (val5))
#define PUSH_LONG_EXPR(op, val, val2, val3, val4, val5)			\
    xmlXPathCompExprAdd(ctxt->comp, ctxt->comp->last, -1,		\
	                (op), (val), (val2), (val3), (val4), (val5))

#define PUSH_LEAVE_EXPR(op, val, val2) 					\
xmlXPathCompExprAdd(ctxt->comp, -1, -1, (op), (val), (val2), 0 ,NULL ,NULL)

#define PUSH_UNARY_EXPR(op, ch, val, val2) 				\
xmlXPathCompExprAdd(ctxt->comp, (ch), -1, (op), (val), (val2), 0 ,NULL ,NULL)

#define PUSH_BINARY_EXPR(op, ch1, ch2, val, val2) 			\
xmlXPathCompExprAdd(ctxt->comp, (ch1), (ch2), (op), (val), (val2), 0 ,NULL ,NULL)

/************************************************************************
 *									*
 * 		Debugging related functions				*
 *									*
 ************************************************************************/

#define TODO 								\
    xmlGenericError(xmlGenericErrorContext,				\
	    "Unimplemented block at %s:%d\n",				\
            __FILE__, __LINE__);

#define STRANGE 							\
    xmlGenericError(xmlGenericErrorContext,				\
	    "Internal error at %s:%d\n",				\
            __FILE__, __LINE__);

#ifdef LIBXML_DEBUG_ENABLED
static void
xmlXPathDebugDumpNode(FILE *output, xmlNodePtr cur, int depth) {
    int i;
    char shift[100];

    for (i = 0;((i < depth) && (i < 25));i++)
        shift[2 * i] = shift[2 * i + 1] = ' ';
    shift[2 * i] = shift[2 * i + 1] = 0;
    if (cur == NULL) {
	fprintf(output, shift);
	fprintf(output, "Node is NULL !\n");
	return;
        
    }

    if ((cur->type == XML_DOCUMENT_NODE) ||
	     (cur->type == XML_HTML_DOCUMENT_NODE)) {
	fprintf(output, shift);
	fprintf(output, " /\n");
    } else if (cur->type == XML_ATTRIBUTE_NODE)
	xmlDebugDumpAttr(output, (xmlAttrPtr)cur, depth);
    else
	xmlDebugDumpOneNode(output, cur, depth);
}
static void
xmlXPathDebugDumpNodeList(FILE *output, xmlNodePtr cur, int depth) {
    xmlNodePtr tmp;
    int i;
    char shift[100];

    for (i = 0;((i < depth) && (i < 25));i++)
        shift[2 * i] = shift[2 * i + 1] = ' ';
    shift[2 * i] = shift[2 * i + 1] = 0;
    if (cur == NULL) {
	fprintf(output, shift);
	fprintf(output, "Node is NULL !\n");
	return;
        
    }

    while (cur != NULL) {
	tmp = cur;
	cur = cur->next;
	xmlDebugDumpOneNode(output, tmp, depth);
    }
}

static void
xmlXPathDebugDumpNodeSet(FILE *output, xmlNodeSetPtr cur, int depth) {
    int i;
    char shift[100];

    for (i = 0;((i < depth) && (i < 25));i++)
        shift[2 * i] = shift[2 * i + 1] = ' ';
    shift[2 * i] = shift[2 * i + 1] = 0;

    if (cur == NULL) {
	fprintf(output, shift);
	fprintf(output, "NodeSet is NULL !\n");
	return;
        
    }

    if (cur != NULL) {
	fprintf(output, "Set contains %d nodes:\n", cur->nodeNr);
	for (i = 0;i < cur->nodeNr;i++) {
	    fprintf(output, shift);
	    fprintf(output, "%d", i + 1);
	    xmlXPathDebugDumpNode(output, cur->nodeTab[i], depth + 1);
	}
    }
}

static void
xmlXPathDebugDumpValueTree(FILE *output, xmlNodeSetPtr cur, int depth) {
    int i;
    char shift[100];

    for (i = 0;((i < depth) && (i < 25));i++)
        shift[2 * i] = shift[2 * i + 1] = ' ';
    shift[2 * i] = shift[2 * i + 1] = 0;

    if ((cur == NULL) || (cur->nodeNr == 0) || (cur->nodeTab[0] == NULL)) {
	fprintf(output, shift);
	fprintf(output, "Value Tree is NULL !\n");
	return;
        
    }

    fprintf(output, shift);
    fprintf(output, "%d", i + 1);
    xmlXPathDebugDumpNodeList(output, cur->nodeTab[0]->children, depth + 1);
}
#if defined(LIBXML_XPTR_ENABLED)
static void
xmlXPathDebugDumpLocationSet(FILE *output, xmlLocationSetPtr cur, int depth) {
    int i;
    char shift[100];

    for (i = 0;((i < depth) && (i < 25));i++)
        shift[2 * i] = shift[2 * i + 1] = ' ';
    shift[2 * i] = shift[2 * i + 1] = 0;

    if (cur == NULL) {
	fprintf(output, shift);
	fprintf(output, "LocationSet is NULL !\n");
	return;
        
    }

    for (i = 0;i < cur->locNr;i++) {
	fprintf(output, shift);
        fprintf(output, "%d : ", i + 1);
	xmlXPathDebugDumpObject(output, cur->locTab[i], depth + 1);
    }
}
#endif /* LIBXML_XPTR_ENABLED */

/**
 * xmlXPathDebugDumpObject:
 * @output:  the FILE * to dump the output
 * @cur:  the object to inspect
 * @depth:  indentation level
 *
 * Dump the content of the object for debugging purposes
 */
void
xmlXPathDebugDumpObject(FILE *output, xmlXPathObjectPtr cur, int depth) {
    int i;
    char shift[100];

    for (i = 0;((i < depth) && (i < 25));i++)
        shift[2 * i] = shift[2 * i + 1] = ' ';
    shift[2 * i] = shift[2 * i + 1] = 0;

    fprintf(output, shift);

    if (cur == NULL) {
        fprintf(output, "Object is empty (NULL)\n");
	return;
    }
    switch(cur->type) {
        case XPATH_UNDEFINED:
	    fprintf(output, "Object is uninitialized\n");
	    break;
        case XPATH_NODESET:
	    fprintf(output, "Object is a Node Set :\n");
	    xmlXPathDebugDumpNodeSet(output, cur->nodesetval, depth);
	    break;
	case XPATH_XSLT_TREE:
	    fprintf(output, "Object is an XSLT value tree :\n");
	    xmlXPathDebugDumpValueTree(output, cur->nodesetval, depth);
	    break;
        case XPATH_BOOLEAN:
	    fprintf(output, "Object is a Boolean : ");
	    if (cur->boolval) fprintf(output, "true\n");
	    else fprintf(output, "false\n");
	    break;
        case XPATH_NUMBER:
	    switch (xmlXPathIsInf(cur->floatval)) {
	    case 1:
		fprintf(output, "Object is a number : Infinity\n");
		break;
	    case -1:
		fprintf(output, "Object is a number : -Infinity\n");
		break;
	    default:
		if (xmlXPathIsNaN(cur->floatval)) {
		    fprintf(output, "Object is a number : NaN\n");
		} else if (cur->floatval == 0 && xmlXPathGetSign(cur->floatval) != 0) {
		    fprintf(output, "Object is a number : 0\n");
		} else {
		    fprintf(output, "Object is a number : %0g\n", cur->floatval);
		}
	    }
	    break;
        case XPATH_STRING:
	    fprintf(output, "Object is a string : ");
	    xmlDebugDumpString(output, cur->stringval);
	    fprintf(output, "\n");
	    break;
	case XPATH_POINT:
	    fprintf(output, "Object is a point : index %d in node", cur->index);
	    xmlXPathDebugDumpNode(output, (xmlNodePtr) cur->user, depth + 1);
	    fprintf(output, "\n");
	    break;
	case XPATH_RANGE:
	    if ((cur->user2 == NULL) ||
		((cur->user2 == cur->user) && (cur->index == cur->index2))) {
		fprintf(output, "Object is a collapsed range :\n");
		fprintf(output, shift);
		if (cur->index >= 0)
		    fprintf(output, "index %d in ", cur->index);
		fprintf(output, "node\n");
		xmlXPathDebugDumpNode(output, (xmlNodePtr) cur->user,
			              depth + 1);
	    } else  {
		fprintf(output, "Object is a range :\n");
		fprintf(output, shift);
		fprintf(output, "From ");
		if (cur->index >= 0)
		    fprintf(output, "index %d in ", cur->index);
		fprintf(output, "node\n");
		xmlXPathDebugDumpNode(output, (xmlNodePtr) cur->user,
			              depth + 1);
		fprintf(output, shift);
		fprintf(output, "To ");
		if (cur->index2 >= 0)
		    fprintf(output, "index %d in ", cur->index2);
		fprintf(output, "node\n");
		xmlXPathDebugDumpNode(output, (xmlNodePtr) cur->user2,
			              depth + 1);
		fprintf(output, "\n");
	    }
	    break;
	case XPATH_LOCATIONSET:
#if defined(LIBXML_XPTR_ENABLED)
	    fprintf(output, "Object is a Location Set:\n");
	    xmlXPathDebugDumpLocationSet(output,
		    (xmlLocationSetPtr) cur->user, depth);
#endif
	    break;
	case XPATH_USERS:
	    fprintf(output, "Object is user defined\n");
	    break;
    }
}

static void
xmlXPathDebugDumpStepOp(FILE *output, xmlXPathCompExprPtr comp,
	                     xmlXPathStepOpPtr op, int depth) {
    int i;
    char shift[100];

    for (i = 0;((i < depth) && (i < 25));i++)
        shift[2 * i] = shift[2 * i + 1] = ' ';
    shift[2 * i] = shift[2 * i + 1] = 0;

    fprintf(output, shift);
    if (op == NULL) {
	fprintf(output, "Step is NULL\n");
	return;
    }
    switch (op->op) {
        case XPATH_OP_END:
	    fprintf(output, "END"); break;
        case XPATH_OP_AND:
	    fprintf(output, "AND"); break;
        case XPATH_OP_OR:
	    fprintf(output, "OR"); break;
        case XPATH_OP_EQUAL:
	     if (op->value)
		 fprintf(output, "EQUAL =");
	     else
		 fprintf(output, "EQUAL !=");
	     break;
        case XPATH_OP_CMP:
	     if (op->value)
		 fprintf(output, "CMP <");
	     else
		 fprintf(output, "CMP >");
	     if (!op->value2)
		 fprintf(output, "=");
	     break;
        case XPATH_OP_PLUS:
	     if (op->value == 0)
		 fprintf(output, "PLUS -");
	     else if (op->value == 1)
		 fprintf(output, "PLUS +");
	     else if (op->value == 2)
		 fprintf(output, "PLUS unary -");
	     else if (op->value == 3)
		 fprintf(output, "PLUS unary - -");
	     break;
        case XPATH_OP_MULT:
	     if (op->value == 0)
		 fprintf(output, "MULT *");
	     else if (op->value == 1)
		 fprintf(output, "MULT div");
	     else
		 fprintf(output, "MULT mod");
	     break;
        case XPATH_OP_UNION:
	     fprintf(output, "UNION"); break;
        case XPATH_OP_ROOT:
	     fprintf(output, "ROOT"); break;
        case XPATH_OP_NODE:
	     fprintf(output, "NODE"); break;
        case XPATH_OP_RESET:
	     fprintf(output, "RESET"); break;
        case XPATH_OP_SORT:
	     fprintf(output, "SORT"); break;
        case XPATH_OP_COLLECT: {
	    xmlXPathAxisVal axis = op->value;
	    xmlXPathTestVal test = op->value2;
	    xmlXPathTypeVal type = op->value3;
	    const xmlChar *prefix = op->value4;
	    const xmlChar *name = op->value5;

	    fprintf(output, "COLLECT ");
	    switch (axis) {
		case AXIS_ANCESTOR:
		    fprintf(output, " 'ancestors' "); break;
		case AXIS_ANCESTOR_OR_SELF:
		    fprintf(output, " 'ancestors-or-self' "); break;
		case AXIS_ATTRIBUTE:
		    fprintf(output, " 'attributes' "); break;
		case AXIS_CHILD:
		    fprintf(output, " 'child' "); break;
		case AXIS_DESCENDANT:
		    fprintf(output, " 'descendant' "); break;
		case AXIS_DESCENDANT_OR_SELF:
		    fprintf(output, " 'descendant-or-self' "); break;
		case AXIS_FOLLOWING:
		    fprintf(output, " 'following' "); break;
		case AXIS_FOLLOWING_SIBLING:
		    fprintf(output, " 'following-siblings' "); break;
		case AXIS_NAMESPACE:
		    fprintf(output, " 'namespace' "); break;
		case AXIS_PARENT:
		    fprintf(output, " 'parent' "); break;
		case AXIS_PRECEDING:
		    fprintf(output, " 'preceding' "); break;
		case AXIS_PRECEDING_SIBLING:
		    fprintf(output, " 'preceding-sibling' "); break;
		case AXIS_SELF:
		    fprintf(output, " 'self' "); break;
	    }
	    switch (test) {
                case NODE_TEST_NONE:
		    fprintf(output, "'none' "); break;
                case NODE_TEST_TYPE:
		    fprintf(output, "'type' "); break;
                case NODE_TEST_PI:
		    fprintf(output, "'PI' "); break;
                case NODE_TEST_ALL:
		    fprintf(output, "'all' "); break;
                case NODE_TEST_NS:
		    fprintf(output, "'namespace' "); break;
                case NODE_TEST_NAME:
		    fprintf(output, "'name' "); break;
	    }
	    switch (type) {
                case NODE_TYPE_NODE:
		    fprintf(output, "'node' "); break;
                case NODE_TYPE_COMMENT:
		    fprintf(output, "'comment' "); break;
                case NODE_TYPE_TEXT:
		    fprintf(output, "'text' "); break;
                case NODE_TYPE_PI:
		    fprintf(output, "'PI' "); break;
	    }
	    if (prefix != NULL)
		fprintf(output, "%s:", prefix);
	    if (name != NULL)
		fprintf(output, "%s", name);
	    break;

        }
	case XPATH_OP_VALUE: {
	    xmlXPathObjectPtr object = (xmlXPathObjectPtr) op->value4;

	    fprintf(output, "ELEM ");
	    xmlXPathDebugDumpObject(output, object, 0);
	    goto finish;
	}
	case XPATH_OP_VARIABLE: {
	    const xmlChar *prefix = op->value5;
	    const xmlChar *name = op->value4;

	    if (prefix != NULL)
		fprintf(output, "VARIABLE %s:%s", prefix, name);
	    else
		fprintf(output, "VARIABLE %s", name);
	    break;
	}
	case XPATH_OP_FUNCTION: {
	    int nbargs = op->value;
	    const xmlChar *prefix = op->value5;
	    const xmlChar *name = op->value4;

	    if (prefix != NULL)
		fprintf(output, "FUNCTION %s:%s(%d args)",
			prefix, name, nbargs);
	    else
		fprintf(output, "FUNCTION %s(%d args)", name, nbargs);
	    break;
	}
        case XPATH_OP_ARG: fprintf(output, "ARG"); break;
        case XPATH_OP_PREDICATE: fprintf(output, "PREDICATE"); break;
        case XPATH_OP_FILTER: fprintf(output, "FILTER"); break;
#ifdef LIBXML_XPTR_ENABLED
        case XPATH_OP_RANGETO: fprintf(output, "RANGETO"); break;
#endif
	default:
        fprintf(output, "UNKNOWN %d\n", op->op); return;
    }
    fprintf(output, "\n");
finish:
    if (op->ch1 >= 0)
	xmlXPathDebugDumpStepOp(output, comp, &comp->steps[op->ch1], depth + 1);
    if (op->ch2 >= 0)
	xmlXPathDebugDumpStepOp(output, comp, &comp->steps[op->ch2], depth + 1);
}

/**
 * xmlXPathDebugDumpCompExpr:
 * @output:  the FILE * for the output
 * @comp:  the precompiled XPath expression
 * @depth:  the indentation level.
 *
 * Dumps the tree of the compiled XPath expression.
 */
void
xmlXPathDebugDumpCompExpr(FILE *output, xmlXPathCompExprPtr comp,
	                  int depth) {
    int i;
    char shift[100];

    for (i = 0;((i < depth) && (i < 25));i++)
        shift[2 * i] = shift[2 * i + 1] = ' ';
    shift[2 * i] = shift[2 * i + 1] = 0;

    fprintf(output, shift);

    if (comp == NULL) {
	fprintf(output, "Compiled Expression is NULL\n");
	return;
    }
    fprintf(output, "Compiled Expression : %d elements\n",
	    comp->nbStep);
    i = comp->last;
    xmlXPathDebugDumpStepOp(output, comp, &comp->steps[i], depth + 1);
}
#endif /* LIBXML_DEBUG_ENABLED */

/************************************************************************
 *									*
 * 		Parser stacks related functions and macros		*
 *									*
 ************************************************************************/

/**
 * valuePop:
 * @ctxt: an XPath evaluation context
 *
 * Pops the top XPath object from the value stack
 *
 * Returns the XPath object just removed
 */
extern xmlXPathObjectPtr
valuePop(xmlXPathParserContextPtr ctxt)
{
    xmlXPathObjectPtr ret;

    if (ctxt->valueNr <= 0)
        return (0);
    ctxt->valueNr--;
    if (ctxt->valueNr > 0)
        ctxt->value = ctxt->valueTab[ctxt->valueNr - 1];
    else
        ctxt->value = NULL;
    ret = ctxt->valueTab[ctxt->valueNr];
    ctxt->valueTab[ctxt->valueNr] = 0;
    return (ret);
}
/**
 * valuePush:
 * @ctxt:  an XPath evaluation context
 * @value:  the XPath object
 *
 * Pushes a new XPath object on top of the value stack
 *
 * returns the number of items on the value stack
 */
extern int
valuePush(xmlXPathParserContextPtr ctxt, xmlXPathObjectPtr value)
{
    if (ctxt->valueNr >= ctxt->valueMax) {
        ctxt->valueMax *= 2;
        ctxt->valueTab =
            (xmlXPathObjectPtr *) xmlRealloc(ctxt->valueTab,
                                             ctxt->valueMax *
                                             sizeof(ctxt->valueTab[0]));
        if (ctxt->valueTab == NULL) {
            xmlGenericError(xmlGenericErrorContext, "realloc failed !\n");
            return (0);
        }
    }
    ctxt->valueTab[ctxt->valueNr] = value;
    ctxt->value = value;
    return (ctxt->valueNr++);
}

/**
 * xmlXPathPopBoolean:
 * @ctxt:  an XPath parser context
 *
 * Pops a boolean from the stack, handling conversion if needed.
 * Check error with #xmlXPathCheckError.
 *
 * Returns the boolean
 */
int
xmlXPathPopBoolean (xmlXPathParserContextPtr ctxt) {
    xmlXPathObjectPtr obj;
    int ret;

    obj = valuePop(ctxt);
    if (obj == NULL) {
	xmlXPathSetError(ctxt, XPATH_INVALID_OPERAND);
	return(0);
    }
    ret = xmlXPathCastToBoolean(obj);
    xmlXPathFreeObject(obj);
    return(ret);
}

/**
 * xmlXPathPopNumber:
 * @ctxt:  an XPath parser context
 *
 * Pops a number from the stack, handling conversion if needed.
 * Check error with #xmlXPathCheckError.
 *
 * Returns the number
 */
double
xmlXPathPopNumber (xmlXPathParserContextPtr ctxt) {
    xmlXPathObjectPtr obj;
    double ret;

    obj = valuePop(ctxt);
    if (obj == NULL) {
	xmlXPathSetError(ctxt, XPATH_INVALID_OPERAND);
	return(0);
    }
    ret = xmlXPathCastToNumber(obj);
    xmlXPathFreeObject(obj);
    return(ret);
}

/**
 * xmlXPathPopString:
 * @ctxt:  an XPath parser context
 *
 * Pops a string from the stack, handling conversion if needed.
 * Check error with #xmlXPathCheckError.
 *
 * Returns the string
 */
xmlChar *
xmlXPathPopString (xmlXPathParserContextPtr ctxt) {
    xmlXPathObjectPtr obj;
    xmlChar * ret;

    obj = valuePop(ctxt);
    if (obj == NULL) {
	xmlXPathSetError(ctxt, XPATH_INVALID_OPERAND);
	return(NULL);
    }
    ret = xmlXPathCastToString(obj);
    /* TODO: needs refactoring somewhere else */
    if (obj->stringval == ret)
	obj->stringval = NULL;
    xmlXPathFreeObject(obj);
    return(ret);
}

/**
 * xmlXPathPopNodeSet:
 * @ctxt:  an XPath parser context
 *
 * Pops a node-set from the stack, handling conversion if needed.
 * Check error with #xmlXPathCheckError.
 *
 * Returns the node-set
 */
xmlNodeSetPtr
xmlXPathPopNodeSet (xmlXPathParserContextPtr ctxt) {
    xmlXPathObjectPtr obj;
    xmlNodeSetPtr ret;

    if (ctxt->value == NULL) {
	xmlXPathSetError(ctxt, XPATH_INVALID_OPERAND);
	return(NULL);
    }
    if (!xmlXPathStackIsNodeSet(ctxt)) {
	xmlXPathSetTypeError(ctxt);
	return(NULL);
    }
    obj = valuePop(ctxt);
    ret = obj->nodesetval;
    xmlXPathFreeNodeSetList(obj);
    return(ret);
}

/**
 * xmlXPathPopExternal:
 * @ctxt:  an XPath parser context
 *
 * Pops an external object from the stack, handling conversion if needed.
 * Check error with #xmlXPathCheckError.
 *
 * Returns the object
 */
void *
xmlXPathPopExternal (xmlXPathParserContextPtr ctxt) {
    xmlXPathObjectPtr obj;
    void * ret;

    if (ctxt->value == NULL) {
	xmlXPathSetError(ctxt, XPATH_INVALID_OPERAND);
	return(NULL);
    }
    if (ctxt->value->type != XPATH_USERS) {
	xmlXPathSetTypeError(ctxt);
	return(NULL);
    }
    obj = valuePop(ctxt);
    ret = obj->user;
    xmlXPathFreeObject(obj);
    return(ret);
}

/*
 * Macros for accessing the content. Those should be used only by the parser,
 * and not exported.
 *
 * Dirty macros, i.e. one need to make assumption on the context to use them
 *
 *   CUR_PTR return the current pointer to the xmlChar to be parsed.
 *   CUR     returns the current xmlChar value, i.e. a 8 bit value
 *           in ISO-Latin or UTF-8.
 *           This should be used internally by the parser
 *           only to compare to ASCII values otherwise it would break when
 *           running with UTF-8 encoding.
 *   NXT(n)  returns the n'th next xmlChar. Same as CUR is should be used only
 *           to compare on ASCII based substring.
 *   SKIP(n) Skip n xmlChar, and must also be used only to skip ASCII defined
 *           strings within the parser.
 *   CURRENT Returns the current char value, with the full decoding of
 *           UTF-8 if we are using this mode. It returns an int.
 *   NEXT    Skip to the next character, this does the proper decoding
 *           in UTF-8 mode. It also pop-up unfinished entities on the fly.
 *           It returns the pointer to the current xmlChar.
 */

#define CUR (*ctxt->cur)
#define SKIP(val) ctxt->cur += (val)
#define NXT(val) ctxt->cur[(val)]
#define CUR_PTR ctxt->cur
#define CUR_CHAR(l) xmlXPathCurrentChar(ctxt, &l)

#define COPY_BUF(l,b,i,v)                                              \
    if (l == 1) b[i++] = (xmlChar) v;                                  \
    else i += xmlCopyChar(l,&b[i],v)

#define NEXTL(l)  ctxt->cur += l

#define SKIP_BLANKS 							\
    while (IS_BLANK(*(ctxt->cur))) NEXT

#define CURRENT (*ctxt->cur)
#define NEXT ((*ctxt->cur) ?  ctxt->cur++: ctxt->cur)


#ifndef DBL_DIG
#define DBL_DIG 16
#endif
#ifndef DBL_EPSILON
#define DBL_EPSILON 1E-9
#endif

#define UPPER_DOUBLE 1E9
#define LOWER_DOUBLE 1E-5

#define INTEGER_DIGITS DBL_DIG
#define FRACTION_DIGITS (DBL_DIG + 1)
#define EXPONENT_DIGITS (3 + 2)

/**
 * xmlXPathFormatNumber:
 * @number:     number to format
 * @buffer:     output buffer
 * @buffersize: size of output buffer
 *
 * Convert the number into a string representation.
 */
static void
xmlXPathFormatNumber(double number, char buffer[], int buffersize)
{
    switch (xmlXPathIsInf(number)) {
    case 1:
	if (buffersize > (int)sizeof("Infinity"))
	    snprintf(buffer, buffersize, "Infinity");
	break;
    case -1:
	if (buffersize > (int)sizeof("-Infinity"))
	    snprintf(buffer, buffersize, "-Infinity");
	break;
    default:
	if (xmlXPathIsNaN(number)) {
	    if (buffersize > (int)sizeof("NaN"))
		snprintf(buffer, buffersize, "NaN");
	} else if (number == 0 && xmlXPathGetSign(number) != 0) {
	    snprintf(buffer, buffersize, "0");
	} else if (number == ((int) number)) {
	    char work[30];
	    char *ptr, *cur;
	    int res, value = (int) number;

            ptr = &buffer[0];
	    if (value < 0) {
		*ptr++ = '-';
		value = -value;
	    }
	    if (value == 0) {
		*ptr++ = '0';
	    } else {
		cur = &work[0];
		while (value != 0) {
		    res = value % 10;
		    value = value / 10;
		    *cur++ = '0' + res;
		}
		cur--;
		while ((cur >= &work[0]) && (ptr - buffer < buffersize)) {
		    *ptr++ = *cur--;
		}
	    }
	    if (ptr - buffer < buffersize) {
		*ptr = 0;
	    } else if (buffersize > 0) {
		ptr--;
		*ptr = 0;
	    }
	} else {
	    /* 3 is sign, decimal point, and terminating zero */
	    char work[DBL_DIG + EXPONENT_DIGITS + 3];
	    int integer_place, fraction_place;
	    char *ptr;
	    char *after_fraction;
	    double absolute_value;
	    int size;

	    absolute_value = fabs(number);

	    /*
	     * First choose format - scientific or regular floating point.
	     * In either case, result is in work, and after_fraction points
	     * just past the fractional part.
	    */
	    if ( ((absolute_value > UPPER_DOUBLE) ||
		  (absolute_value < LOWER_DOUBLE)) &&
		 (absolute_value != 0.0) ) {
		/* Use scientific notation */
		integer_place = DBL_DIG + EXPONENT_DIGITS + 1;
		fraction_place = DBL_DIG - 1;
		snprintf(work, sizeof(work),"%*.*e",
			 integer_place, fraction_place, number);
		after_fraction = strchr(work + DBL_DIG, 'e');
	    }
	    else {
		/* Use regular notation */
		if (absolute_value > 0.0)
		    integer_place = 1 + (int)log10(absolute_value);
		else
		    integer_place = 0;
		fraction_place = (integer_place > 0)
		    ? DBL_DIG - integer_place
		    : DBL_DIG;
		size = snprintf(work, sizeof(work), "%0.*f",
				fraction_place, number);
		after_fraction = work + size;
	    }

	    /* Remove fractional trailing zeroes */
	    ptr = after_fraction;
	    while (*(--ptr) == '0')
		;
	    if (*ptr != '.')
	        ptr++;
	    strcpy(ptr, after_fraction);

	    /* Finally copy result back to caller */
	    size = strlen(work) + 1;
	    if (size > buffersize) {
		work[buffersize - 1] = 0;
		size = buffersize;
	    }
	    memcpy(buffer, work, size);
	}
	break;
    }
}

/************************************************************************
 *									*
 *			Error handling routines				*
 *									*
 ************************************************************************/


static const char *xmlXPathErrorMessages[] = {
    "Ok",
    "Number encoding",
    "Unfinished literal",
    "Start of literal",
    "Expected $ for variable reference",
    "Undefined variable",
    "Invalid predicate",
    "Invalid expression",
    "Missing closing curly brace",
    "Unregistered function",
    "Invalid operand",
    "Invalid type",
    "Invalid number of arguments",
    "Invalid context size",
    "Invalid context position",
    "Memory allocation error",
    "Syntax error",
    "Resource error",
    "Sub resource error",
    "Undefined namespace prefix",
    "Encoding error",
    "Char out of XML range"
};

/**
 * xmlXPatherror:
 * @ctxt:  the XPath Parser context
 * @file:  the file name
 * @line:  the line number
 * @no:  the error number
 *
 * Formats an error message.
 */
void
xmlXPatherror(xmlXPathParserContextPtr ctxt, const char *file ATTRIBUTE_UNUSED,
              int line ATTRIBUTE_UNUSED, int no) {
    int n;
    const xmlChar *cur;
    const xmlChar *base;

    cur = ctxt->cur;
    base = ctxt->base;
    if ((cur == NULL) || (base == NULL)) {
	if ((ctxt->comp != NULL) && (ctxt->comp->expr != NULL)) {
	    xmlGenericError(xmlGenericErrorContext,
		    "XPath error %s in %s\n", xmlXPathErrorMessages[no],
			    ctxt->comp->expr);
	} else {
	    xmlGenericError(xmlGenericErrorContext,
		    "XPath error %s\n", xmlXPathErrorMessages[no]);
	}

	return;
    }
    xmlGenericError(xmlGenericErrorContext,
	    "XPath error %s\n", xmlXPathErrorMessages[no]);

    while ((cur > base) && ((*cur == '\n') || (*cur == '\r'))) {
	cur--;
    }
    n = 0;
    while ((n++ < 80) && (cur > base) && (*cur != '\n') && (*cur != '\r'))
        cur--;
    if ((*cur == '\n') || (*cur == '\r')) cur++;
    base = cur;
    n = 0;
    while ((*cur != 0) && (*cur != '\n') && (*cur != '\r') && (n < 79)) {
        xmlGenericError(xmlGenericErrorContext, "%c", (unsigned char) *cur++);
	n++;
    }
    xmlGenericError(xmlGenericErrorContext, "\n");
    cur = ctxt->cur;
    while ((*cur == '\n') || (*cur == '\r'))
	cur--;
    n = 0;
    while ((cur != base) && (n++ < 80)) {
        xmlGenericError(xmlGenericErrorContext, " ");
        base++;
    }
    xmlGenericError(xmlGenericErrorContext,"^\n");
}


/************************************************************************
 *									*
 *			Routines to handle NodeSets			*
 *									*
 ************************************************************************/

/**
 * xmlXPathCmpNodes:
 * @node1:  the first node
 * @node2:  the second node
 *
 * Compare two nodes w.r.t document order
 *
 * Returns -2 in case of error 1 if first point < second point, 0 if
 *         that's the same node, -1 otherwise
 */
int
xmlXPathCmpNodes(xmlNodePtr node1, xmlNodePtr node2) {
    int depth1, depth2;
    xmlNodePtr cur, root;

    if ((node1 == NULL) || (node2 == NULL))
	return(-2);
    /*
     * a couple of optimizations which will avoid computations in most cases
     */
    if (node1 == node2)
	return(0);
    if ((node1->type == XML_NAMESPACE_DECL) ||
        (node2->type == XML_NAMESPACE_DECL))
	return(1);
    if (node1 == node2->prev)
	return(1);
    if (node1 == node2->next)
	return(-1);

#if 0
    Unfortunately this does not work. Line number in entities reset
    to 1 within the entity :-(

    /*
     * Speedup using line numbers if availble.
     */
    if ((node1->type == XML_ELEMENT_NODE) &&
	(node2->type == XML_ELEMENT_NODE) &&
	(0 != (int) node1->content) && (0 != (int) node2->content)) {
	int l1, l2;
	l1 = (int) node1->content;
	l2 = (int) node2->content;
	if (l1 < l2)
	    return(1);
	if (l1 > l2)
	    return(-1);
    }
#endif
    /*
     * compute depth to root
     */
    for (depth2 = 0, cur = node2;cur->parent != NULL;cur = cur->parent) {
	if (cur == node1)
	    return(1);
	depth2++;
    }
    root = cur;
    for (depth1 = 0, cur = node1;cur->parent != NULL;cur = cur->parent) {
	if (cur == node2)
	    return(-1);
	depth1++;
    }
    /*
     * Distinct document (or distinct entities :-( ) case.
     */
    if (root != cur) {
	return(-2);
    }
    /*
     * get the nearest common ancestor.
     */
    while (depth1 > depth2) {
	depth1--;
	node1 = node1->parent;
    }
    while (depth2 > depth1) {
	depth2--;
	node2 = node2->parent;
    }
    while (node1->parent != node2->parent) {
	node1 = node1->parent;
	node2 = node2->parent;
	/* should not happen but just in case ... */
	if ((node1 == NULL) || (node2 == NULL))
	    return(-2);
    }
    /*
     * Find who's first.
     */
    if (node1 == node2->next)
	return(-1);
    for (cur = node1->next;cur != NULL;cur = cur->next)
	if (cur == node2)
	    return(1);
    return(-1); /* assume there is no sibling list corruption */
}

/**
 * xmlXPathNodeSetSort:
 * @set:  the node set
 *
 * Sort the node set in document order
 */
void
xmlXPathNodeSetSort(xmlNodeSetPtr set) {
    int i, j, incr, len;
    xmlNodePtr tmp;

    if (set == NULL)
	return;

    /* Use Shell's sort to sort the node-set */
    len = set->nodeNr;
    for (incr = len / 2; incr > 0; incr /= 2) {
	for (i = incr; i < len; i++) {
	    j = i - incr;
	    while (j >= 0) {
		if (xmlXPathCmpNodes(set->nodeTab[j],
				     set->nodeTab[j + incr]) == -1) {
		    tmp = set->nodeTab[j];
		    set->nodeTab[j] = set->nodeTab[j + incr];
		    set->nodeTab[j + incr] = tmp;
		    j -= incr;
		} else
		    break;
	    }
	}
    }
}

#define XML_NODESET_DEFAULT	10
/**
 * xmlXPathNodeSetDupNs:
 * @node:  the parent node of the namespace XPath node
 * @ns:  the libxml namespace declaration node.
 *
 * Namespace node in libxml don't match the XPath semantic. In a node set
 * the namespace nodes are duplicated and the next pointer is set to the
 * parent node in the XPath semantic.
 *
 * Returns the newly created object.
 */
static xmlNodePtr
xmlXPathNodeSetDupNs(xmlNodePtr node, xmlNsPtr ns) {
    xmlNsPtr cur;

    if ((ns == NULL) || (ns->type != XML_NAMESPACE_DECL))
	return(NULL);
    if ((node == NULL) || (node->type == XML_NAMESPACE_DECL))
	return((xmlNodePtr) ns);

    /*
     * Allocate a new Namespace and fill the fields.
     */
    cur = (xmlNsPtr) xmlMalloc(sizeof(xmlNs));
    if (cur == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlXPathNodeSetDupNs : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xmlNs));
    cur->type = XML_NAMESPACE_DECL;
    if (ns->href != NULL)
	cur->href = xmlStrdup(ns->href); 
    if (ns->prefix != NULL)
	cur->prefix = xmlStrdup(ns->prefix); 
    cur->next = (xmlNsPtr) node;
    return((xmlNodePtr) cur);
}

/**
 * xmlXPathNodeSetFreeNs:
 * @ns:  the XPath namespace node found in a nodeset.
 *
 * Namespace node in libxml don't match the XPath semantic. In a node set
 * the namespace nodes are duplicated and the next pointer is set to the
 * parent node in the XPath semantic. Check if such a node need to be freed
 */
void
xmlXPathNodeSetFreeNs(xmlNsPtr ns) {
    if ((ns == NULL) || (ns->type != XML_NAMESPACE_DECL))
	return;

    if ((ns->next != NULL) && (ns->next->type != XML_NAMESPACE_DECL)) {
	if (ns->href != NULL)
	    xmlFree((xmlChar *)ns->href);
	if (ns->prefix != NULL)
	    xmlFree((xmlChar *)ns->prefix);
	xmlFree(ns);
    }
}

/**
 * xmlXPathNodeSetCreate:
 * @val:  an initial xmlNodePtr, or NULL
 *
 * Create a new xmlNodeSetPtr of type double and of value @val
 *
 * Returns the newly created object.
 */
xmlNodeSetPtr
xmlXPathNodeSetCreate(xmlNodePtr val) {
    xmlNodeSetPtr ret;

    ret = (xmlNodeSetPtr) xmlMalloc(sizeof(xmlNodeSet));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlXPathNodeSetCreate: out of memory\n");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlNodeSet));
    if (val != NULL) {
        ret->nodeTab = (xmlNodePtr *) xmlMalloc(XML_NODESET_DEFAULT *
					     sizeof(xmlNodePtr));
	if (ret->nodeTab == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "xmlXPathNodeSetCreate: out of memory\n");
	    return(NULL);
	}
	memset(ret->nodeTab, 0 ,
	       XML_NODESET_DEFAULT * (size_t) sizeof(xmlNodePtr));
        ret->nodeMax = XML_NODESET_DEFAULT;
	if (val->type == XML_NAMESPACE_DECL) {
	    xmlNsPtr ns = (xmlNsPtr) val;

	    ret->nodeTab[ret->nodeNr++] =
		xmlXPathNodeSetDupNs((xmlNodePtr) ns->next, ns);
	} else
	    ret->nodeTab[ret->nodeNr++] = val;
    }
    return(ret);
}

/**
 * xmlXPathNodeSetContains:
 * @cur:  the node-set
 * @val:  the node
 *
 * checks whether @cur contains @val
 *
 * Returns true (1) if @cur contains @val, false (0) otherwise
 */
int
xmlXPathNodeSetContains (xmlNodeSetPtr cur, xmlNodePtr val) {
    int i;

    if (val->type == XML_NAMESPACE_DECL) {
	for (i = 0; i < cur->nodeNr; i++) {
	    if (cur->nodeTab[i]->type == XML_NAMESPACE_DECL) {
		xmlNsPtr ns1, ns2;

		ns1 = (xmlNsPtr) val;
		ns2 = (xmlNsPtr) cur->nodeTab[i];
		if (ns1 == ns2)
		    return(1);
		if ((ns1->next != NULL) && (ns2->next == ns1->next) &&
	            (xmlStrEqual(ns1->prefix, ns2->prefix)))
		    return(1);
	    }
	}
    } else {
	for (i = 0; i < cur->nodeNr; i++) {
	    if (cur->nodeTab[i] == val)
		return(1);
	}
    }
    return(0);
}

/**
 * xmlXPathNodeSetAddNs:
 * @cur:  the initial node set
 * @node:  the hosting node
 * @ns:  a the namespace node
 *
 * add a new namespace node to an existing NodeSet
 */
void
xmlXPathNodeSetAddNs(xmlNodeSetPtr cur, xmlNodePtr node, xmlNsPtr ns) {
    int i;

    if ((ns == NULL) || (node == NULL) || (ns->type != XML_NAMESPACE_DECL) ||
	(node->type != XML_ELEMENT_NODE))
	return;

    /* @@ with_ns to check wether namespace nodes should be looked at @@ */
    /*
     * check against doublons
     */
    for (i = 0;i < cur->nodeNr;i++) {
        if ((cur->nodeTab[i] != NULL) &&
	    (cur->nodeTab[i]->type == XML_NAMESPACE_DECL) &&
	    (((xmlNsPtr)cur->nodeTab[i])->next == (xmlNsPtr) node) &&
	    (xmlStrEqual(ns->prefix, ((xmlNsPtr)cur->nodeTab[i])->prefix)))
	    return;
    }

    /*
     * grow the nodeTab if needed
     */
    if (cur->nodeMax == 0) {
        cur->nodeTab = (xmlNodePtr *) xmlMalloc(XML_NODESET_DEFAULT *
					     sizeof(xmlNodePtr));
	if (cur->nodeTab == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "xmlXPathNodeSetAdd: out of memory\n");
	    return;
	}
	memset(cur->nodeTab, 0 ,
	       XML_NODESET_DEFAULT * (size_t) sizeof(xmlNodePtr));
        cur->nodeMax = XML_NODESET_DEFAULT;
    } else if (cur->nodeNr == cur->nodeMax) {
        xmlNodePtr *temp;

        cur->nodeMax *= 2;
	temp = (xmlNodePtr *) xmlRealloc(cur->nodeTab, cur->nodeMax *
				      sizeof(xmlNodePtr));
	if (temp == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "xmlXPathNodeSetAdd: out of memory\n");
	    return;
	}
	cur->nodeTab = temp;
    }
    cur->nodeTab[cur->nodeNr++] = xmlXPathNodeSetDupNs(node, ns);
}

/**
 * xmlXPathNodeSetAdd:
 * @cur:  the initial node set
 * @val:  a new xmlNodePtr
 *
 * add a new xmlNodePtr to an existing NodeSet
 */
void
xmlXPathNodeSetAdd(xmlNodeSetPtr cur, xmlNodePtr val) {
    int i;

    if (val == NULL) return;

    /* @@ with_ns to check wether namespace nodes should be looked at @@ */
    /*
     * check against doublons
     */
    for (i = 0;i < cur->nodeNr;i++)
        if (cur->nodeTab[i] == val) return;

    /*
     * grow the nodeTab if needed
     */
    if (cur->nodeMax == 0) {
        cur->nodeTab = (xmlNodePtr *) xmlMalloc(XML_NODESET_DEFAULT *
					     sizeof(xmlNodePtr));
	if (cur->nodeTab == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "xmlXPathNodeSetAdd: out of memory\n");
	    return;
	}
	memset(cur->nodeTab, 0 ,
	       XML_NODESET_DEFAULT * (size_t) sizeof(xmlNodePtr));
        cur->nodeMax = XML_NODESET_DEFAULT;
    } else if (cur->nodeNr == cur->nodeMax) {
        xmlNodePtr *temp;

        cur->nodeMax *= 2;
	temp = (xmlNodePtr *) xmlRealloc(cur->nodeTab, cur->nodeMax *
				      sizeof(xmlNodePtr));
	if (temp == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "xmlXPathNodeSetAdd: out of memory\n");
	    return;
	}
	cur->nodeTab = temp;
    }
    if (val->type == XML_NAMESPACE_DECL) {
	xmlNsPtr ns = (xmlNsPtr) val;

	cur->nodeTab[cur->nodeNr++] = 
	    xmlXPathNodeSetDupNs((xmlNodePtr) ns->next, ns);
    } else
	cur->nodeTab[cur->nodeNr++] = val;
}

/**
 * xmlXPathNodeSetAddUnique:
 * @cur:  the initial node set
 * @val:  a new xmlNodePtr
 *
 * add a new xmlNodePtr to an existing NodeSet, optimized version
 * when we are sure the node is not already in the set.
 */
void
xmlXPathNodeSetAddUnique(xmlNodeSetPtr cur, xmlNodePtr val) {
    if (val == NULL) return;

    /* @@ with_ns to check wether namespace nodes should be looked at @@ */
    /*
     * grow the nodeTab if needed
     */
    if (cur->nodeMax == 0) {
        cur->nodeTab = (xmlNodePtr *) xmlMalloc(XML_NODESET_DEFAULT *
					     sizeof(xmlNodePtr));
	if (cur->nodeTab == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "xmlXPathNodeSetAddUnique: out of memory\n");
	    return;
	}
	memset(cur->nodeTab, 0 ,
	       XML_NODESET_DEFAULT * (size_t) sizeof(xmlNodePtr));
        cur->nodeMax = XML_NODESET_DEFAULT;
    } else if (cur->nodeNr == cur->nodeMax) {
        xmlNodePtr *temp;

        cur->nodeMax *= 2;
	temp = (xmlNodePtr *) xmlRealloc(cur->nodeTab, cur->nodeMax *
				      sizeof(xmlNodePtr));
	if (temp == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "xmlXPathNodeSetAddUnique: out of memory\n");
	    return;
	}
	cur->nodeTab = temp;
    }
    if (val->type == XML_NAMESPACE_DECL) {
	xmlNsPtr ns = (xmlNsPtr) val;

	cur->nodeTab[cur->nodeNr++] = 
	    xmlXPathNodeSetDupNs((xmlNodePtr) ns->next, ns);
    } else
	cur->nodeTab[cur->nodeNr++] = val;
}

/**
 * xmlXPathNodeSetMerge:
 * @val1:  the first NodeSet or NULL
 * @val2:  the second NodeSet
 *
 * Merges two nodesets, all nodes from @val2 are added to @val1
 * if @val1 is NULL, a new set is created and copied from @val2
 *
 * Returns @val1 once extended or NULL in case of error.
 */
xmlNodeSetPtr
xmlXPathNodeSetMerge(xmlNodeSetPtr val1, xmlNodeSetPtr val2) {
    int i, j, initNr, skip;

    if (val2 == NULL) return(val1);
    if (val1 == NULL) {
	val1 = xmlXPathNodeSetCreate(NULL);
    }

    /* @@ with_ns to check wether namespace nodes should be looked at @@ */
    initNr = val1->nodeNr;

    for (i = 0;i < val2->nodeNr;i++) {
	/*
	 * check against doublons
	 */
	skip = 0;
	for (j = 0; j < initNr; j++) {
	    if (val1->nodeTab[j] == val2->nodeTab[i]) {
		skip = 1;
		break;
	    } else if ((val1->nodeTab[j]->type == XML_NAMESPACE_DECL) &&
		       (val2->nodeTab[i]->type == XML_NAMESPACE_DECL)) {
		xmlNsPtr ns1, ns2;
		ns1 = (xmlNsPtr) val1->nodeTab[j];
		ns2 = (xmlNsPtr) val2->nodeTab[i];
		if ((ns1->next == ns2->next) &&
		    (xmlStrEqual(ns1->prefix, ns2->prefix))) {
		    skip = 1;
		    break;
		}
	    }
	}
	if (skip)
	    continue;

	/*
	 * grow the nodeTab if needed
	 */
	if (val1->nodeMax == 0) {
	    val1->nodeTab = (xmlNodePtr *) xmlMalloc(XML_NODESET_DEFAULT *
						    sizeof(xmlNodePtr));
	    if (val1->nodeTab == NULL) {
		xmlGenericError(xmlGenericErrorContext,
				"xmlXPathNodeSetMerge: out of memory\n");
		return(NULL);
	    }
	    memset(val1->nodeTab, 0 ,
		   XML_NODESET_DEFAULT * (size_t) sizeof(xmlNodePtr));
	    val1->nodeMax = XML_NODESET_DEFAULT;
	} else if (val1->nodeNr == val1->nodeMax) {
	    xmlNodePtr *temp;

	    val1->nodeMax *= 2;
	    temp = (xmlNodePtr *) xmlRealloc(val1->nodeTab, val1->nodeMax *
					     sizeof(xmlNodePtr));
	    if (temp == NULL) {
		xmlGenericError(xmlGenericErrorContext,
				"xmlXPathNodeSetMerge: out of memory\n");
		return(NULL);
	    }
	    val1->nodeTab = temp;
	}
	if (val2->nodeTab[i]->type == XML_NAMESPACE_DECL) {
	    xmlNsPtr ns = (xmlNsPtr) val2->nodeTab[i];

	    val1->nodeTab[val1->nodeNr++] =
		xmlXPathNodeSetDupNs((xmlNodePtr) ns->next, ns);
	} else
	    val1->nodeTab[val1->nodeNr++] = val2->nodeTab[i];
    }

    return(val1);
}

/**
 * xmlXPathNodeSetMergeUnique:
 * @val1:  the first NodeSet or NULL
 * @val2:  the second NodeSet
 *
 * Merges two nodesets, all nodes from @val2 are added to @val1
 * if @val1 is NULL, a new set is created and copied from @val2
 *
 * Returns @val1 once extended or NULL in case of error.
 */
static xmlNodeSetPtr
xmlXPathNodeSetMergeUnique(xmlNodeSetPtr val1, xmlNodeSetPtr val2) {
    int i, initNr;

    if (val2 == NULL) return(val1);
    if (val1 == NULL) {
	val1 = xmlXPathNodeSetCreate(NULL);
    }

    /* @@ with_ns to check wether namespace nodes should be looked at @@ */
    initNr = val1->nodeNr;

    for (i = 0;i < val2->nodeNr;i++) {
	/*
	 * grow the nodeTab if needed
	 */
	if (val1->nodeMax == 0) {
	    val1->nodeTab = (xmlNodePtr *) xmlMalloc(XML_NODESET_DEFAULT *
						    sizeof(xmlNodePtr));
	    if (val1->nodeTab == NULL) {
		xmlGenericError(xmlGenericErrorContext,
				"xmlXPathNodeSetMerge: out of memory\n");
		return(NULL);
	    }
	    memset(val1->nodeTab, 0 ,
		   XML_NODESET_DEFAULT * (size_t) sizeof(xmlNodePtr));
	    val1->nodeMax = XML_NODESET_DEFAULT;
	} else if (val1->nodeNr == val1->nodeMax) {
	    xmlNodePtr *temp;

	    val1->nodeMax *= 2;
	    temp = (xmlNodePtr *) xmlRealloc(val1->nodeTab, val1->nodeMax *
					     sizeof(xmlNodePtr));
	    if (temp == NULL) {
		xmlGenericError(xmlGenericErrorContext,
				"xmlXPathNodeSetMerge: out of memory\n");
		return(NULL);
	    }
	    val1->nodeTab = temp;
	}
	if (val2->nodeTab[i]->type == XML_NAMESPACE_DECL) {
	    xmlNsPtr ns = (xmlNsPtr) val2->nodeTab[i];

	    val1->nodeTab[val1->nodeNr++] =
		xmlXPathNodeSetDupNs((xmlNodePtr) ns->next, ns);
	} else
	    val1->nodeTab[val1->nodeNr++] = val2->nodeTab[i];
    }

    return(val1);
}

/**
 * xmlXPathNodeSetDel:
 * @cur:  the initial node set
 * @val:  an xmlNodePtr
 *
 * Removes an xmlNodePtr from an existing NodeSet
 */
void
xmlXPathNodeSetDel(xmlNodeSetPtr cur, xmlNodePtr val) {
    int i;

    if (cur == NULL) return;
    if (val == NULL) return;

    /*
     * check against doublons
     */
    for (i = 0;i < cur->nodeNr;i++)
        if (cur->nodeTab[i] == val) break;

    if (i >= cur->nodeNr) {
#ifdef DEBUG
        xmlGenericError(xmlGenericErrorContext, 
	        "xmlXPathNodeSetDel: Node %s wasn't found in NodeList\n",
		val->name);
#endif
        return;
    }
    if ((cur->nodeTab[i] != NULL) &&
	(cur->nodeTab[i]->type == XML_NAMESPACE_DECL))
	xmlXPathNodeSetFreeNs((xmlNsPtr) cur->nodeTab[i]);
    cur->nodeNr--;
    for (;i < cur->nodeNr;i++)
        cur->nodeTab[i] = cur->nodeTab[i + 1];
    cur->nodeTab[cur->nodeNr] = NULL;
}

/**
 * xmlXPathNodeSetRemove:
 * @cur:  the initial node set
 * @val:  the index to remove
 *
 * Removes an entry from an existing NodeSet list.
 */
void
xmlXPathNodeSetRemove(xmlNodeSetPtr cur, int val) {
    if (cur == NULL) return;
    if (val >= cur->nodeNr) return;
    if ((cur->nodeTab[val] != NULL) &&
	(cur->nodeTab[val]->type == XML_NAMESPACE_DECL))
	xmlXPathNodeSetFreeNs((xmlNsPtr) cur->nodeTab[val]);
    cur->nodeNr--;
    for (;val < cur->nodeNr;val++)
        cur->nodeTab[val] = cur->nodeTab[val + 1];
    cur->nodeTab[cur->nodeNr] = NULL;
}

/**
 * xmlXPathFreeNodeSet:
 * @obj:  the xmlNodeSetPtr to free
 *
 * Free the NodeSet compound (not the actual nodes !).
 */
void
xmlXPathFreeNodeSet(xmlNodeSetPtr obj) {
    if (obj == NULL) return;
    if (obj->nodeTab != NULL) {
	int i;

	/* @@ with_ns to check wether namespace nodes should be looked at @@ */
	for (i = 0;i < obj->nodeNr;i++)
	    if ((obj->nodeTab[i] != NULL) &&
		(obj->nodeTab[i]->type == XML_NAMESPACE_DECL))
		xmlXPathNodeSetFreeNs((xmlNsPtr) obj->nodeTab[i]);
	xmlFree(obj->nodeTab);
    }
    xmlFree(obj);
}

/**
 * xmlXPathFreeValueTree:
 * @obj:  the xmlNodeSetPtr to free
 *
 * Free the NodeSet compound and the actual tree, this is different
 * from xmlXPathFreeNodeSet()
 */
static void
xmlXPathFreeValueTree(xmlNodeSetPtr obj) {
    int i;

    if (obj == NULL) return;

    if (obj->nodeTab != NULL) {
	for (i = 0;i < obj->nodeNr;i++) {
	    if (obj->nodeTab[i] != NULL) {
		if (obj->nodeTab[i]->type == XML_NAMESPACE_DECL) {
		    xmlXPathNodeSetFreeNs((xmlNsPtr) obj->nodeTab[i]);
		} else {
		    xmlFreeNodeList(obj->nodeTab[i]);
		}
	    }
	}
	xmlFree(obj->nodeTab);
    }
    xmlFree(obj);
}

#if defined(DEBUG) || defined(DEBUG_STEP)
/**
 * xmlGenericErrorContextNodeSet:
 * @output:  a FILE * for the output
 * @obj:  the xmlNodeSetPtr to free
 *
 * Quick display of a NodeSet
 */
void
xmlGenericErrorContextNodeSet(FILE *output, xmlNodeSetPtr obj) {
    int i;

    if (output == NULL) output = xmlGenericErrorContext;
    if (obj == NULL)  {
        fprintf(output, "NodeSet == NULL !\n");
	return;
    }
    if (obj->nodeNr == 0) {
        fprintf(output, "NodeSet is empty\n");
	return;
    }
    if (obj->nodeTab == NULL) {
	fprintf(output, " nodeTab == NULL !\n");
	return;
    }
    for (i = 0; i < obj->nodeNr; i++) {
        if (obj->nodeTab[i] == NULL) {
	    fprintf(output, " NULL !\n");
	    return;
        }
	if ((obj->nodeTab[i]->type == XML_DOCUMENT_NODE) ||
	    (obj->nodeTab[i]->type == XML_HTML_DOCUMENT_NODE))
	    fprintf(output, " /");
	else if (obj->nodeTab[i]->name == NULL)
	    fprintf(output, " noname!");
	else fprintf(output, " %s", obj->nodeTab[i]->name);
    }
    fprintf(output, "\n");
}
#endif

/**
 * xmlXPathNewNodeSet:
 * @val:  the NodePtr value
 *
 * Create a new xmlXPathObjectPtr of type NodeSet and initialize
 * it with the single Node @val
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPathNewNodeSet(xmlNodePtr val) {
    xmlXPathObjectPtr ret;

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlXPathNewNodeSet: out of memory\n");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_NODESET;
    ret->boolval = 0;
    ret->nodesetval = xmlXPathNodeSetCreate(val);
    /* @@ with_ns to check wether namespace nodes should be looked at @@ */
    return(ret);
}

/**
 * xmlXPathNewValueTree:
 * @val:  the NodePtr value
 *
 * Create a new xmlXPathObjectPtr of type Value Tree (XSLT) and initialize
 * it with the tree root @val
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPathNewValueTree(xmlNodePtr val) {
    xmlXPathObjectPtr ret;

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlXPathNewNodeSet: out of memory\n");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_XSLT_TREE;
    ret->boolval = 1;
    ret->user = (void *) val;
    ret->nodesetval = xmlXPathNodeSetCreate(val);
    return(ret);
}

/**
 * xmlXPathNewNodeSetList:
 * @val:  an existing NodeSet
 *
 * Create a new xmlXPathObjectPtr of type NodeSet and initialize
 * it with the Nodeset @val
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPathNewNodeSetList(xmlNodeSetPtr val)
{
    xmlXPathObjectPtr ret;
    int i;

    if (val == NULL)
        ret = NULL;
    else if (val->nodeTab == NULL)
        ret = xmlXPathNewNodeSet(NULL);
    else {
        ret = xmlXPathNewNodeSet(val->nodeTab[0]);
        for (i = 1; i < val->nodeNr; ++i)
            xmlXPathNodeSetAddUnique(ret->nodesetval, val->nodeTab[i]);
    }

    return (ret);
}

/**
 * xmlXPathWrapNodeSet:
 * @val:  the NodePtr value
 *
 * Wrap the Nodeset @val in a new xmlXPathObjectPtr
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPathWrapNodeSet(xmlNodeSetPtr val) {
    xmlXPathObjectPtr ret;

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlXPathWrapNodeSet: out of memory\n");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_NODESET;
    ret->nodesetval = val;
    return(ret);
}

/**
 * xmlXPathFreeNodeSetList:
 * @obj:  an existing NodeSetList object
 *
 * Free up the xmlXPathObjectPtr @obj but don't deallocate the objects in
 * the list contrary to xmlXPathFreeObject().
 */
void
xmlXPathFreeNodeSetList(xmlXPathObjectPtr obj) {
    if (obj == NULL) return;
    xmlFree(obj);
}

/**
 * xmlXPathDifference:
 * @nodes1:  a node-set
 * @nodes2:  a node-set
 *
 * Implements the EXSLT - Sets difference() function:
 *    node-set set:difference (node-set, node-set)
 *
 * Returns the difference between the two node sets, or nodes1 if
 *         nodes2 is empty
 */
xmlNodeSetPtr
xmlXPathDifference (xmlNodeSetPtr nodes1, xmlNodeSetPtr nodes2) {
    xmlNodeSetPtr ret;
    int i, l1;
    xmlNodePtr cur;

    if (xmlXPathNodeSetIsEmpty(nodes2))
	return(nodes1);

    ret = xmlXPathNodeSetCreate(NULL);
    if (xmlXPathNodeSetIsEmpty(nodes1))
	return(ret);

    l1 = xmlXPathNodeSetGetLength(nodes1);

    for (i = 0; i < l1; i++) {
	cur = xmlXPathNodeSetItem(nodes1, i);
	if (!xmlXPathNodeSetContains(nodes2, cur))
	    xmlXPathNodeSetAddUnique(ret, cur);
    }
    return(ret);
}

/**
 * xmlXPathIntersection:
 * @nodes1:  a node-set
 * @nodes2:  a node-set
 *
 * Implements the EXSLT - Sets intersection() function:
 *    node-set set:intersection (node-set, node-set)
 *
 * Returns a node set comprising the nodes that are within both the
 *         node sets passed as arguments
 */
xmlNodeSetPtr
xmlXPathIntersection (xmlNodeSetPtr nodes1, xmlNodeSetPtr nodes2) {
    xmlNodeSetPtr ret = xmlXPathNodeSetCreate(NULL);
    int i, l1;
    xmlNodePtr cur;

    if (xmlXPathNodeSetIsEmpty(nodes1))
	return(ret);
    if (xmlXPathNodeSetIsEmpty(nodes2))
	return(ret);

    l1 = xmlXPathNodeSetGetLength(nodes1);

    for (i = 0; i < l1; i++) {
	cur = xmlXPathNodeSetItem(nodes1, i);
	if (xmlXPathNodeSetContains(nodes2, cur))
	    xmlXPathNodeSetAddUnique(ret, cur);
    }
    return(ret);
}

/**
 * xmlXPathDistinctSorted:
 * @nodes:  a node-set, sorted by document order
 *
 * Implements the EXSLT - Sets distinct() function:
 *    node-set set:distinct (node-set)
 * 
 * Returns a subset of the nodes contained in @nodes, or @nodes if
 *         it is empty
 */
xmlNodeSetPtr
xmlXPathDistinctSorted (xmlNodeSetPtr nodes) {
    xmlNodeSetPtr ret;
    xmlHashTablePtr hash;
    int i, l;
    xmlChar * strval;
    xmlNodePtr cur;

    if (xmlXPathNodeSetIsEmpty(nodes))
	return(nodes);

    ret = xmlXPathNodeSetCreate(NULL);
    l = xmlXPathNodeSetGetLength(nodes);
    hash = xmlHashCreate (l);
    for (i = 0; i < l; i++) {
	cur = xmlXPathNodeSetItem(nodes, i);
	strval = xmlXPathCastNodeToString(cur);
	if (xmlHashLookup(hash, strval) == NULL) {
	    xmlHashAddEntry(hash, strval, strval);
	    xmlXPathNodeSetAddUnique(ret, cur);
	} else {
	    xmlFree(strval);
	}
    }
    xmlHashFree(hash, (xmlHashDeallocator) xmlFree);
    return(ret);
}

/**
 * xmlXPathDistinct:
 * @nodes:  a node-set
 *
 * Implements the EXSLT - Sets distinct() function:
 *    node-set set:distinct (node-set)
 * @nodes is sorted by document order, then #exslSetsDistinctSorted
 * is called with the sorted node-set
 *
 * Returns a subset of the nodes contained in @nodes, or @nodes if
 *         it is empty
 */
xmlNodeSetPtr
xmlXPathDistinct (xmlNodeSetPtr nodes) {
    if (xmlXPathNodeSetIsEmpty(nodes))
	return(nodes);

    xmlXPathNodeSetSort(nodes);
    return(xmlXPathDistinctSorted(nodes));
}

/**
 * xmlXPathHasSameNodes:
 * @nodes1:  a node-set
 * @nodes2:  a node-set
 *
 * Implements the EXSLT - Sets has-same-nodes function:
 *    boolean set:has-same-node(node-set, node-set)
 *
 * Returns true (1) if @nodes1 shares any node with @nodes2, false (0)
 *         otherwise
 */
int
xmlXPathHasSameNodes (xmlNodeSetPtr nodes1, xmlNodeSetPtr nodes2) {
    int i, l;
    xmlNodePtr cur;

    if (xmlXPathNodeSetIsEmpty(nodes1) ||
	xmlXPathNodeSetIsEmpty(nodes2))
	return(0);

    l = xmlXPathNodeSetGetLength(nodes1);
    for (i = 0; i < l; i++) {
	cur = xmlXPathNodeSetItem(nodes1, i);
	if (xmlXPathNodeSetContains(nodes2, cur))
	    return(1);
    }
    return(0);
}

/**
 * xmlXPathNodeLeadingSorted:
 * @nodes: a node-set, sorted by document order
 * @node: a node
 *
 * Implements the EXSLT - Sets leading() function:
 *    node-set set:leading (node-set, node-set)
 *
 * Returns the nodes in @nodes that precede @node in document order,
 *         @nodes if @node is NULL or an empty node-set if @nodes
 *         doesn't contain @node
 */
xmlNodeSetPtr
xmlXPathNodeLeadingSorted (xmlNodeSetPtr nodes, xmlNodePtr node) {
    int i, l;
    xmlNodePtr cur;
    xmlNodeSetPtr ret;

    if (node == NULL)
	return(nodes);

    ret = xmlXPathNodeSetCreate(NULL);
    if (xmlXPathNodeSetIsEmpty(nodes) ||
	(!xmlXPathNodeSetContains(nodes, node)))
	return(ret);

    l = xmlXPathNodeSetGetLength(nodes);
    for (i = 0; i < l; i++) {
	cur = xmlXPathNodeSetItem(nodes, i);
	if (cur == node)
	    break;
	xmlXPathNodeSetAddUnique(ret, cur);
    }
    return(ret);
}

/**
 * xmlXPathNodeLeading:
 * @nodes:  a node-set
 * @node:  a node
 *
 * Implements the EXSLT - Sets leading() function:
 *    node-set set:leading (node-set, node-set)
 * @nodes is sorted by document order, then #exslSetsNodeLeadingSorted
 * is called.
 *
 * Returns the nodes in @nodes that precede @node in document order,
 *         @nodes if @node is NULL or an empty node-set if @nodes
 *         doesn't contain @node
 */
xmlNodeSetPtr
xmlXPathNodeLeading (xmlNodeSetPtr nodes, xmlNodePtr node) {
    xmlXPathNodeSetSort(nodes);
    return(xmlXPathNodeLeadingSorted(nodes, node));
}

/**
 * xmlXPathLeadingSorted:
 * @nodes1:  a node-set, sorted by document order
 * @nodes2:  a node-set, sorted by document order
 *
 * Implements the EXSLT - Sets leading() function:
 *    node-set set:leading (node-set, node-set)
 *
 * Returns the nodes in @nodes1 that precede the first node in @nodes2
 *         in document order, @nodes1 if @nodes2 is NULL or empty or
 *         an empty node-set if @nodes1 doesn't contain @nodes2
 */
xmlNodeSetPtr
xmlXPathLeadingSorted (xmlNodeSetPtr nodes1, xmlNodeSetPtr nodes2) {
    if (xmlXPathNodeSetIsEmpty(nodes2))
	return(nodes1);
    return(xmlXPathNodeLeadingSorted(nodes1,
				     xmlXPathNodeSetItem(nodes2, 1)));
}

/**
 * xmlXPathLeading:
 * @nodes1:  a node-set
 * @nodes2:  a node-set
 *
 * Implements the EXSLT - Sets leading() function:
 *    node-set set:leading (node-set, node-set)
 * @nodes1 and @nodes2 are sorted by document order, then
 * #exslSetsLeadingSorted is called.
 *
 * Returns the nodes in @nodes1 that precede the first node in @nodes2
 *         in document order, @nodes1 if @nodes2 is NULL or empty or
 *         an empty node-set if @nodes1 doesn't contain @nodes2
 */
xmlNodeSetPtr
xmlXPathLeading (xmlNodeSetPtr nodes1, xmlNodeSetPtr nodes2) {
    if (xmlXPathNodeSetIsEmpty(nodes2))
	return(nodes1);
    if (xmlXPathNodeSetIsEmpty(nodes1))
	return(xmlXPathNodeSetCreate(NULL));
    xmlXPathNodeSetSort(nodes1);
    xmlXPathNodeSetSort(nodes2);
    return(xmlXPathNodeLeadingSorted(nodes1,
				     xmlXPathNodeSetItem(nodes2, 1)));
}

/**
 * xmlXPathNodeTrailingSorted:
 * @nodes: a node-set, sorted by document order
 * @node: a node
 *
 * Implements the EXSLT - Sets trailing() function:
 *    node-set set:trailing (node-set, node-set)
 *
 * Returns the nodes in @nodes that follow @node in document order,
 *         @nodes if @node is NULL or an empty node-set if @nodes
 *         doesn't contain @node
 */
xmlNodeSetPtr
xmlXPathNodeTrailingSorted (xmlNodeSetPtr nodes, xmlNodePtr node) {
    int i, l;
    xmlNodePtr cur;
    xmlNodeSetPtr ret;

    if (node == NULL)
	return(nodes);

    ret = xmlXPathNodeSetCreate(NULL);
    if (xmlXPathNodeSetIsEmpty(nodes) ||
	(!xmlXPathNodeSetContains(nodes, node)))
	return(ret);

    l = xmlXPathNodeSetGetLength(nodes);
    for (i = l; i > 0; i--) {
	cur = xmlXPathNodeSetItem(nodes, i);
	if (cur == node)
	    break;
	xmlXPathNodeSetAddUnique(ret, cur);
    }
    return(ret);
}

/**
 * xmlXPathNodeTrailing:
 * @nodes:  a node-set
 * @node:  a node
 *
 * Implements the EXSLT - Sets trailing() function:
 *    node-set set:trailing (node-set, node-set)
 * @nodes is sorted by document order, then #xmlXPathNodeTrailingSorted
 * is called.
 *
 * Returns the nodes in @nodes that follow @node in document order,
 *         @nodes if @node is NULL or an empty node-set if @nodes
 *         doesn't contain @node
 */
xmlNodeSetPtr
xmlXPathNodeTrailing (xmlNodeSetPtr nodes, xmlNodePtr node) {
    xmlXPathNodeSetSort(nodes);
    return(xmlXPathNodeTrailingSorted(nodes, node));
}

/**
 * xmlXPathTrailingSorted:
 * @nodes1:  a node-set, sorted by document order
 * @nodes2:  a node-set, sorted by document order
 *
 * Implements the EXSLT - Sets trailing() function:
 *    node-set set:trailing (node-set, node-set)
 *
 * Returns the nodes in @nodes1 that follow the first node in @nodes2
 *         in document order, @nodes1 if @nodes2 is NULL or empty or
 *         an empty node-set if @nodes1 doesn't contain @nodes2
 */
xmlNodeSetPtr
xmlXPathTrailingSorted (xmlNodeSetPtr nodes1, xmlNodeSetPtr nodes2) {
    if (xmlXPathNodeSetIsEmpty(nodes2))
	return(nodes1);
    return(xmlXPathNodeTrailingSorted(nodes1,
				      xmlXPathNodeSetItem(nodes2, 0)));
}

/**
 * xmlXPathTrailing:
 * @nodes1:  a node-set
 * @nodes2:  a node-set
 *
 * Implements the EXSLT - Sets trailing() function:
 *    node-set set:trailing (node-set, node-set)
 * @nodes1 and @nodes2 are sorted by document order, then
 * #xmlXPathTrailingSorted is called.
 *
 * Returns the nodes in @nodes1 that follow the first node in @nodes2
 *         in document order, @nodes1 if @nodes2 is NULL or empty or
 *         an empty node-set if @nodes1 doesn't contain @nodes2
 */
xmlNodeSetPtr
xmlXPathTrailing (xmlNodeSetPtr nodes1, xmlNodeSetPtr nodes2) {
    if (xmlXPathNodeSetIsEmpty(nodes2))
	return(nodes1);
    if (xmlXPathNodeSetIsEmpty(nodes1))
	return(xmlXPathNodeSetCreate(NULL));
    xmlXPathNodeSetSort(nodes1);
    xmlXPathNodeSetSort(nodes2);
    return(xmlXPathNodeTrailingSorted(nodes1,
				      xmlXPathNodeSetItem(nodes2, 0)));
}

/************************************************************************
 *									*
 *		Routines to handle extra functions			*
 *									*
 ************************************************************************/

/**
 * xmlXPathRegisterFunc:
 * @ctxt:  the XPath context
 * @name:  the function name
 * @f:  the function implementation or NULL
 *
 * Register a new function. If @f is NULL it unregisters the function
 *
 * Returns 0 in case of success, -1 in case of error
 */
int		  
xmlXPathRegisterFunc(xmlXPathContextPtr ctxt, const xmlChar *name,
		     xmlXPathFunction f) {
    return(xmlXPathRegisterFuncNS(ctxt, name, NULL, f));
}

/**
 * xmlXPathRegisterFuncNS:
 * @ctxt:  the XPath context
 * @name:  the function name
 * @ns_uri:  the function namespace URI
 * @f:  the function implementation or NULL
 *
 * Register a new function. If @f is NULL it unregisters the function
 *
 * Returns 0 in case of success, -1 in case of error
 */
int
xmlXPathRegisterFuncNS(xmlXPathContextPtr ctxt, const xmlChar *name,
		       const xmlChar *ns_uri, xmlXPathFunction f) {
    if (ctxt == NULL)
	return(-1);
    if (name == NULL)
	return(-1);

    if (ctxt->funcHash == NULL)
	ctxt->funcHash = xmlHashCreate(0);
    if (ctxt->funcHash == NULL)
	return(-1);
    return(xmlHashAddEntry2(ctxt->funcHash, name, ns_uri, (void *) f));
}

/**
 * xmlXPathRegisterFuncLookup:
 * @ctxt:  the XPath context
 * @f:  the lookup function
 * @funcCtxt:  the lookup data
 *
 * Registers an external mechanism to do function lookup.
 */
void
xmlXPathRegisterFuncLookup (xmlXPathContextPtr ctxt,
			    xmlXPathFuncLookupFunc f,
			    void *funcCtxt) {
    if (ctxt == NULL)
	return;
    ctxt->funcLookupFunc = (void *) f;
    ctxt->funcLookupData = funcCtxt;
}

/**
 * xmlXPathFunctionLookup:
 * @ctxt:  the XPath context
 * @name:  the function name
 *
 * Search in the Function array of the context for the given
 * function.
 *
 * Returns the xmlXPathFunction or NULL if not found
 */
xmlXPathFunction
xmlXPathFunctionLookup(xmlXPathContextPtr ctxt, const xmlChar *name) {
    if (ctxt == NULL)
	return (NULL);

    if (ctxt->funcLookupFunc != NULL) {
	xmlXPathFunction ret;
	xmlXPathFuncLookupFunc f;

	f = (xmlXPathFuncLookupFunc) ctxt->funcLookupFunc;
	ret = f(ctxt->funcLookupData, name, NULL);
	if (ret != NULL)
	    return(ret);
    }
    return(xmlXPathFunctionLookupNS(ctxt, name, NULL));
}

/**
 * xmlXPathFunctionLookupNS:
 * @ctxt:  the XPath context
 * @name:  the function name
 * @ns_uri:  the function namespace URI
 *
 * Search in the Function array of the context for the given
 * function.
 *
 * Returns the xmlXPathFunction or NULL if not found
 */
xmlXPathFunction
xmlXPathFunctionLookupNS(xmlXPathContextPtr ctxt, const xmlChar *name,
			 const xmlChar *ns_uri) {
    if (ctxt == NULL)
	return(NULL);
    if (name == NULL)
	return(NULL);

    if (ctxt->funcLookupFunc != NULL) {
	xmlXPathFunction ret;
	xmlXPathFuncLookupFunc f;

	f = (xmlXPathFuncLookupFunc) ctxt->funcLookupFunc;
	ret = f(ctxt->funcLookupData, name, ns_uri);
	if (ret != NULL)
	    return(ret);
    }

    if (ctxt->funcHash == NULL)
	return(NULL);

    return((xmlXPathFunction) xmlHashLookup2(ctxt->funcHash, name, ns_uri));
}

/**
 * xmlXPathRegisteredFuncsCleanup:
 * @ctxt:  the XPath context
 *
 * Cleanup the XPath context data associated to registered functions
 */
void
xmlXPathRegisteredFuncsCleanup(xmlXPathContextPtr ctxt) {
    if (ctxt == NULL)
	return;

    xmlHashFree(ctxt->funcHash, NULL);
    ctxt->funcHash = NULL;
}

/************************************************************************
 *									*
 *			Routines to handle Variable			*
 *									*
 ************************************************************************/

/**
 * xmlXPathRegisterVariable:
 * @ctxt:  the XPath context
 * @name:  the variable name
 * @value:  the variable value or NULL
 *
 * Register a new variable value. If @value is NULL it unregisters
 * the variable
 *
 * Returns 0 in case of success, -1 in case of error
 */
int		  
xmlXPathRegisterVariable(xmlXPathContextPtr ctxt, const xmlChar *name,
			 xmlXPathObjectPtr value) {
    return(xmlXPathRegisterVariableNS(ctxt, name, NULL, value));
}

/**
 * xmlXPathRegisterVariableNS:
 * @ctxt:  the XPath context
 * @name:  the variable name
 * @ns_uri:  the variable namespace URI
 * @value:  the variable value or NULL
 *
 * Register a new variable value. If @value is NULL it unregisters
 * the variable
 *
 * Returns 0 in case of success, -1 in case of error
 */
int
xmlXPathRegisterVariableNS(xmlXPathContextPtr ctxt, const xmlChar *name,
			   const xmlChar *ns_uri,
			   xmlXPathObjectPtr value) {
    if (ctxt == NULL)
	return(-1);
    if (name == NULL)
	return(-1);

    if (ctxt->varHash == NULL)
	ctxt->varHash = xmlHashCreate(0);
    if (ctxt->varHash == NULL)
	return(-1);
    return(xmlHashUpdateEntry2(ctxt->varHash, name, ns_uri,
			       (void *) value,
			       (xmlHashDeallocator)xmlXPathFreeObject));
}

/**
 * xmlXPathRegisterVariableLookup:
 * @ctxt:  the XPath context
 * @f:  the lookup function
 * @data:  the lookup data
 *
 * register an external mechanism to do variable lookup
 */
void
xmlXPathRegisterVariableLookup(xmlXPathContextPtr ctxt,
	 xmlXPathVariableLookupFunc f, void *data) {
    if (ctxt == NULL)
	return;
    ctxt->varLookupFunc = (void *) f;
    ctxt->varLookupData = data;
}

/**
 * xmlXPathVariableLookup:
 * @ctxt:  the XPath context
 * @name:  the variable name
 *
 * Search in the Variable array of the context for the given
 * variable value.
 *
 * Returns a copy of the value or NULL if not found
 */
xmlXPathObjectPtr
xmlXPathVariableLookup(xmlXPathContextPtr ctxt, const xmlChar *name) {
    if (ctxt == NULL)
	return(NULL);

    if (ctxt->varLookupFunc != NULL) {
	xmlXPathObjectPtr ret;

	ret = ((xmlXPathVariableLookupFunc)ctxt->varLookupFunc)
	        (ctxt->varLookupData, name, NULL);
	return(ret);
    }
    return(xmlXPathVariableLookupNS(ctxt, name, NULL));
}

/**
 * xmlXPathVariableLookupNS:
 * @ctxt:  the XPath context
 * @name:  the variable name
 * @ns_uri:  the variable namespace URI
 *
 * Search in the Variable array of the context for the given
 * variable value. 
 *
 * Returns the a copy of the value or NULL if not found
 */
xmlXPathObjectPtr
xmlXPathVariableLookupNS(xmlXPathContextPtr ctxt, const xmlChar *name,
			 const xmlChar *ns_uri) {
    if (ctxt == NULL)
	return(NULL);

    if (ctxt->varLookupFunc != NULL) {
	xmlXPathObjectPtr ret;

	ret = ((xmlXPathVariableLookupFunc)ctxt->varLookupFunc)
	        (ctxt->varLookupData, name, ns_uri);
	if (ret != NULL) return(ret);
    }

    if (ctxt->varHash == NULL)
	return(NULL);
    if (name == NULL)
	return(NULL);

    return(xmlXPathObjectCopy((xmlXPathObjectPtr)
		xmlHashLookup2(ctxt->varHash, name, ns_uri)));
}

/**
 * xmlXPathRegisteredVariablesCleanup:
 * @ctxt:  the XPath context
 *
 * Cleanup the XPath context data associated to registered variables
 */
void
xmlXPathRegisteredVariablesCleanup(xmlXPathContextPtr ctxt) {
    if (ctxt == NULL)
	return;

    xmlHashFree(ctxt->varHash, (xmlHashDeallocator)xmlXPathFreeObject);
    ctxt->varHash = NULL;
}

/**
 * xmlXPathRegisterNs:
 * @ctxt:  the XPath context
 * @prefix:  the namespace prefix
 * @ns_uri:  the namespace name
 *
 * Register a new namespace. If @ns_uri is NULL it unregisters
 * the namespace
 *
 * Returns 0 in case of success, -1 in case of error
 */
int
xmlXPathRegisterNs(xmlXPathContextPtr ctxt, const xmlChar *prefix,
			   const xmlChar *ns_uri) {
    if (ctxt == NULL)
	return(-1);
    if (prefix == NULL)
	return(-1);

    if (ctxt->nsHash == NULL)
	ctxt->nsHash = xmlHashCreate(10);
    if (ctxt->nsHash == NULL)
	return(-1);
    return(xmlHashUpdateEntry(ctxt->nsHash, prefix, (void *) xmlStrdup(ns_uri),
			      (xmlHashDeallocator)xmlFree));
}

/**
 * xmlXPathNsLookup:
 * @ctxt:  the XPath context
 * @prefix:  the namespace prefix value
 *
 * Search in the namespace declaration array of the context for the given
 * namespace name associated to the given prefix
 *
 * Returns the value or NULL if not found
 */
const xmlChar *
xmlXPathNsLookup(xmlXPathContextPtr ctxt, const xmlChar *prefix) {
    if (ctxt == NULL)
	return(NULL);
    if (prefix == NULL)
	return(NULL);

#ifdef XML_XML_NAMESPACE
    if (xmlStrEqual(prefix, (const xmlChar *) "xml"))
	return(XML_XML_NAMESPACE);
#endif

    if (ctxt->namespaces != NULL) {
	int i;

	for (i = 0;i < ctxt->nsNr;i++) {
	    if ((ctxt->namespaces[i] != NULL) &&
		(xmlStrEqual(ctxt->namespaces[i]->prefix, prefix)))
		return(ctxt->namespaces[i]->href);
	}
    }

    return((const xmlChar *) xmlHashLookup(ctxt->nsHash, prefix));
}

/**
 * xmlXPathRegisteredNsCleanup:
 * @ctxt:  the XPath context
 *
 * Cleanup the XPath context data associated to registered variables
 */
void
xmlXPathRegisteredNsCleanup(xmlXPathContextPtr ctxt) {
    if (ctxt == NULL)
	return;

    xmlHashFree(ctxt->nsHash, (xmlHashDeallocator)xmlFree);
    ctxt->nsHash = NULL;
}

/************************************************************************
 *									*
 *			Routines to handle Values			*
 *									*
 ************************************************************************/

/* Allocations are terrible, one need to optimize all this !!! */

/**
 * xmlXPathNewFloat:
 * @val:  the double value
 *
 * Create a new xmlXPathObjectPtr of type double and of value @val
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPathNewFloat(double val) {
    xmlXPathObjectPtr ret;

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlXPathNewFloat: out of memory\n");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_NUMBER;
    ret->floatval = val;
    return(ret);
}

/**
 * xmlXPathNewBoolean:
 * @val:  the boolean value
 *
 * Create a new xmlXPathObjectPtr of type boolean and of value @val
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPathNewBoolean(int val) {
    xmlXPathObjectPtr ret;

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlXPathNewBoolean: out of memory\n");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_BOOLEAN;
    ret->boolval = (val != 0);
    return(ret);
}

/**
 * xmlXPathNewString:
 * @val:  the xmlChar * value
 *
 * Create a new xmlXPathObjectPtr of type string and of value @val
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPathNewString(const xmlChar *val) {
    xmlXPathObjectPtr ret;

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlXPathNewString: out of memory\n");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_STRING;
    if (val != NULL)
	ret->stringval = xmlStrdup(val);
    else
	ret->stringval = xmlStrdup((const xmlChar *)"");
    return(ret);
}

/**
 * xmlXPathWrapString:
 * @val:  the xmlChar * value
 *
 * Wraps the @val string into an XPath object.
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPathWrapString (xmlChar *val) {
    xmlXPathObjectPtr ret;

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlXPathWrapString: out of memory\n");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_STRING;
    ret->stringval = val;
    return(ret);
}

/**
 * xmlXPathNewCString:
 * @val:  the char * value
 *
 * Create a new xmlXPathObjectPtr of type string and of value @val
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPathNewCString(const char *val) {
    xmlXPathObjectPtr ret;

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlXPathNewCString: out of memory\n");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_STRING;
    ret->stringval = xmlStrdup(BAD_CAST val);
    return(ret);
}

/**
 * xmlXPathWrapCString:
 * @val:  the char * value
 *
 * Wraps a string into an XPath object.
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPathWrapCString (char * val) {
    return(xmlXPathWrapString((xmlChar *)(val)));
}

/**
 * xmlXPathWrapExternal:
 * @val:  the user data
 *
 * Wraps the @val data into an XPath object.
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPathWrapExternal (void *val) {
    xmlXPathObjectPtr ret;

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlXPathWrapExternal: out of memory\n");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_USERS;
    ret->user = val;
    return(ret);
}

/**
 * xmlXPathObjectCopy:
 * @val:  the original object
 *
 * allocate a new copy of a given object
 *
 * Returns the newly created object.
 */
xmlXPathObjectPtr
xmlXPathObjectCopy(xmlXPathObjectPtr val) {
    xmlXPathObjectPtr ret;

    if (val == NULL)
	return(NULL);

    ret = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlXPathObjectCopy: out of memory\n");
	return(NULL);
    }
    memcpy(ret, val , (size_t) sizeof(xmlXPathObject));
    switch (val->type) {
	case XPATH_BOOLEAN:
	case XPATH_NUMBER:
	case XPATH_POINT:
	case XPATH_RANGE:
	    break;
	case XPATH_STRING:
	    ret->stringval = xmlStrdup(val->stringval);
	    break;
	case XPATH_XSLT_TREE:
	    if ((val->nodesetval != NULL) &&
		(val->nodesetval->nodeTab != NULL)) {
		ret->boolval = 1;
		ret->user = xmlDocCopyNode(val->nodesetval->nodeTab[0],
				       val->nodesetval->nodeTab[0]->doc, 1);
		ret->nodesetval = xmlXPathNodeSetCreate(
					  (xmlNodePtr) ret->user);
	    } else
		ret->nodesetval = xmlXPathNodeSetCreate(NULL);
	    /* Deallocate the copied tree value */
	    break;
	case XPATH_NODESET:
	    ret->nodesetval = xmlXPathNodeSetMerge(NULL, val->nodesetval);
	    /* Do not deallocate the copied tree value */
	    ret->boolval = 0;
	    break;
	case XPATH_LOCATIONSET:
#ifdef LIBXML_XPTR_ENABLED
	{
	    xmlLocationSetPtr loc = val->user;
	    ret->user = (void *) xmlXPtrLocationSetMerge(NULL, loc);
	    break;
	}
#endif
        case XPATH_USERS:
	    ret->user = val->user;
	    break; 
        case XPATH_UNDEFINED:
	    xmlGenericError(xmlGenericErrorContext,
		    "xmlXPathObjectCopy: unsupported type %d\n",
		    val->type);
	    break;
    }
    return(ret);
}

/**
 * xmlXPathFreeObject:
 * @obj:  the object to free
 *
 * Free up an xmlXPathObjectPtr object.
 */
void
xmlXPathFreeObject(xmlXPathObjectPtr obj) {
    if (obj == NULL) return;
    if ((obj->type == XPATH_NODESET) || (obj->type == XPATH_XSLT_TREE)) {
	if (obj->boolval) {
	    if (obj->user != NULL) {
                xmlXPathFreeNodeSet(obj->nodesetval);
		xmlFreeNodeList((xmlNodePtr) obj->user);
	    } else if (obj->nodesetval != NULL)
		xmlXPathFreeValueTree(obj->nodesetval);
	} else {
	    if (obj->nodesetval != NULL)
		xmlXPathFreeNodeSet(obj->nodesetval);
	}
#ifdef LIBXML_XPTR_ENABLED
    } else if (obj->type == XPATH_LOCATIONSET) {
	if (obj->user != NULL)
	    xmlXPtrFreeLocationSet(obj->user);
#endif
    } else if (obj->type == XPATH_STRING) {
	if (obj->stringval != NULL)
	    xmlFree(obj->stringval);
    }

    xmlFree(obj);
}


/************************************************************************
 *									*
 *			Type Casting Routines				*
 *									*
 ************************************************************************/

/**
 * xmlXPathCastBooleanToString:
 * @val:  a boolean
 *
 * Converts a boolean to its string value.
 *
 * Returns a newly allocated string.
 */
xmlChar *
xmlXPathCastBooleanToString (int val) {
    xmlChar *ret;
    if (val)
	ret = xmlStrdup((const xmlChar *) "true");
    else
	ret = xmlStrdup((const xmlChar *) "false");
    return(ret);
}

/**
 * xmlXPathCastNumberToString:
 * @val:  a number
 *
 * Converts a number to its string value.
 *
 * Returns a newly allocated string.
 */
xmlChar *
xmlXPathCastNumberToString (double val) {
    xmlChar *ret;
    switch (xmlXPathIsInf(val)) {
    case 1:
	ret = xmlStrdup((const xmlChar *) "Infinity");
	break;
    case -1:
	ret = xmlStrdup((const xmlChar *) "-Infinity");
	break;
    default:
	if (xmlXPathIsNaN(val)) {
	    ret = xmlStrdup((const xmlChar *) "NaN");
	} else if (val == 0 && xmlXPathGetSign(val) != 0) {
	    ret = xmlStrdup((const xmlChar *) "0");
	} else {
	    /* could be improved */
	    char buf[100];
	    xmlXPathFormatNumber(val, buf, 100);
	    ret = xmlStrdup((const xmlChar *) buf);
	}
    }
    return(ret);
}

/**
 * xmlXPathCastNodeToString:
 * @node:  a node
 *
 * Converts a node to its string value.
 *
 * Returns a newly allocated string.
 */
xmlChar *
xmlXPathCastNodeToString (xmlNodePtr node) {
    if ((node != NULL) && (node->type == XML_DOCUMENT_NODE))
	node = xmlDocGetRootElement((xmlDocPtr) node);
    return(xmlNodeGetContent(node));
}

/**
 * xmlXPathCastNodeSetToString:
 * @ns:  a node-set
 *
 * Converts a node-set to its string value.
 *
 * Returns a newly allocated string.
 */
xmlChar *
xmlXPathCastNodeSetToString (xmlNodeSetPtr ns) {
    if ((ns == NULL) || (ns->nodeNr == 0) || (ns->nodeTab == NULL))
	return(xmlStrdup((const xmlChar *) ""));

    xmlXPathNodeSetSort(ns);
    return(xmlXPathCastNodeToString(ns->nodeTab[0]));
}

/**
 * xmlXPathCastToString:
 * @val:  an XPath object
 *
 * Converts an existing object to its string() equivalent
 *
 * Returns the string value of the object, NULL in case of error.
 *         A new string is allocated only if needed (@val isn't a
 *         string object).
 */
xmlChar *
xmlXPathCastToString(xmlXPathObjectPtr val) {
    xmlChar *ret = NULL;

    if (val == NULL)
	return(xmlStrdup((const xmlChar *) ""));
    switch (val->type) {
	case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
	    xmlGenericError(xmlGenericErrorContext, "String: undefined\n");
#endif
	    ret = xmlStrdup((const xmlChar *) "");
	    break;
        case XPATH_NODESET:
        case XPATH_XSLT_TREE:
	    ret = xmlXPathCastNodeSetToString(val->nodesetval);
	    break;
	case XPATH_STRING:
	    return(xmlStrdup(val->stringval));
        case XPATH_BOOLEAN:
	    ret = xmlXPathCastBooleanToString(val->boolval);
	    break;
	case XPATH_NUMBER: {
	    ret = xmlXPathCastNumberToString(val->floatval);
	    break;
	}
	case XPATH_USERS:
	case XPATH_POINT:
	case XPATH_RANGE:
	case XPATH_LOCATIONSET:
	    TODO
	    ret = xmlStrdup((const xmlChar *) "");
	    break;
    }
    return(ret);
}

/**
 * xmlXPathConvertString:
 * @val:  an XPath object
 *
 * Converts an existing object to its string() equivalent
 *
 * Returns the new object, the old one is freed (or the operation
 *         is done directly on @val)
 */
xmlXPathObjectPtr
xmlXPathConvertString(xmlXPathObjectPtr val) {
    xmlChar *res = NULL;

    if (val == NULL)
	return(xmlXPathNewCString(""));

    switch (val->type) {
    case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
	xmlGenericError(xmlGenericErrorContext, "STRING: undefined\n");
#endif
	break;
    case XPATH_NODESET:
    case XPATH_XSLT_TREE:
	res = xmlXPathCastNodeSetToString(val->nodesetval);
	break;
    case XPATH_STRING:
	return(val);
    case XPATH_BOOLEAN:
	res = xmlXPathCastBooleanToString(val->boolval);
	break;
    case XPATH_NUMBER:
	res = xmlXPathCastNumberToString(val->floatval);
	break;
    case XPATH_USERS:
    case XPATH_POINT:
    case XPATH_RANGE:
    case XPATH_LOCATIONSET:
	TODO;
	break;
    }
    xmlXPathFreeObject(val);
    if (res == NULL)
	return(xmlXPathNewCString(""));
    return(xmlXPathWrapString(res));
}

/**
 * xmlXPathCastBooleanToNumber:
 * @val:  a boolean
 *
 * Converts a boolean to its number value
 *
 * Returns the number value
 */
double
xmlXPathCastBooleanToNumber(int val) {
    if (val)
	return(1.0);
    return(0.0);
}

/**
 * xmlXPathCastStringToNumber:
 * @val:  a string
 *
 * Converts a string to its number value
 *
 * Returns the number value
 */
double
xmlXPathCastStringToNumber(const xmlChar * val) {
    return(xmlXPathStringEvalNumber(val));
}

/**
 * xmlXPathCastNodeToNumber:
 * @node:  a node
 *
 * Converts a node to its number value
 *
 * Returns the number value
 */
double
xmlXPathCastNodeToNumber (xmlNodePtr node) {
    xmlChar *strval;
    double ret;

    if (node == NULL)
	return(xmlXPathNAN);
    strval = xmlXPathCastNodeToString(node);
    if (strval == NULL)
	return(xmlXPathNAN);
    ret = xmlXPathCastStringToNumber(strval);
    xmlFree(strval);

    return(ret);
}

/**
 * xmlXPathCastNodeSetToNumber:
 * @ns:  a node-set
 *
 * Converts a node-set to its number value
 *
 * Returns the number value
 */
double
xmlXPathCastNodeSetToNumber (xmlNodeSetPtr ns) {
    xmlChar *str;
    double ret;

    if (ns == NULL)
	return(xmlXPathNAN);
    str = xmlXPathCastNodeSetToString(ns);
    ret = xmlXPathCastStringToNumber(str);
    xmlFree(str);
    return(ret);
}

/**
 * xmlXPathCastToNumber:
 * @val:  an XPath object
 *
 * Converts an XPath object to its number value
 *
 * Returns the number value
 */
double
xmlXPathCastToNumber(xmlXPathObjectPtr val) {
    double ret = 0.0;

    if (val == NULL)
	return(xmlXPathNAN);
    switch (val->type) {
    case XPATH_UNDEFINED:
#ifdef DEGUB_EXPR
	xmlGenericError(xmlGenericErrorContext, "NUMBER: undefined\n");
#endif
	ret = xmlXPathNAN;
	break;
    case XPATH_NODESET:
    case XPATH_XSLT_TREE:
	ret = xmlXPathCastNodeSetToNumber(val->nodesetval);
	break;
    case XPATH_STRING:
	ret = xmlXPathCastStringToNumber(val->stringval);
	break;
    case XPATH_NUMBER:
	ret = val->floatval;
	break;
    case XPATH_BOOLEAN:
	ret = xmlXPathCastBooleanToNumber(val->boolval);
	break;
    case XPATH_USERS:
    case XPATH_POINT:
    case XPATH_RANGE:
    case XPATH_LOCATIONSET:
	TODO;
	ret = xmlXPathNAN;
	break;
    }
    return(ret);
}

/**
 * xmlXPathConvertNumber:
 * @val:  an XPath object
 *
 * Converts an existing object to its number() equivalent
 *
 * Returns the new object, the old one is freed (or the operation
 *         is done directly on @val)
 */
xmlXPathObjectPtr
xmlXPathConvertNumber(xmlXPathObjectPtr val) {
    xmlXPathObjectPtr ret;

    if (val == NULL)
	return(xmlXPathNewFloat(0.0));
    if (val->type == XPATH_NUMBER)
	return(val);
    ret = xmlXPathNewFloat(xmlXPathCastToNumber(val));
    xmlXPathFreeObject(val);
    return(ret);
}

/**
 * xmlXPathCastNumberToBoolean:
 * @val:  a number
 *
 * Converts a number to its boolean value
 *
 * Returns the boolean value
 */
int
xmlXPathCastNumberToBoolean (double val) {
     if (xmlXPathIsNaN(val) || (val == 0.0))
	 return(0);
     return(1);
}

/**
 * xmlXPathCastStringToBoolean:
 * @val:  a string
 *
 * Converts a string to its boolean value
 *
 * Returns the boolean value
 */
int
xmlXPathCastStringToBoolean (const xmlChar *val) {
    if ((val == NULL) || (xmlStrlen(val) == 0))
	return(0);
    return(1);
}

/**
 * xmlXPathCastNodeSetToBoolean:
 * @ns:  a node-set
 *
 * Converts a node-set to its boolean value
 *
 * Returns the boolean value
 */
int
xmlXPathCastNodeSetToBoolean (xmlNodeSetPtr ns) {
    if ((ns == NULL) || (ns->nodeNr == 0))
	return(0);
    return(1);
}

/**
 * xmlXPathCastToBoolean:
 * @val:  an XPath object
 *
 * Converts an XPath object to its boolean value
 *
 * Returns the boolean value
 */
int
xmlXPathCastToBoolean (xmlXPathObjectPtr val) {
    int ret = 0;

    if (val == NULL)
	return(0);
    switch (val->type) {
    case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
	xmlGenericError(xmlGenericErrorContext, "BOOLEAN: undefined\n");
#endif
	ret = 0;
	break;
    case XPATH_NODESET:
    case XPATH_XSLT_TREE:
	ret = xmlXPathCastNodeSetToBoolean(val->nodesetval);
	break;
    case XPATH_STRING:
	ret = xmlXPathCastStringToBoolean(val->stringval);
	break;
    case XPATH_NUMBER:
	ret = xmlXPathCastNumberToBoolean(val->floatval);
	break;
    case XPATH_BOOLEAN:
	ret = val->boolval;
	break;
    case XPATH_USERS:
    case XPATH_POINT:
    case XPATH_RANGE:
    case XPATH_LOCATIONSET:
	TODO;
	ret = 0;
	break;
    }
    return(ret);
}


/**
 * xmlXPathConvertBoolean:
 * @val:  an XPath object
 *
 * Converts an existing object to its boolean() equivalent
 *
 * Returns the new object, the old one is freed (or the operation
 *         is done directly on @val)
 */
xmlXPathObjectPtr
xmlXPathConvertBoolean(xmlXPathObjectPtr val) {
    xmlXPathObjectPtr ret;

    if (val == NULL)
	return(xmlXPathNewBoolean(0));
    if (val->type == XPATH_BOOLEAN)
	return(val);
    ret = xmlXPathNewBoolean(xmlXPathCastToBoolean(val));
    xmlXPathFreeObject(val);
    return(ret);
}

/************************************************************************
 *									*
 *		Routines to handle XPath contexts			*
 *									*
 ************************************************************************/

/**
 * xmlXPathNewContext:
 * @doc:  the XML document
 *
 * Create a new xmlXPathContext
 *
 * Returns the xmlXPathContext just allocated. The caller will need to free it.
 */
xmlXPathContextPtr
xmlXPathNewContext(xmlDocPtr doc) {
    xmlXPathContextPtr ret;

    ret = (xmlXPathContextPtr) xmlMalloc(sizeof(xmlXPathContext));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlXPathNewContext: out of memory\n");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathContext));
    ret->doc = doc;
    ret->node = NULL;

    ret->varHash = NULL;

    ret->nb_types = 0;
    ret->max_types = 0;
    ret->types = NULL;

    ret->funcHash = xmlHashCreate(0);

    ret->nb_axis = 0;
    ret->max_axis = 0;
    ret->axis = NULL;

    ret->nsHash = NULL;
    ret->user = NULL;

    ret->contextSize = -1;
    ret->proximityPosition = -1;

    xmlXPathRegisterAllFunctions(ret);
    
    return(ret);
}

/**
 * xmlXPathFreeContext:
 * @ctxt:  the context to free
 *
 * Free up an xmlXPathContext
 */
void
xmlXPathFreeContext(xmlXPathContextPtr ctxt) {
    xmlXPathRegisteredNsCleanup(ctxt);
    xmlXPathRegisteredFuncsCleanup(ctxt);
    xmlXPathRegisteredVariablesCleanup(ctxt);
    xmlFree(ctxt);
}

/************************************************************************
 *									*
 *		Routines to handle XPath parser contexts		*
 *									*
 ************************************************************************/

#define CHECK_CTXT(ctxt)						\
    if (ctxt == NULL) { 						\
        xmlGenericError(xmlGenericErrorContext,				\
		"%s:%d Internal error: ctxt == NULL\n",			\
	        __FILE__, __LINE__);					\
    }									\


#define CHECK_CONTEXT(ctxt)						\
    if (ctxt == NULL) { 						\
        xmlGenericError(xmlGenericErrorContext,				\
		"%s:%d Internal error: no context\n",			\
	        __FILE__, __LINE__);					\
    }									\
    else if (ctxt->doc == NULL) { 					\
        xmlGenericError(xmlGenericErrorContext,				\
		"%s:%d Internal error: no document\n",			\
	        __FILE__, __LINE__);					\
    }									\
    else if (ctxt->doc->children == NULL) { 				\
        xmlGenericError(xmlGenericErrorContext,				\
	        "%s:%d Internal error: document without root\n",	\
	        __FILE__, __LINE__);					\
    }									\


/**
 * xmlXPathNewParserContext:
 * @str:  the XPath expression
 * @ctxt:  the XPath context
 *
 * Create a new xmlXPathParserContext
 *
 * Returns the xmlXPathParserContext just allocated.
 */
xmlXPathParserContextPtr
xmlXPathNewParserContext(const xmlChar *str, xmlXPathContextPtr ctxt) {
    xmlXPathParserContextPtr ret;

    ret = (xmlXPathParserContextPtr) xmlMalloc(sizeof(xmlXPathParserContext));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlXPathNewParserContext: out of memory\n");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathParserContext));
    ret->cur = ret->base = str;
    ret->context = ctxt;

    ret->comp = xmlXPathNewCompExpr();
    if (ret->comp == NULL) {
	xmlFree(ret->valueTab);
	xmlFree(ret);
	return(NULL);
    }

    return(ret);
}

/**
 * xmlXPathCompParserContext:
 * @comp:  the XPath compiled expression
 * @ctxt:  the XPath context
 *
 * Create a new xmlXPathParserContext when processing a compiled expression
 *
 * Returns the xmlXPathParserContext just allocated.
 */
static xmlXPathParserContextPtr
xmlXPathCompParserContext(xmlXPathCompExprPtr comp, xmlXPathContextPtr ctxt) {
    xmlXPathParserContextPtr ret;

    ret = (xmlXPathParserContextPtr) xmlMalloc(sizeof(xmlXPathParserContext));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlXPathCompParserContext: out of memory\n");
	return(NULL);
    }
    memset(ret, 0 , (size_t) sizeof(xmlXPathParserContext));

    /* Allocate the value stack */
    ret->valueTab = (xmlXPathObjectPtr *) 
                     xmlMalloc(10 * sizeof(xmlXPathObjectPtr));
    if (ret->valueTab == NULL) {
	xmlFree(ret);
        xmlGenericError(xmlGenericErrorContext,
		"xmlXPathCompParserContext: out of memory\n");
	return(NULL);
    }
    ret->valueNr = 0;
    ret->valueMax = 10;
    ret->value = NULL;

    ret->context = ctxt;
    ret->comp = comp;

    return(ret);
}

/**
 * xmlXPathFreeParserContext:
 * @ctxt:  the context to free
 *
 * Free up an xmlXPathParserContext
 */
void
xmlXPathFreeParserContext(xmlXPathParserContextPtr ctxt) {
    if (ctxt->valueTab != NULL) {
        xmlFree(ctxt->valueTab);
    }
    if (ctxt->comp)
	xmlXPathFreeCompExpr(ctxt->comp);
    xmlFree(ctxt);
}

/************************************************************************
 *									*
 *		The implicit core function library			*
 *									*
 ************************************************************************/

/**
 * xmlXPathNodeValHash:
 * @node:  a node pointer
 *
 * Function computing the beginning of the string value of the node,
 * used to speed up comparisons
 *
 * Returns an int usable as a hash
 */
static unsigned int
xmlXPathNodeValHash(xmlNodePtr node) {
    int len = 2;
    const xmlChar * string = NULL;
    xmlNodePtr tmp = NULL;
    unsigned int ret = 0;

    if (node == NULL)
	return(0);


    switch (node->type) {
	case XML_COMMENT_NODE:
	case XML_PI_NODE:
	case XML_CDATA_SECTION_NODE:
	case XML_TEXT_NODE:
	    string = node->content;
	    if (string == NULL)
		return(0);
	    if (string[0] == 0)
		return(0);
	    return(((unsigned int) string[0]) +
		   (((unsigned int) string[1]) << 8));
	case XML_NAMESPACE_DECL:
	    string = ((xmlNsPtr)node)->href;
	    if (string == NULL)
		return(0);
	    if (string[0] == 0)
		return(0);
	    return(((unsigned int) string[0]) +
		   (((unsigned int) string[1]) << 8));
	case XML_ATTRIBUTE_NODE:
	    tmp = ((xmlAttrPtr) node)->children;
	    break;
	case XML_ELEMENT_NODE:
	    tmp = node->children;
	    break;
	default:
	    return(0);
    }
    while (tmp != NULL) {
	switch (tmp->type) {
	    case XML_COMMENT_NODE:
	    case XML_PI_NODE:
	    case XML_CDATA_SECTION_NODE:
	    case XML_TEXT_NODE:
		string = tmp->content;
		break;
	    case XML_NAMESPACE_DECL:
		string = ((xmlNsPtr)tmp)->href;
		break;
	    default:
		break;
	}
	if ((string != NULL) && (string[0] != 0)) {
	    if (string[0] == 0)
		return(0);
	    if (len == 1) {
		return(ret + (((unsigned int) string[0]) << 8));
	    }
	    if (string[1] == 0) {
		len = 1;
		ret = (unsigned int) string[0];
	    } else {
		return(((unsigned int) string[0]) +
		       (((unsigned int) string[1]) << 8));
	    }
	}
	/*
	 * Skip to next node
	 */
	if ((tmp->children != NULL) && (tmp->type != XML_DTD_NODE)) {
	    if (tmp->children->type != XML_ENTITY_DECL) {
		tmp = tmp->children;
		continue;
	    }
	}
	if (tmp == node)
	    break;

	if (tmp->next != NULL) {
	    tmp = tmp->next;
	    continue;
	}
	
	do {
	    tmp = tmp->parent;
	    if (tmp == NULL)
		break;
	    if (tmp == node) {
		tmp = NULL;
		break;
	    }
	    if (tmp->next != NULL) {
		tmp = tmp->next;
		break;
	    }
	} while (tmp != NULL);
    }
    return(ret);
}

/**
 * xmlXPathStringHash:
 * @string:  a string
 *
 * Function computing the beginning of the string value of the node,
 * used to speed up comparisons
 *
 * Returns an int usable as a hash
 */
static unsigned int
xmlXPathStringHash(const xmlChar * string) {
    if (string == NULL)
	return((unsigned int) 0);
    if (string[0] == 0)
	return(0);
    return(((unsigned int) string[0]) +
	   (((unsigned int) string[1]) << 8));
}

/**
 * xmlXPathCompareNodeSetFloat:
 * @ctxt:  the XPath Parser context
 * @inf:  less than (1) or greater than (0)
 * @strict:  is the comparison strict
 * @arg:  the node set
 * @f:  the value
 *
 * Implement the compare operation between a nodeset and a number
 *     @ns < @val    (1, 1, ...
 *     @ns <= @val   (1, 0, ...
 *     @ns > @val    (0, 1, ...
 *     @ns >= @val   (0, 0, ...
 *
 * If one object to be compared is a node-set and the other is a number,
 * then the comparison will be true if and only if there is a node in the
 * node-set such that the result of performing the comparison on the number
 * to be compared and on the result of converting the string-value of that
 * node to a number using the number function is true.
 *
 * Returns 0 or 1 depending on the results of the test.
 */
static int
xmlXPathCompareNodeSetFloat(xmlXPathParserContextPtr ctxt, int inf, int strict,
	                    xmlXPathObjectPtr arg, xmlXPathObjectPtr f) {
    int i, ret = 0;
    xmlNodeSetPtr ns;
    xmlChar *str2;

    if ((f == NULL) || (arg == NULL) ||
	((arg->type != XPATH_NODESET) && (arg->type != XPATH_XSLT_TREE))) {
	xmlXPathFreeObject(arg);
	xmlXPathFreeObject(f);
        return(0);
    }
    ns = arg->nodesetval;
    if (ns != NULL) {
	for (i = 0;i < ns->nodeNr;i++) {
	     str2 = xmlXPathCastNodeToString(ns->nodeTab[i]);
	     if (str2 != NULL) {
		 valuePush(ctxt,
			   xmlXPathNewString(str2));
		 xmlFree(str2);
		 xmlXPathNumberFunction(ctxt, 1);
		 valuePush(ctxt, xmlXPathObjectCopy(f));
		 ret = xmlXPathCompareValues(ctxt, inf, strict);
		 if (ret)
		     break;
	     }
	}
    }
    xmlXPathFreeObject(arg);
    xmlXPathFreeObject(f);
    return(ret);
}

/**
 * xmlXPathCompareNodeSetString:
 * @ctxt:  the XPath Parser context
 * @inf:  less than (1) or greater than (0)
 * @strict:  is the comparison strict
 * @arg:  the node set
 * @s:  the value
 *
 * Implement the compare operation between a nodeset and a string
 *     @ns < @val    (1, 1, ...
 *     @ns <= @val   (1, 0, ...
 *     @ns > @val    (0, 1, ...
 *     @ns >= @val   (0, 0, ...
 *
 * If one object to be compared is a node-set and the other is a string,
 * then the comparison will be true if and only if there is a node in
 * the node-set such that the result of performing the comparison on the
 * string-value of the node and the other string is true.
 *
 * Returns 0 or 1 depending on the results of the test.
 */
static int
xmlXPathCompareNodeSetString(xmlXPathParserContextPtr ctxt, int inf, int strict,
	                    xmlXPathObjectPtr arg, xmlXPathObjectPtr s) {
    int i, ret = 0;
    xmlNodeSetPtr ns;
    xmlChar *str2;

    if ((s == NULL) || (arg == NULL) ||
	((arg->type != XPATH_NODESET) && (arg->type != XPATH_XSLT_TREE))) {
	xmlXPathFreeObject(arg);
	xmlXPathFreeObject(s);
        return(0);
    }
    ns = arg->nodesetval;
    if (ns != NULL) {
	for (i = 0;i < ns->nodeNr;i++) {
	     str2 = xmlXPathCastNodeToString(ns->nodeTab[i]);
	     if (str2 != NULL) {
		 valuePush(ctxt,
			   xmlXPathNewString(str2));
		 xmlFree(str2);
		 valuePush(ctxt, xmlXPathObjectCopy(s));
		 ret = xmlXPathCompareValues(ctxt, inf, strict);
		 if (ret)
		     break;
	     }
	}
    }
    xmlXPathFreeObject(arg);
    xmlXPathFreeObject(s);
    return(ret);
}

/**
 * xmlXPathCompareNodeSets:
 * @inf:  less than (1) or greater than (0)
 * @strict:  is the comparison strict
 * @arg1:  the first node set object
 * @arg2:  the second node set object
 *
 * Implement the compare operation on nodesets:
 *
 * If both objects to be compared are node-sets, then the comparison
 * will be true if and only if there is a node in the first node-set
 * and a node in the second node-set such that the result of performing
 * the comparison on the string-values of the two nodes is true. 
 * ....
 * When neither object to be compared is a node-set and the operator
 * is <=, <, >= or >, then the objects are compared by converting both
 * objects to numbers and comparing the numbers according to IEEE 754.
 * ....
 * The number function converts its argument to a number as follows:
 *  - a string that consists of optional whitespace followed by an
 *    optional minus sign followed by a Number followed by whitespace
 *    is converted to the IEEE 754 number that is nearest (according
 *    to the IEEE 754 round-to-nearest rule) to the mathematical value
 *    represented by the string; any other string is converted to NaN
 *
 * Conclusion all nodes need to be converted first to their string value
 * and then the comparison must be done when possible 
 */
static int
xmlXPathCompareNodeSets(int inf, int strict,
	                xmlXPathObjectPtr arg1, xmlXPathObjectPtr arg2) {
    int i, j, init = 0;
    double val1;
    double *values2;
    int ret = 0;
    xmlNodeSetPtr ns1;
    xmlNodeSetPtr ns2;

    if ((arg1 == NULL) ||
	((arg1->type != XPATH_NODESET) && (arg1->type != XPATH_XSLT_TREE))) {
	xmlXPathFreeObject(arg2);
        return(0);
    }
    if ((arg2 == NULL) ||
	((arg2->type != XPATH_NODESET) && (arg2->type != XPATH_XSLT_TREE))) {
	xmlXPathFreeObject(arg1);
	xmlXPathFreeObject(arg2);
        return(0);
    }

    ns1 = arg1->nodesetval;
    ns2 = arg2->nodesetval;

    if ((ns1 == NULL) || (ns1->nodeNr <= 0)) {
	xmlXPathFreeObject(arg1);
	xmlXPathFreeObject(arg2);
	return(0);
    }
    if ((ns2 == NULL) || (ns2->nodeNr <= 0)) {
	xmlXPathFreeObject(arg1);
	xmlXPathFreeObject(arg2);
	return(0);
    }

    values2 = (double *) xmlMalloc(ns2->nodeNr * sizeof(double));
    if (values2 == NULL) {
	xmlXPathFreeObject(arg1);
	xmlXPathFreeObject(arg2);
	return(0);
    }
    for (i = 0;i < ns1->nodeNr;i++) {
	val1 = xmlXPathCastNodeToNumber(ns1->nodeTab[i]);
	if (xmlXPathIsNaN(val1))
	    continue;
	for (j = 0;j < ns2->nodeNr;j++) {
	    if (init == 0) {
		values2[j] = xmlXPathCastNodeToNumber(ns2->nodeTab[j]);
	    }
	    if (xmlXPathIsNaN(values2[j]))
		continue;
	    if (inf && strict) 
		ret = (val1 < values2[j]);
	    else if (inf && !strict)
		ret = (val1 <= values2[j]);
	    else if (!inf && strict)
		ret = (val1 > values2[j]);
	    else if (!inf && !strict)
		ret = (val1 >= values2[j]);
	    if (ret)
		break;
	}
	if (ret)
	    break;
	init = 1;
    }
    xmlFree(values2);
    xmlXPathFreeObject(arg1);
    xmlXPathFreeObject(arg2);
    return(ret);
}

/**
 * xmlXPathCompareNodeSetValue:
 * @ctxt:  the XPath Parser context
 * @inf:  less than (1) or greater than (0)
 * @strict:  is the comparison strict
 * @arg:  the node set
 * @val:  the value
 *
 * Implement the compare operation between a nodeset and a value
 *     @ns < @val    (1, 1, ...
 *     @ns <= @val   (1, 0, ...
 *     @ns > @val    (0, 1, ...
 *     @ns >= @val   (0, 0, ...
 *
 * If one object to be compared is a node-set and the other is a boolean,
 * then the comparison will be true if and only if the result of performing
 * the comparison on the boolean and on the result of converting
 * the node-set to a boolean using the boolean function is true.
 *
 * Returns 0 or 1 depending on the results of the test.
 */
static int
xmlXPathCompareNodeSetValue(xmlXPathParserContextPtr ctxt, int inf, int strict,
	                    xmlXPathObjectPtr arg, xmlXPathObjectPtr val) {
    if ((val == NULL) || (arg == NULL) ||
	((arg->type != XPATH_NODESET) && (arg->type != XPATH_XSLT_TREE)))
        return(0);

    switch(val->type) {
        case XPATH_NUMBER:
	    return(xmlXPathCompareNodeSetFloat(ctxt, inf, strict, arg, val));
        case XPATH_NODESET:
        case XPATH_XSLT_TREE:
	    return(xmlXPathCompareNodeSets(inf, strict, arg, val));
        case XPATH_STRING:
	    return(xmlXPathCompareNodeSetString(ctxt, inf, strict, arg, val));
        case XPATH_BOOLEAN:
	    valuePush(ctxt, arg);
	    xmlXPathBooleanFunction(ctxt, 1);
	    valuePush(ctxt, val);
	    return(xmlXPathCompareValues(ctxt, inf, strict));
	default:
	    TODO
    }
    return(0);
}

/**
 * xmlXPathEqualNodeSetString:
 * @arg:  the nodeset object argument
 * @str:  the string to compare to.
 * @neq:  flag to show whether for '=' (0) or '!=' (1)
 *
 * Implement the equal operation on XPath objects content: @arg1 == @arg2
 * If one object to be compared is a node-set and the other is a string,
 * then the comparison will be true if and only if there is a node in
 * the node-set such that the result of performing the comparison on the
 * string-value of the node and the other string is true.
 *
 * Returns 0 or 1 depending on the results of the test.
 */
static int
xmlXPathEqualNodeSetString(xmlXPathObjectPtr arg, const xmlChar * str, int neq)
{
    int i;
    xmlNodeSetPtr ns;
    xmlChar *str2;
    unsigned int hash;

    if ((str == NULL) || (arg == NULL) ||
        ((arg->type != XPATH_NODESET) && (arg->type != XPATH_XSLT_TREE)))
        return (0);
    ns = arg->nodesetval;
    hash = xmlXPathStringHash(str);
    if (ns == NULL)
        return (0);
    if (ns->nodeNr <= 0) {
	if (hash == 0)
	    return(neq ^ 1);
        return(neq);
    }
    for (i = 0; i < ns->nodeNr; i++) {
        if (xmlXPathNodeValHash(ns->nodeTab[i]) == hash) {
            str2 = xmlNodeGetContent(ns->nodeTab[i]);
            if ((str2 != NULL) && (xmlStrEqual(str, str2))) {
                xmlFree(str2);
		if (neq)
		    continue;
                return (1);
            } else if (neq) {
		if (str2 != NULL)
		    xmlFree(str2);
		return (1);
	    }
            if (str2 != NULL)
                xmlFree(str2);
        } else if (neq)
	    return (1);
    }
    return (0);
}

/**
 * xmlXPathEqualNodeSetFloat:
 * @arg:  the nodeset object argument
 * @f:  the float to compare to
 * @neq:  flag to show whether to compare '=' (0) or '!=' (1)
 *
 * Implement the equal operation on XPath objects content: @arg1 == @arg2
 * If one object to be compared is a node-set and the other is a number,
 * then the comparison will be true if and only if there is a node in
 * the node-set such that the result of performing the comparison on the
 * number to be compared and on the result of converting the string-value
 * of that node to a number using the number function is true.
 *
 * Returns 0 or 1 depending on the results of the test.
 */
static int
xmlXPathEqualNodeSetFloat(xmlXPathParserContextPtr ctxt,
    xmlXPathObjectPtr arg, double f, int neq) {
  int i, ret=0;
  xmlNodeSetPtr ns;
  xmlChar *str2;
  xmlXPathObjectPtr val;
  double v;

    if ((arg == NULL) ||
	((arg->type != XPATH_NODESET) && (arg->type != XPATH_XSLT_TREE)))
        return(0);

    ns = arg->nodesetval;
    if (ns != NULL) {
	for (i=0;i<ns->nodeNr;i++) {
	    str2 = xmlXPathCastNodeToString(ns->nodeTab[i]);
	    if (str2 != NULL) {
		valuePush(ctxt, xmlXPathNewString(str2));
		xmlFree(str2);
		xmlXPathNumberFunction(ctxt, 1);
		val = valuePop(ctxt);
		v = val->floatval;
		xmlXPathFreeObject(val);
		if (!xmlXPathIsNaN(v)) {
		    if ((!neq) && (v==f)) {
			ret = 1;
			break;
		    } else if ((neq) && (v!=f)) {
			ret = 1;
			break;
		    }
		}
	    }
	}
    }

    return(ret);
}


/**
 * xmlXPathEqualNodeSets:
 * @arg1:  first nodeset object argument
 * @arg2:  second nodeset object argument
 * @neq:   flag to show whether to test '=' (0) or '!=' (1)
 *
 * Implement the equal / not equal operation on XPath nodesets:
 * @arg1 == @arg2  or  @arg1 != @arg2
 * If both objects to be compared are node-sets, then the comparison
 * will be true if and only if there is a node in the first node-set and
 * a node in the second node-set such that the result of performing the
 * comparison on the string-values of the two nodes is true.
 *
 * (needless to say, this is a costly operation)
 *
 * Returns 0 or 1 depending on the results of the test.
 */
static int
xmlXPathEqualNodeSets(xmlXPathObjectPtr arg1, xmlXPathObjectPtr arg2, int neq) {
    int i, j;
    unsigned int *hashs1;
    unsigned int *hashs2;
    xmlChar **values1;
    xmlChar **values2;
    int ret = 0;
    xmlNodeSetPtr ns1;
    xmlNodeSetPtr ns2;

    if ((arg1 == NULL) ||
	((arg1->type != XPATH_NODESET) && (arg1->type != XPATH_XSLT_TREE)))
        return(0);
    if ((arg2 == NULL) ||
	((arg2->type != XPATH_NODESET) && (arg2->type != XPATH_XSLT_TREE)))
        return(0);

    ns1 = arg1->nodesetval;
    ns2 = arg2->nodesetval;

    if ((ns1 == NULL) || (ns1->nodeNr <= 0))
	return(0);
    if ((ns2 == NULL) || (ns2->nodeNr <= 0))
	return(0);

    /*
     * for equal, check if there is a node pertaining to both sets
     */
    if (neq == 0)
	for (i = 0;i < ns1->nodeNr;i++)
	    for (j = 0;j < ns2->nodeNr;j++)
		if (ns1->nodeTab[i] == ns2->nodeTab[j])
		    return(1);

    values1 = (xmlChar **) xmlMalloc(ns1->nodeNr * sizeof(xmlChar *));
    if (values1 == NULL)
	return(0);
    hashs1 = (unsigned int *) xmlMalloc(ns1->nodeNr * sizeof(unsigned int));
    if (hashs1 == NULL) {
	xmlFree(values1);
	return(0);
    }
    memset(values1, 0, ns1->nodeNr * sizeof(xmlChar *));
    values2 = (xmlChar **) xmlMalloc(ns2->nodeNr * sizeof(xmlChar *));
    if (values2 == NULL) {
	xmlFree(hashs1);
	xmlFree(values1);
	return(0);
    }
    hashs2 = (unsigned int *) xmlMalloc(ns2->nodeNr * sizeof(unsigned int));
    if (hashs2 == NULL) {
	xmlFree(hashs1);
	xmlFree(values1);
	xmlFree(values2);
	return(0);
    }
    memset(values2, 0, ns2->nodeNr * sizeof(xmlChar *));
    for (i = 0;i < ns1->nodeNr;i++) {
	hashs1[i] = xmlXPathNodeValHash(ns1->nodeTab[i]);
	for (j = 0;j < ns2->nodeNr;j++) {
	    if (i == 0)
		hashs2[j] = xmlXPathNodeValHash(ns2->nodeTab[j]);
	    if (hashs1[i] != hashs2[j]) {
		if (neq) {
		    ret = 1;
		    break;
		}
	    }
	    else {
		if (values1[i] == NULL)
		    values1[i] = xmlNodeGetContent(ns1->nodeTab[i]);
		if (values2[j] == NULL)
		    values2[j] = xmlNodeGetContent(ns2->nodeTab[j]);
		ret = xmlStrEqual(values1[i], values2[j]) ^ neq;
		if (ret)
		    break;
	    }
	}
	if (ret)
	    break;
    }
    for (i = 0;i < ns1->nodeNr;i++)
	if (values1[i] != NULL)
	    xmlFree(values1[i]);
    for (j = 0;j < ns2->nodeNr;j++)
	if (values2[j] != NULL)
	    xmlFree(values2[j]);
    xmlFree(values1);
    xmlFree(values2);
    xmlFree(hashs1);
    xmlFree(hashs2);
    return(ret);
}

static int
xmlXPathEqualValuesCommon(xmlXPathParserContextPtr ctxt,
  xmlXPathObjectPtr arg1, xmlXPathObjectPtr arg2) {
    int ret = 0;
    /*
     *At this point we are assured neither arg1 nor arg2
     *is a nodeset, so we can just pick the appropriate routine.
     */
    switch (arg1->type) {
        case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
	    xmlGenericError(xmlGenericErrorContext,
		    "Equal: undefined\n");
#endif
	    break;
        case XPATH_BOOLEAN:
	    switch (arg2->type) {
	        case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
		    xmlGenericError(xmlGenericErrorContext,
			    "Equal: undefined\n");
#endif
		    break;
		case XPATH_BOOLEAN:
#ifdef DEBUG_EXPR
		    xmlGenericError(xmlGenericErrorContext,
			    "Equal: %d boolean %d \n",
			    arg1->boolval, arg2->boolval);
#endif
		    ret = (arg1->boolval == arg2->boolval);
		    break;
		case XPATH_NUMBER:
		    ret = (arg1->boolval ==
			   xmlXPathCastNumberToBoolean(arg2->floatval));
		    break;
		case XPATH_STRING:
		    if ((arg2->stringval == NULL) ||
			(arg2->stringval[0] == 0)) ret = 0;
		    else 
			ret = 1;
		    ret = (arg1->boolval == ret);
		    break;
		case XPATH_USERS:
		case XPATH_POINT:
		case XPATH_RANGE:
		case XPATH_LOCATIONSET:
		    TODO
		    break;
		case XPATH_NODESET:
		case XPATH_XSLT_TREE:
		    break;
	    }
	    break;
        case XPATH_NUMBER:
	    switch (arg2->type) {
	        case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
		    xmlGenericError(xmlGenericErrorContext,
			    "Equal: undefined\n");
#endif
		    break;
		case XPATH_BOOLEAN:
		    ret = (arg2->boolval==
			   xmlXPathCastNumberToBoolean(arg1->floatval));
		    break;
		case XPATH_STRING:
		    valuePush(ctxt, arg2);
		    xmlXPathNumberFunction(ctxt, 1);
		    arg2 = valuePop(ctxt);
		    /* no break on purpose */
		case XPATH_NUMBER:
		    /* Hand check NaN and Infinity equalities */
		    if (xmlXPathIsNaN(arg1->floatval) || xmlXPathIsNaN(arg2->floatval)) {
		        ret = 0;
		    } else if (xmlXPathIsInf(arg1->floatval) == 1) {
		        if (xmlXPathIsInf(arg2->floatval) == 1)
			    ret = 1;
			else
			    ret = 0;
		    } else if (xmlXPathIsInf(arg1->floatval) == -1) {
			if (xmlXPathIsInf(arg2->floatval) == -1)
			    ret = 1;
			else
			    ret = 0;
		    } else if (xmlXPathIsInf(arg2->floatval) == 1) {
			if (xmlXPathIsInf(arg1->floatval) == 1)
			    ret = 1;
			else
			    ret = 0;
		    } else if (xmlXPathIsInf(arg2->floatval) == -1) {
			if (xmlXPathIsInf(arg1->floatval) == -1)
			    ret = 1;
			else
			    ret = 0;
		    } else {
		        ret = (arg1->floatval == arg2->floatval);
		    }
		    break;
		case XPATH_USERS:
		case XPATH_POINT:
		case XPATH_RANGE:
		case XPATH_LOCATIONSET:
		    TODO
		    break;
		case XPATH_NODESET:
		case XPATH_XSLT_TREE:
		    break;
	    }
	    break;
        case XPATH_STRING:
	    switch (arg2->type) {
	        case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
		    xmlGenericError(xmlGenericErrorContext,
			    "Equal: undefined\n");
#endif
		    break;
		case XPATH_BOOLEAN:
		    if ((arg1->stringval == NULL) ||
			(arg1->stringval[0] == 0)) ret = 0;
		    else 
			ret = 1;
		    ret = (arg2->boolval == ret);
		    break;
		case XPATH_STRING:
		    ret = xmlStrEqual(arg1->stringval, arg2->stringval);
		    break;
		case XPATH_NUMBER:
		    valuePush(ctxt, arg1);
		    xmlXPathNumberFunction(ctxt, 1);
		    arg1 = valuePop(ctxt);
		    /* Hand check NaN and Infinity equalities */
		    if (xmlXPathIsNaN(arg1->floatval) || xmlXPathIsNaN(arg2->floatval)) {
		        ret = 0;
		    } else if (xmlXPathIsInf(arg1->floatval) == 1) {
			if (xmlXPathIsInf(arg2->floatval) == 1)
			    ret = 1;
			else
			    ret = 0;
		    } else if (xmlXPathIsInf(arg1->floatval) == -1) {
			if (xmlXPathIsInf(arg2->floatval) == -1)
			    ret = 1;
			else
			    ret = 0;
		    } else if (xmlXPathIsInf(arg2->floatval) == 1) {
			if (xmlXPathIsInf(arg1->floatval) == 1)
			    ret = 1;
			else
			    ret = 0;
		    } else if (xmlXPathIsInf(arg2->floatval) == -1) {
			if (xmlXPathIsInf(arg1->floatval) == -1)
			    ret = 1;
			else
			    ret = 0;
		    } else {
		        ret = (arg1->floatval == arg2->floatval);
		    }
		    break;
		case XPATH_USERS:
		case XPATH_POINT:
		case XPATH_RANGE:
		case XPATH_LOCATIONSET:
		    TODO
		    break;
		case XPATH_NODESET:
		case XPATH_XSLT_TREE:
		    break;
	    }
	    break;
        case XPATH_USERS:
	case XPATH_POINT:
	case XPATH_RANGE:
	case XPATH_LOCATIONSET:
	    TODO
	    break;
	case XPATH_NODESET:
	case XPATH_XSLT_TREE:
	    break;
    }
    xmlXPathFreeObject(arg1);
    xmlXPathFreeObject(arg2);
    return(ret);
}

/**
 * xmlXPathEqualValues:
 * @ctxt:  the XPath Parser context
 *
 * Implement the equal operation on XPath objects content: @arg1 == @arg2
 *
 * Returns 0 or 1 depending on the results of the test.
 */
int
xmlXPathEqualValues(xmlXPathParserContextPtr ctxt) {
    xmlXPathObjectPtr arg1, arg2, argtmp;
    int ret = 0;

    arg2 = valuePop(ctxt); 
    arg1 = valuePop(ctxt);
    if ((arg1 == NULL) || (arg2 == NULL)) {
	if (arg1 != NULL)
	    xmlXPathFreeObject(arg1);
	else
	    xmlXPathFreeObject(arg2);
	XP_ERROR0(XPATH_INVALID_OPERAND);
    }

    if (arg1 == arg2) {
#ifdef DEBUG_EXPR
        xmlGenericError(xmlGenericErrorContext,
		"Equal: by pointer\n");
#endif
        return(1);
    }

    /*
     *If either argument is a nodeset, it's a 'special case'
     */
    if ((arg2->type == XPATH_NODESET) || (arg2->type == XPATH_XSLT_TREE) ||
      (arg1->type == XPATH_NODESET) || (arg1->type == XPATH_XSLT_TREE)) {
	/*
	 *Hack it to assure arg1 is the nodeset
	 */
	if ((arg1->type != XPATH_NODESET) && (arg1->type != XPATH_XSLT_TREE)) {
		argtmp = arg2;
		arg2 = arg1;
		arg1 = argtmp;
	}
	switch (arg2->type) {
	    case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
		xmlGenericError(xmlGenericErrorContext,
			"Equal: undefined\n");
#endif
		break;
	    case XPATH_NODESET:
	    case XPATH_XSLT_TREE:
		ret = xmlXPathEqualNodeSets(arg1, arg2, 0);
		break;
	    case XPATH_BOOLEAN:
		if ((arg1->nodesetval == NULL) ||
		  (arg1->nodesetval->nodeNr == 0)) ret = 0;
		else 
		    ret = 1;
		ret = (ret == arg2->boolval);
		break;
	    case XPATH_NUMBER:
		ret = xmlXPathEqualNodeSetFloat(ctxt, arg1, arg2->floatval, 0);
		break;
	    case XPATH_STRING:
		ret = xmlXPathEqualNodeSetString(arg1, arg2->stringval, 0);
		break;
	    case XPATH_USERS:
	    case XPATH_POINT:
	    case XPATH_RANGE:
	    case XPATH_LOCATIONSET:
		TODO
		break;
	}
	xmlXPathFreeObject(arg1);
	xmlXPathFreeObject(arg2);
	return(ret);
    }

    return (xmlXPathEqualValuesCommon(ctxt, arg1, arg2));
}

/**
 * xmlXPathNotEqualValues:
 * @ctxt:  the XPath Parser context
 *
 * Implement the equal operation on XPath objects content: @arg1 == @arg2
 *
 * Returns 0 or 1 depending on the results of the test.
 */
int
xmlXPathNotEqualValues(xmlXPathParserContextPtr ctxt) {
    xmlXPathObjectPtr arg1, arg2, argtmp;
    int ret = 0;

    arg2 = valuePop(ctxt); 
    arg1 = valuePop(ctxt);
    if ((arg1 == NULL) || (arg2 == NULL)) {
	if (arg1 != NULL)
	    xmlXPathFreeObject(arg1);
	else
	    xmlXPathFreeObject(arg2);
	XP_ERROR0(XPATH_INVALID_OPERAND);
    }

    if (arg1 == arg2) {
#ifdef DEBUG_EXPR
        xmlGenericError(xmlGenericErrorContext,
		"NotEqual: by pointer\n");
#endif
        return(0);
    }

    /*
     *If either argument is a nodeset, it's a 'special case'
     */
    if ((arg2->type == XPATH_NODESET) || (arg2->type == XPATH_XSLT_TREE) ||
      (arg1->type == XPATH_NODESET) || (arg1->type == XPATH_XSLT_TREE)) {
	/*
	 *Hack it to assure arg1 is the nodeset
	 */
	if ((arg1->type != XPATH_NODESET) && (arg1->type != XPATH_XSLT_TREE)) {
		argtmp = arg2;
		arg2 = arg1;
		arg1 = argtmp;
	}
	switch (arg2->type) {
	    case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
		xmlGenericError(xmlGenericErrorContext,
			"NotEqual: undefined\n");
#endif
		break;
	    case XPATH_NODESET:
	    case XPATH_XSLT_TREE:
		ret = xmlXPathEqualNodeSets(arg1, arg2, 1);
		break;
	    case XPATH_BOOLEAN:
		if ((arg1->nodesetval == NULL) ||
		  (arg1->nodesetval->nodeNr == 0)) ret = 0;
		else 
		    ret = 1;
		ret = (ret != arg2->boolval);
		break;
	    case XPATH_NUMBER:
		ret = xmlXPathEqualNodeSetFloat(ctxt, arg1, arg2->floatval, 1);
		break;
	    case XPATH_STRING:
		ret = xmlXPathEqualNodeSetString(arg1, arg2->stringval,1);
		break;
	    case XPATH_USERS:
	    case XPATH_POINT:
	    case XPATH_RANGE:
	    case XPATH_LOCATIONSET:
		TODO
		break;
	}
	xmlXPathFreeObject(arg1);
	xmlXPathFreeObject(arg2);
	return(ret);
    }

    return (!xmlXPathEqualValuesCommon(ctxt, arg1, arg2));
}

/**
 * xmlXPathCompareValues:
 * @ctxt:  the XPath Parser context
 * @inf:  less than (1) or greater than (0)
 * @strict:  is the comparison strict
 *
 * Implement the compare operation on XPath objects: 
 *     @arg1 < @arg2    (1, 1, ...
 *     @arg1 <= @arg2   (1, 0, ...
 *     @arg1 > @arg2    (0, 1, ...
 *     @arg1 >= @arg2   (0, 0, ...
 *
 * When neither object to be compared is a node-set and the operator is
 * <=, <, >=, >, then the objects are compared by converted both objects
 * to numbers and comparing the numbers according to IEEE 754. The <
 * comparison will be true if and only if the first number is less than the
 * second number. The <= comparison will be true if and only if the first
 * number is less than or equal to the second number. The > comparison
 * will be true if and only if the first number is greater than the second
 * number. The >= comparison will be true if and only if the first number
 * is greater than or equal to the second number.
 *
 * Returns 1 if the comparison succeeded, 0 if it failed
 */
int
xmlXPathCompareValues(xmlXPathParserContextPtr ctxt, int inf, int strict) {
    int ret = 0, arg1i = 0, arg2i = 0;
    xmlXPathObjectPtr arg1, arg2;

    arg2 = valuePop(ctxt); 
    arg1 = valuePop(ctxt);
    if ((arg1 == NULL) || (arg2 == NULL)) {
	if (arg1 != NULL)
	    xmlXPathFreeObject(arg1);
	else
	    xmlXPathFreeObject(arg2);
	XP_ERROR0(XPATH_INVALID_OPERAND);
    }

    if ((arg2->type == XPATH_NODESET) || (arg2->type == XPATH_XSLT_TREE) ||
      (arg1->type == XPATH_NODESET) || (arg1->type == XPATH_XSLT_TREE)) {
	if (((arg2->type == XPATH_NODESET) || (arg2->type == XPATH_XSLT_TREE)) &&
	  ((arg1->type == XPATH_NODESET) || (arg1->type == XPATH_XSLT_TREE))){
	    ret = xmlXPathCompareNodeSets(inf, strict, arg1, arg2);
	} else {
	    if ((arg1->type == XPATH_NODESET) || (arg1->type == XPATH_XSLT_TREE)) {
		ret = xmlXPathCompareNodeSetValue(ctxt, inf, strict,
			                          arg1, arg2);
	    } else {
		ret = xmlXPathCompareNodeSetValue(ctxt, !inf, strict,
			                          arg2, arg1);
	    }
	}
	return(ret);
    }

    if (arg1->type != XPATH_NUMBER) {
	valuePush(ctxt, arg1);
	xmlXPathNumberFunction(ctxt, 1);
	arg1 = valuePop(ctxt);
    }
    if (arg1->type != XPATH_NUMBER) {
	xmlXPathFreeObject(arg1);
	xmlXPathFreeObject(arg2);
	XP_ERROR0(XPATH_INVALID_OPERAND);
    }
    if (arg2->type != XPATH_NUMBER) {
	valuePush(ctxt, arg2);
	xmlXPathNumberFunction(ctxt, 1);
	arg2 = valuePop(ctxt);
    }
    if (arg2->type != XPATH_NUMBER) {
	xmlXPathFreeObject(arg1);
	xmlXPathFreeObject(arg2);
	XP_ERROR0(XPATH_INVALID_OPERAND);
    }
    /*
     * Add tests for infinity and nan
     * => feedback on 3.4 for Inf and NaN
     */
    /* Hand check NaN and Infinity comparisons */
    if (xmlXPathIsNaN(arg1->floatval) || xmlXPathIsNaN(arg2->floatval)) {
	ret=0;
    } else {
	arg1i=xmlXPathIsInf(arg1->floatval);
	arg2i=xmlXPathIsInf(arg2->floatval);
	if (inf && strict) {
	    if ((arg1i == -1 && arg2i != -1) ||
		(arg2i == 1 && arg1i != 1)) {
		ret = 1;
	    } else if (arg1i == 0 && arg2i == 0) {
		ret = (arg1->floatval < arg2->floatval);
	    } else {
		ret = 0;
	    }
	}
	else if (inf && !strict) {
	    if (arg1i == -1 || arg2i == 1) {
		ret = 1;
	    } else if (arg1i == 0 && arg2i == 0) {
		ret = (arg1->floatval <= arg2->floatval);
	    } else {
		ret = 0;
	    }
	}
	else if (!inf && strict) {
	    if ((arg1i == 1 && arg2i != 1) ||
		(arg2i == -1 && arg1i != -1)) {
		ret = 1;
	    } else if (arg1i == 0 && arg2i == 0) {
		ret = (arg1->floatval > arg2->floatval);
	    } else {
		ret = 0;
	    }
	}
	else if (!inf && !strict) {
	    if (arg1i == 1 || arg2i == -1) {
		ret = 1;
	    } else if (arg1i == 0 && arg2i == 0) {
		ret = (arg1->floatval >= arg2->floatval);
	    } else {
		ret = 0;
	    }
	}
    }
    xmlXPathFreeObject(arg1);
    xmlXPathFreeObject(arg2);
    return(ret);
}

/**
 * xmlXPathValueFlipSign:
 * @ctxt:  the XPath Parser context
 *
 * Implement the unary - operation on an XPath object
 * The numeric operators convert their operands to numbers as if
 * by calling the number function.
 */
void
xmlXPathValueFlipSign(xmlXPathParserContextPtr ctxt) {
    CAST_TO_NUMBER;
    CHECK_TYPE(XPATH_NUMBER);
    if (xmlXPathIsNaN(ctxt->value->floatval))
        ctxt->value->floatval=xmlXPathNAN;
    else if (xmlXPathIsInf(ctxt->value->floatval) == 1)
        ctxt->value->floatval=xmlXPathNINF;
    else if (xmlXPathIsInf(ctxt->value->floatval) == -1)
        ctxt->value->floatval=xmlXPathPINF;
    else if (ctxt->value->floatval == 0) {
        if (xmlXPathGetSign(ctxt->value->floatval) == 0)
	    ctxt->value->floatval = xmlXPathNZERO;
	else
	    ctxt->value->floatval = 0;
    }
    else
        ctxt->value->floatval = - ctxt->value->floatval;
}

/**
 * xmlXPathAddValues:
 * @ctxt:  the XPath Parser context
 *
 * Implement the add operation on XPath objects:
 * The numeric operators convert their operands to numbers as if
 * by calling the number function.
 */
void
xmlXPathAddValues(xmlXPathParserContextPtr ctxt) {
    xmlXPathObjectPtr arg;
    double val;

    arg = valuePop(ctxt);
    if (arg == NULL)
	XP_ERROR(XPATH_INVALID_OPERAND);
    val = xmlXPathCastToNumber(arg);
    xmlXPathFreeObject(arg);

    CAST_TO_NUMBER;
    CHECK_TYPE(XPATH_NUMBER);
    ctxt->value->floatval += val;
}

/**
 * xmlXPathSubValues:
 * @ctxt:  the XPath Parser context
 *
 * Implement the subtraction operation on XPath objects:
 * The numeric operators convert their operands to numbers as if
 * by calling the number function.
 */
void
xmlXPathSubValues(xmlXPathParserContextPtr ctxt) {
    xmlXPathObjectPtr arg;
    double val;

    arg = valuePop(ctxt);
    if (arg == NULL)
	XP_ERROR(XPATH_INVALID_OPERAND);
    val = xmlXPathCastToNumber(arg);
    xmlXPathFreeObject(arg);

    CAST_TO_NUMBER;
    CHECK_TYPE(XPATH_NUMBER);
    ctxt->value->floatval -= val;
}

/**
 * xmlXPathMultValues:
 * @ctxt:  the XPath Parser context
 *
 * Implement the multiply operation on XPath objects:
 * The numeric operators convert their operands to numbers as if
 * by calling the number function.
 */
void
xmlXPathMultValues(xmlXPathParserContextPtr ctxt) {
    xmlXPathObjectPtr arg;
    double val;

    arg = valuePop(ctxt);
    if (arg == NULL)
	XP_ERROR(XPATH_INVALID_OPERAND);
    val = xmlXPathCastToNumber(arg);
    xmlXPathFreeObject(arg);

    CAST_TO_NUMBER;
    CHECK_TYPE(XPATH_NUMBER);
    ctxt->value->floatval *= val;
}

/**
 * xmlXPathDivValues:
 * @ctxt:  the XPath Parser context
 *
 * Implement the div operation on XPath objects @arg1 / @arg2:
 * The numeric operators convert their operands to numbers as if
 * by calling the number function.
 */
void
xmlXPathDivValues(xmlXPathParserContextPtr ctxt) {
    xmlXPathObjectPtr arg;
    double val;

    arg = valuePop(ctxt);
    if (arg == NULL)
	XP_ERROR(XPATH_INVALID_OPERAND);
    val = xmlXPathCastToNumber(arg);
    xmlXPathFreeObject(arg);

    CAST_TO_NUMBER;
    CHECK_TYPE(XPATH_NUMBER);
    if (xmlXPathIsNaN(val) || xmlXPathIsNaN(ctxt->value->floatval))
	ctxt->value->floatval = xmlXPathNAN;
    else if (val == 0 && xmlXPathGetSign(val) != 0) {
	if (ctxt->value->floatval == 0)
	    ctxt->value->floatval = xmlXPathNAN;
	else if (ctxt->value->floatval > 0)
	    ctxt->value->floatval = xmlXPathNINF;
	else if (ctxt->value->floatval < 0)
	    ctxt->value->floatval = xmlXPathPINF;
    }
    else if (val == 0) {
	if (ctxt->value->floatval == 0)
	    ctxt->value->floatval = xmlXPathNAN;
	else if (ctxt->value->floatval > 0)
	    ctxt->value->floatval = xmlXPathPINF;
	else if (ctxt->value->floatval < 0)
	    ctxt->value->floatval = xmlXPathNINF;
    } else 
	ctxt->value->floatval /= val;
}

/**
 * xmlXPathModValues:
 * @ctxt:  the XPath Parser context
 *
 * Implement the mod operation on XPath objects: @arg1 / @arg2
 * The numeric operators convert their operands to numbers as if
 * by calling the number function.
 */
void
xmlXPathModValues(xmlXPathParserContextPtr ctxt) {
    xmlXPathObjectPtr arg;
    double arg1, arg2;

    arg = valuePop(ctxt);
    if (arg == NULL)
	XP_ERROR(XPATH_INVALID_OPERAND);
    arg2 = xmlXPathCastToNumber(arg);
    xmlXPathFreeObject(arg);

    CAST_TO_NUMBER;
    CHECK_TYPE(XPATH_NUMBER);
    arg1 = ctxt->value->floatval;
    if (arg2 == 0)
	ctxt->value->floatval = xmlXPathNAN;
    else {
	ctxt->value->floatval = fmod(arg1, arg2);
    }
}

/************************************************************************
 *									*
 *		The traversal functions					*
 *									*
 ************************************************************************/

/*
 * A traversal function enumerates nodes along an axis.
 * Initially it must be called with NULL, and it indicates
 * termination on the axis by returning NULL.
 */
typedef xmlNodePtr (*xmlXPathTraversalFunction)
                    (xmlXPathParserContextPtr ctxt, xmlNodePtr cur);

/**
 * xmlXPathNextSelf:
 * @ctxt:  the XPath Parser context
 * @cur:  the current node in the traversal
 *
 * Traversal function for the "self" direction
 * The self axis contains just the context node itself
 *
 * Returns the next element following that axis
 */
xmlNodePtr
xmlXPathNextSelf(xmlXPathParserContextPtr ctxt, xmlNodePtr cur) {
    if (cur == NULL)
        return(ctxt->context->node);
    return(NULL);
}

/**
 * xmlXPathNextChild:
 * @ctxt:  the XPath Parser context
 * @cur:  the current node in the traversal
 *
 * Traversal function for the "child" direction
 * The child axis contains the children of the context node in document order.
 *
 * Returns the next element following that axis
 */
xmlNodePtr
xmlXPathNextChild(xmlXPathParserContextPtr ctxt, xmlNodePtr cur) {
    if (cur == NULL) {
	if (ctxt->context->node == NULL) return(NULL);
	switch (ctxt->context->node->type) {
            case XML_ELEMENT_NODE:
            case XML_TEXT_NODE:
            case XML_CDATA_SECTION_NODE:
            case XML_ENTITY_REF_NODE:
            case XML_ENTITY_NODE:
            case XML_PI_NODE:
            case XML_COMMENT_NODE:
            case XML_NOTATION_NODE:
            case XML_DTD_NODE:
		return(ctxt->context->node->children);
            case XML_DOCUMENT_NODE:
            case XML_DOCUMENT_TYPE_NODE:
            case XML_DOCUMENT_FRAG_NODE:
            case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
	    case XML_DOCB_DOCUMENT_NODE:
#endif
		return(((xmlDocPtr) ctxt->context->node)->children);
	    case XML_ELEMENT_DECL:
	    case XML_ATTRIBUTE_DECL:
	    case XML_ENTITY_DECL:
            case XML_ATTRIBUTE_NODE:
	    case XML_NAMESPACE_DECL:
	    case XML_XINCLUDE_START:
	    case XML_XINCLUDE_END:
		return(NULL);
	}
	return(NULL);
    }
    if ((cur->type == XML_DOCUMENT_NODE) ||
        (cur->type == XML_HTML_DOCUMENT_NODE))
	return(NULL);
    return(cur->next);
}

/**
 * xmlXPathNextDescendant:
 * @ctxt:  the XPath Parser context
 * @cur:  the current node in the traversal
 *
 * Traversal function for the "descendant" direction
 * the descendant axis contains the descendants of the context node in document
 * order; a descendant is a child or a child of a child and so on.
 *
 * Returns the next element following that axis
 */
xmlNodePtr
xmlXPathNextDescendant(xmlXPathParserContextPtr ctxt, xmlNodePtr cur) {
    if (cur == NULL) {
	if (ctxt->context->node == NULL)
	    return(NULL);
	if ((ctxt->context->node->type == XML_ATTRIBUTE_NODE) ||
	    (ctxt->context->node->type == XML_NAMESPACE_DECL))
	    return(NULL);

        if (ctxt->context->node == (xmlNodePtr) ctxt->context->doc)
	    return(ctxt->context->doc->children);
        return(ctxt->context->node->children);
    }

    if (cur->children != NULL) {
	/*
	 * Do not descend on entities declarations
	 */
    	if (cur->children->type != XML_ENTITY_DECL) {
	    cur = cur->children;
	    /*
	     * Skip DTDs
	     */
	    if (cur->type != XML_DTD_NODE)
		return(cur);
	}
    }

    if (cur == ctxt->context->node) return(NULL);

    while (cur->next != NULL) {
	cur = cur->next;
	if ((cur->type != XML_ENTITY_DECL) &&
	    (cur->type != XML_DTD_NODE))
	    return(cur);
    }
    
    do {
        cur = cur->parent;
	if (cur == NULL) return(NULL);
	if (cur == ctxt->context->node) return(NULL);
	if (cur->next != NULL) {
	    cur = cur->next;
	    return(cur);
	}
    } while (cur != NULL);
    return(cur);
}

/**
 * xmlXPathNextDescendantOrSelf:
 * @ctxt:  the XPath Parser context
 * @cur:  the current node in the traversal
 *
 * Traversal function for the "descendant-or-self" direction
 * the descendant-or-self axis contains the context node and the descendants
 * of the context node in document order; thus the context node is the first
 * node on the axis, and the first child of the context node is the second node
 * on the axis
 *
 * Returns the next element following that axis
 */
xmlNodePtr
xmlXPathNextDescendantOrSelf(xmlXPathParserContextPtr ctxt, xmlNodePtr cur) {
    if (cur == NULL) {
	if (ctxt->context->node == NULL)
	    return(NULL);
	if ((ctxt->context->node->type == XML_ATTRIBUTE_NODE) ||
	    (ctxt->context->node->type == XML_NAMESPACE_DECL))
	    return(NULL);
        return(ctxt->context->node);
    }

    return(xmlXPathNextDescendant(ctxt, cur));
}

/**
 * xmlXPathNextParent:
 * @ctxt:  the XPath Parser context
 * @cur:  the current node in the traversal
 *
 * Traversal function for the "parent" direction
 * The parent axis contains the parent of the context node, if there is one.
 *
 * Returns the next element following that axis
 */
xmlNodePtr
xmlXPathNextParent(xmlXPathParserContextPtr ctxt, xmlNodePtr cur) {
    /*
     * the parent of an attribute or namespace node is the element
     * to which the attribute or namespace node is attached
     * Namespace handling !!!
     */
    if (cur == NULL) {
	if (ctxt->context->node == NULL) return(NULL);
	switch (ctxt->context->node->type) {
            case XML_ELEMENT_NODE:
            case XML_TEXT_NODE:
            case XML_CDATA_SECTION_NODE:
            case XML_ENTITY_REF_NODE:
            case XML_ENTITY_NODE:
            case XML_PI_NODE:
            case XML_COMMENT_NODE:
            case XML_NOTATION_NODE:
            case XML_DTD_NODE:
	    case XML_ELEMENT_DECL:
	    case XML_ATTRIBUTE_DECL:
	    case XML_XINCLUDE_START:
	    case XML_XINCLUDE_END:
	    case XML_ENTITY_DECL:
		if (ctxt->context->node->parent == NULL)
		    return((xmlNodePtr) ctxt->context->doc);
		return(ctxt->context->node->parent);
            case XML_ATTRIBUTE_NODE: {
		xmlAttrPtr att = (xmlAttrPtr) ctxt->context->node;

		return(att->parent);
	    }
            case XML_DOCUMENT_NODE:
            case XML_DOCUMENT_TYPE_NODE:
            case XML_DOCUMENT_FRAG_NODE:
            case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
	    case XML_DOCB_DOCUMENT_NODE:
#endif
                return(NULL);
	    case XML_NAMESPACE_DECL: {
		xmlNsPtr ns = (xmlNsPtr) ctxt->context->node;
		
		if ((ns->next != NULL) &&
		    (ns->next->type != XML_NAMESPACE_DECL))
		    return((xmlNodePtr) ns->next);
                return(NULL);
	    }
	}
    }
    return(NULL);
}

/**
 * xmlXPathNextAncestor:
 * @ctxt:  the XPath Parser context
 * @cur:  the current node in the traversal
 *
 * Traversal function for the "ancestor" direction
 * the ancestor axis contains the ancestors of the context node; the ancestors
 * of the context node consist of the parent of context node and the parent's
 * parent and so on; the nodes are ordered in reverse document order; thus the
 * parent is the first node on the axis, and the parent's parent is the second
 * node on the axis
 *
 * Returns the next element following that axis
 */
xmlNodePtr
xmlXPathNextAncestor(xmlXPathParserContextPtr ctxt, xmlNodePtr cur) {
    /*
     * the parent of an attribute or namespace node is the element
     * to which the attribute or namespace node is attached
     * !!!!!!!!!!!!!
     */
    if (cur == NULL) {
	if (ctxt->context->node == NULL) return(NULL);
	switch (ctxt->context->node->type) {
            case XML_ELEMENT_NODE:
            case XML_TEXT_NODE:
            case XML_CDATA_SECTION_NODE:
            case XML_ENTITY_REF_NODE:
            case XML_ENTITY_NODE:
            case XML_PI_NODE:
            case XML_COMMENT_NODE:
	    case XML_DTD_NODE:
	    case XML_ELEMENT_DECL:
	    case XML_ATTRIBUTE_DECL:
	    case XML_ENTITY_DECL:
            case XML_NOTATION_NODE:
	    case XML_XINCLUDE_START:
	    case XML_XINCLUDE_END:
		if (ctxt->context->node->parent == NULL)
		    return((xmlNodePtr) ctxt->context->doc);
		return(ctxt->context->node->parent);
            case XML_ATTRIBUTE_NODE: {
		xmlAttrPtr tmp = (xmlAttrPtr) ctxt->context->node;

		return(tmp->parent);
	    }
            case XML_DOCUMENT_NODE:
            case XML_DOCUMENT_TYPE_NODE:
            case XML_DOCUMENT_FRAG_NODE:
            case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
	    case XML_DOCB_DOCUMENT_NODE:
#endif
                return(NULL);
	    case XML_NAMESPACE_DECL: {
		xmlNsPtr ns = (xmlNsPtr) ctxt->context->node;
		
		if ((ns->next != NULL) &&
		    (ns->next->type != XML_NAMESPACE_DECL))
		    return((xmlNodePtr) ns->next);
		/* Bad, how did that namespace ended-up there ? */
                return(NULL);
	    }
	}
	return(NULL);
    }
    if (cur == ctxt->context->doc->children)
	return((xmlNodePtr) ctxt->context->doc);
    if (cur == (xmlNodePtr) ctxt->context->doc)
	return(NULL);
    switch (cur->type) {
	case XML_ELEMENT_NODE:
	case XML_TEXT_NODE:
	case XML_CDATA_SECTION_NODE:
	case XML_ENTITY_REF_NODE:
	case XML_ENTITY_NODE:
	case XML_PI_NODE:
	case XML_COMMENT_NODE:
	case XML_NOTATION_NODE:
	case XML_DTD_NODE:
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
	case XML_XINCLUDE_START:
	case XML_XINCLUDE_END:
	    return(cur->parent);
	case XML_ATTRIBUTE_NODE: {
	    xmlAttrPtr att = (xmlAttrPtr) ctxt->context->node;

	    return(att->parent);
	}
	case XML_NAMESPACE_DECL: {
	    xmlNsPtr ns = (xmlNsPtr) ctxt->context->node;
		
	    if ((ns->next != NULL) &&
	        (ns->next->type != XML_NAMESPACE_DECL))
	        return((xmlNodePtr) ns->next);
	    /* Bad, how did that namespace ended-up there ? */
            return(NULL);
	}
	case XML_DOCUMENT_NODE:
	case XML_DOCUMENT_TYPE_NODE:
	case XML_DOCUMENT_FRAG_NODE:
	case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
	case XML_DOCB_DOCUMENT_NODE:
#endif
	    return(NULL);
    }
    return(NULL);
}

/**
 * xmlXPathNextAncestorOrSelf:
 * @ctxt:  the XPath Parser context
 * @cur:  the current node in the traversal
 *
 * Traversal function for the "ancestor-or-self" direction
 * he ancestor-or-self axis contains the context node and ancestors of
 * the context node in reverse document order; thus the context node is
 * the first node on the axis, and the context node's parent the second;
 * parent here is defined the same as with the parent axis.
 *
 * Returns the next element following that axis
 */
xmlNodePtr
xmlXPathNextAncestorOrSelf(xmlXPathParserContextPtr ctxt, xmlNodePtr cur) {
    if (cur == NULL)
        return(ctxt->context->node);
    return(xmlXPathNextAncestor(ctxt, cur));
}

/**
 * xmlXPathNextFollowingSibling:
 * @ctxt:  the XPath Parser context
 * @cur:  the current node in the traversal
 *
 * Traversal function for the "following-sibling" direction
 * The following-sibling axis contains the following siblings of the context
 * node in document order.
 *
 * Returns the next element following that axis
 */
xmlNodePtr
xmlXPathNextFollowingSibling(xmlXPathParserContextPtr ctxt, xmlNodePtr cur) {
    if ((ctxt->context->node->type == XML_ATTRIBUTE_NODE) ||
	(ctxt->context->node->type == XML_NAMESPACE_DECL))
	return(NULL);
    if (cur == (xmlNodePtr) ctxt->context->doc)
        return(NULL);
    if (cur == NULL)
        return(ctxt->context->node->next);
    return(cur->next);
}

/**
 * xmlXPathNextPrecedingSibling:
 * @ctxt:  the XPath Parser context
 * @cur:  the current node in the traversal
 *
 * Traversal function for the "preceding-sibling" direction
 * The preceding-sibling axis contains the preceding siblings of the context
 * node in reverse document order; the first preceding sibling is first on the
 * axis; the sibling preceding that node is the second on the axis and so on.
 *
 * Returns the next element following that axis
 */
xmlNodePtr
xmlXPathNextPrecedingSibling(xmlXPathParserContextPtr ctxt, xmlNodePtr cur) {
    if ((ctxt->context->node->type == XML_ATTRIBUTE_NODE) ||
	(ctxt->context->node->type == XML_NAMESPACE_DECL))
	return(NULL);
    if (cur == (xmlNodePtr) ctxt->context->doc)
        return(NULL);
    if (cur == NULL)
        return(ctxt->context->node->prev);
    if ((cur->prev != NULL) && (cur->prev->type == XML_DTD_NODE)) {
	cur = cur->prev;
	if (cur == NULL)
	    return(ctxt->context->node->prev);
    }
    return(cur->prev);
}

/**
 * xmlXPathNextFollowing:
 * @ctxt:  the XPath Parser context
 * @cur:  the current node in the traversal
 *
 * Traversal function for the "following" direction
 * The following axis contains all nodes in the same document as the context
 * node that are after the context node in document order, excluding any
 * descendants and excluding attribute nodes and namespace nodes; the nodes
 * are ordered in document order
 *
 * Returns the next element following that axis
 */
xmlNodePtr
xmlXPathNextFollowing(xmlXPathParserContextPtr ctxt, xmlNodePtr cur) {
    if (cur != NULL && cur->children != NULL)
        return cur->children ;
    if (cur == NULL) cur = ctxt->context->node;
    if (cur == NULL) return(NULL) ; /* ERROR */
    if (cur->next != NULL) return(cur->next) ;
    do {
        cur = cur->parent;
        if (cur == NULL) return(NULL);
        if (cur == (xmlNodePtr) ctxt->context->doc) return(NULL);
        if (cur->next != NULL) return(cur->next);
    } while (cur != NULL);
    return(cur);
}

/*
 * xmlXPathIsAncestor:
 * @ancestor:  the ancestor node
 * @node:  the current node
 *
 * Check that @ancestor is a @node's ancestor
 *
 * returns 1 if @ancestor is a @node's ancestor, 0 otherwise.
 */
static int
xmlXPathIsAncestor(xmlNodePtr ancestor, xmlNodePtr node) {
    if ((ancestor == NULL) || (node == NULL)) return(0);
    /* nodes need to be in the same document */
    if (ancestor->doc != node->doc) return(0);
    /* avoid searching if ancestor or node is the root node */
    if (ancestor == (xmlNodePtr) node->doc) return(1);
    if (node == (xmlNodePtr) ancestor->doc) return(0);
    while (node->parent != NULL) {
        if (node->parent == ancestor)
            return(1);
	node = node->parent;
    }
    return(0);
}

/**
 * xmlXPathNextPreceding:
 * @ctxt:  the XPath Parser context
 * @cur:  the current node in the traversal
 *
 * Traversal function for the "preceding" direction
 * the preceding axis contains all nodes in the same document as the context
 * node that are before the context node in document order, excluding any
 * ancestors and excluding attribute nodes and namespace nodes; the nodes are
 * ordered in reverse document order
 *
 * Returns the next element following that axis
 */
xmlNodePtr
xmlXPathNextPreceding(xmlXPathParserContextPtr ctxt, xmlNodePtr cur)
{
    if (cur == NULL)
        cur = ctxt->context->node;
    if (cur == NULL)
	return (NULL);
    if ((cur->prev != NULL) && (cur->prev->type == XML_DTD_NODE))
	cur = cur->prev;
    do {
        if (cur->prev != NULL) {
            for (cur = cur->prev; cur->last != NULL; cur = cur->last) ;
            return (cur);
        }

        cur = cur->parent;
        if (cur == NULL)
            return (NULL);
        if (cur == ctxt->context->doc->children)
            return (NULL);
    } while (xmlXPathIsAncestor(cur, ctxt->context->node));
    return (cur);
}

/**
 * xmlXPathNextPrecedingInternal:
 * @ctxt:  the XPath Parser context
 * @cur:  the current node in the traversal
 *
 * Traversal function for the "preceding" direction
 * the preceding axis contains all nodes in the same document as the context
 * node that are before the context node in document order, excluding any
 * ancestors and excluding attribute nodes and namespace nodes; the nodes are
 * ordered in reverse document order
 * This is a faster implementation but internal only since it requires a 
 * state kept in the parser context: ctxt->ancestor.
 *
 * Returns the next element following that axis
 */
static xmlNodePtr
xmlXPathNextPrecedingInternal(xmlXPathParserContextPtr ctxt,
                              xmlNodePtr cur)
{
    if (cur == NULL) {
        cur = ctxt->context->node;
        if (cur == NULL)
            return (NULL);
        ctxt->ancestor = cur->parent;
    }
    if ((cur->prev != NULL) && (cur->prev->type == XML_DTD_NODE))
	cur = cur->prev;
    while (cur->prev == NULL) {
        cur = cur->parent;
        if (cur == NULL)
            return (NULL);
        if (cur == ctxt->context->doc->children)
            return (NULL);
        if (cur != ctxt->ancestor)
            return (cur);
        ctxt->ancestor = cur->parent;
    }
    cur = cur->prev;
    while (cur->last != NULL)
        cur = cur->last;
    return (cur);
}

/**
 * xmlXPathNextNamespace:
 * @ctxt:  the XPath Parser context
 * @cur:  the current attribute in the traversal
 *
 * Traversal function for the "namespace" direction
 * the namespace axis contains the namespace nodes of the context node;
 * the order of nodes on this axis is implementation-defined; the axis will
 * be empty unless the context node is an element
 *
 * We keep the XML namespace node at the end of the list.
 *
 * Returns the next element following that axis
 */
xmlNodePtr
xmlXPathNextNamespace(xmlXPathParserContextPtr ctxt, xmlNodePtr cur) {
    if (ctxt->context->node->type != XML_ELEMENT_NODE) return(NULL);
    if (ctxt->context->tmpNsList == NULL && cur != (xmlNodePtr) xmlXPathXMLNamespace) {
        if (ctxt->context->tmpNsList != NULL)
	    xmlFree(ctxt->context->tmpNsList);
	ctxt->context->tmpNsList = 
	    xmlGetNsList(ctxt->context->doc, ctxt->context->node);
	ctxt->context->tmpNsNr = 0;
	if (ctxt->context->tmpNsList != NULL) {
	    while (ctxt->context->tmpNsList[ctxt->context->tmpNsNr] != NULL) {
		ctxt->context->tmpNsNr++;
	    }
	}
	return((xmlNodePtr) xmlXPathXMLNamespace);
    }
    if (ctxt->context->tmpNsNr > 0) {
	return (xmlNodePtr)ctxt->context->tmpNsList[--ctxt->context->tmpNsNr];
    } else {
	if (ctxt->context->tmpNsList != NULL)
	    xmlFree(ctxt->context->tmpNsList);
	ctxt->context->tmpNsList = NULL;
	return(NULL);
    }
}

/**
 * xmlXPathNextAttribute:
 * @ctxt:  the XPath Parser context
 * @cur:  the current attribute in the traversal
 *
 * Traversal function for the "attribute" direction
 * TODO: support DTD inherited default attributes
 *
 * Returns the next element following that axis
 */
xmlNodePtr
xmlXPathNextAttribute(xmlXPathParserContextPtr ctxt, xmlNodePtr cur) {
    if (ctxt->context->node == NULL)
	return(NULL);
    if (ctxt->context->node->type != XML_ELEMENT_NODE)
	return(NULL);
    if (cur == NULL) {
        if (ctxt->context->node == (xmlNodePtr) ctxt->context->doc)
	    return(NULL);
        return((xmlNodePtr)ctxt->context->node->properties);
    }
    return((xmlNodePtr)cur->next);
}

/************************************************************************
 *									*
 *		NodeTest Functions					*
 *									*
 ************************************************************************/

#define IS_FUNCTION			200


/************************************************************************
 *									*
 *		Implicit tree core function library			*
 *									*
 ************************************************************************/

/**
 * xmlXPathRoot:
 * @ctxt:  the XPath Parser context
 *
 * Initialize the context to the root of the document
 */
void
xmlXPathRoot(xmlXPathParserContextPtr ctxt) {
    ctxt->context->node = (xmlNodePtr) ctxt->context->doc;
    valuePush(ctxt, xmlXPathNewNodeSet(ctxt->context->node));
}

/************************************************************************
 *									*
 *		The explicit core function library			*
 *http://www.w3.org/Style/XSL/Group/1999/07/xpath-19990705.html#corelib	*
 *									*
 ************************************************************************/


/**
 * xmlXPathLastFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the last() XPath function
 *    number last()
 * The last function returns the number of nodes in the context node list.
 */
void
xmlXPathLastFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    CHECK_ARITY(0);
    if (ctxt->context->contextSize >= 0) {
	valuePush(ctxt, xmlXPathNewFloat((double) ctxt->context->contextSize));
#ifdef DEBUG_EXPR
	xmlGenericError(xmlGenericErrorContext,
		"last() : %d\n", ctxt->context->contextSize);
#endif
    } else {
	XP_ERROR(XPATH_INVALID_CTXT_SIZE);
    }
}

/**
 * xmlXPathPositionFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the position() XPath function
 *    number position()
 * The position function returns the position of the context node in the
 * context node list. The first position is 1, and so the last position
 * will be equal to last().
 */
void
xmlXPathPositionFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    CHECK_ARITY(0);
    if (ctxt->context->proximityPosition >= 0) {
	valuePush(ctxt,
	      xmlXPathNewFloat((double) ctxt->context->proximityPosition));
#ifdef DEBUG_EXPR
	xmlGenericError(xmlGenericErrorContext, "position() : %d\n",
		ctxt->context->proximityPosition);
#endif
    } else {
	XP_ERROR(XPATH_INVALID_CTXT_POSITION);
    }
}

/**
 * xmlXPathCountFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the count() XPath function
 *    number count(node-set)
 */
void
xmlXPathCountFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    xmlXPathObjectPtr cur;

    CHECK_ARITY(1);
    if ((ctxt->value == NULL) || 
	((ctxt->value->type != XPATH_NODESET) &&
	 (ctxt->value->type != XPATH_XSLT_TREE)))
	XP_ERROR(XPATH_INVALID_TYPE);
    cur = valuePop(ctxt);

    if ((cur == NULL) || (cur->nodesetval == NULL))
	valuePush(ctxt, xmlXPathNewFloat((double) 0));
    else if ((cur->type == XPATH_NODESET) || (cur->type == XPATH_XSLT_TREE)) {
	valuePush(ctxt, xmlXPathNewFloat((double) cur->nodesetval->nodeNr));
    } else {
	if ((cur->nodesetval->nodeNr != 1) ||
	    (cur->nodesetval->nodeTab == NULL)) {
	    valuePush(ctxt, xmlXPathNewFloat((double) 0));
	} else {
	    xmlNodePtr tmp;
	    int i = 0;

	    tmp = cur->nodesetval->nodeTab[0];
	    if (tmp != NULL) {
		tmp = tmp->children;
		while (tmp != NULL) {
		    tmp = tmp->next;
		    i++;
		}
	    }
	    valuePush(ctxt, xmlXPathNewFloat((double) i));
	}
    }
    xmlXPathFreeObject(cur);
}

/**
 * xmlXPathGetElementsByIds:
 * @doc:  the document
 * @ids:  a whitespace separated list of IDs
 *
 * Selects elements by their unique ID.
 *
 * Returns a node-set of selected elements.
 */
static xmlNodeSetPtr
xmlXPathGetElementsByIds (xmlDocPtr doc, const xmlChar *ids) {
    xmlNodeSetPtr ret;
    const xmlChar *cur = ids;
    xmlChar *ID;
    xmlAttrPtr attr;
    xmlNodePtr elem = NULL;

    ret = xmlXPathNodeSetCreate(NULL);

    while (IS_BLANK(*cur)) cur++;
    while (*cur != 0) {
	while ((IS_LETTER(*cur)) || (IS_DIGIT(*cur)) ||
	       (*cur == '.') || (*cur == '-') ||
	       (*cur == '_') || (*cur == ':') || 
	       (IS_COMBINING(*cur)) ||
	       (IS_EXTENDER(*cur)))
	       cur++;

	if ((!IS_BLANK(*cur)) && (*cur != 0)) break;

        ID = xmlStrndup(ids, cur - ids);
	attr = xmlGetID(doc, ID);
	if (attr != NULL) {
	    elem = attr->parent;
            xmlXPathNodeSetAdd(ret, elem);
        }
	if (ID != NULL)
	    xmlFree(ID);

	while (IS_BLANK(*cur)) cur++;
	ids = cur;
    }
    return(ret);
}

/**
 * xmlXPathIdFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the id() XPath function
 *    node-set id(object)
 * The id function selects elements by their unique ID
 * (see [5.2.1 Unique IDs]). When the argument to id is of type node-set,
 * then the result is the union of the result of applying id to the
 * string value of each of the nodes in the argument node-set. When the
 * argument to id is of any other type, the argument is converted to a
 * string as if by a call to the string function; the string is split
 * into a whitespace-separated list of tokens (whitespace is any sequence
 * of characters matching the production S); the result is a node-set
 * containing the elements in the same document as the context node that
 * have a unique ID equal to any of the tokens in the list.
 */
void
xmlXPathIdFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    xmlChar *tokens;
    xmlNodeSetPtr ret;
    xmlXPathObjectPtr obj;

    CHECK_ARITY(1);
    obj = valuePop(ctxt);
    if (obj == NULL) XP_ERROR(XPATH_INVALID_OPERAND);
    if ((obj->type == XPATH_NODESET) || (obj->type == XPATH_XSLT_TREE)) {
	xmlNodeSetPtr ns;
	int i;

	ret = xmlXPathNodeSetCreate(NULL);

	if (obj->nodesetval != NULL) {
	    for (i = 0; i < obj->nodesetval->nodeNr; i++) {
		tokens =
		    xmlXPathCastNodeToString(obj->nodesetval->nodeTab[i]);
		ns = xmlXPathGetElementsByIds(ctxt->context->doc, tokens);
		ret = xmlXPathNodeSetMerge(ret, ns);
		xmlXPathFreeNodeSet(ns);
		if (tokens != NULL)
		    xmlFree(tokens);
	    }
	}

	xmlXPathFreeObject(obj);
	valuePush(ctxt, xmlXPathWrapNodeSet(ret));
	return;
    }
    obj = xmlXPathConvertString(obj);

    ret = xmlXPathGetElementsByIds(ctxt->context->doc, obj->stringval);
    valuePush(ctxt, xmlXPathWrapNodeSet(ret));

    xmlXPathFreeObject(obj);
    return;
}

/**
 * xmlXPathLocalNameFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the local-name() XPath function
 *    string local-name(node-set?)
 * The local-name function returns a string containing the local part
 * of the name of the node in the argument node-set that is first in
 * document order. If the node-set is empty or the first node has no
 * name, an empty string is returned. If the argument is omitted it
 * defaults to the context node.
 */
void
xmlXPathLocalNameFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    xmlXPathObjectPtr cur;

    if (nargs == 0) {
	valuePush(ctxt, xmlXPathNewNodeSet(ctxt->context->node));
	nargs = 1;
    }

    CHECK_ARITY(1);
    if ((ctxt->value == NULL) || 
	((ctxt->value->type != XPATH_NODESET) &&
	 (ctxt->value->type != XPATH_XSLT_TREE)))
	XP_ERROR(XPATH_INVALID_TYPE);
    cur = valuePop(ctxt);

    if ((cur->nodesetval == NULL) || (cur->nodesetval->nodeNr == 0)) {
	valuePush(ctxt, xmlXPathNewCString(""));
    } else {
	int i = 0; /* Should be first in document order !!!!! */
	switch (cur->nodesetval->nodeTab[i]->type) {
	case XML_ELEMENT_NODE:
	case XML_ATTRIBUTE_NODE:
	case XML_PI_NODE:
	    valuePush(ctxt,
		      xmlXPathNewString(cur->nodesetval->nodeTab[i]->name));
	    break;
	case XML_NAMESPACE_DECL:
	    valuePush(ctxt, xmlXPathNewString(
			((xmlNsPtr)cur->nodesetval->nodeTab[i])->prefix));
	    break;
	default:
	    valuePush(ctxt, xmlXPathNewCString(""));
	}
    }
    xmlXPathFreeObject(cur);
}

/**
 * xmlXPathNamespaceURIFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the namespace-uri() XPath function
 *    string namespace-uri(node-set?)
 * The namespace-uri function returns a string containing the
 * namespace URI of the expanded name of the node in the argument
 * node-set that is first in document order. If the node-set is empty,
 * the first node has no name, or the expanded name has no namespace
 * URI, an empty string is returned. If the argument is omitted it
 * defaults to the context node.
 */
void
xmlXPathNamespaceURIFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    xmlXPathObjectPtr cur;

    if (nargs == 0) {
        valuePush(ctxt, xmlXPathNewNodeSet(ctxt->context->node));
	nargs = 1;
    }
    CHECK_ARITY(1);
    if ((ctxt->value == NULL) || 
	((ctxt->value->type != XPATH_NODESET) &&
	 (ctxt->value->type != XPATH_XSLT_TREE)))
	XP_ERROR(XPATH_INVALID_TYPE);
    cur = valuePop(ctxt);

    if ((cur->nodesetval == NULL) || (cur->nodesetval->nodeNr == 0)) {
	valuePush(ctxt, xmlXPathNewCString(""));
    } else {
	int i = 0; /* Should be first in document order !!!!! */
	switch (cur->nodesetval->nodeTab[i]->type) {
	case XML_ELEMENT_NODE:
	case XML_ATTRIBUTE_NODE:
	    if (cur->nodesetval->nodeTab[i]->ns == NULL)
		valuePush(ctxt, xmlXPathNewCString(""));
	    else
		valuePush(ctxt, xmlXPathNewString(
			  cur->nodesetval->nodeTab[i]->ns->href));
	    break;
	default:
	    valuePush(ctxt, xmlXPathNewCString(""));
	}
    }
    xmlXPathFreeObject(cur);
}

/**
 * xmlXPathNameFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the name() XPath function
 *    string name(node-set?)
 * The name function returns a string containing a QName representing
 * the name of the node in the argument node-set that is first in document
 * order. The QName must represent the name with respect to the namespace
 * declarations in effect on the node whose name is being represented.
 * Typically, this will be the form in which the name occurred in the XML
 * source. This need not be the case if there are namespace declarations
 * in effect on the node that associate multiple prefixes with the same
 * namespace. However, an implementation may include information about
 * the original prefix in its representation of nodes; in this case, an
 * implementation can ensure that the returned string is always the same
 * as the QName used in the XML source. If the argument it omitted it
 * defaults to the context node.
 * Libxml keep the original prefix so the "real qualified name" used is
 * returned.
 */
static void
xmlXPathNameFunction(xmlXPathParserContextPtr ctxt, int nargs)
{
    xmlXPathObjectPtr cur;

    if (nargs == 0) {
        valuePush(ctxt, xmlXPathNewNodeSet(ctxt->context->node));
        nargs = 1;
    }

    CHECK_ARITY(1);
    if ((ctxt->value == NULL) ||
        ((ctxt->value->type != XPATH_NODESET) &&
         (ctxt->value->type != XPATH_XSLT_TREE)))
        XP_ERROR(XPATH_INVALID_TYPE);
    cur = valuePop(ctxt);

    if ((cur->nodesetval == NULL) || (cur->nodesetval->nodeNr == 0)) {
        valuePush(ctxt, xmlXPathNewCString(""));
    } else {
        int i = 0;              /* Should be first in document order !!!!! */

        switch (cur->nodesetval->nodeTab[i]->type) {
            case XML_ELEMENT_NODE:
            case XML_ATTRIBUTE_NODE:
                if ((cur->nodesetval->nodeTab[i]->ns == NULL) ||
                    (cur->nodesetval->nodeTab[i]->ns->prefix == NULL))
                    valuePush(ctxt,
                              xmlXPathNewString(cur->nodesetval->
                                                nodeTab[i]->name));

                else {
                    char name[2000];

                    snprintf(name, sizeof(name), "%s:%s",
                             (char *) cur->nodesetval->nodeTab[i]->ns->
                             prefix,
                             (char *) cur->nodesetval->nodeTab[i]->name);
                    name[sizeof(name) - 1] = 0;
                    valuePush(ctxt, xmlXPathNewCString(name));
                }
                break;
            default:
                valuePush(ctxt,
                          xmlXPathNewNodeSet(cur->nodesetval->nodeTab[i]));
                xmlXPathLocalNameFunction(ctxt, 1);
        }
    }
    xmlXPathFreeObject(cur);
}


/**
 * xmlXPathStringFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the string() XPath function
 *    string string(object?)
 * he string function converts an object to a string as follows:
 *    - A node-set is converted to a string by returning the value of
 *      the node in the node-set that is first in document order.
 *      If the node-set is empty, an empty string is returned.
 *    - A number is converted to a string as follows
 *      + NaN is converted to the string NaN 
 *      + positive zero is converted to the string 0 
 *      + negative zero is converted to the string 0 
 *      + positive infinity is converted to the string Infinity 
 *      + negative infinity is converted to the string -Infinity 
 *      + if the number is an integer, the number is represented in
 *        decimal form as a Number with no decimal point and no leading
 *        zeros, preceded by a minus sign (-) if the number is negative
 *      + otherwise, the number is represented in decimal form as a
 *        Number including a decimal point with at least one digit
 *        before the decimal point and at least one digit after the
 *        decimal point, preceded by a minus sign (-) if the number
 *        is negative; there must be no leading zeros before the decimal
 *        point apart possibly from the one required digit immediately
 *        before the decimal point; beyond the one required digit
 *        after the decimal point there must be as many, but only as
 *        many, more digits as are needed to uniquely distinguish the
 *        number from all other IEEE 754 numeric values.
 *    - The boolean false value is converted to the string false.
 *      The boolean true value is converted to the string true.
 *
 * If the argument is omitted, it defaults to a node-set with the
 * context node as its only member.
 */
void
xmlXPathStringFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    xmlXPathObjectPtr cur;

    if (nargs == 0) {
	valuePush(ctxt,
		  xmlXPathWrapString(
			xmlXPathCastNodeToString(ctxt->context->node)));
	return;
    }

    CHECK_ARITY(1);
    cur = valuePop(ctxt);
    if (cur == NULL) XP_ERROR(XPATH_INVALID_OPERAND);
    cur = xmlXPathConvertString(cur);
    valuePush(ctxt, cur);
}

/**
 * xmlXPathStringLengthFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the string-length() XPath function
 *    number string-length(string?)
 * The string-length returns the number of characters in the string
 * (see [3.6 Strings]). If the argument is omitted, it defaults to
 * the context node converted to a string, in other words the value
 * of the context node.
 */
void
xmlXPathStringLengthFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    xmlXPathObjectPtr cur;

    if (nargs == 0) {
	if (ctxt->context->node == NULL) {
	    valuePush(ctxt, xmlXPathNewFloat(0));
	} else {
	    xmlChar *content;

	    content = xmlXPathCastNodeToString(ctxt->context->node);
	    valuePush(ctxt, xmlXPathNewFloat(xmlUTF8Strlen(content)));
	    xmlFree(content);
	}
	return;
    }
    CHECK_ARITY(1);
    CAST_TO_STRING;
    CHECK_TYPE(XPATH_STRING);
    cur = valuePop(ctxt);
    valuePush(ctxt, xmlXPathNewFloat(xmlUTF8Strlen(cur->stringval)));
    xmlXPathFreeObject(cur);
}

/**
 * xmlXPathConcatFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the concat() XPath function
 *    string concat(string, string, string*)
 * The concat function returns the concatenation of its arguments.
 */
void
xmlXPathConcatFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    xmlXPathObjectPtr cur, newobj;
    xmlChar *tmp;

    if (nargs < 2) {
	CHECK_ARITY(2);
    }

    CAST_TO_STRING;
    cur = valuePop(ctxt);
    if ((cur == NULL) || (cur->type != XPATH_STRING)) {
        xmlXPathFreeObject(cur);
	return;
    }
    nargs--;

    while (nargs > 0) {
	CAST_TO_STRING;
	newobj = valuePop(ctxt);
	if ((newobj == NULL) || (newobj->type != XPATH_STRING)) {
	    xmlXPathFreeObject(newobj);
	    xmlXPathFreeObject(cur);
	    XP_ERROR(XPATH_INVALID_TYPE);
	}
	tmp = xmlStrcat(newobj->stringval, cur->stringval);
	newobj->stringval = cur->stringval;
	cur->stringval = tmp;

	xmlXPathFreeObject(newobj);
	nargs--;
    }
    valuePush(ctxt, cur);
}

/**
 * xmlXPathContainsFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the contains() XPath function
 *    boolean contains(string, string)
 * The contains function returns true if the first argument string
 * contains the second argument string, and otherwise returns false.
 */
void
xmlXPathContainsFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    xmlXPathObjectPtr hay, needle;

    CHECK_ARITY(2);
    CAST_TO_STRING;
    CHECK_TYPE(XPATH_STRING);
    needle = valuePop(ctxt);
    CAST_TO_STRING;
    hay = valuePop(ctxt);
    if ((hay == NULL) || (hay->type != XPATH_STRING)) {
        xmlXPathFreeObject(hay);
        xmlXPathFreeObject(needle);
	XP_ERROR(XPATH_INVALID_TYPE);
    }
    if (xmlStrstr(hay->stringval, needle->stringval))
        valuePush(ctxt, xmlXPathNewBoolean(1));
    else
        valuePush(ctxt, xmlXPathNewBoolean(0));
    xmlXPathFreeObject(hay);
    xmlXPathFreeObject(needle);
}

/**
 * xmlXPathStartsWithFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the starts-with() XPath function
 *    boolean starts-with(string, string)
 * The starts-with function returns true if the first argument string
 * starts with the second argument string, and otherwise returns false.
 */
void
xmlXPathStartsWithFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    xmlXPathObjectPtr hay, needle;
    int n;

    CHECK_ARITY(2);
    CAST_TO_STRING;
    CHECK_TYPE(XPATH_STRING);
    needle = valuePop(ctxt);
    CAST_TO_STRING;
    hay = valuePop(ctxt);
    if ((hay == NULL) || (hay->type != XPATH_STRING)) {
        xmlXPathFreeObject(hay);
        xmlXPathFreeObject(needle);
	XP_ERROR(XPATH_INVALID_TYPE);
    }
    n = xmlStrlen(needle->stringval);
    if (xmlStrncmp(hay->stringval, needle->stringval, n))
        valuePush(ctxt, xmlXPathNewBoolean(0));
    else
        valuePush(ctxt, xmlXPathNewBoolean(1));
    xmlXPathFreeObject(hay);
    xmlXPathFreeObject(needle);
}

/**
 * xmlXPathSubstringFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the substring() XPath function
 *    string substring(string, number, number?)
 * The substring function returns the substring of the first argument
 * starting at the position specified in the second argument with
 * length specified in the third argument. For example,
 * substring("12345",2,3) returns "234". If the third argument is not
 * specified, it returns the substring starting at the position specified
 * in the second argument and continuing to the end of the string. For
 * example, substring("12345",2) returns "2345".  More precisely, each
 * character in the string (see [3.6 Strings]) is considered to have a
 * numeric position: the position of the first character is 1, the position
 * of the second character is 2 and so on. The returned substring contains
 * those characters for which the position of the character is greater than
 * or equal to the second argument and, if the third argument is specified,
 * less than the sum of the second and third arguments; the comparisons
 * and addition used for the above follow the standard IEEE 754 rules. Thus:
 *  - substring("12345", 1.5, 2.6) returns "234" 
 *  - substring("12345", 0, 3) returns "12" 
 *  - substring("12345", 0 div 0, 3) returns "" 
 *  - substring("12345", 1, 0 div 0) returns "" 
 *  - substring("12345", -42, 1 div 0) returns "12345" 
 *  - substring("12345", -1 div 0, 1 div 0) returns "" 
 */
void
xmlXPathSubstringFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    xmlXPathObjectPtr str, start, len;
    double le=0, in;
    int i, l, m;
    xmlChar *ret;

    if (nargs < 2) {
	CHECK_ARITY(2);
    }
    if (nargs > 3) {
	CHECK_ARITY(3);
    }
    /*
     * take care of possible last (position) argument
    */
    if (nargs == 3) {
	CAST_TO_NUMBER;
	CHECK_TYPE(XPATH_NUMBER);
	len = valuePop(ctxt);
	le = len->floatval;
        xmlXPathFreeObject(len);
    }

    CAST_TO_NUMBER;
    CHECK_TYPE(XPATH_NUMBER);
    start = valuePop(ctxt);
    in = start->floatval;
    xmlXPathFreeObject(start);
    CAST_TO_STRING;
    CHECK_TYPE(XPATH_STRING);
    str = valuePop(ctxt);
    m = xmlUTF8Strlen((const unsigned char *)str->stringval);

    /*
     * If last pos not present, calculate last position
    */
    if (nargs != 3) {
	le = (double)m;
	if (in < 1.0) 
	    in = 1.0;
    }

    /* Need to check for the special cases where either 
     * the index is NaN, the length is NaN, or both
     * arguments are infinity (relying on Inf + -Inf = NaN)
     */
    if (!xmlXPathIsNaN(in + le) && !xmlXPathIsInf(in)) {
        /*
         * To meet the requirements of the spec, the arguments
	 * must be converted to integer format before 
	 * initial index calculations are done
         *
         * First we go to integer form, rounding up
	 * and checking for special cases
         */
        i = (int) in;
        if (((double)i)+0.5 <= in) i++;

	if (xmlXPathIsInf(le) == 1) {
	    l = m;
	    if (i < 1)
		i = 1;
	}
	else if (xmlXPathIsInf(le) == -1 || le < 0.0)
	    l = 0;
	else {
	    l = (int) le;
	    if (((double)l)+0.5 <= le) l++;
	}

	/* Now we normalize inidices */
        i -= 1;
        l += i;
        if (i < 0)
            i = 0;
        if (l > m)
            l = m;

        /* number of chars to copy */
        l -= i;

        ret = xmlUTF8Strsub(str->stringval, i, l);
    }
    else {
        ret = NULL;
    }

    if (ret == NULL)
	valuePush(ctxt, xmlXPathNewCString(""));
    else {
	valuePush(ctxt, xmlXPathNewString(ret));
	xmlFree(ret);
    }

    xmlXPathFreeObject(str);
}

/**
 * xmlXPathSubstringBeforeFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the substring-before() XPath function
 *    string substring-before(string, string)
 * The substring-before function returns the substring of the first
 * argument string that precedes the first occurrence of the second
 * argument string in the first argument string, or the empty string
 * if the first argument string does not contain the second argument
 * string. For example, substring-before("1999/04/01","/") returns 1999.
 */
void
xmlXPathSubstringBeforeFunction(xmlXPathParserContextPtr ctxt, int nargs) {
  xmlXPathObjectPtr str;
  xmlXPathObjectPtr find;
  xmlBufferPtr target;
  const xmlChar *point;
  int offset;
  
  CHECK_ARITY(2);
  CAST_TO_STRING;
  find = valuePop(ctxt);
  CAST_TO_STRING;
  str = valuePop(ctxt);
  
  target = xmlBufferCreate();
  if (target) {
    point = xmlStrstr(str->stringval, find->stringval);
    if (point) {
      offset = (int)(point - str->stringval);
      xmlBufferAdd(target, str->stringval, offset);
    }
    valuePush(ctxt, xmlXPathNewString(xmlBufferContent(target)));
    xmlBufferFree(target);
  }
  
  xmlXPathFreeObject(str);
  xmlXPathFreeObject(find);
}

/**
 * xmlXPathSubstringAfterFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the substring-after() XPath function
 *    string substring-after(string, string)
 * The substring-after function returns the substring of the first
 * argument string that follows the first occurrence of the second
 * argument string in the first argument string, or the empty stringi
 * if the first argument string does not contain the second argument
 * string. For example, substring-after("1999/04/01","/") returns 04/01,
 * and substring-after("1999/04/01","19") returns 99/04/01.
 */
void
xmlXPathSubstringAfterFunction(xmlXPathParserContextPtr ctxt, int nargs) {
  xmlXPathObjectPtr str;
  xmlXPathObjectPtr find;
  xmlBufferPtr target;
  const xmlChar *point;
  int offset;
  
  CHECK_ARITY(2);
  CAST_TO_STRING;
  find = valuePop(ctxt);
  CAST_TO_STRING;
  str = valuePop(ctxt);
  
  target = xmlBufferCreate();
  if (target) {
    point = xmlStrstr(str->stringval, find->stringval);
    if (point) {
      offset = (int)(point - str->stringval) + xmlStrlen(find->stringval);
      xmlBufferAdd(target, &str->stringval[offset],
		   xmlStrlen(str->stringval) - offset);
    }
    valuePush(ctxt, xmlXPathNewString(xmlBufferContent(target)));
    xmlBufferFree(target);
  }
  
  xmlXPathFreeObject(str);
  xmlXPathFreeObject(find);
}

/**
 * xmlXPathNormalizeFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the normalize-space() XPath function
 *    string normalize-space(string?)
 * The normalize-space function returns the argument string with white
 * space normalized by stripping leading and trailing whitespace
 * and replacing sequences of whitespace characters by a single
 * space. Whitespace characters are the same allowed by the S production
 * in XML. If the argument is omitted, it defaults to the context
 * node converted to a string, in other words the value of the context node.
 */
void
xmlXPathNormalizeFunction(xmlXPathParserContextPtr ctxt, int nargs) {
  xmlXPathObjectPtr obj = NULL;
  xmlChar *source = NULL;
  xmlBufferPtr target;
  xmlChar blank;
  
  if (nargs == 0) {
    /* Use current context node */
    valuePush(ctxt,
	      xmlXPathWrapString(
		  xmlXPathCastNodeToString(ctxt->context->node)));
    nargs = 1;
  }

  CHECK_ARITY(1);
  CAST_TO_STRING;
  CHECK_TYPE(XPATH_STRING);
  obj = valuePop(ctxt);
  source = obj->stringval;

  target = xmlBufferCreate();
  if (target && source) {
    
    /* Skip leading whitespaces */
    while (IS_BLANK(*source))
      source++;
  
    /* Collapse intermediate whitespaces, and skip trailing whitespaces */
    blank = 0;
    while (*source) {
      if (IS_BLANK(*source)) {
	blank = 0x20;
      } else {
	if (blank) {
	  xmlBufferAdd(target, &blank, 1);
	  blank = 0;
	}
	xmlBufferAdd(target, source, 1);
      }
      source++;
    }
  
    valuePush(ctxt, xmlXPathNewString(xmlBufferContent(target)));
    xmlBufferFree(target);
  }
  xmlXPathFreeObject(obj);
}

/**
 * xmlXPathTranslateFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the translate() XPath function
 *    string translate(string, string, string)
 * The translate function returns the first argument string with
 * occurrences of characters in the second argument string replaced
 * by the character at the corresponding position in the third argument
 * string. For example, translate("bar","abc","ABC") returns the string
 * BAr. If there is a character in the second argument string with no
 * character at a corresponding position in the third argument string
 * (because the second argument string is longer than the third argument
 * string), then occurrences of that character in the first argument
 * string are removed. For example, translate("--aaa--","abc-","ABC")
 * returns "AAA". If a character occurs more than once in second
 * argument string, then the first occurrence determines the replacement
 * character. If the third argument string is longer than the second
 * argument string, then excess characters are ignored.
 */
void
xmlXPathTranslateFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    xmlXPathObjectPtr str;
    xmlXPathObjectPtr from;
    xmlXPathObjectPtr to;
    xmlBufferPtr target;
    int offset, max;
    xmlChar ch;
    xmlChar *point;
    xmlChar *cptr;

    CHECK_ARITY(3);

    CAST_TO_STRING;
    to = valuePop(ctxt);
    CAST_TO_STRING;
    from = valuePop(ctxt);
    CAST_TO_STRING;
    str = valuePop(ctxt);

    target = xmlBufferCreate();
    if (target) {
	max = xmlUTF8Strlen(to->stringval);
	for (cptr = str->stringval; (ch=*cptr); ) {
	    offset = xmlUTF8Strloc(from->stringval, cptr);
	    if (offset >= 0) {
		if (offset < max) {
		    point = xmlUTF8Strpos(to->stringval, offset);
		    if (point)
			xmlBufferAdd(target, point, xmlUTF8Strsize(point, 1));
		}
	    } else
		xmlBufferAdd(target, cptr, xmlUTF8Strsize(cptr, 1));

	    /* Step to next character in input */
	    cptr++;
	    if ( ch & 0x80 ) {
		/* if not simple ascii, verify proper format */
		if ( (ch & 0xc0) != 0xc0 ) {
		    xmlGenericError(xmlGenericErrorContext,
			"xmlXPathTranslateFunction: Invalid UTF8 string\n");
		    break;
		}
		/* then skip over remaining bytes for this char */
		while ( (ch <<= 1) & 0x80 )
		    if ( (*cptr++ & 0xc0) != 0x80 ) {
			xmlGenericError(xmlGenericErrorContext,
			    "xmlXPathTranslateFunction: Invalid UTF8 string\n");
			break;
		    }
		if (ch & 0x80) /* must have had error encountered */
		    break;
	    }
	}
    }
    valuePush(ctxt, xmlXPathNewString(xmlBufferContent(target)));
    xmlBufferFree(target);
    xmlXPathFreeObject(str);
    xmlXPathFreeObject(from);
    xmlXPathFreeObject(to);
}

/**
 * xmlXPathBooleanFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the boolean() XPath function
 *    boolean boolean(object)
 * he boolean function converts its argument to a boolean as follows:
 *    - a number is true if and only if it is neither positive or
 *      negative zero nor NaN
 *    - a node-set is true if and only if it is non-empty
 *    - a string is true if and only if its length is non-zero
 */
void
xmlXPathBooleanFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    xmlXPathObjectPtr cur;

    CHECK_ARITY(1);
    cur = valuePop(ctxt);
    if (cur == NULL) XP_ERROR(XPATH_INVALID_OPERAND);
    cur = xmlXPathConvertBoolean(cur);
    valuePush(ctxt, cur);
}

/**
 * xmlXPathNotFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the not() XPath function
 *    boolean not(boolean)
 * The not function returns true if its argument is false,
 * and false otherwise.
 */
void
xmlXPathNotFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    CHECK_ARITY(1);
    CAST_TO_BOOLEAN;
    CHECK_TYPE(XPATH_BOOLEAN);
    ctxt->value->boolval = ! ctxt->value->boolval;
}

/**
 * xmlXPathTrueFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the true() XPath function
 *    boolean true()
 */
void
xmlXPathTrueFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    CHECK_ARITY(0);
    valuePush(ctxt, xmlXPathNewBoolean(1));
}

/**
 * xmlXPathFalseFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the false() XPath function
 *    boolean false()
 */
void
xmlXPathFalseFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    CHECK_ARITY(0);
    valuePush(ctxt, xmlXPathNewBoolean(0));
}

/**
 * xmlXPathLangFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the lang() XPath function
 *    boolean lang(string)
 * The lang function returns true or false depending on whether the
 * language of the context node as specified by xml:lang attributes
 * is the same as or is a sublanguage of the language specified by
 * the argument string. The language of the context node is determined
 * by the value of the xml:lang attribute on the context node, or, if
 * the context node has no xml:lang attribute, by the value of the
 * xml:lang attribute on the nearest ancestor of the context node that
 * has an xml:lang attribute. If there is no such attribute, then lang
 * returns false. If there is such an attribute, then lang returns
 * true if the attribute value is equal to the argument ignoring case,
 * or if there is some suffix starting with - such that the attribute
 * value is equal to the argument ignoring that suffix of the attribute
 * value and ignoring case.
 */
void
xmlXPathLangFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    xmlXPathObjectPtr val;
    const xmlChar *theLang;
    const xmlChar *lang;
    int ret = 0;
    int i;

    CHECK_ARITY(1);
    CAST_TO_STRING;
    CHECK_TYPE(XPATH_STRING);
    val = valuePop(ctxt);
    lang = val->stringval;
    theLang = xmlNodeGetLang(ctxt->context->node);
    if ((theLang != NULL) && (lang != NULL)) {
        for (i = 0;lang[i] != 0;i++)
	    if (toupper(lang[i]) != toupper(theLang[i]))
	        goto not_equal;
        ret = 1;
    }
not_equal:
    xmlXPathFreeObject(val);
    valuePush(ctxt, xmlXPathNewBoolean(ret));
}

/**
 * xmlXPathNumberFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the number() XPath function
 *    number number(object?)
 */
void
xmlXPathNumberFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    xmlXPathObjectPtr cur;
    double res;

    if (nargs == 0) {
	if (ctxt->context->node == NULL) {
	    valuePush(ctxt, xmlXPathNewFloat(0.0));
	} else {
	    xmlChar* content = xmlNodeGetContent(ctxt->context->node);

	    res = xmlXPathStringEvalNumber(content);
	    valuePush(ctxt, xmlXPathNewFloat(res));
	    xmlFree(content);
	}
	return;
    }

    CHECK_ARITY(1);
    cur = valuePop(ctxt);
    cur = xmlXPathConvertNumber(cur);
    valuePush(ctxt, cur);
}

/**
 * xmlXPathSumFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the sum() XPath function
 *    number sum(node-set)
 * The sum function returns the sum of the values of the nodes in
 * the argument node-set.
 */
void
xmlXPathSumFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    xmlXPathObjectPtr cur;
    int i;
    double res = 0.0;

    CHECK_ARITY(1);
    if ((ctxt->value == NULL) || 
	((ctxt->value->type != XPATH_NODESET) &&
	 (ctxt->value->type != XPATH_XSLT_TREE)))
	XP_ERROR(XPATH_INVALID_TYPE);
    cur = valuePop(ctxt);

    if ((cur->nodesetval == NULL) || (cur->nodesetval->nodeNr == 0)) {
	valuePush(ctxt, xmlXPathNewFloat(0.0));
    } else {
	for (i = 0; i < cur->nodesetval->nodeNr; i++) {
	    res += xmlXPathCastNodeToNumber(cur->nodesetval->nodeTab[i]);
	}
	valuePush(ctxt, xmlXPathNewFloat(res));
    }
    xmlXPathFreeObject(cur);
}

/**
 * xmlXPathFloorFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the floor() XPath function
 *    number floor(number)
 * The floor function returns the largest (closest to positive infinity)
 * number that is not greater than the argument and that is an integer.
 */
void
xmlXPathFloorFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    double f;

    CHECK_ARITY(1);
    CAST_TO_NUMBER;
    CHECK_TYPE(XPATH_NUMBER);

    f = (double)((int) ctxt->value->floatval);
    if (f != ctxt->value->floatval) {
	if (ctxt->value->floatval > 0)
	    ctxt->value->floatval = f;
	else
	    ctxt->value->floatval = f - 1;
    }
}

/**
 * xmlXPathCeilingFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the ceiling() XPath function
 *    number ceiling(number)
 * The ceiling function returns the smallest (closest to negative infinity)
 * number that is not less than the argument and that is an integer.
 */
void
xmlXPathCeilingFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    double f;

    CHECK_ARITY(1);
    CAST_TO_NUMBER;
    CHECK_TYPE(XPATH_NUMBER);

#if 0
    ctxt->value->floatval = ceil(ctxt->value->floatval);
#else
    f = (double)((int) ctxt->value->floatval);
    if (f != ctxt->value->floatval) {
	if (ctxt->value->floatval > 0)
	    ctxt->value->floatval = f + 1;
	else {
	    if (ctxt->value->floatval < 0 && f == 0)
	        ctxt->value->floatval = xmlXPathNZERO;
	    else
	        ctxt->value->floatval = f;
	}

    }
#endif
}

/**
 * xmlXPathRoundFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the round() XPath function
 *    number round(number)
 * The round function returns the number that is closest to the
 * argument and that is an integer. If there are two such numbers,
 * then the one that is even is returned.
 */
void
xmlXPathRoundFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    double f;

    CHECK_ARITY(1);
    CAST_TO_NUMBER;
    CHECK_TYPE(XPATH_NUMBER);

    if ((xmlXPathIsNaN(ctxt->value->floatval)) ||
	(xmlXPathIsInf(ctxt->value->floatval) == 1) ||
	(xmlXPathIsInf(ctxt->value->floatval) == -1) ||
	(ctxt->value->floatval == 0.0))
	return;

    f = (double)((int) ctxt->value->floatval);
    if (ctxt->value->floatval < 0) {
	if (ctxt->value->floatval < f - 0.5)
	    ctxt->value->floatval = f - 1;
	else 
	    ctxt->value->floatval = f;
	if (ctxt->value->floatval == 0)
	    ctxt->value->floatval = xmlXPathNZERO;
    } else {
	if (ctxt->value->floatval < f + 0.5)
	    ctxt->value->floatval = f;
	else 
	    ctxt->value->floatval = f + 1;
    }
}

/************************************************************************
 *									*
 *			The Parser					*
 *									*
 ************************************************************************/

/*
 * a couple of forward declarations since we use a recursive call based
 * implementation.
 */
static void xmlXPathCompileExpr(xmlXPathParserContextPtr ctxt);
static void xmlXPathCompPredicate(xmlXPathParserContextPtr ctxt, int filter);
static void xmlXPathCompLocationPath(xmlXPathParserContextPtr ctxt);
static void xmlXPathCompRelativeLocationPath(xmlXPathParserContextPtr ctxt);
static xmlChar * xmlXPathParseNameComplex(xmlXPathParserContextPtr ctxt,
	                                  int qualified);

/**
 * xmlXPathCurrentChar:
 * @ctxt:  the XPath parser context
 * @cur:  pointer to the beginning of the char
 * @len:  pointer to the length of the char read
 *
 * The current char value, if using UTF-8 this may actually span multiple
 * bytes in the input buffer.
 *
 * Returns the current char value and its length
 */

static int
xmlXPathCurrentChar(xmlXPathParserContextPtr ctxt, int *len) {
    unsigned char c;
    unsigned int val;
    const xmlChar *cur;

    if (ctxt == NULL)
	return(0);
    cur = ctxt->cur;

    /*
     * We are supposed to handle UTF8, check it's valid
     * From rfc2044: encoding of the Unicode values on UTF-8:
     *
     * UCS-4 range (hex.)           UTF-8 octet sequence (binary)
     * 0000 0000-0000 007F   0xxxxxxx
     * 0000 0080-0000 07FF   110xxxxx 10xxxxxx
     * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx 
     *
     * Check for the 0x110000 limit too
     */
    c = *cur;
    if (c & 0x80) {
	if ((cur[1] & 0xc0) != 0x80)
	    goto encoding_error;
	if ((c & 0xe0) == 0xe0) {

	    if ((cur[2] & 0xc0) != 0x80)
		goto encoding_error;
	    if ((c & 0xf0) == 0xf0) {
		if (((c & 0xf8) != 0xf0) ||
		    ((cur[3] & 0xc0) != 0x80))
		    goto encoding_error;
		/* 4-byte code */
		*len = 4;
		val = (cur[0] & 0x7) << 18;
		val |= (cur[1] & 0x3f) << 12;
		val |= (cur[2] & 0x3f) << 6;
		val |= cur[3] & 0x3f;
	    } else {
	      /* 3-byte code */
		*len = 3;
		val = (cur[0] & 0xf) << 12;
		val |= (cur[1] & 0x3f) << 6;
		val |= cur[2] & 0x3f;
	    }
	} else {
	  /* 2-byte code */
	    *len = 2;
	    val = (cur[0] & 0x1f) << 6;
	    val |= cur[1] & 0x3f;
	}
	if (!IS_CHAR(val)) {
	    XP_ERROR0(XPATH_INVALID_CHAR_ERROR);
	}    
	return(val);
    } else {
	/* 1-byte code */
	*len = 1;
	return((int) *cur);
    }
encoding_error:
    /*
     * If we detect an UTF8 error that probably mean that the
     * input encoding didn't get properly advertized in the
     * declaration header. Report the error and switch the encoding
     * to ISO-Latin-1 (if you don't like this policy, just declare the
     * encoding !)
     */
    *len = 0;
    XP_ERROR0(XPATH_ENCODING_ERROR);
}

/**
 * xmlXPathParseNCName:
 * @ctxt:  the XPath Parser context
 *
 * parse an XML namespace non qualified name.
 *
 * [NS 3] NCName ::= (Letter | '_') (NCNameChar)*
 *
 * [NS 4] NCNameChar ::= Letter | Digit | '.' | '-' | '_' |
 *                       CombiningChar | Extender
 *
 * Returns the namespace name or NULL
 */

xmlChar *
xmlXPathParseNCName(xmlXPathParserContextPtr ctxt) {
    const xmlChar *in;
    xmlChar *ret;
    int count = 0;

    /*
     * Accelerator for simple ASCII names
     */
    in = ctxt->cur;
    if (((*in >= 0x61) && (*in <= 0x7A)) ||
	((*in >= 0x41) && (*in <= 0x5A)) ||
	(*in == '_')) {
	in++;
	while (((*in >= 0x61) && (*in <= 0x7A)) ||
	       ((*in >= 0x41) && (*in <= 0x5A)) ||
	       ((*in >= 0x30) && (*in <= 0x39)) ||
	       (*in == '_') || (*in == '.') ||
	       (*in == '-'))
	    in++;
	if ((*in == ' ') || (*in == '>') || (*in == '/') ||
            (*in == '[') || (*in == ']') || (*in == ':') ||
            (*in == '@') || (*in == '*')) {
	    count = in - ctxt->cur;
	    if (count == 0)
		return(NULL);
	    ret = xmlStrndup(ctxt->cur, count);
	    ctxt->cur = in;
	    return(ret);
	}
    }
    return(xmlXPathParseNameComplex(ctxt, 0));
}


/**
 * xmlXPathParseQName:
 * @ctxt:  the XPath Parser context
 * @prefix:  a xmlChar ** 
 *
 * parse an XML qualified name
 *
 * [NS 5] QName ::= (Prefix ':')? LocalPart
 *
 * [NS 6] Prefix ::= NCName
 *
 * [NS 7] LocalPart ::= NCName
 *
 * Returns the function returns the local part, and prefix is updated
 *   to get the Prefix if any.
 */

static xmlChar *
xmlXPathParseQName(xmlXPathParserContextPtr ctxt, xmlChar **prefix) {
    xmlChar *ret = NULL;

    *prefix = NULL;
    ret = xmlXPathParseNCName(ctxt);
    if (CUR == ':') {
        *prefix = ret;
	NEXT;
	ret = xmlXPathParseNCName(ctxt);
    }
    return(ret);
}

/**
 * xmlXPathParseName:
 * @ctxt:  the XPath Parser context
 *
 * parse an XML name
 *
 * [4] NameChar ::= Letter | Digit | '.' | '-' | '_' | ':' |
 *                  CombiningChar | Extender
 *
 * [5] Name ::= (Letter | '_' | ':') (NameChar)*
 *
 * Returns the namespace name or NULL
 */

xmlChar *
xmlXPathParseName(xmlXPathParserContextPtr ctxt) {
    const xmlChar *in;
    xmlChar *ret;
    int count = 0;

    /*
     * Accelerator for simple ASCII names
     */
    in = ctxt->cur;
    if (((*in >= 0x61) && (*in <= 0x7A)) ||
	((*in >= 0x41) && (*in <= 0x5A)) ||
	(*in == '_') || (*in == ':')) {
	in++;
	while (((*in >= 0x61) && (*in <= 0x7A)) ||
	       ((*in >= 0x41) && (*in <= 0x5A)) ||
	       ((*in >= 0x30) && (*in <= 0x39)) ||
	       (*in == '_') || (*in == '-') ||
	       (*in == ':') || (*in == '.'))
	    in++;
	if ((*in > 0) && (*in < 0x80)) {
	    count = in - ctxt->cur;
	    ret = xmlStrndup(ctxt->cur, count);
	    ctxt->cur = in;
	    return(ret);
	}
    }
    return(xmlXPathParseNameComplex(ctxt, 1));
}

static xmlChar *
xmlXPathParseNameComplex(xmlXPathParserContextPtr ctxt, int qualified) {
    xmlChar buf[XML_MAX_NAMELEN + 5];
    int len = 0, l;
    int c;

    /*
     * Handler for more complex cases
     */
    c = CUR_CHAR(l);
    if ((c == ' ') || (c == '>') || (c == '/') || /* accelerators */
        (c == '[') || (c == ']') || (c == '@') || /* accelerators */
        (c == '*') || /* accelerators */
	(!IS_LETTER(c) && (c != '_') &&
         ((qualified) && (c != ':')))) {
	return(NULL);
    }

    while ((c != ' ') && (c != '>') && (c != '/') && /* test bigname.xml */
	   ((IS_LETTER(c)) || (IS_DIGIT(c)) ||
            (c == '.') || (c == '-') ||
	    (c == '_') || ((qualified) && (c == ':')) || 
	    (IS_COMBINING(c)) ||
	    (IS_EXTENDER(c)))) {
	COPY_BUF(l,buf,len,c);
	NEXTL(l);
	c = CUR_CHAR(l);
	if (len >= XML_MAX_NAMELEN) {
	    /*
	     * Okay someone managed to make a huge name, so he's ready to pay
	     * for the processing speed.
	     */
	    xmlChar *buffer;
	    int max = len * 2;
	    
	    buffer = (xmlChar *) xmlMalloc(max * sizeof(xmlChar));
	    if (buffer == NULL) {
		XP_ERROR0(XPATH_MEMORY_ERROR);
	    }
	    memcpy(buffer, buf, len);
	    while ((IS_LETTER(c)) || (IS_DIGIT(c)) || /* test bigname.xml */
		   (c == '.') || (c == '-') ||
		   (c == '_') || ((qualified) && (c == ':')) || 
		   (IS_COMBINING(c)) ||
		   (IS_EXTENDER(c))) {
		if (len + 10 > max) {
		    max *= 2;
		    buffer = (xmlChar *) xmlRealloc(buffer,
			                            max * sizeof(xmlChar));
		    if (buffer == NULL) {
			XP_ERROR0(XPATH_MEMORY_ERROR);
		    }
		}
		COPY_BUF(l,buffer,len,c);
		NEXTL(l);
		c = CUR_CHAR(l);
	    }
	    buffer[len] = 0;
	    return(buffer);
	}
    }
    if (len == 0)
	return(NULL);
    return(xmlStrndup(buf, len));
}

#define MAX_FRAC 20

static double my_pow10[MAX_FRAC] = {
    1.0, 10.0, 100.0, 1000.0, 10000.0,
    100000.0, 1000000.0, 10000000.0, 100000000.0, 1000000000.0,
    10000000000.0, 100000000000.0, 1000000000000.0, 10000000000000.0,
    100000000000000.0,
    1000000000000000.0, 10000000000000000.0, 100000000000000000.0,
    1000000000000000000.0, 10000000000000000000.0
};

/**
 * xmlXPathStringEvalNumber:
 * @str:  A string to scan
 *
 *  [30a]  Float  ::= Number ('e' Digits?)?
 *
 *  [30]   Number ::=   Digits ('.' Digits?)?
 *                    | '.' Digits 
 *  [31]   Digits ::=   [0-9]+
 *
 * Compile a Number in the string
 * In complement of the Number expression, this function also handles
 * negative values : '-' Number.
 *
 * Returns the double value.
 */
double
xmlXPathStringEvalNumber(const xmlChar *str) {
    const xmlChar *cur = str;
    double ret;
    int ok = 0;
    int isneg = 0;
    int exponent = 0;
    int is_exponent_negative = 0;
#ifdef __GNUC__
    unsigned long tmp = 0;
    double temp;
#endif
    if (cur == NULL) return(0);
    while (IS_BLANK(*cur)) cur++;
    if ((*cur != '.') && ((*cur < '0') || (*cur > '9')) && (*cur != '-')) {
        return(xmlXPathNAN);
    }
    if (*cur == '-') {
	isneg = 1;
	cur++;
    }

#ifdef __GNUC__
    /*
     * tmp/temp is a workaround against a gcc compiler bug
     * http://veillard.com/gcc.bug
     */
    ret = 0;
    while ((*cur >= '0') && (*cur <= '9')) {
	ret = ret * 10;
	tmp = (*cur - '0');
	ok = 1;
	cur++;
	temp = (double) tmp;
	ret = ret + temp;
    }
#else
    ret = 0;
    while ((*cur >= '0') && (*cur <= '9')) {
	ret = ret * 10 + (*cur - '0');
	ok = 1;
	cur++;
    }
#endif

    if (*cur == '.') {
	int v, frac = 0;
	double fraction = 0;

        cur++;
	if (((*cur < '0') || (*cur > '9')) && (!ok)) {
	    return(xmlXPathNAN);
	}
	while (((*cur >= '0') && (*cur <= '9')) && (frac < MAX_FRAC)) {
	    v = (*cur - '0');
	    fraction = fraction * 10 + v;
	    frac = frac + 1;
	    cur++;
	}
	fraction /= my_pow10[frac];
	ret = ret + fraction;
	while ((*cur >= '0') && (*cur <= '9'))
	    cur++;
    }
    if ((*cur == 'e') || (*cur == 'E')) {
      cur++;
      if (*cur == '-') {
	is_exponent_negative = 1;
	cur++;
      }
      while ((*cur >= '0') && (*cur <= '9')) {
	exponent = exponent * 10 + (*cur - '0');
	cur++;
      }
    }
    while (IS_BLANK(*cur)) cur++;
    if (*cur != 0) return(xmlXPathNAN);
    if (isneg) ret = -ret;
    if (is_exponent_negative) exponent = -exponent;
    ret *= pow(10.0, (double)exponent);
    return(ret);
}

/**
 * xmlXPathCompNumber:
 * @ctxt:  the XPath Parser context
 *
 *  [30]   Number ::=   Digits ('.' Digits?)?
 *                    | '.' Digits 
 *  [31]   Digits ::=   [0-9]+
 *
 * Compile a Number, then push it on the stack
 *
 */
static void
xmlXPathCompNumber(xmlXPathParserContextPtr ctxt)
{
    double ret = 0.0;
    double mult = 1;
    int ok = 0;
    int exponent = 0;
    int is_exponent_negative = 0;
#ifdef __GNUC__
    unsigned long tmp = 0;
    double temp;
#endif

    CHECK_ERROR;
    if ((CUR != '.') && ((CUR < '0') || (CUR > '9'))) {
        XP_ERROR(XPATH_NUMBER_ERROR);
    }
#ifdef __GNUC__
    /*
     * tmp/temp is a workaround against a gcc compiler bug
     * http://veillard.com/gcc.bug
     */
    ret = 0;
    while ((CUR >= '0') && (CUR <= '9')) {
	ret = ret * 10;
	tmp = (CUR - '0');
        ok = 1;
        NEXT;
	temp = (double) tmp;
	ret = ret + temp;
    }
#else
    ret = 0;
    while ((CUR >= '0') && (CUR <= '9')) {
	ret = ret * 10 + (CUR - '0');
	ok = 1;
	NEXT;
    }
#endif
    if (CUR == '.') {
        NEXT;
        if (((CUR < '0') || (CUR > '9')) && (!ok)) {
            XP_ERROR(XPATH_NUMBER_ERROR);
        }
        while ((CUR >= '0') && (CUR <= '9')) {
            mult /= 10;
            ret = ret + (CUR - '0') * mult;
            NEXT;
        }
    }
    if ((CUR == 'e') || (CUR == 'E')) {
        NEXT;
        if (CUR == '-') {
            is_exponent_negative = 1;
            NEXT;
        }
        while ((CUR >= '0') && (CUR <= '9')) {
            exponent = exponent * 10 + (CUR - '0');
            NEXT;
        }
        if (is_exponent_negative)
            exponent = -exponent;
        ret *= pow(10.0, (double) exponent);
    }
    PUSH_LONG_EXPR(XPATH_OP_VALUE, XPATH_NUMBER, 0, 0,
                   xmlXPathNewFloat(ret), NULL);
}

/**
 * xmlXPathParseLiteral:
 * @ctxt:  the XPath Parser context
 *
 * Parse a Literal
 *
 *  [29]   Literal ::=   '"' [^"]* '"'
 *                    | "'" [^']* "'"
 *
 * Returns the value found or NULL in case of error
 */
static xmlChar *
xmlXPathParseLiteral(xmlXPathParserContextPtr ctxt) {
    const xmlChar *q;
    xmlChar *ret = NULL;

    if (CUR == '"') {
        NEXT;
	q = CUR_PTR;
	while ((IS_CHAR(CUR)) && (CUR != '"'))
	    NEXT;
	if (!IS_CHAR(CUR)) {
	    XP_ERROR0(XPATH_UNFINISHED_LITERAL_ERROR);
	} else {
	    ret = xmlStrndup(q, CUR_PTR - q);
	    NEXT;
        }
    } else if (CUR == '\'') {
        NEXT;
	q = CUR_PTR;
	while ((IS_CHAR(CUR)) && (CUR != '\''))
	    NEXT;
	if (!IS_CHAR(CUR)) {
	    XP_ERROR0(XPATH_UNFINISHED_LITERAL_ERROR);
	} else {
	    ret = xmlStrndup(q, CUR_PTR - q);
	    NEXT;
        }
    } else {
	XP_ERROR0(XPATH_START_LITERAL_ERROR);
    }
    return(ret);
}

/**
 * xmlXPathCompLiteral:
 * @ctxt:  the XPath Parser context
 *
 * Parse a Literal and push it on the stack.
 *
 *  [29]   Literal ::=   '"' [^"]* '"'
 *                    | "'" [^']* "'"
 *
 * TODO: xmlXPathCompLiteral memory allocation could be improved.
 */
static void
xmlXPathCompLiteral(xmlXPathParserContextPtr ctxt) {
    const xmlChar *q;
    xmlChar *ret = NULL;

    if (CUR == '"') {
        NEXT;
	q = CUR_PTR;
	while ((IS_CHAR(CUR)) && (CUR != '"'))
	    NEXT;
	if (!IS_CHAR(CUR)) {
	    XP_ERROR(XPATH_UNFINISHED_LITERAL_ERROR);
	} else {
	    ret = xmlStrndup(q, CUR_PTR - q);
	    NEXT;
        }
    } else if (CUR == '\'') {
        NEXT;
	q = CUR_PTR;
	while ((IS_CHAR(CUR)) && (CUR != '\''))
	    NEXT;
	if (!IS_CHAR(CUR)) {
	    XP_ERROR(XPATH_UNFINISHED_LITERAL_ERROR);
	} else {
	    ret = xmlStrndup(q, CUR_PTR - q);
	    NEXT;
        }
    } else {
	XP_ERROR(XPATH_START_LITERAL_ERROR);
    }
    if (ret == NULL) return;
    PUSH_LONG_EXPR(XPATH_OP_VALUE, XPATH_STRING, 0, 0,
	           xmlXPathNewString(ret), NULL);
    xmlFree(ret);
}

/**
 * xmlXPathCompVariableReference:
 * @ctxt:  the XPath Parser context
 *
 * Parse a VariableReference, evaluate it and push it on the stack.
 *
 * The variable bindings consist of a mapping from variable names
 * to variable values. The value of a variable is an object, which
 * of any of the types that are possible for the value of an expression,
 * and may also be of additional types not specified here.
 *
 * Early evaluation is possible since:
 * The variable bindings [...] used to evaluate a subexpression are
 * always the same as those used to evaluate the containing expression. 
 *
 *  [36]   VariableReference ::=   '$' QName 
 */
static void
xmlXPathCompVariableReference(xmlXPathParserContextPtr ctxt) {
    xmlChar *name;
    xmlChar *prefix;

    SKIP_BLANKS;
    if (CUR != '$') {
	XP_ERROR(XPATH_VARIABLE_REF_ERROR);
    }
    NEXT;
    name = xmlXPathParseQName(ctxt, &prefix);
    if (name == NULL) {
	XP_ERROR(XPATH_VARIABLE_REF_ERROR);
    }
    ctxt->comp->last = -1;
    PUSH_LONG_EXPR(XPATH_OP_VARIABLE, 0, 0, 0,
	           name, prefix);
    SKIP_BLANKS;
}

/**
 * xmlXPathIsNodeType:
 * @name:  a name string
 *
 * Is the name given a NodeType one.
 *
 *  [38]   NodeType ::=   'comment'
 *                    | 'text'
 *                    | 'processing-instruction'
 *                    | 'node'
 *
 * Returns 1 if true 0 otherwise
 */
int
xmlXPathIsNodeType(const xmlChar *name) {
    if (name == NULL)
	return(0);

    if (xmlStrEqual(name, BAD_CAST "node"))
	return(1);
    if (xmlStrEqual(name, BAD_CAST "text"))
	return(1);
    if (xmlStrEqual(name, BAD_CAST "comment"))
	return(1);
    if (xmlStrEqual(name, BAD_CAST "processing-instruction"))
	return(1);
    return(0);
}

/**
 * xmlXPathCompFunctionCall:
 * @ctxt:  the XPath Parser context
 *
 *  [16]   FunctionCall ::=   FunctionName '(' ( Argument ( ',' Argument)*)? ')'
 *  [17]   Argument ::=   Expr 
 *
 * Compile a function call, the evaluation of all arguments are
 * pushed on the stack
 */
static void
xmlXPathCompFunctionCall(xmlXPathParserContextPtr ctxt) {
    xmlChar *name;
    xmlChar *prefix;
    int nbargs = 0;

    name = xmlXPathParseQName(ctxt, &prefix);
    if (name == NULL) {
	XP_ERROR(XPATH_EXPR_ERROR);
    }
    SKIP_BLANKS;
#ifdef DEBUG_EXPR
    if (prefix == NULL)
	xmlGenericError(xmlGenericErrorContext, "Calling function %s\n",
			name);
    else
	xmlGenericError(xmlGenericErrorContext, "Calling function %s:%s\n",
			prefix, name);
#endif

    if (CUR != '(') {
	XP_ERROR(XPATH_EXPR_ERROR);
    }
    NEXT;
    SKIP_BLANKS;

    ctxt->comp->last = -1;
    while (CUR != ')') {
	int op1 = ctxt->comp->last;
	ctxt->comp->last = -1;
        xmlXPathCompileExpr(ctxt);
	CHECK_ERROR;
	PUSH_BINARY_EXPR(XPATH_OP_ARG, op1, ctxt->comp->last, 0, 0);
	nbargs++;
	if (CUR == ')') break;
	if (CUR != ',') {
	    XP_ERROR(XPATH_EXPR_ERROR);
	}
	NEXT;
	SKIP_BLANKS;
    }
    PUSH_LONG_EXPR(XPATH_OP_FUNCTION, nbargs, 0, 0,
	           name, prefix);
    NEXT;
    SKIP_BLANKS;
}

/**
 * xmlXPathCompPrimaryExpr:
 * @ctxt:  the XPath Parser context
 *
 *  [15]   PrimaryExpr ::=   VariableReference 
 *                | '(' Expr ')'
 *                | Literal 
 *                | Number 
 *                | FunctionCall 
 *
 * Compile a primary expression.
 */
static void
xmlXPathCompPrimaryExpr(xmlXPathParserContextPtr ctxt) {
    SKIP_BLANKS;
    if (CUR == '$') xmlXPathCompVariableReference(ctxt);
    else if (CUR == '(') {
	NEXT;
	SKIP_BLANKS;
	xmlXPathCompileExpr(ctxt);
	CHECK_ERROR;
	if (CUR != ')') {
	    XP_ERROR(XPATH_EXPR_ERROR);
	}
	NEXT;
	SKIP_BLANKS;
    } else if (IS_DIGIT(CUR) || (CUR == '.' && IS_DIGIT(NXT(1)))) {
	xmlXPathCompNumber(ctxt);
    } else if ((CUR == '\'') || (CUR == '"')) {
	xmlXPathCompLiteral(ctxt);
    } else {
	xmlXPathCompFunctionCall(ctxt);
    }
    SKIP_BLANKS;
}

/**
 * xmlXPathCompFilterExpr:
 * @ctxt:  the XPath Parser context
 *
 *  [20]   FilterExpr ::=   PrimaryExpr 
 *               | FilterExpr Predicate 
 *
 * Compile a filter expression.
 * Square brackets are used to filter expressions in the same way that
 * they are used in location paths. It is an error if the expression to
 * be filtered does not evaluate to a node-set. The context node list
 * used for evaluating the expression in square brackets is the node-set
 * to be filtered listed in document order.
 */

static void
xmlXPathCompFilterExpr(xmlXPathParserContextPtr ctxt) {
    xmlXPathCompPrimaryExpr(ctxt);
    CHECK_ERROR;
    SKIP_BLANKS;
    
    while (CUR == '[') {
	xmlXPathCompPredicate(ctxt, 1);
	SKIP_BLANKS;
    }

    
}

/**
 * xmlXPathScanName:
 * @ctxt:  the XPath Parser context
 *
 * Trickery: parse an XML name but without consuming the input flow
 * Needed to avoid insanity in the parser state.
 *
 * [4] NameChar ::= Letter | Digit | '.' | '-' | '_' | ':' |
 *                  CombiningChar | Extender
 *
 * [5] Name ::= (Letter | '_' | ':') (NameChar)*
 *
 * [6] Names ::= Name (S Name)*
 *
 * Returns the Name parsed or NULL
 */

static xmlChar *
xmlXPathScanName(xmlXPathParserContextPtr ctxt) {
    xmlChar buf[XML_MAX_NAMELEN];
    int len = 0;

    SKIP_BLANKS;
    if (!IS_LETTER(CUR) && (CUR != '_') &&
        (CUR != ':')) {
	return(NULL);
    }

    while ((IS_LETTER(NXT(len))) || (IS_DIGIT(NXT(len))) ||
           (NXT(len) == '.') || (NXT(len) == '-') ||
	   (NXT(len) == '_') || (NXT(len) == ':') || 
	   (IS_COMBINING(NXT(len))) ||
	   (IS_EXTENDER(NXT(len)))) {
	buf[len] = NXT(len);
	len++;
	if (len >= XML_MAX_NAMELEN) {
	    xmlGenericError(xmlGenericErrorContext, 
	       "xmlScanName: reached XML_MAX_NAMELEN limit\n");
	    while ((IS_LETTER(NXT(len))) || (IS_DIGIT(NXT(len))) ||
		   (NXT(len) == '.') || (NXT(len) == '-') ||
		   (NXT(len) == '_') || (NXT(len) == ':') || 
		   (IS_COMBINING(NXT(len))) ||
		   (IS_EXTENDER(NXT(len))))
		 len++;
	    break;
	}
    }
    return(xmlStrndup(buf, len));
}

/**
 * xmlXPathCompPathExpr:
 * @ctxt:  the XPath Parser context
 *
 *  [19]   PathExpr ::=   LocationPath 
 *               | FilterExpr 
 *               | FilterExpr '/' RelativeLocationPath 
 *               | FilterExpr '//' RelativeLocationPath 
 *
 * Compile a path expression.
 * The / operator and // operators combine an arbitrary expression
 * and a relative location path. It is an error if the expression
 * does not evaluate to a node-set.
 * The / operator does composition in the same way as when / is
 * used in a location path. As in location paths, // is short for
 * /descendant-or-self::node()/.
 */

static void
xmlXPathCompPathExpr(xmlXPathParserContextPtr ctxt) {
    int lc = 1;           /* Should we branch to LocationPath ?         */
    xmlChar *name = NULL; /* we may have to preparse a name to find out */

    SKIP_BLANKS;
    if ((CUR == '$') || (CUR == '(') || (IS_DIGIT(CUR)) ||
        (CUR == '\'') || (CUR == '"') || (CUR == '.' && IS_DIGIT(NXT(1)))) {
	lc = 0;
    } else if (CUR == '*') {
	/* relative or absolute location path */
	lc = 1;
    } else if (CUR == '/') {
	/* relative or absolute location path */
	lc = 1;
    } else if (CUR == '@') {
	/* relative abbreviated attribute location path */
	lc = 1;
    } else if (CUR == '.') {
	/* relative abbreviated attribute location path */
	lc = 1;
    } else {
	/*
	 * Problem is finding if we have a name here whether it's:
	 *   - a nodetype
	 *   - a function call in which case it's followed by '('
	 *   - an axis in which case it's followed by ':'
	 *   - a element name
	 * We do an a priori analysis here rather than having to
	 * maintain parsed token content through the recursive function
	 * calls. This looks uglier but makes the code quite easier to
	 * read/write/debug.
	 */
	SKIP_BLANKS;
	name = xmlXPathScanName(ctxt);
	if ((name != NULL) && (xmlStrstr(name, (xmlChar *) "::") != NULL)) {
#ifdef DEBUG_STEP
	    xmlGenericError(xmlGenericErrorContext,
		    "PathExpr: Axis\n");
#endif
	    lc = 1;
	    xmlFree(name);
	} else if (name != NULL) {
	    int len =xmlStrlen(name);
	    int blank = 0;

	    
	    while (NXT(len) != 0) {
		if (NXT(len) == '/') {
		    /* element name */
#ifdef DEBUG_STEP
		    xmlGenericError(xmlGenericErrorContext,
			    "PathExpr: AbbrRelLocation\n");
#endif
		    lc = 1;
		    break;
		} else if (IS_BLANK(NXT(len))) {
		    /* skip to next */
		    blank = 1;
		} else if (NXT(len) == ':') {
#ifdef DEBUG_STEP
		    xmlGenericError(xmlGenericErrorContext,
			    "PathExpr: AbbrRelLocation\n");
#endif
		    lc = 1;
		    break;
		} else if ((NXT(len) == '(')) {
		    /* Note Type or Function */
		    if (xmlXPathIsNodeType(name)) {
#ifdef DEBUG_STEP
		        xmlGenericError(xmlGenericErrorContext,
				"PathExpr: Type search\n");
#endif
			lc = 1;
		    } else {
#ifdef DEBUG_STEP
		        xmlGenericError(xmlGenericErrorContext,
				"PathExpr: function call\n");
#endif
			lc = 0;
		    }
                    break;
		} else if ((NXT(len) == '[')) {
		    /* element name */
#ifdef DEBUG_STEP
		    xmlGenericError(xmlGenericErrorContext,
			    "PathExpr: AbbrRelLocation\n");
#endif
		    lc = 1;
		    break;
		} else if ((NXT(len) == '<') || (NXT(len) == '>') ||
			   (NXT(len) == '=')) {
		    lc = 1;
		    break;
		} else {
		    lc = 1;
		    break;
		}
		len++;
	    }
	    if (NXT(len) == 0) {
#ifdef DEBUG_STEP
		xmlGenericError(xmlGenericErrorContext,
			"PathExpr: AbbrRelLocation\n");
#endif
		/* element name */
		lc = 1;
	    }
	    xmlFree(name);
	} else {
	    /* make sure all cases are covered explicitely */
	    XP_ERROR(XPATH_EXPR_ERROR);
	}
    } 

    if (lc) {
	if (CUR == '/') {
	    PUSH_LEAVE_EXPR(XPATH_OP_ROOT, 0, 0);
	} else {
	    PUSH_LEAVE_EXPR(XPATH_OP_NODE, 0, 0);
	}
	xmlXPathCompLocationPath(ctxt);
    } else {
	xmlXPathCompFilterExpr(ctxt);
	CHECK_ERROR;
	if ((CUR == '/') && (NXT(1) == '/')) {
	    SKIP(2);
	    SKIP_BLANKS;

	    PUSH_LONG_EXPR(XPATH_OP_COLLECT, AXIS_DESCENDANT_OR_SELF,
		    NODE_TEST_TYPE, NODE_TYPE_NODE, NULL, NULL);
	    PUSH_UNARY_EXPR(XPATH_OP_RESET, ctxt->comp->last, 1, 0);

	    xmlXPathCompRelativeLocationPath(ctxt);
	} else if (CUR == '/') {
	    xmlXPathCompRelativeLocationPath(ctxt);
	}
    }
    SKIP_BLANKS;
}

/**
 * xmlXPathCompUnionExpr:
 * @ctxt:  the XPath Parser context
 *
 *  [18]   UnionExpr ::=   PathExpr 
 *               | UnionExpr '|' PathExpr 
 *
 * Compile an union expression.
 */

static void
xmlXPathCompUnionExpr(xmlXPathParserContextPtr ctxt) {
    xmlXPathCompPathExpr(ctxt);
    CHECK_ERROR;
    SKIP_BLANKS;
    while (CUR == '|') {
	int op1 = ctxt->comp->last;
	PUSH_LEAVE_EXPR(XPATH_OP_NODE, 0, 0);

	NEXT;
	SKIP_BLANKS;
	xmlXPathCompPathExpr(ctxt);

	PUSH_BINARY_EXPR(XPATH_OP_UNION, op1, ctxt->comp->last, 0, 0);

	SKIP_BLANKS;
    }
}

/**
 * xmlXPathCompUnaryExpr:
 * @ctxt:  the XPath Parser context
 *
 *  [27]   UnaryExpr ::=   UnionExpr 
 *                   | '-' UnaryExpr 
 *
 * Compile an unary expression.
 */

static void
xmlXPathCompUnaryExpr(xmlXPathParserContextPtr ctxt) {
    int minus = 0;
    int found = 0;

    SKIP_BLANKS;
    while (CUR == '-') {
        minus = 1 - minus;
	found = 1;
	NEXT;
	SKIP_BLANKS;
    }

    xmlXPathCompUnionExpr(ctxt);
    CHECK_ERROR;
    if (found) {
	if (minus)
	    PUSH_UNARY_EXPR(XPATH_OP_PLUS, ctxt->comp->last, 2, 0);
	else
	    PUSH_UNARY_EXPR(XPATH_OP_PLUS, ctxt->comp->last, 3, 0);
    }
}

/**
 * xmlXPathCompMultiplicativeExpr:
 * @ctxt:  the XPath Parser context
 *
 *  [26]   MultiplicativeExpr ::=   UnaryExpr 
 *                   | MultiplicativeExpr MultiplyOperator UnaryExpr 
 *                   | MultiplicativeExpr 'div' UnaryExpr 
 *                   | MultiplicativeExpr 'mod' UnaryExpr 
 *  [34]   MultiplyOperator ::=   '*'
 *
 * Compile an Additive expression.
 */

static void
xmlXPathCompMultiplicativeExpr(xmlXPathParserContextPtr ctxt) {
    xmlXPathCompUnaryExpr(ctxt);
    CHECK_ERROR;
    SKIP_BLANKS;
    while ((CUR == '*') || 
           ((CUR == 'd') && (NXT(1) == 'i') && (NXT(2) == 'v')) ||
           ((CUR == 'm') && (NXT(1) == 'o') && (NXT(2) == 'd'))) {
	int op = -1;
	int op1 = ctxt->comp->last;

        if (CUR == '*') {
	    op = 0;
	    NEXT;
	} else if (CUR == 'd') {
	    op = 1;
	    SKIP(3);
	} else if (CUR == 'm') {
	    op = 2;
	    SKIP(3);
	}
	SKIP_BLANKS;
        xmlXPathCompUnaryExpr(ctxt);
	CHECK_ERROR;
	PUSH_BINARY_EXPR(XPATH_OP_MULT, op1, ctxt->comp->last, op, 0);
	SKIP_BLANKS;
    }
}

/**
 * xmlXPathCompAdditiveExpr:
 * @ctxt:  the XPath Parser context
 *
 *  [25]   AdditiveExpr ::=   MultiplicativeExpr 
 *                   | AdditiveExpr '+' MultiplicativeExpr 
 *                   | AdditiveExpr '-' MultiplicativeExpr 
 *
 * Compile an Additive expression.
 */

static void
xmlXPathCompAdditiveExpr(xmlXPathParserContextPtr ctxt) {

    xmlXPathCompMultiplicativeExpr(ctxt);
    CHECK_ERROR;
    SKIP_BLANKS;
    while ((CUR == '+') || (CUR == '-')) {
	int plus;
	int op1 = ctxt->comp->last;

        if (CUR == '+') plus = 1;
	else plus = 0;
	NEXT;
	SKIP_BLANKS;
        xmlXPathCompMultiplicativeExpr(ctxt);
	CHECK_ERROR;
	PUSH_BINARY_EXPR(XPATH_OP_PLUS, op1, ctxt->comp->last, plus, 0);
	SKIP_BLANKS;
    }
}

/**
 * xmlXPathCompRelationalExpr:
 * @ctxt:  the XPath Parser context
 *
 *  [24]   RelationalExpr ::=   AdditiveExpr 
 *                 | RelationalExpr '<' AdditiveExpr 
 *                 | RelationalExpr '>' AdditiveExpr 
 *                 | RelationalExpr '<=' AdditiveExpr 
 *                 | RelationalExpr '>=' AdditiveExpr 
 *
 *  A <= B > C is allowed ? Answer from James, yes with
 *  (AdditiveExpr <= AdditiveExpr) > AdditiveExpr
 *  which is basically what got implemented.
 *
 * Compile a Relational expression, then push the result
 * on the stack
 */

static void
xmlXPathCompRelationalExpr(xmlXPathParserContextPtr ctxt) {
    xmlXPathCompAdditiveExpr(ctxt);
    CHECK_ERROR;
    SKIP_BLANKS;
    while ((CUR == '<') ||
           (CUR == '>') ||
           ((CUR == '<') && (NXT(1) == '=')) ||
           ((CUR == '>') && (NXT(1) == '='))) {
	int inf, strict;
	int op1 = ctxt->comp->last;

        if (CUR == '<') inf = 1;
	else inf = 0;
	if (NXT(1) == '=') strict = 0;
	else strict = 1;
	NEXT;
	if (!strict) NEXT;
	SKIP_BLANKS;
        xmlXPathCompAdditiveExpr(ctxt);
	CHECK_ERROR;
	PUSH_BINARY_EXPR(XPATH_OP_CMP, op1, ctxt->comp->last, inf, strict);
	SKIP_BLANKS;
    }
}

/**
 * xmlXPathCompEqualityExpr:
 * @ctxt:  the XPath Parser context
 *
 *  [23]   EqualityExpr ::=   RelationalExpr 
 *                 | EqualityExpr '=' RelationalExpr 
 *                 | EqualityExpr '!=' RelationalExpr 
 *
 *  A != B != C is allowed ? Answer from James, yes with
 *  (RelationalExpr = RelationalExpr) = RelationalExpr
 *  (RelationalExpr != RelationalExpr) != RelationalExpr
 *  which is basically what got implemented.
 *
 * Compile an Equality expression.
 *
 */
static void
xmlXPathCompEqualityExpr(xmlXPathParserContextPtr ctxt) {
    xmlXPathCompRelationalExpr(ctxt);
    CHECK_ERROR;
    SKIP_BLANKS;
    while ((CUR == '=') || ((CUR == '!') && (NXT(1) == '='))) {
	int eq;
	int op1 = ctxt->comp->last;

        if (CUR == '=') eq = 1;
	else eq = 0;
	NEXT;
	if (!eq) NEXT;
	SKIP_BLANKS;
        xmlXPathCompRelationalExpr(ctxt);
	CHECK_ERROR;
	PUSH_BINARY_EXPR(XPATH_OP_EQUAL, op1, ctxt->comp->last, eq, 0);
	SKIP_BLANKS;
    }
}

/**
 * xmlXPathCompAndExpr:
 * @ctxt:  the XPath Parser context
 *
 *  [22]   AndExpr ::=   EqualityExpr 
 *                 | AndExpr 'and' EqualityExpr 
 *
 * Compile an AND expression.
 *
 */
static void
xmlXPathCompAndExpr(xmlXPathParserContextPtr ctxt) {
    xmlXPathCompEqualityExpr(ctxt);
    CHECK_ERROR;
    SKIP_BLANKS;
    while ((CUR == 'a') && (NXT(1) == 'n') && (NXT(2) == 'd')) {
	int op1 = ctxt->comp->last;
        SKIP(3);
	SKIP_BLANKS;
        xmlXPathCompEqualityExpr(ctxt);
	CHECK_ERROR;
	PUSH_BINARY_EXPR(XPATH_OP_AND, op1, ctxt->comp->last, 0, 0);
	SKIP_BLANKS;
    }
}

/**
 * xmlXPathCompExpr:
 * @ctxt:  the XPath Parser context
 *
 *  [14]   Expr ::=   OrExpr 
 *  [21]   OrExpr ::=   AndExpr 
 *                 | OrExpr 'or' AndExpr 
 *
 * Parse and compile an expression
 */
static void
xmlXPathCompileExpr(xmlXPathParserContextPtr ctxt) {
    xmlXPathCompAndExpr(ctxt);
    CHECK_ERROR;
    SKIP_BLANKS;
    while ((CUR == 'o') && (NXT(1) == 'r')) {
	int op1 = ctxt->comp->last;
        SKIP(2);
	SKIP_BLANKS;
        xmlXPathCompAndExpr(ctxt);
	CHECK_ERROR;
	PUSH_BINARY_EXPR(XPATH_OP_OR, op1, ctxt->comp->last, 0, 0);
	op1 = ctxt->comp->nbStep;
	SKIP_BLANKS;
    }
    if (ctxt->comp->steps[ctxt->comp->last].op != XPATH_OP_VALUE) {
	/* more ops could be optimized too */
	PUSH_UNARY_EXPR(XPATH_OP_SORT, ctxt->comp->last , 0, 0);
    }
}

/**
 * xmlXPathCompPredicate:
 * @ctxt:  the XPath Parser context
 * @filter:  act as a filter
 *
 *  [8]   Predicate ::=   '[' PredicateExpr ']'
 *  [9]   PredicateExpr ::=   Expr 
 *
 * Compile a predicate expression
 */
static void
xmlXPathCompPredicate(xmlXPathParserContextPtr ctxt, int filter) {
    int op1 = ctxt->comp->last;

    SKIP_BLANKS;
    if (CUR != '[') {
	XP_ERROR(XPATH_INVALID_PREDICATE_ERROR);
    }
    NEXT;
    SKIP_BLANKS;

    ctxt->comp->last = -1;
    xmlXPathCompileExpr(ctxt);
    CHECK_ERROR;

    if (CUR != ']') {
	XP_ERROR(XPATH_INVALID_PREDICATE_ERROR);
    }

    if (filter)
	PUSH_BINARY_EXPR(XPATH_OP_FILTER, op1, ctxt->comp->last, 0, 0);
    else
	PUSH_BINARY_EXPR(XPATH_OP_PREDICATE, op1, ctxt->comp->last, 0, 0);

    NEXT;
    SKIP_BLANKS;
}

/**
 * xmlXPathCompNodeTest:
 * @ctxt:  the XPath Parser context
 * @test:  pointer to a xmlXPathTestVal
 * @type:  pointer to a xmlXPathTypeVal
 * @prefix:  placeholder for a possible name prefix
 *
 * [7] NodeTest ::=   NameTest
 *		    | NodeType '(' ')'
 *		    | 'processing-instruction' '(' Literal ')'
 *
 * [37] NameTest ::=  '*'
 *		    | NCName ':' '*'
 *		    | QName
 * [38] NodeType ::= 'comment'
 *		   | 'text'
 *		   | 'processing-instruction'
 *		   | 'node'
 *
 * Returns the name found and update @test, @type and @prefix appropriately
 */
static xmlChar *
xmlXPathCompNodeTest(xmlXPathParserContextPtr ctxt, xmlXPathTestVal *test,
	             xmlXPathTypeVal *type, const xmlChar **prefix,
		     xmlChar *name) {
    int blanks;

    if ((test == NULL) || (type == NULL) || (prefix == NULL)) {
	STRANGE;
	return(NULL);
    }
    *type = 0;
    *test = 0;
    *prefix = NULL;
    SKIP_BLANKS;

    if ((name == NULL) && (CUR == '*')) {
	/*
	 * All elements
	 */
	NEXT;
	*test = NODE_TEST_ALL;
	return(NULL);
    }

    if (name == NULL)
	name = xmlXPathParseNCName(ctxt);
    if (name == NULL) {
	XP_ERROR0(XPATH_EXPR_ERROR);
    }

    blanks = IS_BLANK(CUR);
    SKIP_BLANKS;
    if (CUR == '(') {
	NEXT;
	/*
	 * NodeType or PI search
	 */
	if (xmlStrEqual(name, BAD_CAST "comment"))
	    *type = NODE_TYPE_COMMENT;
	else if (xmlStrEqual(name, BAD_CAST "node"))
	    *type = NODE_TYPE_NODE;
	else if (xmlStrEqual(name, BAD_CAST "processing-instruction"))
	    *type = NODE_TYPE_PI;
	else if (xmlStrEqual(name, BAD_CAST "text"))
	    *type = NODE_TYPE_TEXT;
	else {
	    if (name != NULL)
		xmlFree(name);
	    XP_ERROR0(XPATH_EXPR_ERROR);
	}

	*test = NODE_TEST_TYPE;
	
	SKIP_BLANKS;
	if (*type == NODE_TYPE_PI) {
	    /*
	     * Specific case: search a PI by name.
	     */
	    if (name != NULL)
		xmlFree(name);
	    name = NULL;
	    if (CUR != ')') {
		name = xmlXPathParseLiteral(ctxt);
		CHECK_ERROR 0;
		*test = NODE_TEST_PI;
		SKIP_BLANKS;
	    }
	}
	if (CUR != ')') {
	    if (name != NULL)
		xmlFree(name);
	    XP_ERROR0(XPATH_UNCLOSED_ERROR);
	}
	NEXT;
	return(name);
    }
    *test = NODE_TEST_NAME;
    if ((!blanks) && (CUR == ':')) {
	NEXT;

	/*
	 * Since currently the parser context don't have a
	 * namespace list associated:
	 * The namespace name for this prefix can be computed
	 * only at evaluation time. The compilation is done
	 * outside of any context.
	 */
#if 0
	*prefix = xmlXPathNsLookup(ctxt->context, name);
	if (name != NULL)
	    xmlFree(name);
	if (*prefix == NULL) {
	    XP_ERROR0(XPATH_UNDEF_PREFIX_ERROR);
	}
#else
	*prefix = name;
#endif

	if (CUR == '*') {
	    /*
	     * All elements
	     */
	    NEXT;
	    *test = NODE_TEST_ALL;
	    return(NULL);
	}

	name = xmlXPathParseNCName(ctxt);
	if (name == NULL) {
	    XP_ERROR0(XPATH_EXPR_ERROR);
	}
    }
    return(name);
}

/**
 * xmlXPathIsAxisName:
 * @name:  a preparsed name token
 *
 * [6] AxisName ::=   'ancestor'
 *                  | 'ancestor-or-self'
 *                  | 'attribute'
 *                  | 'child'
 *                  | 'descendant'
 *                  | 'descendant-or-self'
 *                  | 'following'
 *                  | 'following-sibling'
 *                  | 'namespace'
 *                  | 'parent'
 *                  | 'preceding'
 *                  | 'preceding-sibling'
 *                  | 'self'
 *
 * Returns the axis or 0
 */
static xmlXPathAxisVal
xmlXPathIsAxisName(const xmlChar *name) {
    xmlXPathAxisVal ret = 0;
    switch (name[0]) {
	case 'a':
	    if (xmlStrEqual(name, BAD_CAST "ancestor"))
		ret = AXIS_ANCESTOR;
	    if (xmlStrEqual(name, BAD_CAST "ancestor-or-self"))
		ret = AXIS_ANCESTOR_OR_SELF;
	    if (xmlStrEqual(name, BAD_CAST "attribute"))
		ret = AXIS_ATTRIBUTE;
	    break;
	case 'c':
	    if (xmlStrEqual(name, BAD_CAST "child"))
		ret = AXIS_CHILD;
	    break;
	case 'd':
	    if (xmlStrEqual(name, BAD_CAST "descendant"))
		ret = AXIS_DESCENDANT;
	    if (xmlStrEqual(name, BAD_CAST "descendant-or-self"))
		ret = AXIS_DESCENDANT_OR_SELF;
	    break;
	case 'f':
	    if (xmlStrEqual(name, BAD_CAST "following"))
		ret = AXIS_FOLLOWING;
	    if (xmlStrEqual(name, BAD_CAST "following-sibling"))
		ret = AXIS_FOLLOWING_SIBLING;
	    break;
	case 'n':
	    if (xmlStrEqual(name, BAD_CAST "namespace"))
		ret = AXIS_NAMESPACE;
	    break;
	case 'p':
	    if (xmlStrEqual(name, BAD_CAST "parent"))
		ret = AXIS_PARENT;
	    if (xmlStrEqual(name, BAD_CAST "preceding"))
		ret = AXIS_PRECEDING;
	    if (xmlStrEqual(name, BAD_CAST "preceding-sibling"))
		ret = AXIS_PRECEDING_SIBLING;
	    break;
	case 's':
	    if (xmlStrEqual(name, BAD_CAST "self"))
		ret = AXIS_SELF;
	    break;
    }
    return(ret);
}

/**
 * xmlXPathCompStep:
 * @ctxt:  the XPath Parser context
 *
 * [4] Step ::=   AxisSpecifier NodeTest Predicate*
 *                  | AbbreviatedStep 
 *
 * [12] AbbreviatedStep ::=   '.' | '..'
 *
 * [5] AxisSpecifier ::= AxisName '::'
 *                  | AbbreviatedAxisSpecifier
 *
 * [13] AbbreviatedAxisSpecifier ::= '@'?
 *
 * Modified for XPtr range support as:
 *
 *  [4xptr] Step ::= AxisSpecifier NodeTest Predicate*
 *                     | AbbreviatedStep
 *                     | 'range-to' '(' Expr ')' Predicate*
 *
 * Compile one step in a Location Path
 * A location step of . is short for self::node(). This is
 * particularly useful in conjunction with //. For example, the
 * location path .//para is short for
 * self::node()/descendant-or-self::node()/child::para
 * and so will select all para descendant elements of the context
 * node.
 * Similarly, a location step of .. is short for parent::node().
 * For example, ../title is short for parent::node()/child::title
 * and so will select the title children of the parent of the context
 * node.
 */
static void
xmlXPathCompStep(xmlXPathParserContextPtr ctxt) {
#ifdef LIBXML_XPTR_ENABLED
    int rangeto = 0;
    int op2 = -1;
#endif

    SKIP_BLANKS;
    if ((CUR == '.') && (NXT(1) == '.')) {
	SKIP(2);
	SKIP_BLANKS;
	PUSH_LONG_EXPR(XPATH_OP_COLLECT, AXIS_PARENT,
		    NODE_TEST_TYPE, NODE_TYPE_NODE, NULL, NULL);
    } else if (CUR == '.') {
	NEXT;
	SKIP_BLANKS;
    } else {
	xmlChar *name = NULL;
	const xmlChar *prefix = NULL;
	xmlXPathTestVal test;
	xmlXPathAxisVal axis = 0;
	xmlXPathTypeVal type;
	int op1;

	/*
	 * The modification needed for XPointer change to the production
	 */
#ifdef LIBXML_XPTR_ENABLED
	if (ctxt->xptr) {
	    name = xmlXPathParseNCName(ctxt);
	    if ((name != NULL) && (xmlStrEqual(name, BAD_CAST "range-to"))) {
                op2 = ctxt->comp->last;
		xmlFree(name);
		SKIP_BLANKS;
		if (CUR != '(') {
		    XP_ERROR(XPATH_EXPR_ERROR);
		}
		NEXT;
		SKIP_BLANKS;

		xmlXPathCompileExpr(ctxt);
		/* PUSH_BINARY_EXPR(XPATH_OP_RANGETO, op2, ctxt->comp->last, 0, 0); */
		CHECK_ERROR;

		SKIP_BLANKS;
		if (CUR != ')') {
		    XP_ERROR(XPATH_EXPR_ERROR);
		}
		NEXT;
		rangeto = 1;
		goto eval_predicates;
	    }
	}
#endif
	if (CUR == '*') {
	    axis = AXIS_CHILD;
	} else {
	    if (name == NULL)
		name = xmlXPathParseNCName(ctxt);
	    if (name != NULL) {
		axis = xmlXPathIsAxisName(name);
		if (axis != 0) {
		    SKIP_BLANKS;
		    if ((CUR == ':') && (NXT(1) == ':')) {
			SKIP(2);
			xmlFree(name);
			name = NULL;
		    } else {
			/* an element name can conflict with an axis one :-\ */
			axis = AXIS_CHILD;
		    }
		} else {
		    axis = AXIS_CHILD;
		}
	    } else if (CUR == '@') {
		NEXT;
		axis = AXIS_ATTRIBUTE;
	    } else {
		axis = AXIS_CHILD;
	    }
	}

	CHECK_ERROR;

	name = xmlXPathCompNodeTest(ctxt, &test, &type, &prefix, name);
	if (test == 0)
	    return;

#ifdef DEBUG_STEP
	xmlGenericError(xmlGenericErrorContext,
		"Basis : computing new set\n");
#endif

#ifdef DEBUG_STEP
	xmlGenericError(xmlGenericErrorContext, "Basis : ");
	if (ctxt->value == NULL)
	    xmlGenericError(xmlGenericErrorContext, "no value\n");
	else if (ctxt->value->nodesetval == NULL)
	    xmlGenericError(xmlGenericErrorContext, "Empty\n");
	else
	    xmlGenericErrorContextNodeSet(stdout, ctxt->value->nodesetval);
#endif

eval_predicates:
	op1 = ctxt->comp->last;
	ctxt->comp->last = -1;

	SKIP_BLANKS;
	while (CUR == '[') {
	    xmlXPathCompPredicate(ctxt, 0);
	}

#ifdef LIBXML_XPTR_ENABLED
	if (rangeto) {
	    PUSH_BINARY_EXPR(XPATH_OP_RANGETO, op2, op1, 0, 0);
	} else
#endif
	    PUSH_FULL_EXPR(XPATH_OP_COLLECT, op1, ctxt->comp->last, axis,
			   test, type, (void *)prefix, (void *)name);

    }
#ifdef DEBUG_STEP
    xmlGenericError(xmlGenericErrorContext, "Step : ");
    if (ctxt->value == NULL)
	xmlGenericError(xmlGenericErrorContext, "no value\n");
    else if (ctxt->value->nodesetval == NULL)
	xmlGenericError(xmlGenericErrorContext, "Empty\n");
    else
	xmlGenericErrorContextNodeSet(xmlGenericErrorContext,
		ctxt->value->nodesetval);
#endif
}

/**
 * xmlXPathCompRelativeLocationPath:
 * @ctxt:  the XPath Parser context
 *
 *  [3]   RelativeLocationPath ::=   Step 
 *                     | RelativeLocationPath '/' Step 
 *                     | AbbreviatedRelativeLocationPath 
 *  [11]  AbbreviatedRelativeLocationPath ::=   RelativeLocationPath '//' Step 
 *
 * Compile a relative location path.
 */
static void
xmlXPathCompRelativeLocationPath
(xmlXPathParserContextPtr ctxt) {
    SKIP_BLANKS;
    if ((CUR == '/') && (NXT(1) == '/')) {
	SKIP(2);
	SKIP_BLANKS;
	PUSH_LONG_EXPR(XPATH_OP_COLLECT, AXIS_DESCENDANT_OR_SELF,
		         NODE_TEST_TYPE, NODE_TYPE_NODE, NULL, NULL);
    } else if (CUR == '/') {
	    NEXT;
	SKIP_BLANKS;
    }
    xmlXPathCompStep(ctxt);
    SKIP_BLANKS;
    while (CUR == '/') {
	if ((CUR == '/') && (NXT(1) == '/')) {
	    SKIP(2);
	    SKIP_BLANKS;
	    PUSH_LONG_EXPR(XPATH_OP_COLLECT, AXIS_DESCENDANT_OR_SELF,
			     NODE_TEST_TYPE, NODE_TYPE_NODE, NULL, NULL);
	    xmlXPathCompStep(ctxt);
	} else if (CUR == '/') {
	    NEXT;
	    SKIP_BLANKS;
	    xmlXPathCompStep(ctxt);
	}
	SKIP_BLANKS;
    }
}

/**
 * xmlXPathCompLocationPath:
 * @ctxt:  the XPath Parser context
 *
 *  [1]   LocationPath ::=   RelativeLocationPath 
 *                     | AbsoluteLocationPath 
 *  [2]   AbsoluteLocationPath ::=   '/' RelativeLocationPath?
 *                     | AbbreviatedAbsoluteLocationPath 
 *  [10]   AbbreviatedAbsoluteLocationPath ::=   
 *                           '//' RelativeLocationPath 
 *
 * Compile a location path
 *
 * // is short for /descendant-or-self::node()/. For example,
 * //para is short for /descendant-or-self::node()/child::para and
 * so will select any para element in the document (even a para element
 * that is a document element will be selected by //para since the
 * document element node is a child of the root node); div//para is
 * short for div/descendant-or-self::node()/child::para and so will
 * select all para descendants of div children.
 */
static void
xmlXPathCompLocationPath(xmlXPathParserContextPtr ctxt) {
    SKIP_BLANKS;
    if (CUR != '/') {
        xmlXPathCompRelativeLocationPath(ctxt);
    } else {
	while (CUR == '/') {
	    if ((CUR == '/') && (NXT(1) == '/')) {
		SKIP(2);
		SKIP_BLANKS;
		PUSH_LONG_EXPR(XPATH_OP_COLLECT, AXIS_DESCENDANT_OR_SELF,
			     NODE_TEST_TYPE, NODE_TYPE_NODE, NULL, NULL);
		xmlXPathCompRelativeLocationPath(ctxt);
	    } else if (CUR == '/') {
		NEXT;
		SKIP_BLANKS;
		if ((CUR != 0 ) &&
		    ((IS_LETTER(CUR)) || (CUR == '_') || (CUR == '.') ||
		     (CUR == '@') || (CUR == '*')))
		    xmlXPathCompRelativeLocationPath(ctxt);
	    }
	}
    }
}

/************************************************************************
 *									*
 * 		XPath precompiled expression evaluation			*
 *									*
 ************************************************************************/

static int
xmlXPathCompOpEval(xmlXPathParserContextPtr ctxt, xmlXPathStepOpPtr op);

/**
 * xmlXPathNodeCollectAndTest:
 * @ctxt:  the XPath Parser context
 * @op:  the XPath precompiled step operation
 * @first:  pointer to the first element in document order
 * @last:  pointer to the last element in document order
 *
 * This is the function implementing a step: based on the current list
 * of nodes, it builds up a new list, looking at all nodes under that
 * axis and selecting them it also do the predicate filtering
 *
 * Pushes the new NodeSet resulting from the search.
 *
 * Returns the number of node traversed
 */
static int
xmlXPathNodeCollectAndTest(xmlXPathParserContextPtr ctxt,
                           xmlXPathStepOpPtr op,
			   xmlNodePtr * first, xmlNodePtr * last)
{
    xmlXPathAxisVal axis = op->value;
    xmlXPathTestVal test = op->value2;
    xmlXPathTypeVal type = op->value3;
    const xmlChar *prefix = op->value4;
    const xmlChar *name = op->value5;
    const xmlChar *URI = NULL;

#ifdef DEBUG_STEP
    int n = 0;
#endif
    int i, t = 0;
    xmlNodeSetPtr ret, list;
    xmlXPathTraversalFunction next = NULL;
    void (*addNode) (xmlNodeSetPtr, xmlNodePtr);
    xmlNodeSetPtr (*mergeNodeSet) (xmlNodeSetPtr, xmlNodeSetPtr);
    xmlNodePtr cur = NULL;
    xmlXPathObjectPtr obj;
    xmlNodeSetPtr nodelist;
    xmlNodePtr tmp;

    CHECK_TYPE0(XPATH_NODESET);
    obj = valuePop(ctxt);
    addNode = xmlXPathNodeSetAdd;
    mergeNodeSet = xmlXPathNodeSetMerge;
    if (prefix != NULL) {
        URI = xmlXPathNsLookup(ctxt->context, prefix);
        if (URI == NULL)
            XP_ERROR0(XPATH_UNDEF_PREFIX_ERROR);
    }
#ifdef DEBUG_STEP
    xmlGenericError(xmlGenericErrorContext, "new step : ");
#endif
    switch (axis) {
        case AXIS_ANCESTOR:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, "axis 'ancestors' ");
#endif
            first = NULL;
            next = xmlXPathNextAncestor;
            break;
        case AXIS_ANCESTOR_OR_SELF:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext,
                            "axis 'ancestors-or-self' ");
#endif
            first = NULL;
            next = xmlXPathNextAncestorOrSelf;
            break;
        case AXIS_ATTRIBUTE:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, "axis 'attributes' ");
#endif
            first = NULL;
	    last = NULL;
            next = xmlXPathNextAttribute;
	    mergeNodeSet = xmlXPathNodeSetMergeUnique;
            break;
        case AXIS_CHILD:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, "axis 'child' ");
#endif
	    last = NULL;
            next = xmlXPathNextChild;
	    mergeNodeSet = xmlXPathNodeSetMergeUnique;
            break;
        case AXIS_DESCENDANT:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, "axis 'descendant' ");
#endif
	    last = NULL;
            next = xmlXPathNextDescendant;
            break;
        case AXIS_DESCENDANT_OR_SELF:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext,
                            "axis 'descendant-or-self' ");
#endif
	    last = NULL;
            next = xmlXPathNextDescendantOrSelf;
            break;
        case AXIS_FOLLOWING:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, "axis 'following' ");
#endif
	    last = NULL;
            next = xmlXPathNextFollowing;
            break;
        case AXIS_FOLLOWING_SIBLING:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext,
                            "axis 'following-siblings' ");
#endif
	    last = NULL;
            next = xmlXPathNextFollowingSibling;
            break;
        case AXIS_NAMESPACE:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, "axis 'namespace' ");
#endif
            first = NULL;
	    last = NULL;
            next = (xmlXPathTraversalFunction) xmlXPathNextNamespace;
	    mergeNodeSet = xmlXPathNodeSetMergeUnique;
            break;
        case AXIS_PARENT:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, "axis 'parent' ");
#endif
            first = NULL;
            next = xmlXPathNextParent;
            break;
        case AXIS_PRECEDING:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, "axis 'preceding' ");
#endif
            first = NULL;
            next = xmlXPathNextPrecedingInternal;
            break;
        case AXIS_PRECEDING_SIBLING:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext,
                            "axis 'preceding-sibling' ");
#endif
            first = NULL;
            next = xmlXPathNextPrecedingSibling;
            break;
        case AXIS_SELF:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, "axis 'self' ");
#endif
            first = NULL;
	    last = NULL;
            next = xmlXPathNextSelf;
	    mergeNodeSet = xmlXPathNodeSetMergeUnique;
            break;
    }
    if (next == NULL)
        return(0);

    nodelist = obj->nodesetval;
    if (nodelist == NULL) {
        xmlXPathFreeObject(obj);
        valuePush(ctxt, xmlXPathWrapNodeSet(NULL));
        return(0);
    }
    addNode = xmlXPathNodeSetAddUnique;
    ret = NULL;
#ifdef DEBUG_STEP
    xmlGenericError(xmlGenericErrorContext,
                    " context contains %d nodes\n", nodelist->nodeNr);
    switch (test) {
        case NODE_TEST_NONE:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for none !!!\n");
            break;
        case NODE_TEST_TYPE:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for type %d\n", type);
            break;
        case NODE_TEST_PI:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for PI !!!\n");
            break;
        case NODE_TEST_ALL:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for *\n");
            break;
        case NODE_TEST_NS:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for namespace %s\n",
                            prefix);
            break;
        case NODE_TEST_NAME:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for name %s\n", name);
            if (prefix != NULL)
                xmlGenericError(xmlGenericErrorContext,
                                "           with namespace %s\n", prefix);
            break;
    }
    xmlGenericError(xmlGenericErrorContext, "Testing : ");
#endif
    /*
     * 2.3 Node Tests
     *  - For the attribute axis, the principal node type is attribute. 
     *  - For the namespace axis, the principal node type is namespace. 
     *  - For other axes, the principal node type is element. 
     *
     * A node test * is true for any node of the
     * principal node type. For example, child::* will
     * select all element children of the context node
     */
    tmp = ctxt->context->node;
    for (i = 0; i < nodelist->nodeNr; i++) {
        ctxt->context->node = nodelist->nodeTab[i];

        cur = NULL;
        list = xmlXPathNodeSetCreate(NULL);
        do {
            cur = next(ctxt, cur);
            if (cur == NULL)
                break;
            if ((first != NULL) && (*first == cur))
                break;
	    if (((t % 256) == 0) &&
	        (first != NULL) && (*first != NULL) &&
		(xmlXPathCmpNodes(*first, cur) >= 0))
		break;
	    if ((last != NULL) && (*last == cur))
		break;
	    if (((t % 256) == 0) &&
		(last != NULL) && (*last != NULL) &&
		(xmlXPathCmpNodes(cur, *last) >= 0))
		break;
            t++;
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, " %s", cur->name);
#endif
            switch (test) {
                case NODE_TEST_NONE:
                    ctxt->context->node = tmp;
                    STRANGE return(t);
                case NODE_TEST_TYPE:
                    if ((cur->type == type) ||
                        ((type == NODE_TYPE_NODE) &&
                         ((cur->type == XML_DOCUMENT_NODE) ||
                          (cur->type == XML_HTML_DOCUMENT_NODE) ||
                          (cur->type == XML_ELEMENT_NODE) ||
                          (cur->type == XML_NAMESPACE_DECL) ||
                          (cur->type == XML_ATTRIBUTE_NODE) ||
                          (cur->type == XML_PI_NODE) ||
                          (cur->type == XML_COMMENT_NODE) ||
                          (cur->type == XML_CDATA_SECTION_NODE) ||
                          (cur->type == XML_TEXT_NODE))) ||
			((type == NODE_TYPE_TEXT) &&
			 (cur->type == XML_CDATA_SECTION_NODE))) {
#ifdef DEBUG_STEP
                        n++;
#endif
                        addNode(list, cur);
                    }
                    break;
                case NODE_TEST_PI:
                    if (cur->type == XML_PI_NODE) {
                        if ((name != NULL) &&
                            (!xmlStrEqual(name, cur->name)))
                            break;
#ifdef DEBUG_STEP
                        n++;
#endif
                        addNode(list, cur);
                    }
                    break;
                case NODE_TEST_ALL:
                    if (axis == AXIS_ATTRIBUTE) {
                        if (cur->type == XML_ATTRIBUTE_NODE) {
#ifdef DEBUG_STEP
                            n++;
#endif
                            addNode(list, cur);
                        }
                    } else if (axis == AXIS_NAMESPACE) {
                        if (cur->type == XML_NAMESPACE_DECL) {
#ifdef DEBUG_STEP
                            n++;
#endif
                            xmlXPathNodeSetAddNs(list, ctxt->context->node, 
				                 (xmlNsPtr) cur);
                        }
                    } else {
                        if (cur->type == XML_ELEMENT_NODE) {
                            if (prefix == NULL) {
#ifdef DEBUG_STEP
                                n++;
#endif
                                addNode(list, cur);
                            } else if ((cur->ns != NULL) &&
                                       (xmlStrEqual(URI, cur->ns->href))) {
#ifdef DEBUG_STEP
                                n++;
#endif
                                addNode(list, cur);
                            }
                        }
                    }
                    break;
                case NODE_TEST_NS:{
                        TODO;
                        break;
                    }
                case NODE_TEST_NAME:
                    switch (cur->type) {
                        case XML_ELEMENT_NODE:
                            if (xmlStrEqual(name, cur->name)) {
                                if (prefix == NULL) {
                                    if (cur->ns == NULL) {
#ifdef DEBUG_STEP
                                        n++;
#endif
                                        addNode(list, cur);
                                    }
                                } else {
                                    if ((cur->ns != NULL) &&
                                        (xmlStrEqual(URI,
                                                     cur->ns->href))) {
#ifdef DEBUG_STEP
                                        n++;
#endif
                                        addNode(list, cur);
                                    }
                                }
                            }
                            break;
                        case XML_ATTRIBUTE_NODE:{
                                xmlAttrPtr attr = (xmlAttrPtr) cur;

                                if (xmlStrEqual(name, attr->name)) {
                                    if (prefix == NULL) {
                                        if ((attr->ns == NULL) ||
                                            (attr->ns->prefix == NULL)) {
#ifdef DEBUG_STEP
                                            n++;
#endif
                                            addNode(list,
                                                    (xmlNodePtr) attr);
                                        }
                                    } else {
                                        if ((attr->ns != NULL) &&
                                            (xmlStrEqual(URI,
                                                         attr->ns->
                                                         href))) {
#ifdef DEBUG_STEP
                                            n++;
#endif
                                            addNode(list,
                                                    (xmlNodePtr) attr);
                                        }
                                    }
                                }
                                break;
                            }
                        case XML_NAMESPACE_DECL:
                            if (cur->type == XML_NAMESPACE_DECL) {
                                xmlNsPtr ns = (xmlNsPtr) cur;

                                if ((ns->prefix != NULL) && (name != NULL)
                                    && (xmlStrEqual(ns->prefix, name))) {
#ifdef DEBUG_STEP
                                    n++;
#endif
				    xmlXPathNodeSetAddNs(list,
					ctxt->context->node, (xmlNsPtr) cur);
                                }
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                    break;
            }
        } while (cur != NULL);

        /*
         * If there is some predicate filtering do it now
         */
        if ((op->ch2 != -1) && (list != NULL) && (list->nodeNr > 0)) {
            xmlXPathObjectPtr obj2;

            valuePush(ctxt, xmlXPathWrapNodeSet(list));
            xmlXPathCompOpEval(ctxt, &ctxt->comp->steps[op->ch2]);
            CHECK_TYPE0(XPATH_NODESET);
            obj2 = valuePop(ctxt);
            list = obj2->nodesetval;
            obj2->nodesetval = NULL;
            xmlXPathFreeObject(obj2);
        }
        if (ret == NULL) {
            ret = list;
        } else {
            ret = mergeNodeSet(ret, list);
            xmlXPathFreeNodeSet(list);
        }
    }
    ctxt->context->node = tmp;
#ifdef DEBUG_STEP
    xmlGenericError(xmlGenericErrorContext,
                    "\nExamined %d nodes, found %d nodes at that step\n",
                    t, n);
#endif
    valuePush(ctxt, xmlXPathWrapNodeSet(ret));
    if ((obj->boolval) && (obj->user != NULL)) {
	ctxt->value->boolval = 1;
	ctxt->value->user = obj->user;
	obj->user = NULL;
	obj->boolval = 0;
    }
    xmlXPathFreeObject(obj);
    return(t);
}

/**
 * xmlXPathNodeCollectAndTestNth:
 * @ctxt:  the XPath Parser context
 * @op:  the XPath precompiled step operation
 * @indx:  the index to collect
 * @first:  pointer to the first element in document order
 * @last:  pointer to the last element in document order
 *
 * This is the function implementing a step: based on the current list
 * of nodes, it builds up a new list, looking at all nodes under that
 * axis and selecting them it also do the predicate filtering
 *
 * Pushes the new NodeSet resulting from the search.
 * Returns the number of node traversed
 */
static int
xmlXPathNodeCollectAndTestNth(xmlXPathParserContextPtr ctxt,
                              xmlXPathStepOpPtr op, int indx,
                              xmlNodePtr * first, xmlNodePtr * last)
{
    xmlXPathAxisVal axis = op->value;
    xmlXPathTestVal test = op->value2;
    xmlXPathTypeVal type = op->value3;
    const xmlChar *prefix = op->value4;
    const xmlChar *name = op->value5;
    const xmlChar *URI = NULL;
    int n = 0, t = 0;

    int i;
    xmlNodeSetPtr list;
    xmlXPathTraversalFunction next = NULL;
    void (*addNode) (xmlNodeSetPtr, xmlNodePtr);
    xmlNodePtr cur = NULL;
    xmlXPathObjectPtr obj;
    xmlNodeSetPtr nodelist;
    xmlNodePtr tmp;

    CHECK_TYPE0(XPATH_NODESET);
    obj = valuePop(ctxt);
    addNode = xmlXPathNodeSetAdd;
    if (prefix != NULL) {
        URI = xmlXPathNsLookup(ctxt->context, prefix);
        if (URI == NULL)
            XP_ERROR0(XPATH_UNDEF_PREFIX_ERROR);
    }
#ifdef DEBUG_STEP_NTH
    xmlGenericError(xmlGenericErrorContext, "new step : ");
    if (first != NULL) {
	if (*first != NULL)
	    xmlGenericError(xmlGenericErrorContext, "first = %s ",
		    (*first)->name);
	else
	    xmlGenericError(xmlGenericErrorContext, "first = NULL ");
    }
    if (last != NULL) {
	if (*last != NULL)
	    xmlGenericError(xmlGenericErrorContext, "last = %s ",
		    (*last)->name);
	else
	    xmlGenericError(xmlGenericErrorContext, "last = NULL ");
    }
#endif
    switch (axis) {
        case AXIS_ANCESTOR:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext, "axis 'ancestors' ");
#endif
            first = NULL;
            next = xmlXPathNextAncestor;
            break;
        case AXIS_ANCESTOR_OR_SELF:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext,
                            "axis 'ancestors-or-self' ");
#endif
            first = NULL;
            next = xmlXPathNextAncestorOrSelf;
            break;
        case AXIS_ATTRIBUTE:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext, "axis 'attributes' ");
#endif
            first = NULL;
	    last = NULL;
            next = xmlXPathNextAttribute;
            break;
        case AXIS_CHILD:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext, "axis 'child' ");
#endif
	    last = NULL;
            next = xmlXPathNextChild;
            break;
        case AXIS_DESCENDANT:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext, "axis 'descendant' ");
#endif
	    last = NULL;
            next = xmlXPathNextDescendant;
            break;
        case AXIS_DESCENDANT_OR_SELF:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext,
                            "axis 'descendant-or-self' ");
#endif
	    last = NULL;
            next = xmlXPathNextDescendantOrSelf;
            break;
        case AXIS_FOLLOWING:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext, "axis 'following' ");
#endif
	    last = NULL;
            next = xmlXPathNextFollowing;
            break;
        case AXIS_FOLLOWING_SIBLING:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext,
                            "axis 'following-siblings' ");
#endif
	    last = NULL;
            next = xmlXPathNextFollowingSibling;
            break;
        case AXIS_NAMESPACE:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext, "axis 'namespace' ");
#endif
	    last = NULL;
            first = NULL;
            next = (xmlXPathTraversalFunction) xmlXPathNextNamespace;
            break;
        case AXIS_PARENT:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext, "axis 'parent' ");
#endif
            first = NULL;
            next = xmlXPathNextParent;
            break;
        case AXIS_PRECEDING:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext, "axis 'preceding' ");
#endif
            first = NULL;
            next = xmlXPathNextPrecedingInternal;
            break;
        case AXIS_PRECEDING_SIBLING:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext,
                            "axis 'preceding-sibling' ");
#endif
            first = NULL;
            next = xmlXPathNextPrecedingSibling;
            break;
        case AXIS_SELF:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext, "axis 'self' ");
#endif
            first = NULL;
	    last = NULL;
            next = xmlXPathNextSelf;
            break;
    }
    if (next == NULL)
        return(0);

    nodelist = obj->nodesetval;
    if (nodelist == NULL) {
        xmlXPathFreeObject(obj);
        valuePush(ctxt, xmlXPathWrapNodeSet(NULL));
        return(0);
    }
    addNode = xmlXPathNodeSetAddUnique;
#ifdef DEBUG_STEP_NTH
    xmlGenericError(xmlGenericErrorContext,
                    " context contains %d nodes\n", nodelist->nodeNr);
    switch (test) {
        case NODE_TEST_NONE:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for none !!!\n");
            break;
        case NODE_TEST_TYPE:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for type %d\n", type);
            break;
        case NODE_TEST_PI:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for PI !!!\n");
            break;
        case NODE_TEST_ALL:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for *\n");
            break;
        case NODE_TEST_NS:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for namespace %s\n",
                            prefix);
            break;
        case NODE_TEST_NAME:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for name %s\n", name);
            if (prefix != NULL)
                xmlGenericError(xmlGenericErrorContext,
                                "           with namespace %s\n", prefix);
            break;
    }
    xmlGenericError(xmlGenericErrorContext, "Testing : ");
#endif
    /*
     * 2.3 Node Tests
     *  - For the attribute axis, the principal node type is attribute. 
     *  - For the namespace axis, the principal node type is namespace. 
     *  - For other axes, the principal node type is element. 
     *
     * A node test * is true for any node of the
     * principal node type. For example, child::* will
     * select all element children of the context node
     */
    tmp = ctxt->context->node;
    list = xmlXPathNodeSetCreate(NULL);
    for (i = 0; i < nodelist->nodeNr; i++) {
        ctxt->context->node = nodelist->nodeTab[i];

        cur = NULL;
        n = 0;
        do {
            cur = next(ctxt, cur);
            if (cur == NULL)
                break;
	    if ((first != NULL) && (*first == cur))
		break;
	    if (((t % 256) == 0) &&
	        (first != NULL) && (*first != NULL) &&
		(xmlXPathCmpNodes(*first, cur) >= 0))
		break;
	    if ((last != NULL) && (*last == cur))
		break;
	    if (((t % 256) == 0) &&
	        (last != NULL) && (*last != NULL) &&
		(xmlXPathCmpNodes(cur, *last) >= 0))
		break;
            t++;
            switch (test) {
                case NODE_TEST_NONE:
                    ctxt->context->node = tmp;
                    STRANGE return(0);
                case NODE_TEST_TYPE:
                    if ((cur->type == type) ||
                        ((type == NODE_TYPE_NODE) &&
                         ((cur->type == XML_DOCUMENT_NODE) ||
                          (cur->type == XML_HTML_DOCUMENT_NODE) ||
                          (cur->type == XML_ELEMENT_NODE) ||
                          (cur->type == XML_PI_NODE) ||
                          (cur->type == XML_COMMENT_NODE) ||
                          (cur->type == XML_CDATA_SECTION_NODE) ||
                          (cur->type == XML_TEXT_NODE))) ||
			((type == NODE_TYPE_TEXT) &&
			 (cur->type == XML_CDATA_SECTION_NODE))) {
                        n++;
                        if (n == indx)
                            addNode(list, cur);
                    }
                    break;
                case NODE_TEST_PI:
                    if (cur->type == XML_PI_NODE) {
                        if ((name != NULL) &&
                            (!xmlStrEqual(name, cur->name)))
                            break;
                        n++;
                        if (n == indx)
                            addNode(list, cur);
                    }
                    break;
                case NODE_TEST_ALL:
                    if (axis == AXIS_ATTRIBUTE) {
                        if (cur->type == XML_ATTRIBUTE_NODE) {
                            n++;
                            if (n == indx)
                                addNode(list, cur);
                        }
                    } else if (axis == AXIS_NAMESPACE) {
                        if (cur->type == XML_NAMESPACE_DECL) {
                            n++;
                            if (n == indx)
				xmlXPathNodeSetAddNs(list, ctxt->context->node, 
						     (xmlNsPtr) cur);
                        }
                    } else {
                        if (cur->type == XML_ELEMENT_NODE) {
                            if (prefix == NULL) {
                                n++;
                                if (n == indx)
                                    addNode(list, cur);
                            } else if ((cur->ns != NULL) &&
                                       (xmlStrEqual(URI, cur->ns->href))) {
                                n++;
                                if (n == indx)
                                    addNode(list, cur);
                            }
                        }
                    }
                    break;
                case NODE_TEST_NS:{
                        TODO;
                        break;
                    }
                case NODE_TEST_NAME:
                    switch (cur->type) {
                        case XML_ELEMENT_NODE:
                            if (xmlStrEqual(name, cur->name)) {
                                if (prefix == NULL) {
                                    if (cur->ns == NULL) {
                                        n++;
                                        if (n == indx)
                                            addNode(list, cur);
                                    }
                                } else {
                                    if ((cur->ns != NULL) &&
                                        (xmlStrEqual(URI,
                                                     cur->ns->href))) {
                                        n++;
                                        if (n == indx)
                                            addNode(list, cur);
                                    }
                                }
                            }
                            break;
                        case XML_ATTRIBUTE_NODE:{
                                xmlAttrPtr attr = (xmlAttrPtr) cur;

                                if (xmlStrEqual(name, attr->name)) {
                                    if (prefix == NULL) {
                                        if ((attr->ns == NULL) ||
                                            (attr->ns->prefix == NULL)) {
                                            n++;
                                            if (n == indx)
                                                addNode(list, cur);
                                        }
                                    } else {
                                        if ((attr->ns != NULL) &&
                                            (xmlStrEqual(URI,
                                                         attr->ns->
                                                         href))) {
                                            n++;
                                            if (n == indx)
                                                addNode(list, cur);
                                        }
                                    }
                                }
                                break;
                            }
                        case XML_NAMESPACE_DECL:
                            if (cur->type == XML_NAMESPACE_DECL) {
                                xmlNsPtr ns = (xmlNsPtr) cur;

                                if ((ns->prefix != NULL) && (name != NULL)
                                    && (xmlStrEqual(ns->prefix, name))) {
                                    n++;
                                    if (n == indx)
					xmlXPathNodeSetAddNs(list,
					   ctxt->context->node, (xmlNsPtr) cur);
                                }
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                    break;
            }
        } while (n < indx);
    }
    ctxt->context->node = tmp;
#ifdef DEBUG_STEP_NTH
    xmlGenericError(xmlGenericErrorContext,
                    "\nExamined %d nodes, found %d nodes at that step\n",
                    t, list->nodeNr);
#endif
    valuePush(ctxt, xmlXPathWrapNodeSet(list));
    if ((obj->boolval) && (obj->user != NULL)) {
	ctxt->value->boolval = 1;
	ctxt->value->user = obj->user;
	obj->user = NULL;
	obj->boolval = 0;
    }
    xmlXPathFreeObject(obj);
    return(t);
}

/**
 * xmlXPathCompOpEvalFirst:
 * @ctxt:  the XPath parser context with the compiled expression
 * @op:  an XPath compiled operation
 * @first:  the first elem found so far
 *
 * Evaluate the Precompiled XPath operation searching only the first
 * element in document order
 *
 * Returns the number of examined objects.
 */
static int
xmlXPathCompOpEvalFirst(xmlXPathParserContextPtr ctxt,
                        xmlXPathStepOpPtr op, xmlNodePtr * first)
{
    int total = 0, cur;
    xmlXPathCompExprPtr comp;
    xmlXPathObjectPtr arg1, arg2;

    CHECK_ERROR0;
    comp = ctxt->comp;
    switch (op->op) {
        case XPATH_OP_END:
            return (0);
        case XPATH_OP_UNION:
            total =
                xmlXPathCompOpEvalFirst(ctxt, &comp->steps[op->ch1],
                                        first);
	    CHECK_ERROR0;
            if ((ctxt->value != NULL)
                && (ctxt->value->type == XPATH_NODESET)
                && (ctxt->value->nodesetval != NULL)
                && (ctxt->value->nodesetval->nodeNr >= 1)) {
                /*
                 * limit tree traversing to first node in the result
                 */
                xmlXPathNodeSetSort(ctxt->value->nodesetval);
                *first = ctxt->value->nodesetval->nodeTab[0];
            }
            cur =
                xmlXPathCompOpEvalFirst(ctxt, &comp->steps[op->ch2],
                                        first);
	    CHECK_ERROR0;
            CHECK_TYPE0(XPATH_NODESET);
            arg2 = valuePop(ctxt);

            CHECK_TYPE0(XPATH_NODESET);
            arg1 = valuePop(ctxt);

            arg1->nodesetval = xmlXPathNodeSetMerge(arg1->nodesetval,
                                                    arg2->nodesetval);
            valuePush(ctxt, arg1);
            xmlXPathFreeObject(arg2);
            /* optimizer */
	    if (total > cur)
		xmlXPathCompSwap(op);
            return (total + cur);
        case XPATH_OP_ROOT:
            xmlXPathRoot(ctxt);
            return (0);
        case XPATH_OP_NODE:
            if (op->ch1 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
            if (op->ch2 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    CHECK_ERROR0;
            valuePush(ctxt, xmlXPathNewNodeSet(ctxt->context->node));
            return (total);
        case XPATH_OP_RESET:
            if (op->ch1 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
            if (op->ch2 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    CHECK_ERROR0;
            ctxt->context->node = NULL;
            return (total);
        case XPATH_OP_COLLECT:{
                if (op->ch1 == -1)
                    return (total);

                total = xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
		CHECK_ERROR0;

                /*
                 * Optimization for [n] selection where n is a number
                 */
                if ((op->ch2 != -1) &&
                    (comp->steps[op->ch2].op == XPATH_OP_PREDICATE) &&
                    (comp->steps[op->ch2].ch1 == -1) &&
                    (comp->steps[op->ch2].ch2 != -1) &&
                    (comp->steps[comp->steps[op->ch2].ch2].op ==
                     XPATH_OP_VALUE)) {
                    xmlXPathObjectPtr val;

                    val = comp->steps[comp->steps[op->ch2].ch2].value4;
                    if ((val != NULL) && (val->type == XPATH_NUMBER)) {
                        int indx = (int) val->floatval;

                        if (val->floatval == (float) indx) {
                            xmlXPathNodeCollectAndTestNth(ctxt, op, indx,
                                                          first, NULL);
                            return (total);
                        }
                    }
                }
                total += xmlXPathNodeCollectAndTest(ctxt, op, first, NULL);
                return (total);
            }
        case XPATH_OP_VALUE:
            valuePush(ctxt,
                      xmlXPathObjectCopy((xmlXPathObjectPtr) op->value4));
            return (0);
        case XPATH_OP_SORT:
            if (op->ch1 != -1)
                total +=
                    xmlXPathCompOpEvalFirst(ctxt, &comp->steps[op->ch1],
                                            first);
	    CHECK_ERROR0;
            if ((ctxt->value != NULL)
                && (ctxt->value->type == XPATH_NODESET)
                && (ctxt->value->nodesetval != NULL))
                xmlXPathNodeSetSort(ctxt->value->nodesetval);
            return (total);
        default:
            return (xmlXPathCompOpEval(ctxt, op));
    }
}

/**
 * xmlXPathCompOpEvalLast:
 * @ctxt:  the XPath parser context with the compiled expression
 * @op:  an XPath compiled operation
 * @last:  the last elem found so far
 *
 * Evaluate the Precompiled XPath operation searching only the last
 * element in document order
 *
 * Returns the number of node traversed
 */
static int
xmlXPathCompOpEvalLast(xmlXPathParserContextPtr ctxt, xmlXPathStepOpPtr op,
                       xmlNodePtr * last)
{
    int total = 0, cur;
    xmlXPathCompExprPtr comp;
    xmlXPathObjectPtr arg1, arg2;

    CHECK_ERROR0;
    comp = ctxt->comp;
    switch (op->op) {
        case XPATH_OP_END:
            return (0);
        case XPATH_OP_UNION:
            total =
                xmlXPathCompOpEvalLast(ctxt, &comp->steps[op->ch1], last);
	    CHECK_ERROR0;
            if ((ctxt->value != NULL)
                && (ctxt->value->type == XPATH_NODESET)
                && (ctxt->value->nodesetval != NULL)
                && (ctxt->value->nodesetval->nodeNr >= 1)) {
                /*
                 * limit tree traversing to first node in the result
                 */
                xmlXPathNodeSetSort(ctxt->value->nodesetval);
                *last =
                    ctxt->value->nodesetval->nodeTab[ctxt->value->
                                                     nodesetval->nodeNr -
                                                     1];
            }
            cur =
                xmlXPathCompOpEvalLast(ctxt, &comp->steps[op->ch2], last);
	    CHECK_ERROR0;
            if ((ctxt->value != NULL)
                && (ctxt->value->type == XPATH_NODESET)
                && (ctxt->value->nodesetval != NULL)
                && (ctxt->value->nodesetval->nodeNr >= 1)) {
            }
            CHECK_TYPE0(XPATH_NODESET);
            arg2 = valuePop(ctxt);

            CHECK_TYPE0(XPATH_NODESET);
            arg1 = valuePop(ctxt);

            arg1->nodesetval = xmlXPathNodeSetMerge(arg1->nodesetval,
                                                    arg2->nodesetval);
            valuePush(ctxt, arg1);
            xmlXPathFreeObject(arg2);
            /* optimizer */
	    if (total > cur)
		xmlXPathCompSwap(op);
            return (total + cur);
        case XPATH_OP_ROOT:
            xmlXPathRoot(ctxt);
            return (0);
        case XPATH_OP_NODE:
            if (op->ch1 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
            if (op->ch2 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    CHECK_ERROR0;
            valuePush(ctxt, xmlXPathNewNodeSet(ctxt->context->node));
            return (total);
        case XPATH_OP_RESET:
            if (op->ch1 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
            if (op->ch2 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    CHECK_ERROR0;
            ctxt->context->node = NULL;
            return (total);
        case XPATH_OP_COLLECT:{
                if (op->ch1 == -1)
                    return (0);

                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
		CHECK_ERROR0;

                /*
                 * Optimization for [n] selection where n is a number
                 */
                if ((op->ch2 != -1) &&
                    (comp->steps[op->ch2].op == XPATH_OP_PREDICATE) &&
                    (comp->steps[op->ch2].ch1 == -1) &&
                    (comp->steps[op->ch2].ch2 != -1) &&
                    (comp->steps[comp->steps[op->ch2].ch2].op ==
                     XPATH_OP_VALUE)) {
                    xmlXPathObjectPtr val;

                    val = comp->steps[comp->steps[op->ch2].ch2].value4;
                    if ((val != NULL) && (val->type == XPATH_NUMBER)) {
                        int indx = (int) val->floatval;

                        if (val->floatval == (float) indx) {
                            total +=
                                xmlXPathNodeCollectAndTestNth(ctxt, op,
                                                              indx, NULL,
                                                              last);
                            return (total);
                        }
                    }
                }
                total += xmlXPathNodeCollectAndTest(ctxt, op, NULL, last);
                return (total);
            }
        case XPATH_OP_VALUE:
            valuePush(ctxt,
                      xmlXPathObjectCopy((xmlXPathObjectPtr) op->value4));
            return (0);
        case XPATH_OP_SORT:
            if (op->ch1 != -1)
                total +=
                    xmlXPathCompOpEvalLast(ctxt, &comp->steps[op->ch1],
                                           last);
	    CHECK_ERROR0;
            if ((ctxt->value != NULL)
                && (ctxt->value->type == XPATH_NODESET)
                && (ctxt->value->nodesetval != NULL))
                xmlXPathNodeSetSort(ctxt->value->nodesetval);
            return (total);
        default:
            return (xmlXPathCompOpEval(ctxt, op));
    }
}

/**
 * xmlXPathCompOpEval:
 * @ctxt:  the XPath parser context with the compiled expression
 * @op:  an XPath compiled operation
 *
 * Evaluate the Precompiled XPath operation
 * Returns the number of node traversed
 */
static int
xmlXPathCompOpEval(xmlXPathParserContextPtr ctxt, xmlXPathStepOpPtr op)
{
    int total = 0;
    int equal, ret;
    xmlXPathCompExprPtr comp;
    xmlXPathObjectPtr arg1, arg2;
    xmlNodePtr bak;
    xmlDocPtr bakd;
    int pp;
    int cs;

    CHECK_ERROR0;
    comp = ctxt->comp;
    switch (op->op) {
        case XPATH_OP_END:
            return (0);
        case XPATH_OP_AND:
	    bakd = ctxt->context->doc;
	    bak = ctxt->context->node;
	    pp = ctxt->context->proximityPosition;
	    cs = ctxt->context->contextSize;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
            xmlXPathBooleanFunction(ctxt, 1);
            if ((ctxt->value == NULL) || (ctxt->value->boolval == 0))
                return (total);
            arg2 = valuePop(ctxt);
	    ctxt->context->doc = bakd;
	    ctxt->context->node = bak;
	    ctxt->context->proximityPosition = pp;
	    ctxt->context->contextSize = cs;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    if (ctxt->error) {
		xmlXPathFreeObject(arg2);
		return(0);
	    }
            xmlXPathBooleanFunction(ctxt, 1);
            arg1 = valuePop(ctxt);
            arg1->boolval &= arg2->boolval;
            valuePush(ctxt, arg1);
            xmlXPathFreeObject(arg2);
            return (total);
        case XPATH_OP_OR:
	    bakd = ctxt->context->doc;
	    bak = ctxt->context->node;
	    pp = ctxt->context->proximityPosition;
	    cs = ctxt->context->contextSize;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
            xmlXPathBooleanFunction(ctxt, 1);
            if ((ctxt->value == NULL) || (ctxt->value->boolval == 1))
                return (total);
            arg2 = valuePop(ctxt);
	    ctxt->context->doc = bakd;
	    ctxt->context->node = bak;
	    ctxt->context->proximityPosition = pp;
	    ctxt->context->contextSize = cs;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    if (ctxt->error) {
		xmlXPathFreeObject(arg2);
		return(0);
	    }
            xmlXPathBooleanFunction(ctxt, 1);
            arg1 = valuePop(ctxt);
            arg1->boolval |= arg2->boolval;
            valuePush(ctxt, arg1);
            xmlXPathFreeObject(arg2);
            return (total);
        case XPATH_OP_EQUAL:
	    bakd = ctxt->context->doc;
	    bak = ctxt->context->node;
	    pp = ctxt->context->proximityPosition;
	    cs = ctxt->context->contextSize;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
	    ctxt->context->doc = bakd;
	    ctxt->context->node = bak;
	    ctxt->context->proximityPosition = pp;
	    ctxt->context->contextSize = cs;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    CHECK_ERROR0;
	    if (op->value)
        	equal = xmlXPathEqualValues(ctxt);
	    else
		equal = xmlXPathNotEqualValues(ctxt);
	    valuePush(ctxt, xmlXPathNewBoolean(equal));
            return (total);
        case XPATH_OP_CMP:
	    bakd = ctxt->context->doc;
	    bak = ctxt->context->node;
	    pp = ctxt->context->proximityPosition;
	    cs = ctxt->context->contextSize;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
	    ctxt->context->doc = bakd;
	    ctxt->context->node = bak;
	    ctxt->context->proximityPosition = pp;
	    ctxt->context->contextSize = cs;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    CHECK_ERROR0;
            ret = xmlXPathCompareValues(ctxt, op->value, op->value2);
            valuePush(ctxt, xmlXPathNewBoolean(ret));
            return (total);
        case XPATH_OP_PLUS:
	    bakd = ctxt->context->doc;
	    bak = ctxt->context->node;
	    pp = ctxt->context->proximityPosition;
	    cs = ctxt->context->contextSize;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
            if (op->ch2 != -1) {
		ctxt->context->doc = bakd;
		ctxt->context->node = bak;
		ctxt->context->proximityPosition = pp;
		ctxt->context->contextSize = cs;
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    }
	    CHECK_ERROR0;
            if (op->value == 0)
                xmlXPathSubValues(ctxt);
            else if (op->value == 1)
                xmlXPathAddValues(ctxt);
            else if (op->value == 2)
                xmlXPathValueFlipSign(ctxt);
            else if (op->value == 3) {
                CAST_TO_NUMBER;
                CHECK_TYPE0(XPATH_NUMBER);
            }
            return (total);
        case XPATH_OP_MULT:
	    bakd = ctxt->context->doc;
	    bak = ctxt->context->node;
	    pp = ctxt->context->proximityPosition;
	    cs = ctxt->context->contextSize;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
	    ctxt->context->doc = bakd;
	    ctxt->context->node = bak;
	    ctxt->context->proximityPosition = pp;
	    ctxt->context->contextSize = cs;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    CHECK_ERROR0;
            if (op->value == 0)
                xmlXPathMultValues(ctxt);
            else if (op->value == 1)
                xmlXPathDivValues(ctxt);
            else if (op->value == 2)
                xmlXPathModValues(ctxt);
            return (total);
        case XPATH_OP_UNION:
	    bakd = ctxt->context->doc;
	    bak = ctxt->context->node;
	    pp = ctxt->context->proximityPosition;
	    cs = ctxt->context->contextSize;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
	    ctxt->context->doc = bakd;
	    ctxt->context->node = bak;
	    ctxt->context->proximityPosition = pp;
	    ctxt->context->contextSize = cs;
            total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    CHECK_ERROR0;
            CHECK_TYPE0(XPATH_NODESET);
            arg2 = valuePop(ctxt);

            CHECK_TYPE0(XPATH_NODESET);
            arg1 = valuePop(ctxt);

            arg1->nodesetval = xmlXPathNodeSetMerge(arg1->nodesetval,
                                                    arg2->nodesetval);
            valuePush(ctxt, arg1);
            xmlXPathFreeObject(arg2);
            return (total);
        case XPATH_OP_ROOT:
            xmlXPathRoot(ctxt);
            return (total);
        case XPATH_OP_NODE:
            if (op->ch1 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
            if (op->ch2 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    CHECK_ERROR0;
            valuePush(ctxt, xmlXPathNewNodeSet(ctxt->context->node));
            return (total);
        case XPATH_OP_RESET:
            if (op->ch1 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
            if (op->ch2 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    CHECK_ERROR0;
            ctxt->context->node = NULL;
            return (total);
        case XPATH_OP_COLLECT:{
                if (op->ch1 == -1)
                    return (total);

                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
		CHECK_ERROR0;

                /*
                 * Optimization for [n] selection where n is a number
                 */
                if ((op->ch2 != -1) &&
                    (comp->steps[op->ch2].op == XPATH_OP_PREDICATE) &&
                    (comp->steps[op->ch2].ch1 == -1) &&
                    (comp->steps[op->ch2].ch2 != -1) &&
                    (comp->steps[comp->steps[op->ch2].ch2].op ==
                     XPATH_OP_VALUE)) {
                    xmlXPathObjectPtr val;

                    val = comp->steps[comp->steps[op->ch2].ch2].value4;
                    if ((val != NULL) && (val->type == XPATH_NUMBER)) {
                        int indx = (int) val->floatval;

                        if (val->floatval == (float) indx) {
                            total +=
                                xmlXPathNodeCollectAndTestNth(ctxt, op,
                                                              indx, NULL,
                                                              NULL);
                            return (total);
                        }
                    }
                }
                total += xmlXPathNodeCollectAndTest(ctxt, op, NULL, NULL);
                return (total);
            }
        case XPATH_OP_VALUE:
            valuePush(ctxt,
                      xmlXPathObjectCopy((xmlXPathObjectPtr) op->value4));
            return (total);
        case XPATH_OP_VARIABLE:{
		xmlXPathObjectPtr val;

                if (op->ch1 != -1)
                    total +=
                        xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
                if (op->value5 == NULL) {
		    val = xmlXPathVariableLookup(ctxt->context, op->value4);
		    if (val == NULL) {
			ctxt->error = XPATH_UNDEF_VARIABLE_ERROR;
			return(0);
		    }
                    valuePush(ctxt, val);
		} else {
                    const xmlChar *URI;

                    URI = xmlXPathNsLookup(ctxt->context, op->value5);
                    if (URI == NULL) {
                        xmlGenericError(xmlGenericErrorContext,
                                        "xmlXPathCompOpEval: variable %s bound to undefined prefix %s\n",
                                        op->value4, op->value5);
                        return (total);
                    }
		    val = xmlXPathVariableLookupNS(ctxt->context,
                                                       op->value4, URI);
		    if (val == NULL) {
			ctxt->error = XPATH_UNDEF_VARIABLE_ERROR;
			return(0);
		    }
                    valuePush(ctxt, val);
                }
                return (total);
            }
        case XPATH_OP_FUNCTION:{
                xmlXPathFunction func;
                const xmlChar *oldFunc, *oldFuncURI;
		int i;

                if (op->ch1 != -1)
                    total +=
                        xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
		if (ctxt->valueNr < op->value) {
		    xmlGenericError(xmlGenericErrorContext,
			    "xmlXPathCompOpEval: parameter error\n");
		    ctxt->error = XPATH_INVALID_OPERAND;
		    return (total);
		}
		for (i = 0; i < op->value; i++)
		    if (ctxt->valueTab[(ctxt->valueNr - 1) - i] == NULL) {
			xmlGenericError(xmlGenericErrorContext,
				"xmlXPathCompOpEval: parameter error\n");
			ctxt->error = XPATH_INVALID_OPERAND;
			return (total);
		    }
                if (op->cache != NULL)
                    func = (xmlXPathFunction) op->cache;
                else {
                    const xmlChar *URI = NULL;

                    if (op->value5 == NULL)
                        func =
                            xmlXPathFunctionLookup(ctxt->context,
                                                   op->value4);
                    else {
                        URI = xmlXPathNsLookup(ctxt->context, op->value5);
                        if (URI == NULL) {
                            xmlGenericError(xmlGenericErrorContext,
                                            "xmlXPathCompOpEval: function %s bound to undefined prefix %s\n",
                                            op->value4, op->value5);
                            return (total);
                        }
                        func = xmlXPathFunctionLookupNS(ctxt->context,
                                                        op->value4, URI);
                    }
                    if (func == NULL) {
                        xmlGenericError(xmlGenericErrorContext,
                                        "xmlXPathCompOpEval: function %s not found\n",
                                        op->value4);
                        XP_ERROR0(XPATH_UNKNOWN_FUNC_ERROR);
                    }
                    op->cache = (void *) func;
                    op->cacheURI = (void *) URI;
                }
                oldFunc = ctxt->context->function;
                oldFuncURI = ctxt->context->functionURI;
                ctxt->context->function = op->value4;
                ctxt->context->functionURI = op->cacheURI;
                func(ctxt, op->value);
                ctxt->context->function = oldFunc;
                ctxt->context->functionURI = oldFuncURI;
                return (total);
            }
        case XPATH_OP_ARG:
	    bakd = ctxt->context->doc;
	    bak = ctxt->context->node;
            if (op->ch1 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    ctxt->context->doc = bakd;
	    ctxt->context->node = bak;
	    CHECK_ERROR0;
            if (op->ch2 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch2]);
	    ctxt->context->doc = bakd;
	    ctxt->context->node = bak;
	    CHECK_ERROR0;
            return (total);
        case XPATH_OP_PREDICATE:
        case XPATH_OP_FILTER:{
                xmlXPathObjectPtr res;
                xmlXPathObjectPtr obj, tmp;
                xmlNodeSetPtr newset = NULL;
                xmlNodeSetPtr oldset;
                xmlNodePtr oldnode;
                int i;

                /*
                 * Optimization for ()[1] selection i.e. the first elem
                 */
                if ((op->ch1 != -1) && (op->ch2 != -1) &&
                    (comp->steps[op->ch1].op == XPATH_OP_SORT) &&
                    (comp->steps[op->ch2].op == XPATH_OP_VALUE)) {
                    xmlXPathObjectPtr val;

                    val = comp->steps[op->ch2].value4;
                    if ((val != NULL) && (val->type == XPATH_NUMBER) &&
                        (val->floatval == 1.0)) {
                        xmlNodePtr first = NULL;

                        total +=
                            xmlXPathCompOpEvalFirst(ctxt,
                                                    &comp->steps[op->ch1],
                                                    &first);
			CHECK_ERROR0;
                        /*
                         * The nodeset should be in document order,
                         * Keep only the first value
                         */
                        if ((ctxt->value != NULL) &&
                            (ctxt->value->type == XPATH_NODESET) &&
                            (ctxt->value->nodesetval != NULL) &&
                            (ctxt->value->nodesetval->nodeNr > 1))
                            ctxt->value->nodesetval->nodeNr = 1;
                        return (total);
                    }
                }
                /*
                 * Optimization for ()[last()] selection i.e. the last elem
                 */
                if ((op->ch1 != -1) && (op->ch2 != -1) &&
                    (comp->steps[op->ch1].op == XPATH_OP_SORT) &&
                    (comp->steps[op->ch2].op == XPATH_OP_SORT)) {
                    int f = comp->steps[op->ch2].ch1;

                    if ((f != -1) &&
                        (comp->steps[f].op == XPATH_OP_FUNCTION) &&
                        (comp->steps[f].value5 == NULL) &&
                        (comp->steps[f].value == 0) &&
                        (comp->steps[f].value4 != NULL) &&
                        (xmlStrEqual
                         (comp->steps[f].value4, BAD_CAST "last"))) {
                        xmlNodePtr last = NULL;

                        total +=
                            xmlXPathCompOpEvalLast(ctxt,
                                                   &comp->steps[op->ch1],
                                                   &last);
			CHECK_ERROR0;
                        /*
                         * The nodeset should be in document order,
                         * Keep only the last value
                         */
                        if ((ctxt->value != NULL) &&
                            (ctxt->value->type == XPATH_NODESET) &&
                            (ctxt->value->nodesetval != NULL) &&
                            (ctxt->value->nodesetval->nodeTab != NULL) &&
                            (ctxt->value->nodesetval->nodeNr > 1)) {
                            ctxt->value->nodesetval->nodeTab[0] =
                                ctxt->value->nodesetval->nodeTab[ctxt->
                                                                 value->
                                                                 nodesetval->
                                                                 nodeNr -
                                                                 1];
                            ctxt->value->nodesetval->nodeNr = 1;
                        }
                        return (total);
                    }
                }

                if (op->ch1 != -1)
                    total +=
                        xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
		CHECK_ERROR0;
                if (op->ch2 == -1)
                    return (total);
                if (ctxt->value == NULL)
                    return (total);

                oldnode = ctxt->context->node;

#ifdef LIBXML_XPTR_ENABLED
                /*
                 * Hum are we filtering the result of an XPointer expression
                 */
                if (ctxt->value->type == XPATH_LOCATIONSET) {
                    xmlLocationSetPtr newlocset = NULL;
                    xmlLocationSetPtr oldlocset;

                    /*
                     * Extract the old locset, and then evaluate the result of the
                     * expression for all the element in the locset. use it to grow
                     * up a new locset.
                     */
                    CHECK_TYPE0(XPATH_LOCATIONSET);
                    obj = valuePop(ctxt);
                    oldlocset = obj->user;
                    ctxt->context->node = NULL;

                    if ((oldlocset == NULL) || (oldlocset->locNr == 0)) {
                        ctxt->context->contextSize = 0;
                        ctxt->context->proximityPosition = 0;
                        if (op->ch2 != -1)
                            total +=
                                xmlXPathCompOpEval(ctxt,
                                                   &comp->steps[op->ch2]);
                        res = valuePop(ctxt);
                        if (res != NULL)
                            xmlXPathFreeObject(res);
                        valuePush(ctxt, obj);
                        CHECK_ERROR0;
                        return (total);
                    }
                    newlocset = xmlXPtrLocationSetCreate(NULL);

                    for (i = 0; i < oldlocset->locNr; i++) {
                        /*
                         * Run the evaluation with a node list made of a
                         * single item in the nodelocset.
                         */
                        ctxt->context->node = oldlocset->locTab[i]->user;
                        tmp = xmlXPathNewNodeSet(ctxt->context->node);
                        valuePush(ctxt, tmp);
                        ctxt->context->contextSize = oldlocset->locNr;
                        ctxt->context->proximityPosition = i + 1;

                        if (op->ch2 != -1)
                            total +=
                                xmlXPathCompOpEval(ctxt,
                                                   &comp->steps[op->ch2]);
                        CHECK_ERROR0;

                        /*
                         * The result of the evaluation need to be tested to
                         * decided whether the filter succeeded or not
                         */
                        res = valuePop(ctxt);
                        if (xmlXPathEvaluatePredicateResult(ctxt, res)) {
                            xmlXPtrLocationSetAdd(newlocset,
                                                  xmlXPathObjectCopy
                                                  (oldlocset->locTab[i]));
                        }

                        /*
                         * Cleanup
                         */
                        if (res != NULL)
                            xmlXPathFreeObject(res);
                        if (ctxt->value == tmp) {
                            res = valuePop(ctxt);
                            xmlXPathFreeObject(res);
                        }

                        ctxt->context->node = NULL;
                    }

                    /*
                     * The result is used as the new evaluation locset.
                     */
                    xmlXPathFreeObject(obj);
                    ctxt->context->node = NULL;
                    ctxt->context->contextSize = -1;
                    ctxt->context->proximityPosition = -1;
                    valuePush(ctxt, xmlXPtrWrapLocationSet(newlocset));
                    ctxt->context->node = oldnode;
                    return (total);
                }
#endif /* LIBXML_XPTR_ENABLED */

                /*
                 * Extract the old set, and then evaluate the result of the
                 * expression for all the element in the set. use it to grow
                 * up a new set.
                 */
                CHECK_TYPE0(XPATH_NODESET);
                obj = valuePop(ctxt);
                oldset = obj->nodesetval;

                oldnode = ctxt->context->node;
                ctxt->context->node = NULL;

                if ((oldset == NULL) || (oldset->nodeNr == 0)) {
                    ctxt->context->contextSize = 0;
                    ctxt->context->proximityPosition = 0;
                    if (op->ch2 != -1)
                        total +=
                            xmlXPathCompOpEval(ctxt,
                                               &comp->steps[op->ch2]);
		    CHECK_ERROR0;
                    res = valuePop(ctxt);
                    if (res != NULL)
                        xmlXPathFreeObject(res);
                    valuePush(ctxt, obj);
                    ctxt->context->node = oldnode;
                    CHECK_ERROR0;
                } else {
                    /*
                     * Initialize the new set.
                     */
                    newset = xmlXPathNodeSetCreate(NULL);

                    for (i = 0; i < oldset->nodeNr; i++) {
                        /*
                         * Run the evaluation with a node list made of
                         * a single item in the nodeset.
                         */
                        ctxt->context->node = oldset->nodeTab[i];
                        tmp = xmlXPathNewNodeSet(ctxt->context->node);
                        valuePush(ctxt, tmp);
                        ctxt->context->contextSize = oldset->nodeNr;
                        ctxt->context->proximityPosition = i + 1;

                        if (op->ch2 != -1)
                            total +=
                                xmlXPathCompOpEval(ctxt,
                                                   &comp->steps[op->ch2]);
                        CHECK_ERROR0;

                        /*
                         * The result of the evaluation need to be tested to
                         * decided whether the filter succeeded or not
                         */
                        res = valuePop(ctxt);
                        if (xmlXPathEvaluatePredicateResult(ctxt, res)) {
                            xmlXPathNodeSetAdd(newset, oldset->nodeTab[i]);
                        }

                        /*
                         * Cleanup
                         */
                        if (res != NULL)
                            xmlXPathFreeObject(res);
                        if (ctxt->value == tmp) {
                            res = valuePop(ctxt);
                            xmlXPathFreeObject(res);
                        }

                        ctxt->context->node = NULL;
                    }

                    /*
                     * The result is used as the new evaluation set.
                     */
                    xmlXPathFreeObject(obj);
                    ctxt->context->node = NULL;
                    ctxt->context->contextSize = -1;
                    ctxt->context->proximityPosition = -1;
                    valuePush(ctxt, xmlXPathWrapNodeSet(newset));
                }
                ctxt->context->node = oldnode;
                return (total);
            }
        case XPATH_OP_SORT:
            if (op->ch1 != -1)
                total += xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
	    CHECK_ERROR0;
            if ((ctxt->value != NULL) &&
                (ctxt->value->type == XPATH_NODESET) &&
                (ctxt->value->nodesetval != NULL))
                xmlXPathNodeSetSort(ctxt->value->nodesetval);
            return (total);
#ifdef LIBXML_XPTR_ENABLED
        case XPATH_OP_RANGETO:{
                xmlXPathObjectPtr range;
                xmlXPathObjectPtr res, obj;
                xmlXPathObjectPtr tmp;
                xmlLocationSetPtr newset = NULL;
                xmlNodeSetPtr oldset;
                int i;

                if (op->ch1 != -1)
                    total +=
                        xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
                if (op->ch2 == -1)
                    return (total);

                CHECK_TYPE0(XPATH_NODESET);
                obj = valuePop(ctxt);
                oldset = obj->nodesetval;
                ctxt->context->node = NULL;

                newset = xmlXPtrLocationSetCreate(NULL);

                if (oldset != NULL) {
                    for (i = 0; i < oldset->nodeNr; i++) {
                        /*
                         * Run the evaluation with a node list made of a single item
                         * in the nodeset.
                         */
                        ctxt->context->node = oldset->nodeTab[i];
                        tmp = xmlXPathNewNodeSet(ctxt->context->node);
                        valuePush(ctxt, tmp);

                        if (op->ch2 != -1)
                            total +=
                                xmlXPathCompOpEval(ctxt,
                                                   &comp->steps[op->ch2]);
                        CHECK_ERROR0;

                        /*
                         * The result of the evaluation need to be tested to
                         * decided whether the filter succeeded or not
                         */
                        res = valuePop(ctxt);
                        range =
                            xmlXPtrNewRangeNodeObject(oldset->nodeTab[i],
                                                      res);
                        if (range != NULL) {
                            xmlXPtrLocationSetAdd(newset, range);
                        }

                        /*
                         * Cleanup
                         */
                        if (res != NULL)
                            xmlXPathFreeObject(res);
                        if (ctxt->value == tmp) {
                            res = valuePop(ctxt);
                            xmlXPathFreeObject(res);
                        }

                        ctxt->context->node = NULL;
                    }
                }

                /*
                 * The result is used as the new evaluation set.
                 */
                xmlXPathFreeObject(obj);
                ctxt->context->node = NULL;
                ctxt->context->contextSize = -1;
                ctxt->context->proximityPosition = -1;
                valuePush(ctxt, xmlXPtrWrapLocationSet(newset));
                return (total);
            }
#endif /* LIBXML_XPTR_ENABLED */
    }
    xmlGenericError(xmlGenericErrorContext,
                    "XPath: unknown precompiled operation %d\n", op->op);
    return (total);
}

/**
 * xmlXPathRunEval:
 * @ctxt:  the XPath parser context with the compiled expression
 *
 * Evaluate the Precompiled XPath expression in the given context.
 */
static void
xmlXPathRunEval(xmlXPathParserContextPtr ctxt) {
    xmlXPathCompExprPtr comp;

    if ((ctxt == NULL) || (ctxt->comp == NULL))
	return;

    if (ctxt->valueTab == NULL) {
	/* Allocate the value stack */
	ctxt->valueTab = (xmlXPathObjectPtr *) 
			 xmlMalloc(10 * sizeof(xmlXPathObjectPtr));
	if (ctxt->valueTab == NULL) {
	    xmlFree(ctxt);
	}
	ctxt->valueNr = 0;
	ctxt->valueMax = 10;
	ctxt->value = NULL;
    }
    comp = ctxt->comp;
    if(comp->last < 0) {
	xmlGenericError(xmlGenericErrorContext,
	    "xmlXPathRunEval: last is less than zero\n");
	return;
    }
    xmlXPathCompOpEval(ctxt, &comp->steps[comp->last]);
}

/************************************************************************
 *									*
 * 			Public interfaces				*
 *									*
 ************************************************************************/

/**
 * xmlXPathEvalPredicate:
 * @ctxt:  the XPath context
 * @res:  the Predicate Expression evaluation result
 *
 * Evaluate a predicate result for the current node.
 * A PredicateExpr is evaluated by evaluating the Expr and converting
 * the result to a boolean. If the result is a number, the result will
 * be converted to true if the number is equal to the position of the
 * context node in the context node list (as returned by the position
 * function) and will be converted to false otherwise; if the result
 * is not a number, then the result will be converted as if by a call
 * to the boolean function. 
 *
 * Returns 1 if predicate is true, 0 otherwise
 */
int
xmlXPathEvalPredicate(xmlXPathContextPtr ctxt, xmlXPathObjectPtr res) {
    if (res == NULL) return(0);
    switch (res->type) {
        case XPATH_BOOLEAN:
	    return(res->boolval);
        case XPATH_NUMBER:
	    return(res->floatval == ctxt->proximityPosition);
        case XPATH_NODESET:
        case XPATH_XSLT_TREE:
	    if (res->nodesetval == NULL)
		return(0);
	    return(res->nodesetval->nodeNr != 0);
        case XPATH_STRING:
	    return((res->stringval != NULL) &&
	           (xmlStrlen(res->stringval) != 0));
        default:
	    STRANGE
    }
    return(0);
}

/**
 * xmlXPathEvaluatePredicateResult:
 * @ctxt:  the XPath Parser context
 * @res:  the Predicate Expression evaluation result
 *
 * Evaluate a predicate result for the current node.
 * A PredicateExpr is evaluated by evaluating the Expr and converting
 * the result to a boolean. If the result is a number, the result will
 * be converted to true if the number is equal to the position of the
 * context node in the context node list (as returned by the position
 * function) and will be converted to false otherwise; if the result
 * is not a number, then the result will be converted as if by a call
 * to the boolean function. 
 *
 * Returns 1 if predicate is true, 0 otherwise
 */
int
xmlXPathEvaluatePredicateResult(xmlXPathParserContextPtr ctxt, 
                                xmlXPathObjectPtr res) {
    if (res == NULL) return(0);
    switch (res->type) {
        case XPATH_BOOLEAN:
	    return(res->boolval);
        case XPATH_NUMBER:
	    return(res->floatval == ctxt->context->proximityPosition);
        case XPATH_NODESET:
        case XPATH_XSLT_TREE:
	    if (res->nodesetval == NULL)
		return(0);
	    return(res->nodesetval->nodeNr != 0);
        case XPATH_STRING:
	    return((res->stringval != NULL) &&
	           (xmlStrlen(res->stringval) != 0));
        default:
	    STRANGE
    }
    return(0);
}

/**
 * xmlXPathCompile:
 * @str:  the XPath expression
 *
 * Compile an XPath expression
 *
 * Returns the xmlXPathObjectPtr resulting from the evaluation or NULL.
 *         the caller has to free the object.
 */
xmlXPathCompExprPtr
xmlXPathCompile(const xmlChar *str) {
    xmlXPathParserContextPtr ctxt;
    xmlXPathCompExprPtr comp;

    xmlXPathInit();

    ctxt = xmlXPathNewParserContext(str, NULL);
    xmlXPathCompileExpr(ctxt);
    
    if (*ctxt->cur != 0) {
	/* 
	 * aleksey: in some cases this line prints *second* error message
	 * (see bug #78858) and probably this should be fixed.
	 * However, we are not sure that all error messages are printed
	 * out in other places. It's not critical so we leave it as-is for now
	 */
	xmlXPatherror(ctxt, __FILE__, __LINE__, XPATH_EXPR_ERROR);
	comp = NULL;
    } else {
	comp = ctxt->comp;
	ctxt->comp = NULL;
    }
    xmlXPathFreeParserContext(ctxt);
    if (comp != NULL) {
	comp->expr = xmlStrdup(str);
#ifdef DEBUG_EVAL_COUNTS
	comp->string = xmlStrdup(str);
	comp->nb = 0;
#endif
    }
    return(comp);
}

/**
 * xmlXPathCompiledEval:
 * @comp:  the compiled XPath expression
 * @ctx:  the XPath context
 *
 * Evaluate the Precompiled XPath expression in the given context.
 *
 * Returns the xmlXPathObjectPtr resulting from the evaluation or NULL.
 *         the caller has to free the object.
 */
xmlXPathObjectPtr
xmlXPathCompiledEval(xmlXPathCompExprPtr comp, xmlXPathContextPtr ctx) {
    xmlXPathParserContextPtr ctxt;
    xmlXPathObjectPtr res, tmp, init = NULL;
    int stack = 0;
#ifndef LIBXML_THREAD_ENABLED
    static int reentance = 0;
#endif

    if ((comp == NULL) || (ctx == NULL))
	return(NULL);
    xmlXPathInit();

    CHECK_CONTEXT(ctx)

#ifndef LIBXML_THREAD_ENABLED
    reentance++;
    if (reentance > 1)
	xmlXPathDisableOptimizer = 1;
#endif

#ifdef DEBUG_EVAL_COUNTS
    comp->nb++;
    if ((comp->string != NULL) && (comp->nb > 100)) {
	fprintf(stderr, "100 x %s\n", comp->string);
	comp->nb = 0;
    }
#endif
    ctxt = xmlXPathCompParserContext(comp, ctx);
    xmlXPathRunEval(ctxt);

    if (ctxt->value == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlXPathCompiledEval: evaluation failed\n");
	res = NULL;
    } else {
	res = valuePop(ctxt);
    }

    
    do {
        tmp = valuePop(ctxt);
	if (tmp != NULL) {
	    if (tmp != init)
		stack++;    
	    xmlXPathFreeObject(tmp);
        }
    } while (tmp != NULL);
    if ((stack != 0) && (res != NULL)) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlXPathCompiledEval: %d object left on the stack\n",
	        stack);
    }
    if (ctxt->error != XPATH_EXPRESSION_OK) {
	xmlXPathFreeObject(res);
	res = NULL;
    }
        

    ctxt->comp = NULL;
    xmlXPathFreeParserContext(ctxt);
#ifndef LIBXML_THREAD_ENABLED
    reentance--;
#endif
    return(res);
}

/**
 * xmlXPathEvalExpr:
 * @ctxt:  the XPath Parser context
 *
 * Parse and evaluate an XPath expression in the given context,
 * then push the result on the context stack
 */
void
xmlXPathEvalExpr(xmlXPathParserContextPtr ctxt) {
    xmlXPathCompileExpr(ctxt);
    CHECK_ERROR;
    xmlXPathRunEval(ctxt);
}

/**
 * xmlXPathEval:
 * @str:  the XPath expression
 * @ctx:  the XPath context
 *
 * Evaluate the XPath Location Path in the given context.
 *
 * Returns the xmlXPathObjectPtr resulting from the evaluation or NULL.
 *         the caller has to free the object.
 */
xmlXPathObjectPtr
xmlXPathEval(const xmlChar *str, xmlXPathContextPtr ctx) {
    xmlXPathParserContextPtr ctxt;
    xmlXPathObjectPtr res, tmp, init = NULL;
    int stack = 0;

    xmlXPathInit();

    CHECK_CONTEXT(ctx)

    ctxt = xmlXPathNewParserContext(str, ctx);
    xmlXPathEvalExpr(ctxt);

    if (ctxt->value == NULL) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlXPathEval: evaluation failed\n");
	res = NULL;
    } else if (*ctxt->cur != 0) {
	xmlXPatherror(ctxt, __FILE__, __LINE__, XPATH_EXPR_ERROR);
	res = NULL;
    } else {
	res = valuePop(ctxt);
    }

    do {
        tmp = valuePop(ctxt);
	if (tmp != NULL) {
	    if (tmp != init)
		stack++;    
	    xmlXPathFreeObject(tmp);
        }
    } while (tmp != NULL);
    if ((stack != 0) && (res != NULL)) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlXPathEval: %d object left on the stack\n",
	        stack);
    }
    if (ctxt->error != XPATH_EXPRESSION_OK) {
	xmlXPathFreeObject(res);
	res = NULL;
    }

    xmlXPathFreeParserContext(ctxt);
    return(res);
}

/**
 * xmlXPathEvalExpression:
 * @str:  the XPath expression
 * @ctxt:  the XPath context
 *
 * Evaluate the XPath expression in the given context.
 *
 * Returns the xmlXPathObjectPtr resulting from the evaluation or NULL.
 *         the caller has to free the object.
 */
xmlXPathObjectPtr
xmlXPathEvalExpression(const xmlChar *str, xmlXPathContextPtr ctxt) {
    xmlXPathParserContextPtr pctxt;
    xmlXPathObjectPtr res, tmp;
    int stack = 0;

    xmlXPathInit();

    CHECK_CONTEXT(ctxt)

    pctxt = xmlXPathNewParserContext(str, ctxt);
    xmlXPathEvalExpr(pctxt);

    if (*pctxt->cur != 0) {
	xmlXPatherror(pctxt, __FILE__, __LINE__, XPATH_EXPR_ERROR);
	res = NULL;
    } else {
	res = valuePop(pctxt);
    }
    do {
        tmp = valuePop(pctxt);
	if (tmp != NULL) {
	    xmlXPathFreeObject(tmp);
	    stack++;
	}
    } while (tmp != NULL);
    if ((stack != 0) && (res != NULL)) {
	xmlGenericError(xmlGenericErrorContext,
		"xmlXPathEvalExpression: %d object left on the stack\n",
	        stack);
    }
    xmlXPathFreeParserContext(pctxt);
    return(res);
}

/************************************************************************
 *									*
 *	Extra functions not pertaining to the XPath spec		*
 *									*
 ************************************************************************/
/**
 * xmlXPathEscapeUriFunction:
 * @ctxt:  the XPath Parser context
 * @nargs:  the number of arguments
 *
 * Implement the escape-uri() XPath function
 *    string escape-uri(string $str, bool $escape-reserved)
 *
 * This function applies the URI escaping rules defined in section 2 of [RFC
 * 2396] to the string supplied as $uri-part, which typically represents all
 * or part of a URI. The effect of the function is to replace any special
 * character in the string by an escape sequence of the form %xx%yy...,
 * where xxyy... is the hexadecimal representation of the octets used to
 * represent the character in UTF-8.
 *
 * The set of characters that are escaped depends on the setting of the
 * boolean argument $escape-reserved.
 *
 * If $escape-reserved is true, all characters are escaped other than lower
 * case letters a-z, upper case letters A-Z, digits 0-9, and the characters
 * referred to in [RFC 2396] as "marks": specifically, "-" | "_" | "." | "!"
 * | "~" | "*" | "'" | "(" | ")". The "%" character itself is escaped only
 * if it is not followed by two hexadecimal digits (that is, 0-9, a-f, and
 * A-F).
 *
 * If $escape-reserved is false, the behavior differs in that characters
 * referred to in [RFC 2396] as reserved characters are not escaped. These
 * characters are ";" | "/" | "?" | ":" | "@" | "&" | "=" | "+" | "$" | ",".
 * 
 * [RFC 2396] does not define whether escaped URIs should use lower case or
 * upper case for hexadecimal digits. To ensure that escaped URIs can be
 * compared using string comparison functions, this function must always use
 * the upper-case letters A-F.
 * 
 * Generally, $escape-reserved should be set to true when escaping a string
 * that is to form a single part of a URI, and to false when escaping an
 * entire URI or URI reference.
 * 
 * In the case of non-ascii characters, the string is encoded according to 
 * utf-8 and then converted according to RFC 2396.
 *
 * Examples
 *  xf:escape-uri ("gopher://spinaltap.micro.umn.edu/00/Weather/California/Los%20Angeles#ocean"), true()) 
 *  returns "gopher%3A%2F%2Fspinaltap.micro.umn.edu%2F00%2FWeather%2FCalifornia%2FLos%20Angeles%23ocean"
 *  xf:escape-uri ("gopher://spinaltap.micro.umn.edu/00/Weather/California/Los%20Angeles#ocean"), false())
 *  returns "gopher://spinaltap.micro.umn.edu/00/Weather/California/Los%20Angeles%23ocean"
 *
 */
static void
xmlXPathEscapeUriFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    xmlXPathObjectPtr str;
    int escape_reserved;
    xmlBufferPtr target;
    xmlChar *cptr;
    xmlChar escape[4];
    
    CHECK_ARITY(2);
    
    escape_reserved = xmlXPathPopBoolean(ctxt);
    
    CAST_TO_STRING;
    str = valuePop(ctxt);
    
    target = xmlBufferCreate();
    
    escape[0] = '%';
    escape[3] = 0;
    
    if (target) {
	for (cptr = str->stringval; *cptr; cptr++) {
	    if ((*cptr >= 'A' && *cptr <= 'Z') ||
		(*cptr >= 'a' && *cptr <= 'z') ||
		(*cptr >= '0' && *cptr <= '9') ||
		*cptr == '-' || *cptr == '_' || *cptr == '.' || 
		*cptr == '!' || *cptr == '~' || *cptr == '*' ||
		*cptr == '\''|| *cptr == '(' || *cptr == ')' ||
		(*cptr == '%' && 
		 ((cptr[1] >= 'A' && cptr[1] <= 'F') ||
		  (cptr[1] >= 'a' && cptr[1] <= 'f') ||
		  (cptr[1] >= '0' && cptr[1] <= '9')) &&
		 ((cptr[2] >= 'A' && cptr[2] <= 'F') ||
		  (cptr[2] >= 'a' && cptr[2] <= 'f') ||
		  (cptr[2] >= '0' && cptr[2] <= '9'))) ||
		(!escape_reserved &&
		 (*cptr == ';' || *cptr == '/' || *cptr == '?' ||
		  *cptr == ':' || *cptr == '@' || *cptr == '&' ||
		  *cptr == '=' || *cptr == '+' || *cptr == '$' ||
		  *cptr == ','))) {
		xmlBufferAdd(target, cptr, 1);
	    } else {
		if ((*cptr >> 4) < 10)
		    escape[1] = '0' + (*cptr >> 4);
		else
		    escape[1] = 'A' - 10 + (*cptr >> 4);
		if ((*cptr & 0xF) < 10)
		    escape[2] = '0' + (*cptr & 0xF);
		else
		    escape[2] = 'A' - 10 + (*cptr & 0xF);
		
		xmlBufferAdd(target, &escape[0], 3);
	    }
	}
    }
    valuePush(ctxt, xmlXPathNewString(xmlBufferContent(target)));
    xmlBufferFree(target);
    xmlXPathFreeObject(str);
}

/**
 * xmlXPathRegisterAllFunctions:
 * @ctxt:  the XPath context
 *
 * Registers all default XPath functions in this context
 */
void
xmlXPathRegisterAllFunctions(xmlXPathContextPtr ctxt)
{
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"boolean",
                         xmlXPathBooleanFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"ceiling",
                         xmlXPathCeilingFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"count",
                         xmlXPathCountFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"concat",
                         xmlXPathConcatFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"contains",
                         xmlXPathContainsFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"id",
                         xmlXPathIdFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"false",
                         xmlXPathFalseFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"floor",
                         xmlXPathFloorFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"last",
                         xmlXPathLastFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"lang",
                         xmlXPathLangFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"local-name",
                         xmlXPathLocalNameFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"not",
                         xmlXPathNotFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"name",
                         xmlXPathNameFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"namespace-uri",
                         xmlXPathNamespaceURIFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"normalize-space",
                         xmlXPathNormalizeFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"number",
                         xmlXPathNumberFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"position",
                         xmlXPathPositionFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"round",
                         xmlXPathRoundFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"string",
                         xmlXPathStringFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"string-length",
                         xmlXPathStringLengthFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"starts-with",
                         xmlXPathStartsWithFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"substring",
                         xmlXPathSubstringFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"substring-before",
                         xmlXPathSubstringBeforeFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"substring-after",
                         xmlXPathSubstringAfterFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"sum",
                         xmlXPathSumFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"true",
                         xmlXPathTrueFunction);
    xmlXPathRegisterFunc(ctxt, (const xmlChar *)"translate",
                         xmlXPathTranslateFunction);

    xmlXPathRegisterFuncNS(ctxt, (const xmlChar *)"escape-uri",
	 (const xmlChar *)"http://www.w3.org/2002/08/xquery-functions",
                         xmlXPathEscapeUriFunction);
}

#endif /* LIBXML_XPATH_ENABLED */
