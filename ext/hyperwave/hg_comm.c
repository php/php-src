/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Uwe Steinmann <Uwe.Steinmann@fernuni-hagen.de>               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* #define HW_DEBUG */

#include <stdlib.h>
#include "php.h"
#include "php_globals.h"
#include "SAPI.h"

#if HYPERWAVE

#include <stdio.h>
#include <string.h> 
#include <sys/types.h>
#ifdef PHP_WIN32
# include <winsock.h>
# define EWOULDBLOCK WSAEWOULDBLOCK
# define ETIMEDOUT WSAETIMEDOUT
# define bcopy memcpy
# define bzero(a, b) memset(a, 0, b)
#else
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <unistd.h>
# include <sys/param.h>
#endif
#include <fcntl.h>
#include <errno.h>
#include "hg_comm.h"
#include "ext/standard/head.h"

/* Defining hw_optimize does optimize the send_objectbyidquery() function.
   Instead of getting the complete return message including the objectrecords
   with recv_hg_msg(), only the header of the return message is fetched.
   The object records itself are fetched as they are needed straight from
   the socket. This method requires less memory and is twice as fast because
   reading from the net seems to be a bottleneck which has less impact if
   the processing of the data is done in parallel.
*/
#define hw_optimize

/* Define hw_less_server_stress does reduce the stress on the hw server, by
   using send_objectbyidquery() instead of send_getobject() multiple times.
   send_objectbyidquery() gets a bunch of object records with one message.
   This also reduced the number of lines in the servers log files.
   Unfortunately this is not faster unless hw_optimize is defined, because
   getting object records with multiple send_getobject() is already optimized.
   First all request messages for each object are send and the the answers
   are read. This gives the server the possibility to answer request already
   while more request are comming in.
*/
#define hw_less_server_stress

static int set_nonblocking(int fd);
/* static int set_blocking(int fd); */

static int hg_read_exact(int sockfd, char *buf, int size);
/* static int hg_read(int sockfd, char *buf, int size); */
static int hg_write(int sockfd, char *buf, int size);

static int   send_hg_msg(int sockfd, hg_msg *msg, int length);
static void  build_msg_header(hg_msg *msg, int length, int version_msgid, int msg_type);
static char *build_msg_int(char *buf, int val);
static char *build_msg_str(char *buf, char *str);
static int   swap(int val);


int version      = HW_VERSION;
/* F_DISTRIBUTED has the effect that all object ids are
   virtual. This means whenever an object is requested a
   new id is generated for this session. Wavemaster and
   Harmony set this flag. How do I know? tcpdump tells
   a lot if  the output is investigated. The bit is also
   need to allow access on other server through the local
   server. The hw_mapid() function won't work unless you
   set F_DISTRIBUTED */
/* int version      = HW_VERSION | F_DISTRIBUTED; */
/* int version   = HW_VERSION | F_DISTRIBUTED | F_COMPRESSED; */
static int msgid        =  1;
static int sock_flags   = -1;
static int non_blocking =  0;
static int swap_on = 0;
static int rtimeout = 40;
static int wtimeout = 40;
static int lowerror = 0;

/***********************************************************************
* Function fnInsStr()                                                  *
*                                                                      *
* Insert string in string at position. The old string will be freed    *
* with efree!!! The new string is allocated with malloc.               *
* Parameter: string *str: string in which insstr is to be inserted     *
*            int pos: Position where string is to inserted (0=first)   *
*            string *insstr: string to be inserted                     *
* Return: pointer to new string or NULL. If NULL is returned the       *
*         memory for the old string has not been freed.                *
***********************************************************************/
char *fnInsStr(char *str, int pos, char *insstr)
{
	char *newstr, *ptr;

	if((str == NULL) || (insstr == NULL))
		return NULL;
	if(pos > (int)strlen(str))
		return NULL;

	if(insstr[0] == '\0')
		return str;

	if(NULL == (newstr = malloc(strlen(str) + strlen(insstr) + 1))) {
		lowerror = LE_MALLOC;
		return NULL;
		}

	ptr = newstr;
	memcpy(newstr, str, pos);
	ptr += pos;
	strcpy(ptr, insstr);
	ptr += strlen(insstr);
	strcpy(ptr, str+pos);

	free(str);
	return newstr;
}

/***********************************************************************
* Function fnAddAnchor()                                               *
*                                                                      *
* Inserts new anchor into anchor list.                                 *
* Parameter: DLIST pList: Anchor list                                  *
*            int objectID: object ID of Anchor                         *
*            int start: start position                                 *
*            int end: end position                                     *
* Return: Pointer to new anchor, NULL if error                         *
***********************************************************************/
#ifdef newlist
ANCHOR *fnAddAnchor(zend_llist *pAnchorList, 
                    int objectID,
                    int start, int end)
#else
ANCHOR *fnAddAnchor(DLIST *pAnchorList, 
                    int objectID,
                    int start, int end)
#endif
{
	ANCHOR *cur_ptr;

#ifdef newlist
	ANCHOR **ptr;
	if(NULL == (cur_ptr = (ANCHOR *) emalloc(sizeof(ANCHOR))))
		return NULL;
#else
	if((cur_ptr = (ANCHOR *) dlst_newnode(sizeof(ANCHOR))) == NULL) {
		return NULL;
	}
#endif

	memset(cur_ptr, 0, sizeof(ANCHOR));
	cur_ptr->start = start;
	cur_ptr->end = end;
	cur_ptr->id = objectID;
	cur_ptr->destdocname = NULL;
	cur_ptr->nameanchor = NULL;
	cur_ptr->link = NULL;
	cur_ptr->tagattr = NULL;
	cur_ptr->htmlattr = NULL;
	cur_ptr->codebase = NULL;
	cur_ptr->code = NULL;
	cur_ptr->keyword = NULL;
	cur_ptr->fragment = NULL;

#ifdef newlist
	zend_llist_prepend_element(pAnchorList, &cur_ptr);
	ptr = (ANCHOR **) zend_llist_get_first(pAnchorList);
#else
	dlst_insertafter(pAnchorList, cur_ptr, PHP_DLST_HEAD(pAnchorList));
#endif

	return(cur_ptr);
}

/***********************************************************************
* Function fnDeleteAnchor()                                            *
*                                                                      *
* Inserts new anchor into anchor list.                                 *
* Parameter: ptr: pointer to node                                      *
* Return: void                                                         *
***********************************************************************/
#ifdef newlist
void fnDeleteAnchor(void *ptr1)
#else
void fnDeleteAnchor(ANCHOR *ptr)
#endif
{
#ifdef newlist
	ANCHOR **ptr2, *ptr;
	ptr2 = (ANCHOR **) ptr1;
	ptr = *ptr2;
#endif

	if(ptr->destdocname) efree(ptr->destdocname);
	if(ptr->nameanchor) efree(ptr->nameanchor);
	if(ptr->link) efree(ptr->link);
	if(ptr->tagattr) efree(ptr->tagattr);
	if(ptr->htmlattr) efree(ptr->htmlattr);
	if(ptr->codebase) efree(ptr->codebase);
	if(ptr->code) efree(ptr->code);
	if(ptr->keyword) efree(ptr->keyword);
	if(ptr->fragment) efree(ptr->fragment);

#ifdef newlist
	efree(ptr);
#else
	dlst_freenode(ptr);
#endif
}

