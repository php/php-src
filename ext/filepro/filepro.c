/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Chad Robinson <chadr@brttech.com>                           |
   +----------------------------------------------------------------------+

  filePro 4.x support developed by Chad Robinson, chadr@brttech.com
  Contact Chad Robinson at BRT Technical Services Corp. for details.
  filePro is a registered trademark by Fiserv, Inc.  This file contains
  no code or information that is not freely available from the filePro
  web site at http://www.fileproplus.com/

 */
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "php.h"
#include "safe_mode.h"
#include "fopen-wrappers.h"
#include <string.h>
#if MSVC5
#include <windows.h>
#else
#include <sys/param.h>
#endif
#include <errno.h>
#include "php_globals.h"

#include "filepro.h"
#if HAVE_FILEPRO

typedef struct fp_field {
	char *name;
	char *format;
	int width;
	struct fp_field *next;
} FP_FIELD;

#ifdef THREAD_SAFE
DWORD FPTls;
static int numthreads=0;

typedef struct fp_global_struct{
	char *fp_database;
	signed int fp_fcount;
	signed int fp_keysize;
	FP_FIELD *fp_fieldlist;
}fp_global_struct;

#define FP_GLOBAL(a) fp_globals->a

#define FP_TLS_VARS \
	fp_global_struct *fp_globals; \
	fp_globals=TlsGetValue(FPTls); 

#else
#define FP_GLOBAL(a) a
#define FP_TLS_VARS
static char *fp_database = NULL;			/* Database directory */
static signed int fp_fcount = -1;			/* Column count */
static signed int fp_keysize = -1;			/* Size of key records */
static FP_FIELD *fp_fieldlist = NULL;		/* List of fields */
#endif


int php3_minit_filepro(INIT_FUNC_ARGS)
{
#ifdef THREAD_SAFE
	fp_global_struct *fp_globals;
#if !COMPILE_DL
	CREATE_MUTEX(fp_mutex,"FP_TLS");
	SET_MUTEX(fp_mutex);
	numthreads++;
	if (numthreads==1){
	if ((FPTls=TlsAlloc())==0xFFFFFFFF){
		FREE_MUTEX(fp_mutex);
		return 0;
	}}
	FREE_MUTEX(fp_mutex);
#endif
	fp_globals = (fp_global_struct *) LocalAlloc(LPTR, sizeof(fp_global_struct)); 
	TlsSetValue(FPTls, (void *) fp_globals);
#endif
	FP_GLOBAL(fp_database)=NULL;
	FP_GLOBAL(fp_fcount)=-1;
	FP_GLOBAL(fp_keysize)=-1;
	FP_GLOBAL(fp_fieldlist)=NULL;

	return SUCCESS;
}

int php3_mend_filepro(void){
#ifdef THREAD_SAFE
	fp_global_struct *fp_globals;
	fp_globals = TlsGetValue(FPTls); 
	if (fp_globals != 0) 
		LocalFree((HLOCAL) fp_globals); 
#if !COMPILE_DL
	SET_MUTEX(fp_mutex);
	numthreads--;
	if (!numthreads){
	if (!TlsFree(FPTls)){
		FREE_MUTEX(fp_mutex);
		return 0;
	}}
	FREE_MUTEX(fp_mutex);
#endif
#endif
	return SUCCESS;
}


function_entry filepro_functions[] = {
	{"filepro",				php3_filepro,				NULL},
	{"filepro_rowcount",	php3_filepro_rowcount,		NULL},
	{"filepro_fieldname",	php3_filepro_fieldname,		NULL},
	{"filepro_fieldtype",	php3_filepro_fieldtype,		NULL},
	{"filepro_fieldwidth",	php3_filepro_fieldwidth,	NULL},
	{"filepro_fieldcount",	php3_filepro_fieldcount,	NULL},
	{"filepro_retrieve",	php3_filepro_retrieve,		NULL},
	{NULL, NULL, NULL}
};

php3_module_entry filepro_module_entry = {
	"FilePro", filepro_functions, php3_minit_filepro, php3_mend_filepro, NULL, NULL, NULL, STANDARD_MODULE_PROPERTIES
};


#if COMPILE_DL
#include "dl/phpdl.h"
DLEXPORT php3_module_entry *get_module(void) { return &filepro_module_entry; }
#if (WIN32|WINNT) && defined(THREAD_SAFE)

/*NOTE: You should have an odbc.def file where you
export DllMain*/
BOOL WINAPI DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
    switch( ul_reason_for_call ) {
    case DLL_PROCESS_ATTACH:
		if ((FPTls=TlsAlloc())==0xFFFFFFFF){
			return 0;
		}
		break;    
    case DLL_THREAD_ATTACH:
		break;
    case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		if (!TlsFree(FPTls)){
			return 0;
		}
		break;
    }
    return 1;
}
#endif
#endif

/*
 * LONG filePro(STRING directory)
 * 
 * Read and verify the map file.  We store the field count and field info
 * internally, which means we become unstable if you modify the table while
 * a user is using it!  We cannot lock anything since Web connections don't
 * provide the ability to later unlock what we locked.  Be smart, be safe.
 */
