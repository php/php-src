/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of the GNU General Public License as published by |
   | the Free Software Foundation; either version 2 of the License, or    |
   | (at your option) any later version.                                  |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of the GNU General Public License    |
   | along with this program; if not, write to the Free Software          |
   | Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.            |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef _HW_H
#define _HW_H

#if HYPERWAVE
#include "hg_comm.h"

extern php3_module_entry hw_module_entry;
#define hw_module_ptr &hw_module_entry

typedef struct {
  long default_link;
  long num_links,num_persistent;
  long max_links,max_persistent;
  long allow_persistent;
  int le_socketp, le_psocketp, le_document;
} hw_module;

extern hw_module php3_hw_module;

typedef struct {
        int size;
        char *data;
        char *attributes;
        char *bodytag;
} hw_document;

extern hw_connection php3_hw_connection;

extern int php3_minit_hw(INIT_FUNC_ARGS);
extern void php3_hw_connect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_pconnect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_close(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_root(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_info(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_error(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_errormsg(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_mv(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_cp(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_deleteobject(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_changeobject(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_getparents(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_getparentsobj(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_children(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_childrenobj(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_getchildcoll(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_getchildcollobj(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_getobject(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_getandlock(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_unlock(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_gettext(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_edittext(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_getcgi(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_getremote(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_getremotechildren(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_pipedocument(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_pipecgi(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_insertdocument(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_docbyanchorobj(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_docbyanchor(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_getobjectbyquery(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_getobjectbyqueryobj(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_getobjectbyquerycoll(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_getobjectbyquerycollobj(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_getchilddoccoll(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_getchilddoccollobj(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_getanchors(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_getanchorsobj(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_getusername(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_setlinkroot(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_inscoll(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_incollections(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_insertobject(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_insdoc(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_identify(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_free_document(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_new_document(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_output_document(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_document_size(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_document_attributes(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_document_bodytag(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_document_content(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_objrec2array(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_array2objrec(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_connection_info(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_info_hw(ZEND_MODULE_INFO_FUNC_ARGS);
extern void php3_hw_getsrcbydestobj(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_getrellink(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_dummy(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_who(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hw_stat(INTERNAL_FUNCTION_PARAMETERS);
#else
#define hw_module_ptr NULL
#endif /* HYPERWAVE */
#endif /* _HW_H */