/***********************************************************************
* Function fnListAnchor()                                              *
*                                                                      *
* Lists all anchors in  anchor list.                                   *
* Parameter: ptr: pointer to list                                      *
* Return: void                                                         *
***********************************************************************/
#ifdef newlist
void fnListAnchor(zend_llist *pAnchorList)
#else
void fnListAnchor(DLIST *pAnchorList)
#endif
{
#ifdef newlist
	ANCHOR *cur_ptr, **ptr;
	ptr = (ANCHOR **) zend_llist_get_last(pAnchorList);
	if(ptr)
		cur_ptr = *ptr;
	while(ptr) {
		fprintf(stderr, "0x%X->0x%X ", (int) ptr, (int) cur_ptr);
#else
	ANCHOR *cur_ptr;
	cur_ptr = (ANCHOR *) dlst_last(pAnchorList);
	while(cur_ptr) {
		fprintf(stderr, "0x%X ", (int) cur_ptr);
#endif

		fprintf(stderr, "%d, %d, %s, %s, %s, %s %s\n", cur_ptr->start,
		                                           cur_ptr->end,
		                                           cur_ptr->tanchor == 1 ? "src" : "dest",
		                                           cur_ptr->destdocname,
		                                           cur_ptr->nameanchor,
		                                           cur_ptr->link,
		                                           cur_ptr->tagattr);
#ifdef newlist
		ptr = (ANCHOR **) zend_llist_get_prev(pAnchorList);
		if(ptr)
			cur_ptr = *ptr;
#else
		cur_ptr = (ANCHOR *) dlst_prev(cur_ptr);
#endif
	}
}

/***********************************************************************
* Function fnCmpAnchors()                                              *
*                                                                      *
* Compares to Anchors by its start position                            *
* Parameter: ANCHOR a1: First Anchor                                   *
*            ANCHOR a2: Second Anchor                                  *
* Return: As strcmp                                                    *
***********************************************************************/
#ifdef newlist
int fnCmpAnchors(const void *e1, const void *e2 TSRMLS_DC)
{
	ANCHOR *a1, **aa1, *a2, **aa2;
	zend_llist_element **ee1, **ee2;
	ee1 = (zend_llist_element **) e1;
	ee2 = (zend_llist_element **) e2;
	aa1 = (ANCHOR **) (*ee1)->data;
	aa2 = (ANCHOR **) (*ee2)->data;
	a1 = *aa1;
	a2 = *aa2;
#else
int fnCmpAnchors(ANCHOR *a1, ANCHOR *a2 TSRMLS_DC)
{
#endif
	if(a1->start < a2->start)
		return -1;
	if(a1->start == a2->start) {
		/* It's importent to check this case as well, because a link with
	   	a bigger end has to be inserted first.
		*/
		if(a1->end < a2->end)
			return -1;
		/* If both start and end are equal (yes, it is possible)
		   we will Src Anchor before a Dest anchor. There has been
		   a case where an IMG was surrounded by a NAME which was
		   surrounded by a HREF. In such a case the HREF doesn't
		   work.
		*/
		if(a1->end == a2->end) {
			if(a1->tanchor > a2->tanchor)
				return -1;
		}
	}

	return 1;
}

/***********************************************************************
* Function fnCreateAnchorList()                                        *
* Uses either docofanchorrec or reldestrec to create a list of anchors *
* depending on anchormode                                              *
*                                                                      *
* Returns a list of Anchors converted from an object record            *
* Parameter: int objectID: the object for which the list is created    *
*            char **anchors: object records of anchors                 *
*            char **docofanchorrec: Name of destination absolut        *
*            char **reldestrec: Name of destination relativ to current *
*                               object                                 *
*            int ancount: number of anchors                            *
*            int anchormode: 0 = use absolut dest, else rel. dest      *
* Return: List of Anchors, NULL if error                               *
***********************************************************************/
#ifdef newlist
zend_llist *fnCreateAnchorList(hw_objectID objID, char **anchors, char **docofanchorrec, char **reldestrec, int ancount, int anchormode)
#else
DLIST *fnCreateAnchorList(hw_objectID objID, char **anchors, char **docofanchorrec, char **reldestrec, int ancount, int anchormode)
#endif
{
	int start, end, i, destid, anchordestid, objectID;
	ANCHOR *cur_ptr = NULL;
#ifdef newlist
	zend_llist *pAnchorList;
	pAnchorList = (zend_llist *) emalloc(sizeof(zend_llist));
	zend_llist_init(pAnchorList, sizeof(char *), fnDeleteAnchor, 0);
#else
	DLIST *pAnchorList = dlst_init();
#endif

	for(i=ancount-1; i>=0; i--) {
		char *object = NULL;
		char *docofanchorptr = NULL;
		char *reldestptr = NULL;
		char *str, *str1, link[200];

		if(NULL != anchors[i]) {
			object = anchors[i];
			docofanchorptr = docofanchorrec[i];
			if(reldestrec) /* FIXME reldestrec may only be NULL if anchormode != 0 */
				reldestptr = reldestrec[i];
	
			/* Determine Position. Doesn't matter if Src or Dest
			   The Position field should always be there. Though there
			   are case in which the position has no meaning, e.g. if
			   a document is annotated and the annotation text doesn't
			   contain a link of type annotation,
			   In such a case the Position has the value 'invisible' */
			str = strstr(object, "Position");
			str += 9;
			if(((int) str != 9) && (0 != strncmp(str, "invisible", 9))) {
				sscanf(str, "0x%X 0x%X", &start, &end);
		
				/* Determine ObjectID */
				objectID = 0;
				if(NULL != (str = strstr(object, "ObjectID"))) {
					str += 9;
					sscanf(str, "0x%X", &objectID);
				}
		
				cur_ptr = fnAddAnchor(pAnchorList, objectID, start, end);
		
				/* Determine Type of Anchor */
				str = strstr(object, "TAnchor");
				str += 8;
				if(*str == 'S') {
					char destdocname[200];
					char nameanchor[200];
					cur_ptr->tanchor = 1;
		
					cur_ptr->destdocname = NULL;
					if(NULL != (str = strstr(object, "Dest"))) {
						char *tempptr;
	
						/* No need to care about the value of Dest, because we take the info
						   from docofanchorptr.
						   Since the anchor has a destination there are two possibilities.
						   1. The destination is an anchor or
						   2. or the destination is a document already.
						   In both cases docofanchorptr has the proper info because GETDOCBYANCHOR
						   is such a nice message.
						*/
						switch(anchormode) {
							case 0:
								tempptr = docofanchorptr;
								break;
							default:
								tempptr = reldestptr;
						}
						if(NULL != tempptr) {
							destid = 0;
							if(NULL != (str = strstr(tempptr, "ObjectID="))) {
								str += 9;
								sscanf(str, "0x%X", &destid);
							}
							/* This is basically for NAME tags. There is no need
							   to add the destname if it is the document itself.
							*/
/*							if(destid == objID) {
								cur_ptr->destdocname = NULL;
							} else { */
								/* It's always nice to deal with names, so let's first check
						  			for a name. If there is none we take the ObjectID.
								*/
								if(NULL != (str = strstr(tempptr, "Name="))) {
									str += 5;
								} else if(NULL != (str = strstr(tempptr, "ObjectID="))) {
									str += 9;
								}
								if(sscanf(str, "%s\n", destdocname)) {
									cur_ptr->destdocname = estrdup(destdocname);
								}
/*							} */
						}
					}
		
					/* Get the Id of the anchor destination and the document id that belongs
					   to that anchor. We need that soon in order to determine if the anchor
					   points to a document or a dest anchor in a document.
					*/
					anchordestid = 0;
					if(NULL != (str = strstr(object, "Dest="))) {
						str += 5;
						sscanf(str, "0x%X", &anchordestid);
					}
					
					/* if anchordestid != destid then the destination is an anchor in a document whose
					   name (objectID) is already in destdocname. We will have to extend the link
					   by '#...'
					*/
					cur_ptr->nameanchor = NULL;
					if(anchordestid != destid) {
						if(NULL != (str = strstr(object, "Dest="))) {
							str += 5;
							if(sscanf(str, "%s\n", nameanchor))
								cur_ptr->nameanchor = estrdup(nameanchor);
						}
					}
		
					if(!cur_ptr->destdocname) {
						cur_ptr->link = NULL;
						if(NULL != (str = strstr(object, "Hint=URL:"))) {
							str += 9;
							if(sscanf(str, "%s\n", link))
								cur_ptr->link = estrdup(link);
						} else if(NULL != (str = strstr(object, "Hint="))) {
							str += 5;
							if(sscanf(str, "%s\n", link))
								cur_ptr->link = estrdup(link);
						}
					}
		
					cur_ptr->fragment = NULL;
					if(NULL != (str = strstr(object, "Fragment="))) {
						str += 9;
						if(sscanf(str, "%s\n", link))
							cur_ptr->fragment = estrdup(link);
					}
		
					{
					char *htmlattr, *str2;
					int offset;
					str1 = object;
					htmlattr = emalloc(strlen(object)); /* alloc mem big enough for htmlattr */
					htmlattr[0] = '\0';
					offset = 0;
					while(NULL != (str = strstr(str1, "HtmlAttr="))) {
						str += 9;
						str1 = str;
						while((*str1 != '\n') && (*str1 != '\0'))
							str1++;
						/* Find the '=' in the HTML attr and make sure it is part of the
						   attr and not somewhere in the objrec. */
						if((NULL != (str2 = strchr(str, '='))) && (str2 < str1)) {
							str2++;
							strncpy(&htmlattr[offset], str, str2 - str);
							offset = offset + (str2 - str);
							htmlattr[offset++] = '"';
							strncpy(&htmlattr[offset], str2, str1 - str2);
							offset = offset + (str1 - str2);
							htmlattr[offset++] = '"';
							htmlattr[offset++] = ' ';
							htmlattr[offset] = '\0';
						}
					}
					if(offset){
						/* remove last space */
						htmlattr[offset-1] = '\0';
						cur_ptr->htmlattr = estrdup(htmlattr);
					}
					efree(htmlattr);
					}
		
					if(NULL != (str = strstr(object, "LinkType="))) {
						str += 9;
						if(strncmp(str, "background", 10) == 0)
							cur_ptr->linktype=HW_BACKGROUND_LINK;
						else
						if(strncmp(str, "intagnodel", 10) == 0) { /* New type introduced by Uwe Steinmann 16.03.2001 */
							cur_ptr->linktype=HW_INTAGNODEL_LINK;
							cur_ptr->tagattr = NULL;
							if(NULL != (str = strstr(object, "TagAttr="))) {
								str += 8;
								str1 = str;
								while((*str1 != '\n') && (*str1 != '\0'))
									str1++;
								cur_ptr->tagattr = emalloc(str1 - str + 1);
								memcpy(cur_ptr->tagattr, str, str1 - str);
								cur_ptr->tagattr[str1 - str] = '\0';
							}
						} else
						if(strncmp(str, "intag", 5) == 0) {
							cur_ptr->linktype=HW_INTAG_LINK;
							cur_ptr->tagattr = NULL;
							if(NULL != (str = strstr(object, "TagAttr="))) {
								str += 8;
								str1 = str;
								while((*str1 != '\n') && (*str1 != '\0'))
									str1++;
								cur_ptr->tagattr = emalloc(str1 - str + 1);
								memcpy(cur_ptr->tagattr, str, str1 - str);
								cur_ptr->tagattr[str1 - str] = '\0';
							}
						} else
						if(strncmp(str, "applet", 6) == 0) {
							cur_ptr->linktype=HW_APPLET_LINK;
							cur_ptr->codebase = NULL;
							if(NULL != (str = strstr(object, "CodeBase="))) {
								str += 9;
								str1 = str;
								while((*str1 != '\n') && (*str1 != '\0'))
									str1++;
								cur_ptr->codebase = emalloc(str1 - str + 1);
								memcpy(cur_ptr->codebase, str, str1 - str);
								cur_ptr->codebase[str1 - str] = '\0';
							}
							cur_ptr->code = NULL;
							if(NULL != (str = strstr(object, "Code="))) {
								str += 5;
								str1 = str;
								while((*str1 != '\n') && (*str1 != '\0'))
									str1++;
								cur_ptr->code = emalloc(str1 - str + 1);
								memcpy(cur_ptr->code, str, str1 - str);
								cur_ptr->code[str1 - str] = '\0';
							}
						} else
							cur_ptr->linktype=HW_DEFAULT_LINK;
					} else
						cur_ptr->linktype=HW_DEFAULT_LINK;
		
				} else { /* Destination Anchor */
					char nameanchor[200];
		
					cur_ptr->tanchor = 2;
					cur_ptr->link = NULL;
		
					/* Here is the only additional info for the name attribute */
					cur_ptr->nameanchor = NULL;
					if(NULL != (str = strstr(object, "ObjectID="))) {
						str += 9;
						if(sscanf(str, "%s\n", nameanchor))
							cur_ptr->nameanchor = estrdup(nameanchor);
					}
		
					cur_ptr->keyword = NULL;
					if(NULL != (str = strstr(object, "Keyword="))) {
						str += 8;
						if(sscanf(str, "%s\n", nameanchor))
							cur_ptr->keyword = estrdup(nameanchor);
					}
		
				}
		
			}
			/* free memory even if it is an invisible anchor */
			efree(anchors[i]);
			if(docofanchorrec[i]) efree(docofanchorrec[i]);
			if(reldestrec)
				if(reldestrec[i]) efree(reldestrec[i]);
		}
	}
	return pAnchorList;
}

/***********************************************************************
* Function fnInsAnchorsIntoText()                                      *
*                                                                      *
* Returns the text document with all anchors inserted form list        *
* Parameter: char *text: text without anchors                          *
*            DList *pAnchorList: list of anchors                       *
* Return: Text with anchors                                            *
***********************************************************************/
#define BUFFERLEN 200
#ifdef newlist
char *fnInsAnchorsIntoText(char *text, zend_llist *pAnchorList, char **bodytag, char **urlprefix) {
	ANCHOR **ptr;
#else
char *fnInsAnchorsIntoText(char *text, DLIST *pAnchorList, char **bodytag, char **urlprefix) {
#endif
	ANCHOR *cur_ptr;
	char bgstr[BUFFERLEN], istr[BUFFERLEN];
	char **scriptname;
	char *newtext;
	int offset = 0;
	int laststart=0;
	char emptystring[BUFFERLEN];
	int i;
	TSRMLS_FETCH();
	
	emptystring[0] = '\0';

/* The following is very tricky and depends on how rewriting is setup on your webserver.
   If you skip the scriptname in the url you will have to map each hyperwave name
   to http://<hwname>. This may not always be a good idea. The best solution is
   probably to provide a prefix for such
   a case which is an optional parameter to hw_gettext() or hw_pipedocument().
   FIXME: Currently, the variable SCRIPT_NAME is empty thouht SCRIPT_URL is
   not. In our case this is OK, since as mentioned above it is better to have no
   SCRIPT_NAME than to have if rewriting is on.
*/
	if(urlprefix) {
		scriptname = urlprefix;
	} else {
		zval **script_name;
		scriptname = emalloc(5*sizeof(char *));
		if (zend_hash_find(&EG(symbol_table), "SCRIPT_NAME", sizeof("SCRIPT_NAME"), (void **) &script_name)==FAILURE)
			for(i=0; i<5; i++)
				scriptname[i] = &emptystring;
		else {
			convert_to_string_ex(script_name);
			for(i=0; i<5; i++)
				scriptname[i] = Z_STRVAL_PP(script_name);
		}

#if 0
#if APACHE
		{
		int j;
		array_header *arr = table_elts(((request_rec *) SG(server_context))->subprocess_env);
		table_entry *elts = (table_entry *)arr->elts;

		for (j=0; j < arr->nelts; j++) {
			if((0 == strcmp(elts[j].key, "SCRIPT_NAME")) ||
			   (0 == strcmp(elts[j].key, "SCRIPT_URL")))
			break;
		}
		scriptname = elts[j].val;
		}
#else
		scriptname = getenv("SCRIPT_FILENAME");
#endif
#endif
	}

	newtext = text;
	bgstr[0] = '\0';
#ifdef newlist
	zend_llist_sort(pAnchorList, fnCmpAnchors TSRMLS_CC);
	ptr = (ANCHOR **) zend_llist_get_last(pAnchorList);
	if(ptr)
		cur_ptr = *ptr;
	while(NULL != ptr) {
#else
	dlst_mergesort(pAnchorList, fnCmpAnchors);
	cur_ptr = (ANCHOR *) dlst_last(pAnchorList);
	while(NULL != cur_ptr) {
#endif

		istr[0] = '\0';
		if(cur_ptr->tanchor == 1) { /* Src Anchor */
			if(laststart >= cur_ptr->end)
				offset = 0;
			if((cur_ptr->link != NULL) && (cur_ptr->link[0] != '\0')) {
				/* The link is only set if the Link points to an external document */
				switch(cur_ptr->linktype) {
					case HW_BACKGROUND_LINK:
						snprintf(istr, BUFFERLEN, " background='%s'", cur_ptr->link);
						break;
					case HW_INTAG_LINK:
						snprintf(istr, BUFFERLEN, " %s='%s'", cur_ptr->tagattr, cur_ptr->link);
						offset -= 4; /* because there is no closing tag </A> */
/*						laststart = cur_ptr->start; */
						break;
					case HW_INTAGNODEL_LINK:
						snprintf(istr, BUFFERLEN, "%s", cur_ptr->link);
						offset -= 4; /* because there is no closing tag </A> */
/*						laststart = cur_ptr->start; */
						break;
					case HW_APPLET_LINK:
						if(cur_ptr->codebase)
						  snprintf(istr, BUFFERLEN, " CODEBASE='%s' CODE='%s'", cur_ptr->codebase, cur_ptr->code);
						else
						  snprintf(istr, BUFFERLEN, " CODEBASE='/' CODE='%s'", cur_ptr->code);
						break;
					default:
						newtext = fnInsStr(newtext, cur_ptr->end+offset, "</A>");
						if(cur_ptr->fragment)
							snprintf(istr, BUFFERLEN, "<A HREF='%s#%s'", cur_ptr->link, cur_ptr->fragment);
						else
							snprintf(istr, BUFFERLEN, "<A HREF='%s'", cur_ptr->link);
						if(cur_ptr->htmlattr) {
							strncat(istr, " ", BUFFERLEN - 1 - strlen(istr));
							strncat(istr, cur_ptr->htmlattr, BUFFERLEN - 1 - strlen(istr));
						}
						strncat(istr, ">", BUFFERLEN - 1 - strlen(istr));
				}
			} else {
				switch(cur_ptr->linktype) {
					case HW_BACKGROUND_LINK:
						if(NULL != cur_ptr->destdocname) {
							snprintf(istr, BUFFERLEN, " background='%s/%s'", scriptname[HW_BACKGROUND_LINK], cur_ptr->destdocname);
						} else
							istr[0] = '\0';
						break;
					case HW_INTAG_LINK:
						if(cur_ptr->fragment)
							snprintf(istr, BUFFERLEN, " %s='#%s'", cur_ptr->tagattr, cur_ptr->fragment);
						else
							snprintf(istr, BUFFERLEN, " %s='%s/%s'", cur_ptr->tagattr, scriptname[HW_INTAG_LINK], cur_ptr->destdocname); 
						offset -= 4; /* because there is no closing tag </A> */
						break;
					case HW_INTAGNODEL_LINK:
						snprintf(istr, BUFFERLEN, "%s", cur_ptr->destdocname);
						offset -= 4; /* because there is no closing tag </A> */
						break;
					case HW_APPLET_LINK:
						if(cur_ptr->codebase)
/*						  snprintf(istr, BUFFERLEN, " CODEBASE='%s%s' CODE='%s'", scriptname == NULL ? "" : scriptname, cur_ptr->codebase, cur_ptr->code); */
						  snprintf(istr, BUFFERLEN, " CODEBASE='%s%s' CODE='%s'", scriptname[HW_APPLET_LINK], cur_ptr->codebase, cur_ptr->code); 
						else
						  snprintf(istr, BUFFERLEN, " CODEBASE='/' CODE='%s'", cur_ptr->code);
						break;
					default:
						newtext = fnInsStr(newtext, cur_ptr->end+offset, "</A>");

						if(cur_ptr->nameanchor)
							snprintf(istr, BUFFERLEN, "<A HREF='%s/%s#%s'", scriptname[HW_DEFAULT_LINK], cur_ptr->destdocname, cur_ptr->nameanchor);
						else if(cur_ptr->fragment)
							snprintf(istr, BUFFERLEN, "<A HREF=\"%s/%s#%s\"", scriptname[HW_DEFAULT_LINK], cur_ptr->destdocname, cur_ptr->fragment);
						else
							snprintf(istr, BUFFERLEN, "<A HREF='%s/%s'", scriptname[HW_DEFAULT_LINK], cur_ptr->destdocname);

						if(cur_ptr->htmlattr) {
							strncat(istr, " ", BUFFERLEN - 1 - strlen(istr));
							strncat(istr, cur_ptr->htmlattr, BUFFERLEN - 1 - strlen(istr));
						}
						strncat(istr, ">", BUFFERLEN - 1 - strlen(istr));
				}
			}
		} else {
			if(laststart >= cur_ptr->end)
				offset = 0;
			newtext = fnInsStr(newtext, cur_ptr->end+offset, "</a>");

			/* If we have a keyword, we assume we had a fragment which has been used
			   instead of the destdocname
			*/
			if(cur_ptr->keyword)
				snprintf(istr, BUFFERLEN, "<A NAME='%s'>", cur_ptr->keyword);
			else if(cur_ptr->nameanchor)
				snprintf(istr, BUFFERLEN, "<A NAME='%s'>", cur_ptr->nameanchor);
		}
		newtext = fnInsStr(newtext, cur_ptr->start, istr);
		/* In case there are several TAGS nested, we accumulate the offset
		   You wonder what the 4 means? It's the length of </A> */
		offset += strlen(istr) + 4;
		laststart = cur_ptr->start;
#ifdef newlist
		ptr = (ANCHOR **) zend_llist_get_prev(pAnchorList);
		if(ptr)
			cur_ptr = *ptr;
#else
		cur_ptr = (ANCHOR *) dlst_prev(cur_ptr);
#endif
	}
	snprintf(istr, BUFFERLEN, "<BODY %s>", bgstr);
	*bodytag = estrdup(istr);
/*	if(scriptname != urlprefix) efree(scriptname); */
	if(scriptname != NULL) efree(scriptname);
	return(newtext);
}
#undef BUFFERLEN
	
/***********************************************************************
* Function fnAttributeValue()                                          *
*                                                                      *
* Returns the value of an attribute                                    *
* Parameter: char *object: object record                               *
*            char *attrname: attribute name                            *
* Return: char*: attribute value, NULL if name not found               *
***********************************************************************/
char *fnAttributeValue(char *object, char *attrname)
{
	char *str, *str1, *attrvalue;
	int len;

	str = strstr(object, attrname);
	if(NULL == str)
		return(NULL);
	str += strlen(attrname);
	str++;
	str1 = str;
	while((*str1 != '\0') && (*str1 != '\n'))
		str1++;
	len = str1 - str;
	if(NULL == (attrvalue = emalloc(len+1))) {
		lowerror = LE_MALLOC;
		return NULL;
		}
	memcpy(attrvalue, str, len);
	attrvalue[len] = '\0';
	return(attrvalue);
}

/***********************************************************************
* Function fnAttributeCompare()                                        *
*                                                                      *
* Checks if an attribute in an objrec has a certain value              *
* Parameter: char *object: object record                               *
*            char *attrname: attribute name                            *
*            char *value: value of attribute                           *
* Return: char*: as strcmp                                             *
***********************************************************************/
int fnAttributeCompare(char *object, char *attrname, char *value)
{
	char *str, *str1;
	int len;

	if((NULL == object) || (NULL == attrname) || (NULL == value))
		return -2;

	/* Find the attribute Name and make sure it is followed by
	   a '=' sign and preceded by a '\n';
	*/
	str = strstr(object, attrname);
	if((NULL == str) ||
	   (str[strlen(attrname)] != '=') ||
	   (str[-1] != '\n')) {
		return(-2);
	}
	str += strlen(attrname); /* skip the attribute name */
	str++; /* skip the equal sign */

	/* Search for end of attribute value */
	str1 = str;
	while((*str1 != '\0') && (*str1 != '\n'))
		str1++;
	len = str1 - str;
	return(strncmp(str, value, len));
}

/*********************************************************************
* Function fnCOpenDataCon()                                          *
*                                                                    *
* Opens data connection on client side. This function is called      *
* right after the client has requested any data from the server      *
* Parameter: int sockfd: socket of control connection                *
*            int *port: port of control und data connection          *
* Return   : sockfd on success, <0 if error                          *
*********************************************************************/
static int fnCOpenDataCon(int sockfd, int *port)
  {
  int fd;
  struct sockaddr_in  serv_addr;
  int len;
  int option = 1;

/*  len = sizeof(com_addr);
  if(getsockname(sockfd, (struct sockaddr *) &com_addr, &len) < 0)
    {
    return(-1);
    }

  *port = htons(com_addr.sin_port); */

  /*
  ** Open a TCP socket (an Internet stream socket)
  */
  if((fd = socket(AF_INET, SOCK_STREAM, 0)) == SOCK_ERR)
    {
    return(-1);
    }

  /*
  ** Make sure that address may be reused
  */
#if defined(SUN) || defined(PHP_WIN32)
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option));
#else
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
#endif

  /*
  ** Open connection aktiv
  ** Let bind() select a port number
  */
  bzero((char *) &serv_addr, sizeof(serv_addr));
  if(bind(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
    return(-1);
    }

  /*
  ** Get the port number bind selected
  */
  len = sizeof (serv_addr);
  if(getsockname(fd, (struct sockaddr *)&serv_addr, &len) < 0)
    {
    return(-1);
    }
  *port = ntohs(serv_addr.sin_port);

  listen(fd, 5);

  return(fd);
  }

/*======================================================================
 *
 *  Read/write routines with timeout detection.
 *
 *  Usage: write_to(fd, buffer, n, timeout)
 *          read_to(fd, buffer, n, timeout)
 *
 *  David Chavez
 *  Engineering Services & Software
 *  7841 New Salem Street
 *  San Diego, CA 92126
 *  USA
 *
 *  dec@essw.com
 *
 *====================================================================*/
#ifdef PHP_WIN32
#include <time.h>
#else
#include <sys/fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#endif
#include <errno.h>
#include <signal.h>

#ifndef PHP_WIN32
static sigset_t newmask, oldmask, zeromask;
#endif

static int set_noblock(int fd)
{
#ifdef PHP_WIN32
	u_long argp=1;

	return ioctlsocket (fd, FIONBIO , &argp); 
#else

	sigemptyset(&zeromask);
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGHUP);
	sigaddset(&newmask, SIGUSR1);
	sigaddset(&newmask, SIGUSR2);
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) return -1;

	return fcntl(fd, F_SETFL, O_NONBLOCK | O_NDELAY /* | FNBIO*/);
#endif
}

int write_to(int fd, void *buffer, int n, int timeout)
{
	int nrem, nw=0;
	char *bptr;
	int  error=0;
#if defined(SYSV) || defined(PHP_WIN32)
	int    width = 20;
#else
	int    width = getdtablesize();
#endif
	fd_set writefds;
	struct timeval select_timeout;

	select_timeout.tv_sec = timeout;
#ifdef PHP_WIN32
	select_timeout.tv_usec = 0;
#else /* is this just a typo? */
	select_timeout.tv_usec = 0.;
#endif

	/*  Set fd to non-blocking  */

	if (set_noblock(fd) != 0) return -1;

	/*  Write to fd until no more can be written  */

	FD_ZERO(&writefds);

	FD_SET((unsigned int)fd, &writefds);
 
	for( nrem = n, bptr = buffer; nrem;)
		{
		if(( error = select(width, 
		                    (fd_set *) 0, 
		                    &writefds, 
                                    (fd_set *) 0, 
                           &select_timeout)) <= 0 && errno != EINTR) break;
    
		if(errno != EINTR && ( nw = write(fd, bptr, nrem)) <= 0) {
			/*
			 *  check for error number - and keep trying to
			 *  write
			 */
			if(errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR)
				{
				continue;
				}
			else
				{
				error = nw;
				break;
				}
		} else {
			nrem -= nw;
			bptr += nw;
		}
	}
#ifndef PHP_WIN32
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) return -1;
#endif
	if( ! error ) {
		errno = ETIMEDOUT;
		return(-2);
	} else if (error < 0) {
		return(error);
	} else {
		return(n);
	}
}

