/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Brendan W. McAdams <brendan@plexmedia.com>                  |
   |              Doug DeJulio <ddj@redhat.com>                           |
   +----------------------------------------------------------------------+
*/ 

	#include <cv_api.h>
	
	
	#define ccvs_module_ptr &ccvs_module_entry
	#define phpext_ccvs_ptr ccvs_module_ptr
	
	/* Declare functions not in cv_api.h but in libccvs.a. */
	char *hks_ptr_ptrtostring(void *vptr);
	void *hks_ptr_stringtoptr(char *str);
	
	/* Declare the Functions this Module Makes Available to Zend */
	
	
		/* Pre-declarations of functions */
		ZEND_FUNCTION(ccvs_init);
		ZEND_FUNCTION(ccvs_done);
		ZEND_FUNCTION(ccvs_new);
		ZEND_FUNCTION(ccvs_add);
		ZEND_FUNCTION(ccvs_delete);
		ZEND_FUNCTION(ccvs_auth);
		ZEND_FUNCTION(ccvs_return);
		ZEND_FUNCTION(ccvs_reverse);
		ZEND_FUNCTION(ccvs_sale);
		ZEND_FUNCTION(ccvs_void);
		ZEND_FUNCTION(ccvs_status);
		ZEND_FUNCTION(ccvs_count);
		ZEND_FUNCTION(ccvs_lookup);
		ZEND_FUNCTION(ccvs_report);
		ZEND_FUNCTION(ccvs_command);
		ZEND_FUNCTION(ccvs_textvalue); 
		PHP_MINFO_FUNCTION(ccvs); 
		
		/* 
		*	Create the Zend Internal hash construct to track this modules functions
		*
		*	In case anyone is wondering why we use ccvs_<action> instead of cv_<action>,
		*	it's because we are directly importing functions of the actual CCVS, which uses functions that are 
		*	cv_<action>, and we had problems implementing ZEND_NAMED_FE calls (bug in NAMED_FE? investigate
		* 	later).  We don't want our PHP calls to conflict with the C calls in the CCVS API.
		*	   
		*	BWM - 2000.07.27@16.41.EST - Added FALIAS Calls.  While I'm of the opinion that naming the 
		* 	functions in PHP ccvs_<action> is much more readable and clear to anyone reading the code than 
		*	cv_<action>, It strikes me that many people coming from php3 -> php4 will need backwards 
		*  compatibility.  It was kind of careless to simply change the function calls (There were reasons other
		*  than readability behind this; the ZEND_NAMED_FE macro was misbehaving) and not provide for 
		*  backwards compatibility - this *IS* an API and should scale with compatibility. 
		* 
		*/
		
		zend_function_entry ccvs_functions[] = {
			ZEND_FE(ccvs_init,NULL)
			ZEND_FALIAS(cv_init,ccvs_init,NULL)
			ZEND_FE(ccvs_done,NULL)
			ZEND_FALIAS(cv_done,ccvs_done,NULL)
			ZEND_FE(ccvs_new,NULL)
			ZEND_FALIAS(cv_new,ccvs_new,NULL)
			ZEND_FE(ccvs_add,NULL)
			ZEND_FALIAS(cv_add,ccvs_add,NULL)
			ZEND_FE(ccvs_delete,NULL)
			ZEND_FALIAS(cv_delete,ccvs_delete,NULL)
			ZEND_FE(ccvs_auth,NULL)
			ZEND_FALIAS(cv_auth,ccvs_auth,NULL)
			ZEND_FE(ccvs_return,NULL)
			ZEND_FALIAS(cv_return,ccvs_return,NULL)
			ZEND_FE(ccvs_reverse,NULL)
			ZEND_FALIAS(cv_reverse,ccvs_reverse,NULL)
			ZEND_FE(ccvs_sale,NULL)
			ZEND_FALIAS(cv_sale,ccvs_sale,NULL)
			ZEND_FE(ccvs_void,NULL)
			ZEND_FALIAS(cv_void,ccvs_void,NULL)
			ZEND_FE(ccvs_status,NULL)
			ZEND_FALIAS(cv_status,ccvs_status,NULL)
			ZEND_FE(ccvs_count,NULL)
			ZEND_FALIAS(cv_count,ccvs_count,NULL)
			ZEND_FE(ccvs_lookup,NULL)
			ZEND_FALIAS(cv_lookup,ccvs_lookup,NULL)
			ZEND_FE(ccvs_report,NULL)
			ZEND_FALIAS(cv_report,ccvs_report,NULL)
			ZEND_FE(ccvs_command,NULL)
			ZEND_FALIAS(cv_command,ccvs_command,NULL)
			ZEND_FE(ccvs_textvalue,NULL)
			ZEND_FALIAS(cv_textvalue,ccvs_textvalue,NULL)
			{NULL,NULL}
		};
	
	/* End function declarations */
	
	/* Zend Engine Exports - module information */
	
		/* Declare our module to the Zend engine */
		zend_module_entry ccvs_module_entry = {
			"CCVS",
			ccvs_functions,
			NULL,NULL,NULL,NULL,
			PHP_MINFO(ccvs),
	   	 	STANDARD_MODULE_PROPERTIES                                                                 
		};   
		
		/* Declare the information we need to dynamically link this module later */
		#if COMPILE_DL
		DLEXPORT zend_module_entry *get_module(void) { return &ccvs_module_entry; }
		#endif
		
	/* End exports */
	