void php3_filepro(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *dir;
	FILE *fp;
	char workbuf[256]; /* FIX - should really be the max filename length */
	char readbuf[256];
	int i;
	FP_FIELD *new_field, *tmp;
	FP_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &dir) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(dir);

	/* FIX - we should really check and free these if they are used! */
	FP_GLOBAL(fp_database) = NULL;
    FP_GLOBAL(fp_fieldlist) = NULL;
	FP_GLOBAL(fp_fcount) = -1;
    FP_GLOBAL(fp_keysize) = -1;
	
	sprintf(workbuf, "%s/map", dir->value.str.val);

	if (PG(safe_mode) && (!_php3_checkuid(workbuf, 2))) {
		RETURN_FALSE;
	}
	
	if (_php3_check_open_basedir(workbuf)) {
		RETURN_FALSE;
	}

	if (!(fp = fopen(workbuf, "r"))) {
		php3_error(E_WARNING, "filePro: cannot open map: [%d] %s",
					errno, strerror(errno));
		RETURN_FALSE;
	}
	if (!fgets(readbuf, 250, fp)) {
		fclose(fp);
		php3_error(E_WARNING, "filePro: cannot read map: [%d] %s",
					errno, strerror(errno));
		RETURN_FALSE;
	}
	
	/* Get the field count, assume the file is readable! */
	if (strcmp(strtok(readbuf, ":"), "map")) {
		php3_error(E_WARNING, "filePro: map file corrupt or encrypted");
		RETURN_FALSE;
	}
	FP_GLOBAL(fp_keysize) = atoi(strtok(NULL, ":"));
	strtok(NULL, ":");
	FP_GLOBAL(fp_fcount) = atoi(strtok(NULL, ":"));
    
    /* Read in the fields themselves */
	for (i = 0; i < FP_GLOBAL(fp_fcount); i++) {
		if (!fgets(readbuf, 250, fp)) {
			fclose(fp);
			php3_error(E_WARNING, "filePro: cannot read map: [%d] %s",
						errno, strerror(errno));
			RETURN_FALSE;
		}
		new_field = emalloc(sizeof(FP_FIELD));
		new_field->next = NULL;
		new_field->name = estrdup(strtok(readbuf, ":"));
		new_field->width = atoi(strtok(NULL, ":"));
		new_field->format = estrdup(strtok(NULL, ":"));
        
		/* Store in forward-order to save time later */
		if (!FP_GLOBAL(fp_fieldlist)) {
			FP_GLOBAL(fp_fieldlist) = new_field;
		} else {
			for (tmp = FP_GLOBAL(fp_fieldlist); tmp; tmp = tmp->next) {
				if (!tmp->next) {
					tmp->next = new_field;
					tmp = new_field;
				}
			}
		}
	}
	fclose(fp);
		
	FP_GLOBAL(fp_database) = estrndup(dir->value.str.val,dir->value.str.len);

	RETVAL_TRUE;
}


/*
 * LONG filePro_rowcount(void)
 * 
 * Count the used rows in the database.  filePro just marks deleted records
 * as deleted; they are not removed.  Since no counts are maintained we need
 * to go in and count records ourselves.  <sigh>
 * 
 * Errors return false, success returns the row count.
 */
void php3_filepro_rowcount(INTERNAL_FUNCTION_PARAMETERS)
{
	FILE *fp;
	char workbuf[MAXPATHLEN];
	char readbuf[256];
	int recsize = 0, records = 0;
	FP_TLS_VARS;

	if (ARG_COUNT(ht) != 0) {
		WRONG_PARAM_COUNT;
	}

	if (!FP_GLOBAL(fp_database)) {
		php3_error(E_WARNING,
					"filePro: must set database directory first!\n");
		RETURN_FALSE;
	}
	
	recsize = FP_GLOBAL(fp_keysize) + 19; /* 20 bytes system info -1 to save time later */
	
	/* Now read the records in, moving forward recsize-1 bytes each time */
	sprintf(workbuf, "%s/key", FP_GLOBAL(fp_database));

	if (PG(safe_mode) && (!_php3_checkuid(workbuf, 2))) {
		RETURN_FALSE;
	}
	
	if (_php3_check_open_basedir(workbuf)) {
		RETURN_FALSE;
	}

	if (!(fp = fopen(workbuf, "r"))) {
		php3_error(E_WARNING, "filePro: cannot open key: [%d] %s",
					errno, strerror(errno));
		RETURN_FALSE;
	}
	while (!feof(fp)) {
		if (fread(readbuf, 1, 1, fp) == 1) {
			if (readbuf[0])
				records++;
			fseek(fp, recsize, SEEK_CUR);
		}
	}
    fclose(fp);
	
	RETVAL_LONG(records);
}


/*
 * STRING filePro_fieldname(LONG field_number)
 * 
 * Errors return false, success returns the name of the field.
 */