int read_to(int fd, char *buffer, int n, int timeout)
  {
  int nrem, nread, nr=0;
  char *bptr;
  int  error=0;

#if defined(SYSV) || defined(PHP_WIN32)
  int    width = 20;
#else
  int    width = getdtablesize();
#endif
  fd_set readfds;
  struct timeval select_timeout;

  select_timeout.tv_sec = timeout;
#ifdef PHP_WIN32
  select_timeout.tv_usec = 0;
#else
  select_timeout.tv_usec = 0.;
#endif
  
  /*  Set fd to non-blocking  */

  if (set_noblock(fd) != 0) return -1;

  FD_ZERO(&readfds);

  FD_SET((unsigned int)fd, &readfds);
  
  for( nrem = n, bptr = buffer, nread = 0; nrem;)
    {
    if(( error = select(width, 
            &readfds, 
            (fd_set *) 0, 
            (fd_set *) 0, 
            &select_timeout)) <= 0 && errno != EINTR) break;
    
    if(errno != EINTR && (nr =  read (fd, bptr, nrem)) < 0)
      {
      /*
       *  check for error number - and keep trying to
       *  read
       */
      if(errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR)
        {
        continue;
        }
      else
        {
        error = nr;
        break;
        }
      }
    else if(nr == 0)
      {
      break;
      }
    else
      {
      nread += nr;
      bptr  += nr;
      nrem  -= nr;
      }
    }

#ifndef PHP_WIN32
  if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) return -1;
#endif
  if( ! error )
    {
    errno = ETIMEDOUT;
    return(-2);
    }
  else if ( error < 0) 
    {
    return(-1);
    }
  else
    {
    return(nread);
    }
  }

void set_swap(int do_swap)
{
     swap_on = do_swap;
}

/***********************************************************************
* Function open_hg_connection()                                        *
*                                                                      *
***********************************************************************/
int open_hg_connection(char *server_name, int port)
{
	int sockfd;
	int option = 1;
	struct sockaddr_in server_addr;
	struct hostent *hp;

	if ( NULL == server_name )
		return(-1);

	if ( (hp = gethostbyname(server_name)) == NULL )  {
		return(-2);
	}

	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	if ( port != 0 )
		server_addr.sin_port = htons(port);
	else
		server_addr.sin_port = htons(HG_SERVER_PORT); 
/*	bcopy(hp->h_addr, (char *) &server_addr.sin_addr, hp->h_length); */
	server_addr.sin_addr = *(struct in_addr *) hp->h_addr;

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) == SOCK_ERR )  {
		return(-3);
	}

#if defined(SUN) || defined(PHP_WIN32)
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option));
#else
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
#endif /* SUN */

	if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		HWSOCK_FCLOSE(sockfd);
		return(-4);
	} 

#ifndef PHP_WIN32
	if ( (sock_flags = fcntl(sockfd, F_GETFL, 0)) == -1 )
#endif

	if ( set_nonblocking(sockfd) == -1 )  {
		HWSOCK_FCLOSE(sockfd);
		return(-5);
	}

	return(sockfd);
}


/***********************************************************************
* Function initialize_hg_connection()                                  *
*                                                                      *
***********************************************************************/
int initialize_hg_connection(int sockfd, int *do_swap, int *version, char **userdata,  char **server_string, char *username, char *password)
{
	char buf, c;
        char *tmp;
	hg_msg *ready_msg, *retmsg, msg;
	int i = 0x01;
	int length;

	*do_swap = 0;
	buf = 'T';
	if ( hg_write(sockfd, &buf, 1) == -1 )  {
		return(-2);
	}

	if ( hg_read_exact(sockfd, &buf, 1) == -1 )  {
		return(-3);
	}
	if ( buf == 'F' )  {
		return(-4);
	}
	if ( buf != 'T' )  {
		return(-5);
	}

	buf = c = ( *(char *)&i )  ? 'l' : 'B';
	if ( hg_write(sockfd, &buf, 1) == -1 )  {
		return(-6);
	}
	if ( hg_read_exact(sockfd, &buf, 1) == -1 )  {
		return(-7);
	}
	if ( c != buf )  {
		swap_on = 1;
		*do_swap = 1;
	} else {
		swap_on = 0;
		*do_swap = 0;
	}

	if ( send_ready(sockfd) == -1) {
		return(-8);
		}

	/* Receive return from Ready message */
	if ( (ready_msg = recv_ready(sockfd)) == NULL )  {
		return(-9);
	}   

	if ((ready_msg->version_msgid & F_VERSION) < HW_VERSION)
		return(-8);
	*version = ready_msg->version_msgid;
	*server_string = strdup(ready_msg->buf+4);
	efree(ready_msg->buf);
	efree(ready_msg);

	/* If we have a username and password then do the identification. */
	if((NULL != username) && (NULL != password)) {
		length = HEADER_LENGTH + sizeof(int) + strlen(username) + 1 + strlen(password) + 1;

		build_msg_header(&msg, length, msgid++, IDENTIFY_MESSAGE);

		if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
			lowerror = LE_MALLOC;
			return(-1);
		}

		tmp = build_msg_int(msg.buf, 0);
		tmp = build_msg_str(tmp, username);
		tmp = build_msg_str(tmp, password);

		if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
			efree(msg.buf);
			return(-10);
		}
		efree(msg.buf);
	}

	if((NULL != username) && (NULL != password)) {
		/* Receive return form identify message */
		retmsg = recv_hg_msg(sockfd);
		if ( retmsg == NULL )
			return(-11);

		*userdata = retmsg->buf;
		efree(retmsg);
	}

	return(0);
}


static int set_nonblocking(int fd)
{
#ifdef PHP_WIN32
	unsigned int argp=0;

/*	if ( sock_flags == -1 )
		getsockopt (fd, SOL_SOCKET, optname, optval, optlen); 
*/	if(ioctlsocket (fd, FIONBIO , &argp) == -1)
		return(-1);
#else
	if ( sock_flags == -1 )
		sock_flags = fcntl(fd, F_GETFL, 0);
	if ( fcntl(fd, F_SETFL, O_NONBLOCK) == -1 )
		return(-1);
#endif
	non_blocking = 1;
	return(0);
}


/*
static int set_blocking(int fd)
{
#ifdef PHP_WIN32
	unsigned int argp=1;

	if(ioctlsocket (fd, FIONBIO , &argp) == -1)
		return(-1);
#else
	if ( fcntl(fd, F_SETFL, sock_flags) == -1 )
		return(-1);
#endif
	return(0);
}
*/

static int hg_read_exact(int sockfd, char *buf, int size)
{
	int len = 0;

	len = read_to(sockfd, (void *) buf, size, rtimeout);
	if ( len < 0 ) 
		return -1;
	return(len);
}

/*
static int hg_read(int sockfd, char *buf, int size)
{
	int try = 0;
	int len = 0;

	if ( !non_blocking )
		set_nonblocking(sockfd);  
	while ( len == 0 )  {
		len = recv(sockfd, (void *) buf, size, 0);
		if ( len == -1 )  {
			if ( (errno == EAGAIN) || (errno == EWOULDBLOCK) )  {
				if ( ++try > 5 )
					return(-1);
				php_sleep(1);
			} 
			else return(-1);
		} 
	}
	return(len);
}
*/

static int hg_write(int sockfd, char *buf, int size)
{
	int try = 0;
	int len = 0;

	if ( !non_blocking )
		set_nonblocking(sockfd);  
	while ( size > 0 )  {
		len = send(sockfd, (void *) buf, size, 0);
		if ( len == -1 )  {
			if ( (errno == EAGAIN) || (errno == EWOULDBLOCK) )  {
				if ( ++try > 5 )
					return(-1);
				php_sleep(1);
			} 
			else return(-1);
		} 
		else  {
			size -= len;
			buf  += len;
			try   = 0;
		}
	}
	return(0);
}

hg_msg *recv_hg_msg_head(int sockfd)
{
	hg_msg *msg;

	if ( (msg = (hg_msg *)emalloc(sizeof(hg_msg))) == NULL )  {
		lowerror = LE_MALLOC;
		return(NULL);
	}

	if ( hg_read_exact(sockfd, (char *)&(msg->length), 4) == -1 )  {
		efree(msg);
		return(NULL);
	}

	if ( hg_read_exact(sockfd, (char *)&(msg->version_msgid), 4) == -1 )  {
		efree(msg);
		return(NULL);
	}

	if ( hg_read_exact(sockfd, (char *)&(msg->msg_type), 4) == -1 )  {
		efree(msg);
		return(NULL);
	}

#ifdef HW_DEBUG
	php_printf("<b>   Recv msg: </b>type = %d -- id = %d<br />\n", msg->msg_type, msg->version_msgid);
#endif
	return(msg);
}


hg_msg *recv_hg_msg(int sockfd)
{
	hg_msg *msg;

	if ( (msg = (hg_msg *)emalloc(sizeof(hg_msg))) == NULL )  {
/*		php_printf("recv_hg_msg"); */
		lowerror = LE_MALLOC;
		return(NULL);
	}

	if ( hg_read_exact(sockfd, (char *)&(msg->length), 4) == -1 )  {
/*		php_printf("recv_hg_msg: hg_read (1) returned -1\n"); */
		efree(msg);
		return(NULL);
	}

	if ( hg_read_exact(sockfd, (char *)&(msg->version_msgid), 4) == -1 )  {
/*		php_printf("recv_hg_msg: hg_read (2) returned -1\n"); */
		efree(msg);
		return(NULL);
	}

	if ( hg_read_exact(sockfd, (char *)&(msg->msg_type), 4) == -1 )  {
/*		php_printf("recv_hg_msg: hg_read (3) returned -1\n"); */
		efree(msg);
		return(NULL);
	}

	if ( msg->length > HEADER_LENGTH )  {
		if ( (msg->buf = (char *) emalloc(msg->length-HEADER_LENGTH)) == NULL )  {
/*			php_printf("recv_hg_msg"); */
			lowerror = LE_MALLOC;
			efree(msg);
			return(NULL);
		}
		if ( hg_read_exact(sockfd, msg->buf, msg->length-HEADER_LENGTH) == -1 )  {
/*			php_printf("recv_hg_msg: hg_read (4) returned -1\n"); */
			efree(msg->buf);
			efree(msg);
			return(NULL);
		}
	}
	else
		msg->buf = NULL;  

#ifdef HW_DEBUG
	php_printf("<b>   Recv msg: </b>type = %d -- id = %d<br />\n", msg->msg_type, msg->version_msgid);
#endif
	return(msg);
}


hg_msg *recv_ready(int sockfd)
{
	hg_msg *ready_msg;

	if ( (ready_msg = recv_hg_msg(sockfd)) == NULL )  {
/*		php_printf("recv_ready: recv_hg_msg returned NULL\n"); */
		return(NULL);
	}    
	if ( ready_msg->msg_type != READY_MESSAGE )  {
/*		php_printf("recv_ready: recv_hg_msg returned wrong message: %d, %d  \n", ready_msg->length, ready_msg->msg_type); */
		efree(ready_msg);
		return(NULL);
	}

	return(ready_msg);
}


hg_msg *recv_command(int sockfd)
{
	hg_msg *comm_msg;

	if ( (comm_msg = recv_hg_msg(sockfd)) == NULL )  {
/*		fprintf(stderr, "recv_command: recv_hg_msg returned NULL\n"); */
		return(NULL);
	}
	if ( comm_msg->msg_type != COMMAND_MESSAGE )  {
/*		fprintf(stderr, "recv_command: recv_hg_msg returned wrong message\n"); */
		return(NULL);
	}

	return(comm_msg);
}

int send_dummy(int sockfd, hw_objectID objectID, int msgid, char **attributes)
{
	hg_msg msg, *retmsg;
	int  length, error;
	char *tmp;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, msgid);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )  {
		*attributes = NULL;
		return(-1);
	}

	if(0 == (int) *(retmsg->buf)) {
		*attributes = estrdup(retmsg->buf+sizeof(int));
		efree(retmsg->buf);
		efree(retmsg);
	} else {
		error = *((int *) retmsg->buf);
		*attributes = NULL;
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

	return(0);
}

static int bh_send_deleteobject(int sockfd, hw_objectID objectID) {
	hg_msg msg;
	int  length;
	char *tmp;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, DELETEOBJECT_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}
	efree(msg.buf);

	return(msgid-1);
}

static int uh_send_deleteobject(int sockfd) {
	hg_msg *retmsg;
	int  error;

	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )  {
		return(-1);
	}

	if(NULL == retmsg->buf) {
		efree(retmsg);
		return -1;
	}
	error = *((int *) retmsg->buf);
	efree(retmsg->buf);
	efree(retmsg);
	return(error);
}

int send_deleteobject(int sockfd, hw_objectID objectID)
{
	if(0 > bh_send_deleteobject(sockfd, objectID))
		return -1;
	return(uh_send_deleteobject(sockfd));
}

static int bh_send_changeobject(int sockfd, hw_objectID objectID, char *mod) {
	hg_msg msg;
	int  length;
	char *tmp;

	length = HEADER_LENGTH + sizeof(hw_objectID) + strlen(mod) + 1;

	build_msg_header(&msg, length, msgid++, CHANGEOBJECT_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);
	tmp = build_msg_str(tmp, mod);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}
	efree(msg.buf);

	return(msgid-1);
}

static int uh_send_changeobject(int sockfd) {
	hg_msg *retmsg;
	int  error;

	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )  {
		return(-1);
	}

	error = *((int *) retmsg->buf);
	efree(retmsg->buf);
	efree(retmsg);
	return(error);
}

int send_changeobject(int sockfd, hw_objectID objectID, char *modification)
{
	if(0 > bh_send_changeobject(sockfd, objectID, modification))
		return -1;
	return(uh_send_changeobject(sockfd));
}

int send_groupchangeobject(int sockfd, hw_objectID objectID, char *modification)
{
	hw_objectID *childIDs;
	int count, i, error;

	if(0 == (error = send_lock(sockfd, objectID))) {
		send_changeobject(sockfd, objectID, modification);
		send_unlock(sockfd, objectID);
	}/* else
		fprintf(stderr, "Could not lock 0x%X (error = %d)\n", objectID, error); */

	if(0 == send_children(sockfd, objectID, &childIDs, &count)) {
/*		fprintf(stderr, "Changing Children of 0x%X\n", objectID); */
		for(i=0; i<count; i++)
			if(0 > send_groupchangeobject(sockfd, childIDs[i], modification))
/*				fprintf(stderr, "Cannot change 0x%X\n", objectID) */;
		if(childIDs)
			efree(childIDs);
	}/*  else
		fprintf(stderr, "No Children of 0x%X\n", objectID);  */
	return(0);
}

static int bh_send_getobject(int sockfd, hw_objectID objectID) {
	hg_msg msg;
	int  length;
	char *tmp;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, GETOBJECT_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}
	efree(msg.buf);

	return(msgid-1);
}

static int uh_send_getobject(int sockfd, char **attributes) {
	hg_msg *retmsg;
	int  error;

	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )  {
		*attributes = NULL;
		return(-1);
	}

	if(0 == (int) *(retmsg->buf)) {
		*attributes = estrdup(retmsg->buf+sizeof(int));
		efree(retmsg->buf);
		efree(retmsg);
	} else {
		error = *((int *) retmsg->buf);
		*attributes = NULL;
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

	return(0);
}

int send_getobject(int sockfd, hw_objectID objectID, char **attributes)
{
	if(0 > bh_send_getobject(sockfd, objectID))
		return -1;
	return(uh_send_getobject(sockfd, attributes));
}

int send_getandlock(int sockfd, hw_objectID objectID, char **attributes)
{
	hg_msg msg, *retmsg;
	int  length, error;
	char *tmp;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, GETANDLOCK_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )  {
		*attributes = NULL;
		return(-1);
	}

	if(0 == (error = (int) *(retmsg->buf))) {
		*attributes = estrdup(retmsg->buf+sizeof(int));
	} else {
		*attributes = NULL;
	}

	efree(retmsg->buf);
	efree(retmsg);
	return error;
}

int send_lock(int sockfd, hw_objectID objectID)
{
	hg_msg msg, *retmsg;
	int  length, error;
	char *tmp;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, GETANDLOCK_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )  {
		return(-1);
	}

	error = *((int *) retmsg->buf);

	efree(retmsg->buf);
	efree(retmsg);
	return error;
}

int send_insertobject(int sockfd, char *objrec, char *parms, hw_objectID *objectID)
{
	hg_msg msg, *retmsg;
	int  length, error;
	char *tmp;
	int *ptr;

	length = HEADER_LENGTH + strlen(objrec) + 1 + strlen(parms) + 1;

	build_msg_header(&msg, length, msgid++, INSERTOBJECT_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_str(msg.buf, objrec);
	tmp = build_msg_str(tmp, parms);
/*fprintf(stderr, "objrec = %s, parms = %s\n", objrec, parms); */

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )  {
		*objectID = 0;
		return(-1);
	}

	ptr = (int *) retmsg->buf;
	if(0 == (error = *ptr)) {
		ptr++;
		*objectID = *ptr;
	} else {
		*objectID = 0;
	}
	efree(retmsg->buf);
	efree(retmsg);
	return error;
}

int send_unlock(int sockfd, hw_objectID objectID)
{
	hg_msg msg;
	int  length;
	char *tmp;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, UNLOCK_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	return 0;
}

int send_incollections(int sockfd, int retcol, int cobjids, hw_objectID *objectIDs, int ccollids, hw_objectID *collIDs, int *count, hw_objectID **retIDs)
{
	hg_msg msg, *retmsg;
	int  length, error;
	char *tmp;
	int *ptr, *ptr1, i;

	length = HEADER_LENGTH + sizeof(hw_objectID) + (cobjids + ccollids) * sizeof(hw_objectID) + 2 * sizeof(int);

	build_msg_header(&msg, length, msgid++, INCOLLECTIONS_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
		lowerror = LE_MALLOC;
		return(-3);
	}

	tmp = build_msg_int(msg.buf, retcol);
	tmp = build_msg_int(tmp, cobjids);
	for(i=0; i<cobjids; i++)
		tmp = build_msg_int(tmp, objectIDs[i]);
	tmp = build_msg_int(tmp, ccollids);
	for(i=0; i<ccollids; i++)
		tmp = build_msg_int(tmp, collIDs[i]);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-2);
	} 

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if(retmsg == NULL) {
		return(-1);
	}

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -1;
	}
	if(*ptr++ == 0) {
		*count = *ptr;
		ptr++;
		if(NULL != (*retIDs = emalloc(*count * sizeof(hw_objectID)))) {
			ptr1 = *retIDs;
			for(i=0; i<*count; ptr++, i++)
				ptr1[i] = *ptr;
			efree(retmsg->buf);
			efree(retmsg);
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
	} else {
		error = *((int *) retmsg->buf);
		if(retmsg->buf) efree(retmsg->buf);
		if(retmsg) efree(retmsg);
		return error;
	}
	return(0);
}


int send_inscoll(int sockfd, hw_objectID objectID, char *objrec, hw_objectID *new_objectID)
{
	hg_msg msg, *retmsg;
	int  length, error;
	char *tmp;
	int *ptr;

	length = HEADER_LENGTH + sizeof(hw_objectID) + strlen(objrec) + 1;

	build_msg_header(&msg, length, msgid++, INSCOLL_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
		lowerror = LE_MALLOC;
		return(-3);
	}

	tmp = build_msg_int(msg.buf, objectID);
	tmp = build_msg_str(tmp, objrec);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-2);
	} 

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if(retmsg == NULL) {
		return(-1);
	}

	ptr = (int *) retmsg->buf;
	if(0 != (error = *ptr)) {
		efree(retmsg->buf);
		efree(retmsg);
		*new_objectID = 0;
		return error;
	}

	ptr++;
	*new_objectID = *ptr;
	efree(retmsg->buf);
	efree(retmsg);
	return 0;
}

int send_insdoc(int sockfd, hw_objectID objectID, char *objrec, char *text, hw_objectID *new_objectID)
{
	hg_msg msg, *retmsg;
	int  length, error;
	char *tmp;
	int *ptr;

	length = HEADER_LENGTH + sizeof(hw_objectID) + strlen(objrec) + 1;
	if(text) {
		length += strlen(text);
		length++;
	}

	build_msg_header(&msg, length, msgid++, INSDOC_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
		lowerror = LE_MALLOC;
		return(-3);
	}

	tmp = build_msg_int(msg.buf, objectID);
	tmp = build_msg_str(tmp, objrec);
	if(text)
		tmp = build_msg_str(tmp, text);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-2);
	} 

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if(retmsg == NULL) {
		return(-1);
	}

	ptr = (int *) retmsg->buf;
	if(0 != (error = *ptr)) {
		efree(retmsg->buf);
		efree(retmsg);
		*new_objectID = 0;
		return error;
	}

	ptr++;
	*new_objectID = *ptr;
	efree(retmsg->buf); 
	efree(retmsg);
	return 0;
}

int send_getdestforanchorsobj(int sockfd, char **anchorrec, char ***destrec, int count);
int send_getreldestforanchorsobj(int sockfd, char **anchorrec, char ***reldestrec, int count, int rootID, int thisID);

int send_gettext(int sockfd, hw_objectID objectID, int mode, int rootid, char **objattr, char **bodytag, char **text, int *count, char *urlprefix)
{
	hg_msg msg, *retmsg;
	int  length, *ptr, ancount, error;
	char *tmp, *attributes, *documenttype;
	char **anchors;
	int i;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, GETOBJECT_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	} 

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if(retmsg == NULL) {
		attributes = NULL;
		return(-1);
	}

	ptr = (int *) retmsg->buf;
	if(*ptr == 0) {
		attributes = estrdup(retmsg->buf+sizeof(int));
		efree(retmsg->buf);
		efree(retmsg);
 	} else {
		error = *ptr;
		attributes = NULL;
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

	length = HEADER_LENGTH + strlen(attributes) + 1;
	build_msg_header(&msg, length, msgid++, GETTEXT_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_str(msg.buf, attributes);

	documenttype = fnAttributeValue(attributes, "DocumentType");
	*objattr = strdup(attributes);
	efree(attributes);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if (retmsg == NULL) {
		*text = NULL;
		return(-1);
	}

	ptr = (int *) retmsg->buf;
	if(*ptr == 0) {
		ptr++;
		*count = retmsg->length-HEADER_LENGTH-sizeof(int);
		if(NULL != (*text = malloc(*count + 1))) {
			memcpy(*text, retmsg->buf+sizeof(int), *count);
/*			*text[*count] = 0; */
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
 	} else {
		error = *ptr;
		efree(retmsg->buf);
		efree(retmsg);
 		*text = NULL;
		return(error);
 	}
	efree(retmsg->buf);
	efree(retmsg);

	if((documenttype != NULL) && (strcmp(documenttype, "Image") != 0)) {
		if(send_getanchorsobj(sockfd, objectID, &anchors, &ancount) == 0) {
			char **destrec, **reldestrec;
#ifdef newlist
			zend_llist *pAnchorList;
#else
			DLIST *pAnchorList;
#endif

			/* Get dest as relative and absolut path */
			send_getdestforanchorsobj(sockfd, anchors, &destrec, ancount);
			send_getreldestforanchorsobj(sockfd, anchors, &reldestrec, ancount, rootid, objectID);
			pAnchorList = fnCreateAnchorList(objectID, anchors, destrec, reldestrec, ancount, mode);

			/* Free only the array, the objrecs has been freed in fnCreateAnchorList() */
			if(anchors) efree(anchors);
			if(destrec) efree(destrec);
			if(reldestrec) efree(reldestrec);

			if(pAnchorList != NULL) {
				char *newtext;
				char *body = NULL;
				char **prefixarray;

				prefixarray = emalloc(5*sizeof(char *));
				for(i=0; i<5; i++)
					prefixarray[i] = urlprefix;

				newtext = fnInsAnchorsIntoText(*text, pAnchorList, &body, prefixarray);

				efree(prefixarray);
#ifdef newlist
				zend_llist_destroy(pAnchorList);
				efree(pAnchorList);
#else
				dlst_kill(pAnchorList, fnDeleteAnchor);
#endif
				*bodytag = strdup(body);
				if(body) efree(body);
				*text = newtext;
				*count = strlen(newtext);
			}
		}
	}

	if(documenttype) efree(documenttype);
	return(0);
}

int send_insertanchors(char **text, int *count, char **anchors, char **destrec, int ancount, char **urlprefix, char **bodytag) {
	char **reldestrec = NULL;
	int mode = 0;
	hw_objectID objectID = 0;
#ifdef newlist
	zend_llist *pAnchorList = NULL;
#else
	DLIST *pAnchorList = NULL;
#endif
	pAnchorList = fnCreateAnchorList(objectID, anchors, destrec, reldestrec, ancount, mode);

	/* Free only the array, the objrecs has been freed in fnCreateAnchorList() */
	if(anchors) efree(anchors);
	if(destrec) efree(destrec);
	if(reldestrec) efree(reldestrec);

	if(pAnchorList != NULL) {
		char *newtext;
		char *body = NULL;

		newtext = fnInsAnchorsIntoText(*text, pAnchorList, &body, urlprefix);

#ifdef newlist
		zend_llist_destroy(pAnchorList);
		efree(pAnchorList);
#else
		dlst_kill(pAnchorList, fnDeleteAnchor);
#endif
		*bodytag = strdup(body);
		if(body) efree(body);
		*text = newtext;
		*count = strlen(newtext);
	}
	return 0;
}

int send_edittext(int sockfd, char *objattr, char *text)
{
	hg_msg msg, *retmsg;
	int  length, *ptr, error;
	char *tmp, *path, *objid;
	hw_objectID objectID; 

	objid = fnAttributeValue(objattr, "ObjectID");
	if(objid == NULL)
		return(-1);
	if(!sscanf(objid, "0x%x", &objectID))
		return(-2);

	path = fnAttributeValue(objattr, "Path");
	if(path == NULL)
		return(-3);

	length = HEADER_LENGTH + sizeof(hw_objectID) + strlen(path) + 1 + 1 + strlen(text) + 1;
	build_msg_header(&msg, length, msgid++, EDITTEXT_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
		lowerror = LE_MALLOC;
		return(-4);
	}

	tmp = build_msg_int(msg.buf, objectID);
	tmp = build_msg_str(tmp, path);
	tmp = build_msg_str(tmp, "");
	tmp = build_msg_str(tmp, text);

	if(path) efree(path);
	if(objid) efree(objid);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-5);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if (retmsg == NULL) {
		*text = '\0';
		return(-6);
	}

	ptr = (int *) retmsg->buf;
	error = *ptr;
	efree(retmsg->buf);
	efree(retmsg);
	return(error);
}

int send_getcgi(int sockfd, hw_objectID objectID, char *cgi_env_str, char **objattr, char **text, int *count)
{
	hg_msg msg, *retmsg;
	int  length, *ptr, error, new_attr_len;
	char *tmp, *attributes, *new_attr;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, GETOBJECT_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	} 

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if(retmsg == NULL) {
		attributes = NULL;
		return(-1);
	}

	ptr = (int *) retmsg->buf;
	if(*ptr == 0) {
		attributes = estrdup(retmsg->buf+sizeof(int));
		efree(retmsg->buf);
		efree(retmsg);
 	} else {
		error = *ptr;
		attributes = NULL;
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

        new_attr_len = strlen(attributes) + strlen(cgi_env_str) + 2;
        new_attr = malloc(new_attr_len);
        strcpy(new_attr, attributes);
        strcat(new_attr, cgi_env_str);
	length = HEADER_LENGTH + strlen(new_attr) + 1 + sizeof(int);
	build_msg_header(&msg, length, msgid++, GETCGI_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_str(msg.buf, new_attr);
	tmp = build_msg_int(tmp, 0);

	*objattr = strdup(attributes);
	efree(attributes);
        free(new_attr);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if (retmsg == NULL) {
		*text = NULL;
		return(-1);
	}

	/* Attention: It looks like the documentation is not quite right.
	   According to the docs the buffer starts with an integer which
	   is followed by the output of the cgi script. This seems not to
	   be true. There is another integer right after the error.
	   The output of the cgi script is also preceded by the 'Content-type'
	   header. */
	ptr = (int *) retmsg->buf;
	if(*ptr++ == 1024) {
		*count = *ptr++;
		if(NULL != (*text = malloc(*count + 1))) {
			memcpy(*text, ptr, *count);
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
 	} else {
		error = *ptr + 1024;  /* move errors to >2024 */
		efree(retmsg->buf);
		efree(retmsg);
 		*text = NULL;
		return(error);
 	}
	efree(retmsg->buf);
	efree(retmsg);

	return(0);
}

int send_getremote(int sockfd, hw_objectID objectID, char **objattr, char **text, int *count)
{
	hg_msg msg, *retmsg;
	int  length, *ptr, error;
	char *tmp, *attributes;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, GETOBJECT_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	} 

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if(retmsg == NULL) {
		attributes = NULL;
		return(-1);
	}

	ptr = (int *) retmsg->buf;
	if(*ptr == 0) {
		attributes = estrdup(retmsg->buf+sizeof(int));
		efree(retmsg->buf);
		efree(retmsg);
 	} else {
		error = *ptr;
		attributes = NULL;
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

	length = HEADER_LENGTH + strlen(attributes) + 1 + sizeof(int);
	build_msg_header(&msg, length, msgid++, GETREMOTE_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_str(msg.buf, attributes);
	tmp = build_msg_int(tmp, 0);

	*objattr = strdup(attributes);
	efree(attributes);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if (retmsg == NULL) {
		*text = NULL;
		return(-1);
	}

	ptr = (int *) retmsg->buf;
	if(*ptr == 1024) {
		*count = retmsg->length-HEADER_LENGTH-sizeof(int)-sizeof(int);
		if(NULL != (*text = malloc(*count + 1))) {
			memcpy(*text, ptr+2, *count);
/*			*text[*count] = 0; */
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
 	} else {
		error = *ptr + 1024;  /* move errors to >2024 */
		efree(retmsg->buf);
		efree(retmsg);
 		*text = NULL;
		return(error);
 	}
	efree(retmsg->buf);
	efree(retmsg);

	return(0);
}

int send_getremotechildren(int sockfd, char *attributes, char **text, int **childIDs, int *count)
{
	hg_msg msg, *retmsg;
	int  length, *ptr, *ptr1, error;
	char *tmp;

/*	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, GETOBJECT_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	} 

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if(retmsg == NULL) {
		attributes = NULL;
		return(-1);
	}

	ptr = (int *) retmsg->buf;
	if(*ptr == 0) {
		attributes = estrdup(retmsg->buf+sizeof(int));
		efree(retmsg->buf);
		efree(retmsg);
 	} else {
		error = *ptr;
		attributes = NULL;
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}
*/
	length = HEADER_LENGTH + strlen(attributes) + 1 + sizeof(int);
	build_msg_header(&msg, length, msgid++, GETREMOTECHILDREN_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_str(msg.buf, attributes);
	tmp = build_msg_int(tmp, 0);

/*	efree(attributes); */ 

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if (retmsg == NULL) {
		*childIDs = NULL;
		return(-1);
	}
	ptr = (int *) retmsg->buf;
	if(*ptr++ == 1024) {
		int i, remlen, sum;
		*count = *ptr;
		ptr++;
		if(NULL != (*childIDs = emalloc(*count * sizeof(hw_objectID)))) {
			ptr1 = *childIDs;
			sum = 0;
			for(i=0; i<*count; ptr++, i++) {
				ptr1[i] = *ptr;
				sum += *ptr;
			}
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}

		remlen = retmsg->length - HEADER_LENGTH - *count * sizeof(int) - 2 * sizeof(int);
/*ptr1[i-1] = remlen; */
/*ptr1[i-2] = sum; */
/*ptr1[i-3] = *count; */
		if(NULL != (*text = emalloc(remlen + 1))) {
			memcpy(*text, ptr, remlen);
		} else {
			efree(childIDs);
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
		efree(retmsg->buf);
		efree(retmsg);
	} else {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		*childIDs = NULL;
		*text = NULL;
		return error;
	}
	return(0);
}

int send_mvcpdocscollscoll(int sockfd, hw_objectID *objectIDs, int count, int from, int dest, int cpmv, int doccoll)
{
	hg_msg msg, *retmsg;
	int  length, i, error;
	char *tmp;

	if(count <= 0)
		return 0;

	/*       HEADER_LENGTH + cpmv                + from                + dest                + count + nr of obj */
	length = HEADER_LENGTH + sizeof(hw_objectID) + sizeof(hw_objectID) + sizeof(hw_objectID) + sizeof(hw_objectID) + count * sizeof(hw_objectID);

	switch(doccoll) {
		case DOCUMENT:
			build_msg_header(&msg, length, msgid++, MVCPDOCSCOLL_MESSAGE);
			break;
		case COLLECTION:
			build_msg_header(&msg, length, msgid++, MVCPCOLLSCOLL_MESSAGE);
			break;
	}

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, cpmv);
	tmp = build_msg_int(tmp, from);
	tmp = build_msg_int(tmp, dest);
	tmp = build_msg_int(tmp, count);
	for(i=0; i<count;i++)
		tmp = build_msg_int(tmp, objectIDs[i]);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )  {
		return(-1);
	}

	error = *((int *) retmsg->buf);
	efree(retmsg->buf);
	efree(retmsg);
	return(error);
}

int send_docbyanchor(int sockfd, hw_objectID objectID, hw_objectID *anchorID)
{
	hg_msg msg, *retmsg;
	int  length, error;
	char *tmp;
	int *ptr;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, GETDOCBYANCHOR_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )  {
		return(-1);
	}

	ptr = (int *) retmsg->buf;
	error = *ptr++;
	if(error == 0) {
		*anchorID = *ptr;
	} else {
		*anchorID = 0;
	}
	efree(retmsg->buf);
	efree(retmsg);
	return error;
}

int send_docbyanchorobj(int sockfd, hw_objectID objectID, char **objrec)
{
	hg_msg msg, *retmsg;
	int  length, error;
	char *tmp;
	int *ptr, anchorID;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, GETDOCBYANCHOR_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )  {
		return(-1);
	}

	ptr = (int *) retmsg->buf;
	error = *ptr++;
	if(error == 0) {
		anchorID = *ptr;
	} else {
		anchorID = 0;
	}
	efree(retmsg->buf);
	efree(retmsg);

	if(0 > bh_send_getobject(sockfd, anchorID))
		return -1;
	return(uh_send_getobject(sockfd, objrec));
}

int send_children(int sockfd, hw_objectID objectID, hw_objectID **childIDs, int *count)
{
	hg_msg msg, *retmsg;
	int length, i, error;
	char *tmp;
	int *ptr, *ptr1;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, CHILDREN_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if (-1 == send_hg_msg(sockfd, &msg, length))  {
		efree(msg.buf);
		return(-2);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )  {
		return(-3);
	}

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -1;
	}
	if(*ptr++ == 0) {
		*count = *ptr++;
		if(NULL != (*childIDs = emalloc(*count * sizeof(hw_objectID)))) {
			ptr1 = *childIDs;
			for(i=0; i<*count; ptr++, i++)
				ptr1[i] = *ptr;
			efree(retmsg->buf);
			efree(retmsg);
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
	} else {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

	return(0);
}

int send_childrenobj(int sockfd, hw_objectID objectID, char ***childrec, int *count)
{
	hg_msg msg, *retmsg;
	int  length, i, error;
	char *tmp;
	int *childIDs = NULL;
	int *ptr;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, CHILDREN_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )
		return(-1);

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -1;
	}
	if(*ptr++ == 0) {
		*count = *ptr++;
		if(NULL != (childIDs = emalloc(*count * sizeof(hw_objectID)))) {
			for(i=0; i<*count; ptr++, i++)
				childIDs[i] = *ptr;
			efree(retmsg->buf);
			efree(retmsg);
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
	} else {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

	/* Now get for each child collection the object record */
#ifdef hw_less_server_stress
  if(0 != send_objectbyidquery(sockfd, childIDs, count, NULL, childrec)) {
		efree(childIDs);
		return -2;
	}
	efree(childIDs);
#else	
	for(i=0; i<*count; i++) {
		
		length = HEADER_LENGTH + sizeof(hw_objectID);
		build_msg_header(&msg, length, childIDs[i], GETOBJECT_MESSAGE);

		if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
			efree(childIDs);
			lowerror = LE_MALLOC;
			return(-1);
		}

		tmp = build_msg_int(msg.buf, childIDs[i]);

		if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
			efree(childIDs);
			efree(msg.buf);
			return(-1);
			}

		efree(msg.buf);
	}
	efree(childIDs);

	if(NULL == (objptr = (char **) emalloc(*count * sizeof(hw_objrec *)))) {
		/* if emalloc fails, get at least all remaining  messages from server */
		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			efree(retmsg->buf);
			efree(retmsg);
		}
  		*childrec = NULL;
		lowerror = LE_MALLOC;
		return(-1);
	} else {
  	*childrec = objptr;

		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			if ( retmsg != NULL )  {
				if(0 == (int) *(retmsg->buf)) {
					*objptr = estrdup(retmsg->buf+sizeof(int));
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				} else {
					*objptr = NULL;
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				}
			}
		}
	}
#endif
	return(0);
}