void php3_filepro_fieldname(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *fno;
	FP_FIELD *lp;
	int i;
	FP_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &fno) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(fno);

	if (!FP_GLOBAL(fp_database)) {
		php3_error(E_WARNING,
					"filePro: must set database directory first!\n");
		RETURN_FALSE;
	}
	
	for (i = 0, lp = FP_GLOBAL(fp_fieldlist); lp; lp = lp->next, i++) {
		if (i == fno->value.lval) {
			RETURN_STRING(lp->name,1);
		}
	}

	php3_error(E_WARNING,
				"filePro: unable to locate field number %d.\n",
				fno->value.lval);

	RETVAL_FALSE;
}


/*
 * STRING filePro_fieldtype(LONG field_number)
 * 
 * Errors return false, success returns the type (edit) of the field
 */
void php3_filepro_fieldtype(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *fno;
	FP_FIELD *lp;
	int i;
	FP_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &fno) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(fno);

	if (!FP_GLOBAL(fp_database)) {
		php3_error(E_WARNING,
					"filePro: must set database directory first!\n");
		RETURN_FALSE;
	}
	
	for (i = 0, lp = FP_GLOBAL(fp_fieldlist); lp; lp = lp->next, i++) {
		if (i == fno->value.lval) {
			RETURN_STRING(lp->format,1);
		}
	}
	php3_error(E_WARNING,
				"filePro: unable to locate field number %d.\n",
				fno->value.lval);
	RETVAL_FALSE;
}


/*
 * STRING filePro_fieldwidth(int field_number)
 * 
 * Errors return false, success returns the character width of the field.
 */
void php3_filepro_fieldwidth(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *fno;
	FP_FIELD *lp;
	int i;
	FP_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &fno) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(fno);

	if (!FP_GLOBAL(fp_database)) {
		php3_error(E_WARNING,
					"filePro: must set database directory first!\n");
		RETURN_FALSE;
	}
	
	for (i = 0, lp = FP_GLOBAL(fp_fieldlist); lp; lp = lp->next, i++) {
		if (i == fno->value.lval) {
			RETURN_LONG(lp->width);
		}
	}
	php3_error(E_WARNING,
				"filePro: unable to locate field number %d.\n",
				fno->value.lval);
	RETVAL_FALSE;
}


/*
 * LONG filePro_fieldcount(void)
 * 
 * Errors return false, success returns the field count.
 */
void php3_filepro_fieldcount(INTERNAL_FUNCTION_PARAMETERS)
{
	FP_TLS_VARS;

	if (ARG_COUNT(ht) != 0) {
		WRONG_PARAM_COUNT;
	}

	if (!FP_GLOBAL(fp_database)) {
		php3_error(E_WARNING,
					"filePro: must set database directory first!\n");
		RETURN_FALSE;
	}
	
	/* Read in the first line from the map file */
	RETVAL_LONG(FP_GLOBAL(fp_fcount));
}


/*
 * STRING filePro_retrieve(int row_number, int field_number)
 * 
 * Errors return false, success returns the datum.
 */
void php3_filepro_retrieve(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *rno, *fno;
    FP_FIELD *lp;
    FILE *fp;
    char workbuf[MAXPATHLEN];
	char readbuf[1024]; /* FIX - Work out better buffering! */
    int i, fnum, rnum;
    long offset;
	FP_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &rno, &fno) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (!FP_GLOBAL(fp_database)) {
		php3_error(E_WARNING,
					"filePro: must set database directory first!\n");
		RETURN_FALSE;
	}
	
	convert_to_long(rno);
	convert_to_long(fno);

	fnum = fno->value.lval;
	rnum = rno->value.lval;
    
    if (rnum < 0 || fnum < 0 || fnum >= FP_GLOBAL(fp_fcount)) {
        php3_error(E_WARNING, "filepro: parameters out of range");
		RETURN_FALSE;
    }
    
    offset = (rnum + 1) * (FP_GLOBAL(fp_keysize) + 20) + 20; /* Record location */
    for (i = 0, lp = FP_GLOBAL(fp_fieldlist); lp && i < fnum; lp = lp->next, i++) {
        offset += lp->width;
    }
    if (!lp) {
        php3_error(E_WARNING, "filePro: cannot locate field");
		RETURN_FALSE;
    }
    
	/* Now read the record in */
	sprintf(workbuf, "%s/key", FP_GLOBAL(fp_database));

	if (PG(safe_mode) && (!_php3_checkuid(workbuf, 2))) {
		RETURN_FALSE;
	}
	
	if (_php3_check_open_basedir(workbuf)) {
		RETURN_FALSE;
	}

	if (!(fp = fopen(workbuf, "r"))) {
		php3_error(E_WARNING, "filePro: cannot open key: [%d] %s",
					errno, strerror(errno));
	    fclose(fp);
		RETURN_FALSE;
	}
    fseek(fp, offset, SEEK_SET);
	if (fread(readbuf, lp->width, 1, fp) != 1) {
        php3_error(E_WARNING, "filePro: cannot read data: [%d] %s",
					errno, strerror(errno));
	    fclose(fp);
		RETURN_FALSE;
    }
    readbuf[lp->width] = '\0';
    fclose(fp);
	RETURN_STRING(readbuf,1);
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