int send_getchildcoll(int sockfd, hw_objectID objectID, hw_objectID **childIDs, int *count)
{
	hg_msg msg, *retmsg;
	int  length, error;
	char *tmp;
	int *ptr, i, *ptr1;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, GETCHILDCOLL_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )  {
		return(-1);
	}

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -1;
	}
	if(*ptr++ == 0) {
		*count = *ptr++;
		if(NULL != (*childIDs = emalloc(*count * sizeof(hw_objectID)))) {
			ptr1 = *childIDs;
			for(i=0; i<*count; ptr++, i++)
				ptr1[i] = *ptr;
			efree(retmsg->buf);
			efree(retmsg);
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
	} else {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

	return(0);
}

int send_getchildcollobj(int sockfd, hw_objectID objectID, char ***childrec, int *count)
{
	hg_msg msg, *retmsg;
	int  length, i, error;
	char *tmp;
	int *childIDs = NULL;
	int *ptr;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, GETCHILDCOLL_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )
		return(-1);

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -1;
	}
	if(*ptr++ == 0) {
		*count = *ptr++;
		if(NULL != (childIDs = emalloc(*count * sizeof(hw_objectID)))) {
			for(i=0; i<*count; ptr++, i++)
				childIDs[i] = *ptr;
			efree(retmsg->buf);
			efree(retmsg);
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
	} else {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

	/* Now get for each child collection the object record */
#ifdef hw_less_server_stress
  if(0 != send_objectbyidquery(sockfd, childIDs, count, NULL, childrec)) {
		efree(childIDs);
		return -2;
	}
	efree(childIDs);
#else	
	for(i=0; i<*count; i++) {
		
		length = HEADER_LENGTH + sizeof(hw_objectID);
		build_msg_header(&msg, length, childIDs[i], GETOBJECT_MESSAGE);

		if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
			efree(childIDs);
			lowerror = LE_MALLOC;
			return(-1);
		}

		tmp = build_msg_int(msg.buf, childIDs[i]);

		if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
			efree(childIDs);
			efree(msg.buf);
			return(-1);
			}

		efree(msg.buf);
	}
	efree(childIDs);

	if(NULL == (objptr = (char **) emalloc(*count * sizeof(hw_objrec *)))) {
		/* if emalloc fails, get at least all remaining  messages from server */
		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			efree(retmsg->buf);
			efree(retmsg);
		}
  		*childrec = NULL;
		lowerror = LE_MALLOC;
		return(-1);
	} else {
  	*childrec = objptr;

		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			if ( retmsg != NULL )  {
				if(0 == (int) *(retmsg->buf)) {
					*objptr = estrdup(retmsg->buf+sizeof(int));
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				} else {
					*objptr = NULL;
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				}
			}
		}
	}
#endif
	return(0);
}

int send_getchilddoccoll(int sockfd, hw_objectID objectID, hw_objectID **childIDs, int *count)
{
	hg_msg msg, *retmsg;
	int  length, error;
	char *tmp;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, GETCHILDDOCCOLL_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	} else {
		efree(msg.buf);
		retmsg = recv_hg_msg(sockfd);
		if ( retmsg != NULL )  {
			int *ptr, i, *ptr1;

			ptr = (int *) retmsg->buf;
			if(ptr == NULL) {
				if(retmsg) efree(retmsg);
				return -1;
			}
			if(*ptr++ == 0) {
				*count = *ptr++;
				if(NULL != (*childIDs = emalloc(*count * sizeof(hw_objectID)))) {
					ptr1 = *childIDs;
					for(i=0; i<*count; ptr++, i++)
						ptr1[i] = *ptr;
					efree(retmsg->buf);
					efree(retmsg);
				} else {
					efree(retmsg->buf);
					efree(retmsg);
					lowerror = LE_MALLOC;
					return(-1);
				}
			} else {
				error = *((int *) retmsg->buf);
				efree(retmsg->buf);
				efree(retmsg);
				return error;
			}
		}
	}

	return(0);

}

int send_getchilddoccollobj(int sockfd, hw_objectID objectID, hw_objrec ***childrec, int *count)
{
	hg_msg msg, *retmsg;
	int  length, error;
	char *tmp;
	int *childIDs = NULL;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, GETCHILDDOCCOLL_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);
	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}
	efree(msg.buf);

	retmsg = recv_hg_msg(sockfd);
	if ( retmsg != NULL )  {
		int *ptr, i;

		ptr = (int *) retmsg->buf;
		if(ptr == NULL) {
			if(retmsg->buf) efree(retmsg->buf);
			return -1;
		}
		if(*ptr++ == 0) {
			*count = *ptr++;
			if(NULL != (childIDs = emalloc(*count * sizeof(hw_objectID)))) {
				for(i=0; i<*count; ptr++, i++)
					childIDs[i] = *ptr;
				efree(retmsg->buf);
				efree(retmsg);
			} else {
				efree(retmsg->buf);
				efree(retmsg);
				lowerror = LE_MALLOC;
				return(-1);
			}
		} else {
			error = *((int *) retmsg->buf);
			efree(retmsg->buf);
			efree(retmsg);
			return error;
		}
	}

	/* Now get for each child collection the object record */
#ifdef hw_less_server_stress
  if(0 != send_objectbyidquery(sockfd, childIDs, count, NULL, childrec)) {
		efree(childIDs);
		return -2;
	}
	efree(childIDs);
#else	
	for(i=0; i<*count; i++) {
		length = HEADER_LENGTH + sizeof(hw_objectID);
		build_msg_header(&msg, length, childIDs[i], GETOBJECT_MESSAGE);

		if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*			perror("send_command"); */
			lowerror = LE_MALLOC;
			return(-1);
		}

		tmp = build_msg_int(msg.buf, childIDs[i]);

		if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
			efree(msg.buf);
			return(-1);
			}

		efree(msg.buf);
	}

	if(NULL == (objptr = (char **) emalloc(*count * sizeof(hw_objrec *)))) {
		/* if emalloc fails, get at least all remaining  messages from server */
		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			efree(retmsg->buf);
			efree(retmsg);
		}
  		*childrec = NULL;
		lowerror = LE_MALLOC;
		return(-1);
	} else {
  	*childrec = objptr;

		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			if ( retmsg != NULL )  {
				if(0 == (int) *(retmsg->buf)) {
					*objptr = estrdup(retmsg->buf+sizeof(int));
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				} else {
					*objptr = NULL;
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				}
			}
		}
	}
#endif
	return(0);
}

int send_getanchors(int sockfd, hw_objectID objectID, int **anchorIDs, int *count)
{
	hg_msg msg, *retmsg;
	int  length, error;
	char *tmp;
	int *ptr, i, *ptr1;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, GETANCHORS_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);
	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}
	efree(msg.buf);

	if (NULL == (retmsg = recv_hg_msg(sockfd)))
	   return(-1);

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -1;
	}
	if(*ptr++ == 0) {
		*count = *ptr++;
		if(NULL != (*anchorIDs = emalloc(*count * sizeof(hw_objectID)))) {
			ptr1 = *anchorIDs;
			for(i=0; i<*count; ptr++, i++)
				ptr1[i] = *ptr;
			efree(retmsg->buf);
			efree(retmsg);
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
	} else {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

	return(0);
}

int send_getanchorsobj(int sockfd, hw_objectID objectID, char ***childrec, int *count)
{
	hg_msg msg, *retmsg;
	int  length, error;
	char *tmp;
	int *ptr, i, *ptr1;
	int *anchorIDs = NULL;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, 50, GETANCHORS_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);
	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}
	efree(msg.buf);

	if (NULL == (retmsg = recv_hg_msg(sockfd)))
	   return(-1);

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -1;
	}
	if(*ptr++ == 0) {
		*count = *ptr++;
		if(NULL != (anchorIDs = emalloc(*count * sizeof(hw_objectID)))) {
			ptr1 = anchorIDs;
			for(i=0; i<*count; ptr++, i++)
				ptr1[i] = *ptr;
			efree(retmsg->buf);
			efree(retmsg);
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
	} else {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

	/* Now get for each anchor the object record */
#ifdef hw_less_server_stress
  if(0 != send_objectbyidquery(sockfd, anchorIDs, count, NULL, childrec)) {
		efree(anchorIDs);
		return -2;
	}
	efree(anchorIDs);
#else
	for(i=0; i<*count; i++) {
		
		length = HEADER_LENGTH + sizeof(hw_objectID);
		build_msg_header(&msg, length, anchorIDs[i], GETOBJECT_MESSAGE);

		if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*			perror("send_command"); */
			lowerror = LE_MALLOC;
			return(-1);
		}

		tmp = build_msg_int(msg.buf, anchorIDs[i]);

		if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
			efree(msg.buf);
			return(-1);
			}

		efree(msg.buf);
	}
	if(anchorIDs) efree(anchorIDs);

	if(NULL == (objptr = (char **) emalloc(*count * sizeof(hw_objrec *)))) {
		/* if emalloc fails, get at least all remaining  messages from server */
		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			efree(retmsg->buf);
			efree(retmsg);
		}
  		*childrec = NULL;
		lowerror = LE_MALLOC;
		return(-1);
	} else {
  	*childrec = objptr;

		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			if ( retmsg != NULL )  {
				if(0 == (int) *(retmsg->buf)) {
					*objptr = estrdup(retmsg->buf+sizeof(int));
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				} else {
					*objptr = NULL;
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				}
			}
		}
	}
#endif
	return(0);
}

int send_getdestforanchorsobj(int sockfd, char **anchorrec, char ***destrec, int count)
{
	int i;
	char *objptr, **destptr;
	char *str;
	int objectID;

	if(NULL == (destptr = (char **) emalloc(count * sizeof(hw_objrec *)))) {
		lowerror = LE_MALLOC;
		return -1;
		}

	/* Now get for each anchor the object record of its destination */
	for(i=0; i<count; i++) {
		/* if you retrieve the anchors you sometimes get more than actually accessible.
		*/
		if((NULL != anchorrec[i]) && (NULL != (str = fnAttributeValue(anchorrec[i], "Dest")))) {
			sscanf(str, "0x%x", &objectID);
			efree(str);

			/* Using send_docbyanchorobj() makes sense because the Destination can
			   be both, an anchor or a document. If it is a document you get the
			   objectrecord of that document. If it is an anchor the function
			   graps the document which belongs to the anchor
			   and you get also the objectrecord of that document.
			*/
			if(0 > send_docbyanchorobj(sockfd, objectID, &objptr)) {
				efree(destptr);
				return -1;
			}
			destptr[i] = objptr;
			/* if we can't get the object rec of the dest, than this document
			   is probably not accessible for us. For later functions simply
			   set anchorrec[i] to NULL */
			if(destptr[i] == NULL) {
				if(anchorrec[i]) efree(anchorrec[i]);
				anchorrec[i] = NULL;
			}
		} else {
			destptr[i] = NULL;
		}
	}
	*destrec = destptr;

	return(0);
}

int send_getreldestforanchorsobj(int sockfd, char **anchorrec, char ***reldestrec, int count, int rootID, int thisID)
{
	int i;
	char *docofanchorptr, **reldestptr;
	char *str;
	int destobjectID;

	if(NULL == (reldestptr = (char **) emalloc(count * sizeof(char *)))) {
		lowerror = LE_MALLOC;
		return -1;
		}

	/* Now get for each anchor the object record of its destination */
	for(i=0; i<count; i++) {
		/* if you retrieve the anchors you sometimes get more than actually accessible.
		*/
		if((NULL != anchorrec[i]) && (NULL != (str = fnAttributeValue(anchorrec[i], "Dest")))) {
			sscanf(str, "0x%x", &destobjectID);
			efree(str);

			/* See note in send_getdestforanchorsobj() at same position in source code */
			if(0 > send_docbyanchorobj(sockfd, destobjectID, &docofanchorptr)) {
				efree(reldestptr);
				return -1;
			}

			reldestptr[i] = docofanchorptr;
			/* if we can't get the object rec of the dest, than this document
			   is probably not accessible for us. For later functions simply
			   set anchorrec[i] to NULL */
			if(reldestptr[i] == NULL) {
				if(anchorrec[i]) efree(anchorrec[i]);
				anchorrec[i] = NULL;
			} else {
				int j, *retthisIDs, *retdestIDs, equaltill, mincount, countthis, countdest, destdocid;
				char destdocname[200];
				char anchorstr[300];
				char temp[200];
				char *strptr;

				if(NULL != (str = strstr(docofanchorptr, "Name="))) {
					str += 5;
					sscanf(str, "%s\n", destdocname);
				}
				if(NULL != (str = strstr(docofanchorptr, "ObjectID="))) {
					str += 9;
					sscanf(str, "0x%X", &destdocid);
				}
				
				send_incollections(sockfd, 1, 1, &thisID, 1, &rootID, &countthis, &retthisIDs);
				send_incollections(sockfd, 1, 1, &destdocid, 1, &rootID, &countdest, &retdestIDs);

/*
fprintf(stderr, "%d: ", thisID);
for(k=0; k<countthis; k++)
	fprintf(stderr, "%d, ", retthisIDs[k]);
fprintf(stderr, "\n");
fprintf(stderr, "%d: ", destdocid);
for(k=0; k<countdest; k++)
	fprintf(stderr, "%d: %d, ", destdocid, retdestIDs[k]);
fprintf(stderr, "\n");
*/

				mincount = (countthis < countdest) ? countthis : countdest;
				for(j=0; (j<mincount) && (retthisIDs[j]==retdestIDs[j]); j++)
					;
				equaltill = j;
				strcpy(anchorstr, "Name=");
				for(j=equaltill; j<countthis; j++)
					strcat(anchorstr, "../");
				strcat(anchorstr, "./");
				for(j=equaltill; j<countdest; j++) {
					char *temprec, tempname[100];
					send_getobject(sockfd, retdestIDs[j], &temprec);
					if(NULL != (str = strstr(temprec, "Name="))) {
						str += 5;
						sscanf(str, "%s\n", tempname);
					} else if(NULL != (str = strstr(temprec, "ObjectID="))) {
						str += 9;
						sscanf(str, "%s\n", tempname);
					}

					sprintf(temp, "%s", tempname);
					strptr = temp;
					while(*strptr != '\0') {
						if(*strptr == '/')
							*strptr = '_';
						strptr++;
					}
/* fprintf(stderr, "Adding '%s' to '%s'\n", temp, anchorstr); */
					strcat(anchorstr, temp);
					strcat(anchorstr, "/");
/* fprintf(stderr, "Is now '%s'\n", anchorstr); */
					efree(temprec);
				}
				/* if the anchor destination is a collection it may not be added anymore. */
				if(destdocid != retdestIDs[countdest-1]) {
					strptr = destdocname;
					while(*strptr != '\0') {
						if(*strptr == '/')
							*strptr = '_';
						strptr++;
					}
					strcat(anchorstr, destdocname);
				} else {
					strcat(anchorstr, "index.html");
				}
				strcat(anchorstr, "\n");
				sprintf(temp, "ObjectID=0x%x", destdocid);
				strcat(anchorstr, temp);
/* fprintf(stderr, "%s\n", anchorstr); */
				efree(retthisIDs);
				efree(retdestIDs);
				efree(reldestptr[i]);
				reldestptr[i] = estrdup(anchorstr);
			}
		} else {
			reldestptr[i] = NULL;
		}
	}
	*reldestrec = reldestptr;

	return(0);
}

int fn_findpath(int sockfd, int *retIDs, int count, int id) {
	int *pathIDs;
	int *parentIDs, pcount, pid;
	int u, j, i;
	
	if(NULL == (pathIDs = emalloc(count * sizeof(int)))) {
		lowerror = LE_MALLOC;
		return -1;
	}
	u = count-1;
	pid = id;
	pcount = 1;
	/* FIXME but parentIDs is not set at this point, why checking it? */
	while((u >= 0) && (pcount != 0) && (parentIDs != NULL) && (pid != 0)) {
/*fprintf(stderr, "Get parents for %d\n", pid); */
		if(0 != send_getparents(sockfd, pid, &parentIDs, &pcount)) {
			efree(pathIDs);
			return -1;
		}
		pid = 0;
		for(i=0; i<pcount; i++) {
			for(j=0; j<count; j++) {
				if(parentIDs[i] == retIDs[j]) {
					pathIDs[u--] = retIDs[j];
					pid = retIDs[j];
				}
			}
		}
		if(pid == 0)
			fprintf(stderr, "parent not found in list\n");
		if(parentIDs) efree(parentIDs);
	}
/*fprintf(stderr, "sorted path: "); */
	for(i=0; i<count; i++) {
		retIDs[i] = pathIDs[i];
/*fprintf(stderr, "%d, ", retIDs[i]); */
	}
/*fprintf(stderr, "\n"); */
	efree(pathIDs);
	return 0;
}

/********************************************************************
* function getrellink()                                             *
*                                                                   *
* Returns the link to point from document with ID sourceID to       *
* document with id destID.                                          *
********************************************************************/
int getrellink(int sockfd, int rootID, int thisID, int destID, char **reldeststr) {
	int i, j, k, *retthisIDs, *retdestIDs, equaltill, count, mincount, countthis, countdest;
	char anchorstr[300];
	char temp[200];
	char *strptr;

	send_incollections(sockfd, 1, 1, &thisID, 1, &rootID, &countthis, &retthisIDs);
	send_incollections(sockfd, 1, 1, &destID, 1, &rootID, &countdest, &retdestIDs);


fprintf(stderr, "%d: ", thisID);
for(k=0; k<countthis; k++)
	fprintf(stderr, "%d, ", retthisIDs[k]);
fprintf(stderr, "\n");
fprintf(stderr, "%d: ", destID);
for(k=0; k<countdest; k++)
	fprintf(stderr, "%d, ", retdestIDs[k]);
fprintf(stderr, "\n");

	/*
	** The message incollections returned a list of collections
	** in which the destID or thisID is contained. Unfortunately
	** this list ist not the path for the destID or thisID, but
	** a sorted list of collection IDs. If for example you are
	** looking for an ID 20 which has a path 1 -> 5 -> 4 -> 20
	** (this means: 20 is child of 4, 4 is child of 5, 5 is child
	** of 1) it will return 1, 4, 5 instead of 1, 5, 4
	** Consequently, we have to create the correct path, by checking
	** for the parents and identifying it in the list.
	** But there is another problem. If the id for which the list of
	** of collection is generated is a colletion itself, it will
	** show up in the list as well. In order to make the algorithmn
	** work proberly it has to be the last member of the list.
	*/
	for(i=0; i<countdest; i++)
		if(retdestIDs[i] == destID) {
			retdestIDs[i] = retdestIDs[countdest-1];
			retdestIDs[countdest-1] = destID;
		}
	count = (retdestIDs[countdest-1] == destID) ? countdest-1 : countdest;
	if(0 != fn_findpath(sockfd, retdestIDs, count, destID)) {
		efree(retthisIDs);
		efree(retdestIDs);
		return -1;
	}
	for(i=0; i<countthis; i++)
		if(retthisIDs[i] == thisID) {
			retthisIDs[i] = retthisIDs[countthis-1];
			retthisIDs[countthis-1] = thisID;
		}
	count = (retthisIDs[countthis-1] == thisID) ? countthis-1 : countthis;
	if(0 != fn_findpath(sockfd, retthisIDs, count, thisID)) {
		efree(retthisIDs);
		efree(retdestIDs);
		return -1;
	}

	mincount = (countthis < countdest) ? countthis : countdest;
fprintf(stderr, "mincount = %d\n", mincount);
	for(j=0; (j<mincount) && (retthisIDs[j]==retdestIDs[j]); j++)
		;
	equaltill = j;
fprintf(stderr, "first unequal = %d\n", j);
	strcpy(anchorstr, "");
	for(j=equaltill; j<countthis; j++)
		strcat(anchorstr, "../");
	strcat(anchorstr, "./");
	for(j=equaltill; j<countdest; j++) {
		char *temprec, *str, tempname[100];
		if(0 == send_getobject(sockfd, retdestIDs[j], &temprec)) {
			if(NULL != (str = strstr(temprec, "Name="))) {
				str += 5;
				sscanf(str, "%s\n", tempname);
			} else if(NULL != (str = strstr(temprec, "ObjectID="))) {
				str += 9;
				sscanf(str, "%s\n", tempname);
			}
	
			sprintf(temp, "%s", tempname);
			strptr = temp;
			while(*strptr != '\0') {
				if(*strptr == '/')
					*strptr = '_';
				strptr++;
			}
fprintf(stderr, "Adding '%s' (%d) to '%s'\n", temp, retdestIDs[j], anchorstr);
			strcat(anchorstr, temp);
			strcat(anchorstr, "/");
fprintf(stderr, "Is now '%s'\n", anchorstr);
			efree(temprec);
		} else {
			strcat(anchorstr, "No access/");
		}
	}
	/* if the anchor destination is a collection it may not be added anymore. */
	if(destID != retdestIDs[countdest-1]) {
		char destdocname[100], *str;
		send_getobject(sockfd, destID, &str);
		if(NULL != (strptr = strstr(str, "Name="))) {
			strptr += 5;
			sscanf(strptr, "%s\n", destdocname);
		}
		strptr = destdocname;
		while(*strptr != '\0') {
			if(*strptr == '/')
				*strptr = '_';
			strptr++;
		}
		strcat(anchorstr, destdocname);
		efree(str);
	} else {
/*		strcat(anchorstr, "index.html"); */
	}
/*fprintf(stderr, "%s\n", anchorstr); */
	efree(retthisIDs);
	efree(retdestIDs);
	*reldeststr = estrdup(anchorstr);
	return 0;
}

int send_identify(int sockfd, char *name, char *passwd, char **userdata) {
	hg_msg msg, *retmsg;
	int  length;
	char *tmp;

	length = HEADER_LENGTH + sizeof(int) + strlen(name) + 1 + strlen(passwd) + 1;

	build_msg_header(&msg, length, msgid++, IDENTIFY_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, 0);
	tmp = build_msg_str(tmp, name);
	tmp = build_msg_str(tmp, passwd);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )
		return(-1);

	*userdata = (char *) retmsg->buf;
	efree(retmsg);

	return(0);
}

int send_objectbyidquery(int sockfd, hw_objectID *IDs, int *count, char *query, char ***objrecs)
{
	hg_msg msg, *retmsg;
	int  length;
	char *tmp, *str;
	int *ptr, i;
	int *offsets, *childIDs;
	char **childrec;

	if(*count <= 0) {
		*objrecs = emalloc(0);
		return(0);
	}
	length = HEADER_LENGTH + sizeof(int) + sizeof(int) + *count * sizeof(hw_objectID);
	if(query)
		length = length + strlen(query) + 1;

	build_msg_header(&msg, length, msgid++, OBJECTBYIDQUERY_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, 1);
	tmp = build_msg_int(tmp, *count);
	for(i=0; i<*count; i++)
		tmp = build_msg_int(tmp, IDs[i]);
	if(query)
		tmp = build_msg_str(tmp, query);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}
	efree(msg.buf);

#ifdef hw_optimize
	{
	int hg_error;
	int c, allc;

	allc = 0;
	retmsg = recv_hg_msg_head(sockfd);
	if ( retmsg == NULL ) 
		return(-1);
	
	/* read error field */
	if ( (c = hg_read_exact(sockfd, (char *) &hg_error, 4)) == -1 ) {
		if(retmsg) efree(retmsg);
		return(-2);
	}
	allc += c;

	if(hg_error) {
		if(retmsg) efree(retmsg);
		return(-3);
	}

	/* read count field */
	if ( (c = hg_read_exact(sockfd, (char *) count, 4)) == -1 ) {
		if(retmsg) efree(retmsg);
		return(-2);
	}
	allc += c;

	if(NULL != (childIDs = emalloc(*count * sizeof(hw_objectID)))) {
		if((c = hg_read_exact(sockfd, (char *) childIDs, *count * sizeof(hw_objectID))) == -1) {
			efree(childIDs);
			if(retmsg) efree(retmsg);
			return(-3);
		}
	} else {
		efree(retmsg);
		lowerror = LE_MALLOC;
		return(-4);
	}
	allc += c;

	if(NULL != (offsets = emalloc(*count * sizeof(int)))) {
		if((c = hg_read_exact(sockfd, (char *) offsets, *count * sizeof(int))) == -1) {
			efree(childIDs);
			efree(offsets);
			if(retmsg) efree(retmsg);
			return(-5);
		}
	} else {
		efree(retmsg);
		efree(childIDs);
		lowerror = LE_MALLOC;
		return(-6);
	}  
	allc += c;

	str = (char *)ptr;
	if(NULL == (childrec = (char **) emalloc(*count * sizeof(hw_objrec *)))) {
		efree(offsets);
		efree(childIDs);
		efree(retmsg);
		lowerror = LE_MALLOC;
		return(-1);
	} else {
		for(i=0; i<*count; i++) {
			char *ptr;
			childrec[i] = emalloc(offsets[i] + 1);
			ptr = childrec[i];
			c = hg_read_exact(sockfd, (char *) ptr, offsets[i]);
			ptr[c] = '\0';
			allc += c;
		}
		/* Reading the trailing '\0' */
		c = hg_read_exact(sockfd, (char *) &hg_error, 1);
		*objrecs = childrec;
	}
	}
#else
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL ) 
		return(-1);

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -1;
	}
	if(*ptr++ != 0) {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

	*count = *ptr++;
	if(NULL != (childIDs = emalloc(*count * sizeof(hw_objectID)))) {
		ptr1 = childIDs;
		for(i=0; i<*count; ptr++, i++)
			ptr1[i] = *ptr;
		if(NULL != (offsets = emalloc(*count * sizeof(int)))) {
			ptr1 = offsets;
			for(i=0; i<*count; ptr++, i++)
				ptr1[i] = *ptr;
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			efree(childIDs);
			lowerror = LE_MALLOC;
			return(-1);
		}
	} else {
		efree(retmsg->buf);
		efree(retmsg);
		lowerror = LE_MALLOC;
		return(-1);
	}

	str = (char *)ptr;
	if(NULL == (childrec = (char **) emalloc(*count * sizeof(hw_objrec *)))) {
		efree(offsets);
		efree(childIDs);
		efree(retmsg->buf);
		efree(retmsg);
		lowerror = LE_MALLOC;
		return(-1);
	} else {
		for(i=0; i<*count; i++) {
			char *ptr;
			childrec[i] = emalloc(offsets[i] + 1);
			ptr = childrec[i];
			memcpy(ptr, str, offsets[i]);
			ptr[offsets[i]] = '\0';
			str += offsets[i];
		}
		*objrecs = childrec;
	}

	efree(retmsg->buf);
#endif

	efree(retmsg);
	efree(childIDs);
	efree(offsets);
	return(0);
}

int send_getobjbyquery(int sockfd, char *query, int maxhits, hw_objectID **childIDs, int *count)
{
	hg_msg msg, *retmsg;
	int  length, error;
	char *tmp;
	int *ptr, i, *ptr1;

	length = HEADER_LENGTH + strlen(query) + 1;

	build_msg_header(&msg, length, msgid++, GETOBJBYQUERY_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_str(msg.buf, query);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}
	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL ) 
		return(-1);

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -1;
	}
	if(*ptr++ == 0) {
		*count = (*ptr < maxhits) ? *ptr : maxhits;
		ptr++;
		if(NULL != (*childIDs = emalloc(*count * sizeof(hw_objectID)))) {
			ptr1 = *childIDs;
			for(i=0; i<*count; ptr++, i++)
				ptr1[i] = *ptr;
			efree(retmsg->buf);
			efree(retmsg);
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
	} else {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}
	return(0);
}

int send_getobjbyqueryobj(int sockfd, char *query, int maxhits, char ***childrec, int *count)
{
	hg_msg msg, *retmsg;
	int length, i, error;
	char *tmp;
	int *childIDs = NULL;
	int *ptr, *ptr1;

	length = HEADER_LENGTH + strlen(query) + 1;

	build_msg_header(&msg, length, msgid++, GETOBJBYQUERY_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_str(msg.buf, query);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-2);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL ) 
		return(-3);

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -4;
	}
	if(*ptr++ == 0) {
		*count = (*ptr < maxhits) ? *ptr : maxhits;
    		ptr++;
		if(NULL != (childIDs = emalloc(*count * sizeof(hw_objectID)))) {
			ptr1 = childIDs;
			for(i=0; i<*count; ptr++, i++)
				ptr1[i] = *ptr;
			efree(retmsg->buf);
			efree(retmsg);
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-5);
		}
	} else {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

	/* Now get for each child collection the object record */
#ifdef hw_less_server_stress
  if(0 != send_objectbyidquery(sockfd, childIDs, count, NULL, childrec)) {
		efree(childIDs);
		return -2;
	}
	efree(childIDs);
#else
	for(i=0; i<*count; i++) {
		length = HEADER_LENGTH + sizeof(hw_objectID);
		build_msg_header(&msg, length, childIDs[i], GETOBJECT_MESSAGE);

		if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*			perror("send_command"); */
			efree(childIDs);
			lowerror = LE_MALLOC;
			return(-6);
		}

		tmp = build_msg_int(msg.buf, childIDs[i]);

		if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
			efree(msg.buf);
			efree(childIDs);
			return(-7);
			}

		efree(msg.buf);
	}
	efree(childIDs);

	if(NULL == (objptr = (char **) emalloc(*count * sizeof(hw_objrec *)))) {
		/* if emalloc fails, get at least all remaining  messages from server */
		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			efree(retmsg->buf);
			efree(retmsg);
		}
  		*childrec = NULL;
		lowerror = LE_MALLOC;
		return(-8);
	} else {
	  	*childrec = objptr;

		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			if ( retmsg != NULL )  {
				if(0 == (int) *(retmsg->buf)) {
					*objptr = estrdup(retmsg->buf+sizeof(int));
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				} else {
					*objptr = NULL;
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				}
			}
		}
	}
#endif
	return(0);
}

int send_getobjbyquerycoll(int sockfd, hw_objectID collID, char *query, int maxhits, hw_objectID **childIDs, int *count)
{
	hg_msg msg, *retmsg;
	int  length, error;
	char *tmp;
	int *ptr, i, *ptr1;

	length = HEADER_LENGTH + strlen(query) + 1 + sizeof(int) + sizeof(collID);

	build_msg_header(&msg, length, msgid++, GETOBJBYQUERYCOLL_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, 1);
	tmp = build_msg_int(tmp, collID);
	tmp = build_msg_str(tmp, query);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL ) 
		return(-1);

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -1;
	}
	if(*ptr++ == 0) {
		*count = (*ptr < maxhits) ? *ptr : maxhits;
		ptr++;
		if(NULL != (*childIDs = emalloc(*count * sizeof(hw_objectID)))) {
			ptr1 = *childIDs;
			for(i=0; i<*count; ptr++, i++)
				ptr1[i] = *ptr;
			efree(retmsg->buf);
			efree(retmsg);
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
	} else {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}
	return(0);
}

int send_getobjbyquerycollobj(int sockfd, hw_objectID collID, char *query, int maxhits, char ***childrec, int *count)
{
	hg_msg msg, *retmsg;
	int length, i, error;
	char *tmp;
	hw_objectID *childIDs = NULL;
	int *ptr, *ptr1;

	length = HEADER_LENGTH + strlen(query) + 1 + sizeof(int) + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, GETOBJBYQUERYCOLL_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, 1);
	tmp = build_msg_int(tmp, collID);
	tmp = build_msg_str(tmp, query);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	} 

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )
		return -1;

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -1;
	}
	if(*ptr++ == 0) {
		*count = (*ptr < maxhits) ? *ptr : maxhits;
		ptr++;
		if(NULL != (childIDs = emalloc(*count * sizeof(hw_objectID)))) {
			ptr1 = childIDs;
			for(i=0; i<*count; ptr++, i++)
				ptr1[i] = *ptr;
			efree(retmsg->buf);
			efree(retmsg);
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
	} else {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

	/* Now get for each child collection the object record */
#ifdef hw_less_server_stress
  if(0 != send_objectbyidquery(sockfd, childIDs, count, NULL, childrec)) {
		if(childIDs) efree(childIDs);
		return -2;
	}
	if(childIDs) efree(childIDs);
#else
	for(i=0; i<*count; i++) {
		length = HEADER_LENGTH + sizeof(hw_objectID);
		build_msg_header(&msg, length, childIDs[i], GETOBJECT_MESSAGE);

		if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*			perror("send_command"); */
			efree(childIDs);
			lowerror = LE_MALLOC;
			return(-1);
		}

		tmp = build_msg_int(msg.buf, childIDs[i]);

		if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
			efree(msg.buf);
			efree(childIDs);
			return(-1);
			}

		efree(msg.buf);
	}
	efree(childIDs);

	if(NULL == (objptr = (char **) emalloc(*count * sizeof(hw_objrec *)))) {
		/* if emalloc fails, get at least all remaining  messages from server */
		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			efree(retmsg->buf);
			efree(retmsg);
		}
  	*childrec = NULL;
		lowerror = LE_MALLOC;
		return(-1);
	} else {
  		*childrec = objptr;

		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			if ( retmsg != NULL )  {
				if(0 == (int) *(retmsg->buf)) {
					*objptr = estrdup(retmsg->buf+sizeof(int));
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				} else {
					*objptr = NULL;
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				}
			}
		}
	}
#endif
	return(0);
}

int send_getobjbyftquery(int sockfd, char *query, int maxhits, hw_objectID **childIDs, float **weights, int *count)
{
	hg_msg msg, *retmsg;
	int  length, error;
	char *tmp;
	int *ptr, i, *ptr1;
	float *ptr2;

	length = HEADER_LENGTH + strlen(query) + 1;

	build_msg_header(&msg, length, msgid++, GETOBJBYFTQUERY_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_str(msg.buf, query);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}
	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL ) 
		return(-1);

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -1;
	}
	if(*ptr++ == 0) {
		char *cptr, tmp[20];
		float weight;
		int j;
		*count = (*ptr < maxhits) ? *ptr : maxhits;
		ptr++;
		if(NULL != (*childIDs = emalloc(*count * sizeof(hw_objectID)))) {
			ptr1 = *childIDs;
			if(NULL != (*weights = emalloc(*count * sizeof(float)))) {
				ptr2 = *weights;
				for(i=0; i<*count; i++) {
					ptr1[i] = *ptr++; /* Object id */
					cptr = (char *) ptr;
					j = 0;
					while(*cptr != ' ') {
						tmp[j++] = *cptr++;
					}
					cptr++; /* Skip space after weight */
					tmp[j] = '\0';
					sscanf(tmp, "%f", &weight);
					ptr2[i] = weight;
					ptr = (int *) cptr;
					ptr++; /* Skip 0-Integer after weight string */
				}
				efree(retmsg->buf);
				efree(retmsg);
			} else {
				efree(*childIDs);
				efree(retmsg->buf);
				efree(retmsg);
				lowerror = LE_MALLOC;
				return(-1);
			}
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
	} else {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}
	return(0);
}

int send_getobjbyftqueryobj(int sockfd, char *query, int maxhits, char ***childrec, float **weights, int *count)
{
	hg_msg msg, *retmsg;
	int length, i, error;
	char *tmp;
	int *childIDs = NULL;
	int *ptr, *ptr1;
	float *ptr2;

	length = HEADER_LENGTH + strlen(query) + 1;

	build_msg_header(&msg, length, msgid++, GETOBJBYFTQUERY_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_str(msg.buf, query);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-2);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL ) 
		return(-3);

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -4;
	}
	if(*ptr++ == 0) {
		char *cptr, tmp[20];
		float weight;
		int j;
		*count = (*ptr < maxhits) ? *ptr : maxhits;
    		ptr++;
		if(NULL != (childIDs = emalloc(*count * sizeof(hw_objectID)))) {
			ptr1 = childIDs;
			if(NULL != (*weights = emalloc(*count * sizeof(float)))) {
				ptr2 = *weights;
				for(i=0; i<*count; i++) {
					ptr1[i] = *ptr++; /* Object id */
					cptr = (char *) ptr;
					j = 0;
					while(*cptr != ' ') {
						tmp[j++] = *cptr++;
					}
					cptr++; /* Skip space after weight */
					tmp[j] = '\0';
					sscanf(tmp, "%f", &weight);
					ptr2[i] = weight;
					ptr = (int *) cptr;
					ptr++; /* Skip 0-Integer after weight string */
				}
				efree(retmsg->buf);
				efree(retmsg);
			} else {
				efree(childIDs);
				efree(retmsg->buf);
				efree(retmsg);
				lowerror = LE_MALLOC;
				return(-5);
			}
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-5);
		}
	} else {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

	/* Now get for each child collection the object record */
#ifdef hw_less_server_stress
  if(0 != send_objectbyidquery(sockfd, childIDs, count, NULL, childrec)) {
		efree(childIDs);
		efree(*weights);
		return -2;
	}
	efree(childIDs);
#else
	for(i=0; i<*count; i++) {
		length = HEADER_LENGTH + sizeof(hw_objectID);
		build_msg_header(&msg, length, childIDs[i], GETOBJECT_MESSAGE);

		if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
			efree(childIDs);
			efree(*weights);
			lowerror = LE_MALLOC;
			return(-6);
		}

		tmp = build_msg_int(msg.buf, childIDs[i]);

		if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
			efree(msg.buf);
			efree(childIDs);
			efree(*weights);
			return(-7);
			}

		efree(msg.buf);
	}
	efree(childIDs);

	if(NULL == (objptr = (char **) emalloc(*count * sizeof(hw_objrec *)))) {
		/* if emalloc fails, get at least all remaining  messages from server */
		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			efree(retmsg->buf);
			efree(retmsg);
		}
  		*childrec = NULL;
		lowerror = LE_MALLOC;
		return(-8);
	} else {
		*childrec = objptr;

		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			if ( retmsg != NULL )  {
				if(0 == (int) *(retmsg->buf)) {
					*objptr = estrdup(retmsg->buf+sizeof(int));
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				} else {
					*objptr = NULL;
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				}
			}
		}
	}
#endif
	return(0);
}

int send_getobjbyftquerycoll(int sockfd, hw_objectID collID, char *query, int maxhits, hw_objectID **childIDs, float **weights, int *count)
{
	hg_msg msg, *retmsg;
	int  length, error;
	char *tmp;
	int *ptr, i, *ptr1;
	float *ptr2;

	length = HEADER_LENGTH + strlen(query) + 1 + sizeof(int) + sizeof(collID);

	build_msg_header(&msg, length, msgid++, GETOBJBYFTQUERYCOLL_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, 1);
	tmp = build_msg_int(tmp, collID);
	tmp = build_msg_str(tmp, query);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL ) 
		return(-1);

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -1;
	}
	if(*ptr++ == 0) {
		char *cptr, tmp[20];
		float weight;
		int j;
		*count = (*ptr < maxhits) ? *ptr : maxhits;
		ptr++;
		if(NULL != (*childIDs = emalloc(*count * sizeof(hw_objectID)))) {
			ptr1 = *childIDs;
			if(NULL != (*weights = emalloc(*count * sizeof(float)))) {
				ptr2 = *weights;
				for(i=0; i<*count; i++) {
					ptr1[i] = *ptr++; /* Object id */
					cptr = (char *) ptr;
					j = 0;
					while(*cptr != ' ') {
						tmp[j++] = *cptr++;
					}
					cptr++; /* Skip space after weight */
					tmp[j] = '\0';
					sscanf(tmp, "%f", &weight);
					ptr2[i] = weight;
					ptr = (int *) cptr;
					ptr++; /* Skip 0-Integer after weight string */
				}
			} else {
				efree(*childIDs);
				efree(retmsg->buf);
				efree(retmsg);
				lowerror = LE_MALLOC;
				return(-1);
			}
			efree(retmsg->buf);
			efree(retmsg);
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
	} else {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}
	return(0);
}

int send_getobjbyftquerycollobj(int sockfd, hw_objectID collID, char *query, int maxhits, char ***childrec, float **weights, int *count)
{
	hg_msg msg, *retmsg;
	int length, i, error;
	char *tmp;
	hw_objectID *childIDs = NULL;
	int *ptr, *ptr1;
	float *ptr2;

	length = HEADER_LENGTH + strlen(query) + 1 + sizeof(int) + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, GETOBJBYFTQUERYCOLL_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, 1);
	tmp = build_msg_int(tmp, collID);
	tmp = build_msg_str(tmp, query);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	} 

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )
		return -1;

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -1;
	}
	if(*ptr++ == 0) {
		char *cptr, tmp[20];
		float weight;
		int j;
		*count = (*ptr < maxhits) ? *ptr : maxhits;
		ptr++;
		if(NULL != (childIDs = emalloc(*count * sizeof(hw_objectID)))) {
			ptr1 = childIDs;
			if(NULL != (*weights = emalloc(*count * sizeof(float)))) {
				ptr2 = *weights;
				for(i=0; i<*count; i++) {
					ptr1[i] = *ptr++; /* Object id */
					cptr = (char *) ptr;
					j = 0;
					while(*cptr != ' ') {
						tmp[j++] = *cptr++;
					}
					cptr++; /* Skip space after weight */
					tmp[j] = '\0';
					sscanf(tmp, "%f", &weight);
					ptr2[i] = weight;
					ptr = (int *) cptr;
					ptr++; /* Skip 0-Integer after weight string */
				}
				efree(retmsg->buf);
				efree(retmsg);
			} else {
				efree(childIDs);
				efree(retmsg->buf);
				efree(retmsg);
				lowerror = LE_MALLOC;
				return(-1);
			}
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
	} else {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

	/* Now get for each child collection the object record */
#ifdef hw_less_server_stress
  if(0 != send_objectbyidquery(sockfd, childIDs, count, NULL, childrec)) {
		if(childIDs) efree(childIDs);
		if(*weights) efree(weights);
		return -2;
	}
	if(childIDs) efree(childIDs);
#else
	for(i=0; i<*count; i++) {
		length = HEADER_LENGTH + sizeof(hw_objectID);
		build_msg_header(&msg, length, childIDs[i], GETOBJECT_MESSAGE);

		if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*			perror("send_command"); */
			efree(childIDs);
			efree(*weights);
			lowerror = LE_MALLOC;
			return(-1);
		}

		tmp = build_msg_int(msg.buf, childIDs[i]);

		if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
			efree(msg.buf);
			efree(childIDs);
			efree(*weights);
			return(-1);
			}

		efree(msg.buf);
	}
	efree(childIDs);

	if(NULL == (objptr = (char **) emalloc(*count * sizeof(hw_objrec *)))) {
		/* if emalloc fails, get at least all remaining  messages from server */
		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			efree(retmsg->buf);
			efree(retmsg);
		}
  	*childrec = NULL;
		lowerror = LE_MALLOC;
		return(-1);
	} else {
  		*childrec = objptr;

		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			if ( retmsg != NULL )  {
				if(0 == (int) *(retmsg->buf)) {
					*objptr = estrdup(retmsg->buf+sizeof(int));
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				} else {
					*objptr = NULL;
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				}
			}
		}
	}
#endif
	return(0);
}

int send_getparents(int sockfd, hw_objectID objectID, hw_objectID **childIDs, int *count)
{
	hg_msg msg, *retmsg;
	int length, i, error;
	char *tmp;
	int *ptr, *ptr1;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, GETPARENT_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if (-1 == send_hg_msg(sockfd, &msg, length))  {
		efree(msg.buf);
		return(-2);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )  {
		return(-3);
	}

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -1;
	}
	if(*ptr++ == 0) {
		*count = *ptr++;
		if(NULL != (*childIDs = emalloc(*count * sizeof(hw_objectID)))) {
			ptr1 = *childIDs;
			for(i=0; i<*count; ptr++, i++)
				ptr1[i] = *ptr;
			efree(retmsg->buf);
			efree(retmsg);
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
	} else {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

	return(0);
}

int send_getparentsobj(int sockfd, hw_objectID objectID, char ***childrec, int *count)
{
	hg_msg msg, *retmsg;
	int length, i, error;
	char *tmp;
	hw_objectID *childIDs = NULL;
	int *ptr;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, GETPARENT_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if (-1 == send_hg_msg(sockfd, &msg, length))  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )  {
		return(-1);
	}

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -1;
	}
	if(*ptr++ == 0) {
		*count = *ptr++;
		if(NULL != (childIDs = emalloc(*count * sizeof(hw_objectID)))) {
			for(i=0; i<*count; ptr++, i++)
				childIDs[i] = *ptr;
			efree(retmsg->buf);
			efree(retmsg);
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
	} else {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

	/* Now get for each parent the object record */
#ifdef hw_less_server_stress
  if(0 != send_objectbyidquery(sockfd, childIDs, count, NULL, childrec)) {
		efree(childIDs);
		return -2;
	}
	efree(childIDs);
#else
	for(i=0; i<*count; i++) {
		length = HEADER_LENGTH + sizeof(hw_objectID);
		build_msg_header(&msg, length, childIDs[i], GETOBJECT_MESSAGE);

		if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*			perror("send_command"); */
			lowerror = LE_MALLOC;
			return(-1);
		}

		tmp = build_msg_int(msg.buf, childIDs[i]);

		if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
			efree(msg.buf);
			return(-1);
			}

		efree(msg.buf);
	}
	efree(childIDs);

	if(NULL == (objptr = (char **) emalloc(*count * sizeof(hw_objrec *)))) {
		/* if emalloc fails, get at least all remaining  messages from server */
		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			efree(retmsg->buf);
			efree(retmsg);
		}
  		*childrec = NULL;
		lowerror = LE_MALLOC;
		return(-1);
	} else {
  		*childrec = objptr;

		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			if ( retmsg != NULL )  {
				if(0 == (int) *(retmsg->buf)) {
					*objptr = estrdup(retmsg->buf+sizeof(int));
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				} else {
					*objptr = NULL;
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				}
			}
		}
	}
#endif
	return(0);
}

int send_pipedocument(int sockfd, char *host, hw_objectID objectID, int mode, int rootid, char **objattr, char **bodytag, char **text, int *count, char **urlprefix)
{
	hg_msg msg, *retmsg;
	int	length, len;
	char *tmp, header[80], *head_ptr, *sizestr;
	struct sockaddr_in	serv_addr;
	struct hostent	    *hostptr;
	char *hostip = NULL;
	char *attributes = NULL;
	char *documenttype;
	char **anchors;
	int newfd, fd, port, size, error, ancount;
	int *ptr;

	if(-1 == (fd = fnCOpenDataCon(sockfd, &port))) {
		 /* not set yet  efree(msg.buf); */
		   return(-1);
	}

	/*
	** Get information about host
	*/
	if(host) {
		if((hostptr = gethostbyname(host)) == NULL) {
			HWSOCK_FCLOSE(fd);
			return(-2);
		}
	} else {
		HWSOCK_FCLOSE(fd);
		return(-2);
	}
	 
	switch(hostptr->h_addrtype) {
		struct in_addr *ptr1;
		char *ptr;
		case AF_INET:
			ptr = hostptr->h_addr_list[0];
			ptr1 = (struct in_addr *) ptr;
			hostip = inet_ntoa(*ptr1);
			break;
		default:
			HWSOCK_FCLOSE(fd);
			return(-3);
			break;
	}
	 
	/* Bottom half of send_getobject */
	if(0 > bh_send_getobject(sockfd, objectID)) {
		HWSOCK_FCLOSE(fd);
		return -4;
	}

	/* Upper half of send_getobject */
	if(0 > (error = uh_send_getobject(sockfd, &attributes))) {
		HWSOCK_FCLOSE(fd);
		return error;
	}

	length = HEADER_LENGTH + sizeof(hw_objectID) + sizeof(int) + strlen(hostip) + 1 + strlen("Refno=0x12345678") + 1;
	build_msg_header(&msg, length, msgid++, PIPEDOCUMENT_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL ) {
		if(attributes) efree(attributes);
		lowerror = LE_MALLOC;
		return(-5);
	}

	tmp = build_msg_int(msg.buf, objectID);
	tmp = build_msg_int(tmp, port);
	tmp = build_msg_str(tmp, hostip);
	tmp = build_msg_str(tmp, "Refno=0x12345678");

	if ( send_hg_msg(sockfd, &msg, length) == -1 )	{
		if(attributes) efree(attributes);
		efree(msg.buf);
		HWSOCK_FCLOSE(fd);
		return(-6);
	}
	efree(msg.buf);

	/* Just check if the command was understood */
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL ) {
		if(attributes) efree(attributes);
		HWSOCK_FCLOSE(fd);
		return(-7);
	}

	ptr = (int *) retmsg->buf;
	if((ptr == NULL) || (*ptr != 0)) {
		error = *ptr;
		if(retmsg->buf) efree(retmsg->buf);
		efree(retmsg);
		if(attributes) efree(attributes);
		HWSOCK_FCLOSE(fd);
		return(error);
	}

	efree(retmsg->buf);
	efree(retmsg);

	/* passively open the data connection. The HG server is probably
	   already waiting for us.
	*/
	len = sizeof(serv_addr);
	if((newfd = accept(fd, (struct sockaddr *) &serv_addr, &len)) < 0) {
/*		php_printf("client: can't open data connection to server\n"); */
		if(attributes) efree(attributes);
		HWSOCK_FCLOSE(fd);
		return(-8);
	} else {
		HWSOCK_FCLOSE(fd);
	}

	/* First of all read the header */
	head_ptr = header;
	while((read_to(newfd, head_ptr, 1, rtimeout) == 1) && (*head_ptr != '\0')) {
		head_ptr++;
	}

	/* Let's see how big the document is and read it into var text */
	sizestr = strstr(header, "sz=");
	if(sizestr) {
		sizestr += 3;
		sscanf(sizestr, "%d\n", &size);
		*count = size;
		if((size != 0) && (NULL != (*text = malloc(size+1)))) {
			read_to(newfd, *text, size, rtimeout);
                (*text)[size] = '\0';
		}
	} else {
		*text = NULL;
	}

	/* close the data connection */
	HWSOCK_FCLOSE(newfd);

	documenttype = fnAttributeValue(attributes, "DocumentType");

	/* Make a copy with strdup (not estrdup), because we want to
	   keep the attributes in hw_document struct.
	*/
	*objattr = strdup(attributes);
	efree(attributes);

	if((documenttype != NULL) && (!strcmp(documenttype, "text") != 0)) {
		if(send_getanchorsobj(sockfd, objectID, &anchors, &ancount) == 0) {
			char **destrec, **reldestrec;
#ifdef newlist
			zend_llist *pAnchorList = NULL;
#else
			DLIST *pAnchorList = NULL;
#endif

			/* Get dest as relative and absolut path */
			send_getdestforanchorsobj(sockfd, anchors, &destrec, ancount);
			send_getreldestforanchorsobj(sockfd, anchors, &reldestrec, ancount, rootid, objectID);
			pAnchorList = fnCreateAnchorList(objectID, anchors, destrec, reldestrec, ancount, mode);
			/* Free only the array, the objrecs has been freed in fnCreateAnchorList() */
			if(anchors) efree(anchors);
			if(destrec) efree(destrec);
			if(reldestrec) efree(reldestrec);

			if(pAnchorList != NULL) {
				char *newtext;
				char *body = NULL;

				newtext = fnInsAnchorsIntoText(*text, pAnchorList, &body, urlprefix);
#ifdef newlist
				zend_llist_destroy(pAnchorList);
				efree(pAnchorList);
#else
				dlst_kill(pAnchorList, fnDeleteAnchor);
#endif
				*bodytag = strdup(body);
				if(body) efree(body);
				*text = newtext;
				*count = strlen(newtext);
			}
		}
	} else {
		*bodytag = NULL;
	}

	if(documenttype) efree(documenttype);
	return(0);
}

int send_pipecgi(int sockfd, char *host, hw_objectID objectID, char *cgi_env_str, char **objattr, char **text, int *count)
{
	hg_msg msg, *retmsg;
	int	length, len, new_attr_len;
	char *tmp, header[80], *head_ptr, *sizestr;
	struct sockaddr_in	serv_addr;
	struct hostent	    *hostptr;
	char *hostip = NULL;
	char *attributes = NULL;
	char *documenttype, *new_attr;
	int newfd, fd, port, size, error;
	int *ptr;

	if(-1 == (fd = fnCOpenDataCon(sockfd, &port))) {
		  /* not set yet? efree(msg.buf); */
		   return(-1);
	}

	/*
	** Get information about host
	*/
	if(host) {
		if((hostptr = gethostbyname(host)) == NULL) {
			HWSOCK_FCLOSE(fd);
			return(-1);
		}
	} else {
		HWSOCK_FCLOSE(fd);
		return(-1);
	}
	 
	switch(hostptr->h_addrtype) {
		struct in_addr *ptr1;
		char *ptr;
		case AF_INET:
			ptr = hostptr->h_addr_list[0];
			ptr1 = (struct in_addr *) ptr;
			hostip = inet_ntoa(*ptr1);
			break;
		default:
/*			php_printf(stderr, "unknown address type\n"); */
			break;
	}
	 
	/* Bottom half of send_getobject */
	if(0 > bh_send_getobject(sockfd, objectID)) {
		HWSOCK_FCLOSE(fd);
		return -1;
	}

	/* Upper half of send_getobject */
	if(0 > (error = uh_send_getobject(sockfd, &attributes))) {
		HWSOCK_FCLOSE(fd);
		return error;
	}

        new_attr_len = strlen(attributes) + strlen(cgi_env_str) + 2;
        new_attr = malloc(new_attr_len);
        strcpy(new_attr, attributes);
        strcat(new_attr, cgi_env_str);
	length = HEADER_LENGTH + strlen(new_attr) + 1 + sizeof(int) + strlen(hostip) + 1 + sizeof(int) + sizeof(int);
	build_msg_header(&msg, length, msgid++, PIPECGI_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL ) {
		if(attributes) efree(attributes);
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_str(msg.buf, hostip);
	tmp = build_msg_int(tmp, port);
	tmp = build_msg_str(tmp, new_attr);
	tmp = build_msg_int(tmp, 1);
	tmp = build_msg_int(tmp, 0x12345678);
	free(new_attr);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )	{
		if(attributes) efree(attributes);
		efree(msg.buf);
		HWSOCK_FCLOSE(fd);
		return(-1);
	}
	efree(msg.buf);

	/* Just check if the command was understood */
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL ) {
		if(attributes) efree(attributes);
		HWSOCK_FCLOSE(fd);
		return(-1);
	}

	ptr = (int *) retmsg->buf;
	if((ptr == NULL) || (*ptr != 0)) {
		if(retmsg->buf) efree(retmsg->buf);
		efree(retmsg);
		if(attributes) efree(attributes);
		HWSOCK_FCLOSE(fd);
		return(-1);
	}

	efree(retmsg->buf);
	efree(retmsg);

	/* passively open the data connection. The HG server is probably
           already waiting for us.
        */
	len = sizeof(serv_addr);
	if((newfd = accept(fd, (struct sockaddr *) &serv_addr, &len)) < 0) {
		if(attributes) efree(attributes);
		HWSOCK_FCLOSE(fd);
		return(-1);
	} else {
		HWSOCK_FCLOSE(fd);
	}

	/* First of all read the header */
	head_ptr = header;
	while((read_to(newfd, head_ptr, 1, rtimeout) == 1) && (*head_ptr != '\0')) {
		head_ptr++;
	}

	/* Let's see how big the document is and read it into var text */
	sizestr = strstr(header, "sz=");
	if(sizestr) {
		sizestr += 3;
		sscanf(sizestr, "%d\n", &size);
		*count = size;
		if((size != 0) && (NULL != (*text = malloc(size+1)))) {
		  read_to(newfd, *text, size, rtimeout);
		}
	} else {
		*text = NULL;
	}

	/* close the data connection */
	HWSOCK_FCLOSE(newfd);

	documenttype = fnAttributeValue(attributes, "DocumentType");

	/* Make a copy with strdup (not estrdup), because we want to
	   keep the attributes in hw_document struct.
	*/
	*objattr = strdup(attributes);
	efree(attributes);

	if(documenttype) efree(documenttype);
	return(0);
}

int send_putdocument(int sockfd, char *host, hw_objectID parentID, char *objectRec, char *text, int count, hw_objectID *objectID)
{
	hg_msg msg, *retmsg;
	int length, len;
	char *tmp, header[80], parms[30], *head_ptr;
	struct sockaddr_in serv_addr;
	struct hostent *hostptr;
	char *hostip = NULL;
	int newfd, fd, port, error;
	int *ptr;

	/* First of all we have to insert the document record */
        sprintf(parms, "Parent=0x%x", parentID);
	length = HEADER_LENGTH + strlen(objectRec) + 1 + strlen(parms) + 1;

	build_msg_header(&msg, length, msgid++, INSERTOBJECT_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_str(msg.buf, objectRec);
	tmp = build_msg_str(tmp, parms);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-2);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )  {
		return(-3);
	}

	ptr = (int *) retmsg->buf;
	if(0 == (error = *ptr)) {
		ptr++;
		*objectID = *ptr;
	} else {
		if(retmsg->buf) efree(retmsg->buf);
		efree(retmsg);
		return(error);
	}

	efree(retmsg->buf);
	efree(retmsg);

	/*
	** Get information about host
	*/
	if(host) {
		if((hostptr = gethostbyname(host)) == NULL) {
			/* close(fd); fd is not set yet */
			return(-4);
		}
	} else {
		/* close(fd); fd is not set yet */
		return(-5);
	}
	 
	switch(hostptr->h_addrtype) {
		struct in_addr *ptr1;
		char *ptr;
		case AF_INET:
			ptr = hostptr->h_addr_list[0];
			ptr1 = (struct in_addr *) ptr;
			hostip = inet_ntoa(*ptr1);
			break;
		default:
/*			fprintf(stderr, "unknown address type\n"); */
			break;
	}
	 
	if(-1 == (fd = fnCOpenDataCon(sockfd, &port))) {
		   efree(msg.buf);
		   return(-6);
	}

	/* Start building the PUTDOCUMENT message. I works even if
	   the Refno is skipped. I guess the path can be omitted too. */
	length = HEADER_LENGTH + sizeof(hw_objectID) + sizeof(int) + strlen(hostip) + 1 + strlen("Hyperwave") + 1+ strlen("Refno=0x12345678") + 1;

	build_msg_header(&msg, length, msgid++, PUTDOCUMENT_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )	{
		lowerror = LE_MALLOC;
		return(-7);
	}

	tmp = build_msg_int(msg.buf, *objectID);
	tmp = build_msg_int(tmp, port);
	tmp = build_msg_str(tmp, hostip);
	tmp = build_msg_str(tmp, "Hyperwave");
	tmp = build_msg_str(tmp, "Refno=0x12345678");

	if ( send_hg_msg(sockfd, &msg, length) == -1 )	{
		efree(msg.buf);
		HWSOCK_FCLOSE(fd);
		return(-8);
	}
	efree(msg.buf);

	/* passively open the data connection. The HG server is probably
           already waiting for us.
        */
	len = sizeof(serv_addr);
	if((newfd = accept(fd, (struct sockaddr *) &serv_addr, &len)) < 0) {
		HWSOCK_FCLOSE(fd);
		return(-9);
	} else {
		HWSOCK_FCLOSE(fd);
	}

	/* First of all write the header. According to the documentation
	   there should be a header first. Well, after some investigation
	   with tcpdump I found out, that Harmony and wavemaster don't
	   sent it. The also do not sent the Refno in the PUTDOCUMENT msg.
	   Anyway, we sent both. */
	head_ptr = header;
	sprintf(header, "HGHDR\nsz=%d\nref=12345678\n", count);
	len = strlen(header) + 1;
	if(len != write_to(newfd, header, len, wtimeout)) {
		HWSOCK_FCLOSE(newfd);
		return(-10);
	}

	/* And now the document */
	if(count != write_to(newfd, text, count, wtimeout)) {
		HWSOCK_FCLOSE(newfd);
		return(-11);
	}
	
 	/* The data connection has to be close before the return
	   msg can be read. The server will not sent it before. */
	HWSOCK_FCLOSE(newfd);

	/* Just check if the command was understood */
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL ) {
		HWSOCK_FCLOSE(fd);
		return(-12);
	}

	ptr = (int *) retmsg->buf;
	if((ptr == NULL) || (*ptr != 0)) {
		if(retmsg->buf) efree(retmsg->buf);
		efree(retmsg);
		HWSOCK_FCLOSE(fd);
		return(-13);
	}

	efree(retmsg->buf);
	efree(retmsg);

	return(0);
}

int send_getsrcbydest(int sockfd, hw_objectID objectID, char ***childrec, int *count)
{
	hg_msg msg, *retmsg;
	int length, i, error;
	char *tmp;
	int *childIDs = NULL;
	char **objptr;
	int *ptr, *ptr1;

	length = HEADER_LENGTH + sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, GETSRCBYDEST_MESSAGE);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, objectID);

	if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
		efree(msg.buf);
		return(-1);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL ) 
		return(-1);

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -1;
	}
	if(*ptr++ == 0) {
		*count = *ptr;
    		ptr++;
		if(NULL != (childIDs = emalloc(*count * sizeof(hw_objectID)))) {
			ptr1 = childIDs;
			for(i=0; i<*count; ptr++, i++)
				ptr1[i] = *ptr;
			efree(retmsg->buf);
			efree(retmsg);
		} else {
			efree(retmsg->buf);
			efree(retmsg);
			lowerror = LE_MALLOC;
			return(-1);
		}
	} else {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

	/* Now get for each source the object record */
	for(i=0; i<*count; i++) {
		length = HEADER_LENGTH + sizeof(hw_objectID);
		build_msg_header(&msg, length, childIDs[i], GETOBJECT_MESSAGE);

		if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*			perror("send_command"); */
			efree(childIDs);
			lowerror = LE_MALLOC;
			return(-1);
		}

		tmp = build_msg_int(msg.buf, childIDs[i]);

		if ( send_hg_msg(sockfd, &msg, length) == -1 )  {
			efree(childIDs);
			efree(msg.buf);
			return(-1);
			}

		efree(msg.buf);
	}
	efree(childIDs);

	if(NULL == (objptr = (char **) emalloc(*count * sizeof(hw_objrec *)))) {
		/* if emalloc fails, get at least all remaining  messages from server */
		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			efree(retmsg->buf);
			efree(retmsg);
		}
  		*childrec = NULL;
		lowerror = LE_MALLOC;
		return(-1);
	} else {
	  	*childrec = objptr;

		for(i=0; i<*count; i++) {
			retmsg = recv_hg_msg(sockfd);
			if ( retmsg != NULL )  {
				if(0 == (int) *(retmsg->buf)) {
					*objptr = estrdup(retmsg->buf+sizeof(int));
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				} else {
					*objptr = NULL;
					objptr++;
					efree(retmsg->buf);
					efree(retmsg);
				}
			}
		}
	}

	return(0);
}

int send_mapid(int sockfd, int servid, hw_objectID id, int *virtid)
{
	hg_msg msg, *retmsg;
	int length, error;
	char *tmp;
	int *ptr;

	length = HEADER_LENGTH + 2 * sizeof(hw_objectID);

	build_msg_header(&msg, length, msgid++, HG_MAPID);

	if ( (msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
		lowerror = LE_MALLOC;
		return(-1);
	}

	tmp = build_msg_int(msg.buf, servid);
	tmp = build_msg_int(tmp, id);

	if (-1 == send_hg_msg(sockfd, &msg, length))  {
		efree(msg.buf);
		return(-2);
	}

	efree(msg.buf);
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL )  {
		return(-3);
	}

	ptr = (int *) retmsg->buf;
	if(ptr == NULL) {
		if(retmsg) efree(retmsg);
		return -1;
	}
	if(*ptr++ == 0) {
		*virtid = *ptr;
	} else {
		error = *((int *) retmsg->buf);
		efree(retmsg->buf);
		efree(retmsg);
		return error;
	}

	return(0);
}

#define BUFFERLEN 200
char *get_hw_info(hw_connection *conn) {
	char temp[BUFFERLEN];
	int len;
	struct sockaddr_in serv_addr;

	len = sizeof (serv_addr);
	if(getsockname(conn->socket, (struct sockaddr *)&serv_addr, &len) < 0)
		return(NULL);

	snprintf(temp, BUFFERLEN, "%s, %s, %d, %s, %d, %d", conn->server_string, conn->hostname,
                                      conn->version, conn->username,
                                      serv_addr.sin_port, conn->swap_on);
	return(estrdup(temp));
}
#undef BUFFERLEN

static int send_hg_msg(int sockfd, hg_msg *msg, int length)
{
     char *buf, *tmp;

#ifdef HW_DEBUG
	php_printf("<b>Sending msg: </b>type = %d -- id = %d<br />\n", msg->msg_type, msg->version_msgid);
#endif
     if ( length < HEADER_LENGTH )  {
/*          fprintf(stderr, "send_hg_msg: bad msg\n"); */
          return(-1);
     }

     if ( (tmp = buf = (char *)emalloc(length)) == NULL )  {
/*		perror("send_hg_msg"); */
		lowerror = LE_MALLOC;
		return(-1);
     }

     memcpy(tmp, (char *) &(msg->length), 4);
     tmp += 4;
     memcpy(tmp, (char *) &(msg->version_msgid), 4);
     tmp += 4;
     memcpy(tmp, (char *) &(msg->msg_type), 4);
     if ( msg->length > HEADER_LENGTH )  {
          tmp += 4;
          memcpy(tmp, msg->buf, length-HEADER_LENGTH);
     }

     if ( hg_write(sockfd, buf, length) == -1 )  {
          efree(buf);
          return(-1);
     }

     efree(buf);
     return(0);
}


int send_ready(int sockfd)
{
     hg_msg ready_msg;

     build_msg_header(&ready_msg, HEADER_LENGTH, version, READY_MESSAGE);
     ready_msg.buf = NULL;

     if ( send_hg_msg(sockfd, &ready_msg, HEADER_LENGTH) == -1 )  {
          return(-1);
     }

     return(0);
}


int send_command(int sockfd, int command, char **answer)
{
	hg_msg comm_msg, *retmsg;
	char *comm_str, *tmp;
	int  respond = 1;
	int  length;

	if ( command == STAT_COMMAND )
		comm_str = STAT_COMMAND_STR;
	else
		comm_str = WHO_COMMAND_STR;
	length = HEADER_LENGTH + sizeof(respond) + strlen(comm_str) + 1;

	build_msg_header(&comm_msg, length, msgid++, COMMAND_MESSAGE);

	if ( (comm_msg.buf = (char *)emalloc(length-HEADER_LENGTH)) == NULL )  {
/*		perror("send_command"); */
		lowerror = LE_MALLOC;
		return(-1);
	}


	tmp = build_msg_int(comm_msg.buf, respond);
	tmp = build_msg_str(tmp, comm_str);


	if ( send_hg_msg(sockfd, &comm_msg, length) == -1 )  {
		efree(comm_msg.buf);
		return(-1);
	}
	efree(comm_msg.buf);

	/* Just check if the command was understood */
	retmsg = recv_hg_msg(sockfd);
	if ( retmsg == NULL ) {
		return(-1);
	}

	*answer = retmsg->buf;
	efree(retmsg);

	return(0);
}


static void build_msg_header(hg_msg *msg, int length, int version_msgid, int msg_type)
{
     if ( swap_on )  {
          msg->length  = swap(length);
          msg->version_msgid = swap(version_msgid);
          msg->msg_type  = swap(msg_type);
     }
     else  {
          msg->length  = length;
          msg->version_msgid = version_msgid;
          msg->msg_type  = msg_type;
     }
}


static char *build_msg_int(char *buf, int val) {
	int tmp;

#ifdef HW_DEBUG
	php_printf("   Added int to header: <b>%d</b><br />\n", val);
#endif
	tmp = swap_on ? swap(val) : val;
	memcpy(buf, (char *)&tmp, 4);

	return(buf+4);
}


static char *build_msg_str(char *buf, char *str)
{
     int len = strlen(str)+1;

#ifdef HW_DEBUG
	php_printf("   Added str to header: <b>%s</b> (%d)<br />\n", str, strlen(str));
#endif

     memcpy(buf, str, len);

     return(buf+len);
}


static int swap(int val)
{
     int tmp;

     ((char*)&tmp)[0] = ((char*)&val)[3];
     ((char*)&tmp)[1] = ((char*)&val)[2];
     ((char*)&tmp)[2] = ((char*)&val)[1];
     ((char*)&tmp)[3] = ((char*)&val)[0];

     return(tmp);
}


void close_hg_connection(int sockfd)
{
	shutdown(sockfd, 2);
	HWSOCK_FCLOSE(sockfd);
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * End:
 */

