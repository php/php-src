/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Brendan W. McAdams <bmcadams@php.net>                       |
   |          Doug DeJulio <ddj@redhat.com>                               |
   +----------------------------------------------------------------------+
 */
/*
*	cvvs.c $Revision$ - PHP4 Interface to the RedHat CCVS API
*	 -------
*	 Interfaces RedHat's CCVS [Credit Card Verification System] <http://www.redhat.com/products/ccvs/>
*	 This code is ported from an original php3 interface written by RedHat's Doug DeJulio <ddj@redhat.com>
*	 The code was subsequently ported to the Zend API by Brendan W. McAdams <bmcadams@php.net>
*	 -------
*/

/*
*	Code started on 2000.07.24@09.04.EST by Brendan W. McAdams <bmcadams@php.net>
*	$Revision$
*/

static char const cvsid[] = "$Id$";

#include <php.h>
#include <stdlib.h>
#include <string.h>
#include <ccvs.h>


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

	function_entry ccvs_functions[] = {
		PHP_FE(ccvs_init, NULL)
		PHP_FALIAS(cv_init, ccvs_init, NULL)
		PHP_FE(ccvs_done, NULL)
		PHP_FALIAS(cv_done, ccvs_done, NULL)
		PHP_FE(ccvs_new, NULL)
		PHP_FALIAS(cv_new, ccvs_new, NULL)
		PHP_FE(ccvs_add, NULL)
		PHP_FALIAS(cv_add, ccvs_add, NULL)
		PHP_FE(ccvs_delete, NULL)
		PHP_FALIAS(cv_delete, ccvs_delete, NULL)
		PHP_FE(ccvs_auth, NULL)
		PHP_FALIAS(cv_auth, ccvs_auth, NULL)
		PHP_FE(ccvs_return, NULL)
		PHP_FALIAS(cv_return, ccvs_return, NULL)
		PHP_FE(ccvs_reverse, NULL)
		PHP_FALIAS(cv_reverse, ccvs_reverse, NULL)
		PHP_FE(ccvs_sale, NULL)
		PHP_FALIAS(cv_sale, ccvs_sale, NULL)
		PHP_FE(ccvs_void, NULL)
		PHP_FALIAS(cv_void, ccvs_void, NULL)
		PHP_FE(ccvs_status, NULL)
		PHP_FALIAS(cv_status, ccvs_status, NULL)
		PHP_FE(ccvs_count, NULL)
		PHP_FALIAS(cv_count, ccvs_count, NULL)
		PHP_FE(ccvs_lookup, NULL)
		PHP_FALIAS(cv_lookup, ccvs_lookup, NULL)
		PHP_FE(ccvs_report, NULL)
		PHP_FALIAS(cv_report, ccvs_report, NULL)
		PHP_FE(ccvs_command, NULL)
		PHP_FALIAS(cv_command, ccvs_command, NULL)
		PHP_FE(ccvs_textvalue, NULL)
		PHP_FALIAS(cv_textvalue, ccvs_textvalue, NULL)
		{NULL, NULL, NULL}
	};

/* End function declarations */

/* Zend Engine Exports - module information */

	/* Declare our module to the Zend engine */
	zend_module_entry ccvs_module_entry = {
        STANDARD_MODULE_HEADER,
		"CCVS",
		ccvs_functions,
		NULL, NULL, NULL, NULL,
		PHP_MINFO(ccvs),
        NO_VERSION_YET,
		STANDARD_MODULE_PROPERTIES
	};

#ifdef COMPILE_DL_CCVS
ZEND_GET_MODULE(ccvs)
#endif

/* Full Functions (The actual CCVS functions and any internal php hooked functions such as MINFO) */

/* {{{ proto string ccvs_init(string name)
   Initialize CCVS for use */
PHP_FUNCTION(ccvs_init) /* cv_init() */
{
  zval **name;
  void *vsess;
  char *p;

  if ((ZEND_NUM_ARGS() != 1) || (zend_get_parameters_ex(1, &name) != SUCCESS))
  /* accept only SUCCESS in case something weird gets returned instead of 'FAILURE' on fail */
    {
      WRONG_PARAM_COUNT;
    }

  convert_to_string_ex(name);

  vsess = cv_init(Z_STRVAL_PP(name));

  /*
  *		-- In the case that we don't run error checking on the return value... --
  *		On 32 bit systems a failure of the cv_init call returns 0,0,0,0 ; on 64 bit systems its 0,0,0,0,0,0,0,0
  *		This unconsistent error (not to mention a string of comma seperated zeros in and of itself) is hard to
  * 	Trap for in PHP (or any language).  However, we can also grab cv_init to return CV_SESS_BAD on
  *		failure at the C API level, and return a set, fixed error code to the user which the user then knows to
  *		trap for... e.g. a NULL Value which PHP can then trap by:
  *		if (!($string = cv_init($config)) { or some such...
  */

  if (vsess == CV_SESS_BAD) /* if the cv_init() call failed... */
  {

  		p = ""; /* set p, the value we will return, to NULL */

  }
  else /* we got a valid session returned, which means it worked */
  {

  p = hks_ptr_ptrtostring(vsess);   /* Convert the (void*) into a string representation. */

  }

  RETVAL_STRING(p, 1);

  free(p);
  return;
}
/* }}} */

/* {{{ proto string ccvs_done(string sess)
   Terminate CCVS engine and do cleanup work */
PHP_FUNCTION(ccvs_done) /* cv_done() */
{
  zval **sess;
  void *vsess;

  if ((ZEND_NUM_ARGS() != 1) || (zend_get_parameters_ex(1, &sess) != SUCCESS)) /* accept only SUCCESS in case something weird gets returned instead of 'FAILURE' on fail */
    {
      WRONG_PARAM_COUNT;
    }

  convert_to_string_ex(sess);

  /* Convert from the string representation back to a (void*) */
  vsess = hks_ptr_stringtoptr(Z_STRVAL_PP(sess));
  cv_done(vsess);

  RETURN_STRING("OK", 1);
}
/* }}} */

/* {{{ proto string ccvs_new(string session, string invoice)
   Create a new, blank transaction */
PHP_FUNCTION(ccvs_new) /* cv_new() */
{
  zval **psess;
  zval **pinvoice;
  void *sess;
  char *invoice;
  int r;

  if ((ZEND_NUM_ARGS() != 2) || (zend_get_parameters_ex(2, &psess, &pinvoice) != SUCCESS)) /* accept only SUCCESS in case something weird gets returned instead of 'FAILURE' on fail */
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(psess);
  if (!Z_STRVAL_PP(psess)) {
    php_error(E_WARNING, "%s(): Invalid session", get_active_function_name(TSRMLS_C));
    RETURN_FALSE;
  }

  sess = hks_ptr_stringtoptr(Z_STRVAL_PP(psess));

  convert_to_string_ex(pinvoice);
  invoice = Z_STRVAL_PP(pinvoice);

  r = cv_new(sess, invoice);

  RETURN_STRING(cv_ret2str(r), 1);
}
/* }}} */

/* {{{ proto string ccvs_add(string session, string invoice, string argtype, string argval)
   Add data to a transaction */
PHP_FUNCTION(ccvs_add) /* cv_add() */
{
  zval **psess;
  zval **pinvoice;
  zval **pargtype;
  zval **pargval;
  void *sess;
  char *invoice;
  int argtype;
  char *argval;
  register int r;

  if ((ZEND_NUM_ARGS() != 4) || (zend_get_parameters_ex(4, &psess, &pinvoice, &pargtype, &pargval) != SUCCESS)) /* accept only SUCCESS in case something weird gets returned instead of 'FAILURE' on fail */
  {
    WRONG_PARAM_COUNT;
  }

  /* Get meaningful arguments. */
  convert_to_string_ex(psess);
  convert_to_string_ex(pinvoice);
  convert_to_string_ex(pargtype);
  convert_to_string_ex(pargval);
  sess = hks_ptr_stringtoptr(Z_STRVAL_PP(psess));
  invoice = Z_STRVAL_PP(pinvoice);
  argtype = cv_str2arg(Z_STRVAL_PP(pargtype));
  argval = Z_STRVAL_PP(pargval);

  r = cv_add(sess, invoice, argtype, argval);

  RETURN_STRING(cv_ret2str(r), 1);
}
/* }}} */

/*
*	cv_create can't be implemented because of vararg limits in PHP3's C API.
*	(COMMENT BY DDJ [from original code])
*
*	BWM: I looked into this, checking in on what cv_create was; it is a deprecated function left in for
*	backwards compatibility according
*  to the CCVS C API ref.  I didn't try to implement it for that reason.  If anyone needs it, they can add it in
*	themselves I'm sure.
*/

/* {{{ proto string ccvs_delete(string session, string invoice)
   Delete a transaction */
PHP_FUNCTION(ccvs_delete) /* cv_delete() */
{
  zval **psess;
  zval **pinvoice;
  void *sess;
  char *invoice;
  register int r;

  if ((ZEND_NUM_ARGS() != 2) || (zend_get_parameters_ex(2, &psess, &pinvoice) != SUCCESS)) /* accept only SUCCESS in case something weird gets returned instead of 'FAILURE' on fail */
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(psess);
  convert_to_string_ex(pinvoice);
  invoice = Z_STRVAL_PP(pinvoice);
  sess = hks_ptr_stringtoptr(Z_STRVAL_PP(psess));

  r = cv_delete(sess, invoice);

  RETURN_STRING(cv_ret2str(r), 1);
}
/* }}} */

/* {{{ proto string ccvs_auth(string session, string invoice)
   Perform credit authorization test on a transaction */
PHP_FUNCTION(ccvs_auth) /* cv_auth() */
{
  zval **psess;
  zval **pinvoice;
  void *sess;
  char *invoice;
  register int r;

  if ((ZEND_NUM_ARGS() != 2) || (zend_get_parameters_ex(2, &psess, &pinvoice) != SUCCESS))  /* accept only SUCCESS in case something weird gets returned instead of 'FAILURE' on fail */
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(psess);
  convert_to_string_ex(pinvoice);
  invoice = Z_STRVAL_PP(pinvoice);
  sess = hks_ptr_stringtoptr(Z_STRVAL_PP(psess));

  r = cv_auth(sess, invoice);

  RETURN_STRING(cv_ret2str(r), 1);
}
/* }}} */

/* {{{ proto string ccvs_return(string session, string invoice)
   Transfer funds from the merchant to the credit card holder */
PHP_FUNCTION(ccvs_return) /* cv_return() */
{
  zval **psess;
  zval **pinvoice;
  void *sess;
  char *invoice;
  register int r;

  if ((ZEND_NUM_ARGS() != 2) || (zend_get_parameters_ex(2, &psess, &pinvoice) != SUCCESS))  /* accept only SUCCESS in case something weird gets returned instead of 'FAILURE' on fail */
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(psess);
  convert_to_string_ex(pinvoice);
  invoice = Z_STRVAL_PP(pinvoice);
  sess = hks_ptr_stringtoptr(Z_STRVAL_PP(psess));

  r = cv_return(sess, invoice);

  RETURN_STRING(cv_ret2str(r), 1);
}
/* }}} */

/* {{{ proto string ccvs_reverse(string session, string invoice)
   Perform a full reversal on an already-processed authorization */
PHP_FUNCTION(ccvs_reverse) /* cv_reverse() */
{
  zval **psess;
  zval **pinvoice;
  void *sess;
  char *invoice;
  register int r;

  if ((ZEND_NUM_ARGS() != 2) || (zend_get_parameters_ex(2, &psess, &pinvoice)  != SUCCESS))  /* accept only SUCCESS in case something weird gets returned instead of 'FAILURE' on fail */
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(psess);
  convert_to_string_ex(pinvoice);
  invoice = Z_STRVAL_PP(pinvoice);
  sess = hks_ptr_stringtoptr(Z_STRVAL_PP(psess));

  r = cv_reverse(sess, invoice);

  RETURN_STRING(cv_ret2str(r), 1);
}
/* }}} */

/* {{{ proto string ccvs_sale(string session, string invoice)
   Transfer funds from the credit card holder to the merchant */
PHP_FUNCTION(ccvs_sale) /* cv_sale() */
{
  zval **psess;
  zval **pinvoice;
  void *sess;
  char *invoice;
  register int r;

  if ((ZEND_NUM_ARGS() != 2) || (zend_get_parameters_ex(2, &psess, &pinvoice) != SUCCESS))  /* accept only SUCCESS in case something weird gets returned instead of 'FAILURE' on fail */
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(psess);
  convert_to_string_ex(pinvoice);
  invoice = Z_STRVAL_PP(pinvoice);
  sess = hks_ptr_stringtoptr(Z_STRVAL_PP(psess));

  r = cv_sale(sess, invoice);

  RETURN_STRING(cv_ret2str(r), 1);
}
/* }}} */

/* {{{ proto string ccvs_void(string session, string invoice)
   Perform a full reversal on a completed transaction */
PHP_FUNCTION(ccvs_void) /* cv_void() */
{
  zval **psess;
  zval **pinvoice;
  void *sess;
  char *invoice;
  register int r;

  if ((ZEND_NUM_ARGS() != 2) || (zend_get_parameters_ex(2, &psess, &pinvoice) != SUCCESS))  /* accept only SUCCESS in case something weird gets returned instead of 'FAILURE' on fail */
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(psess);
  convert_to_string_ex(pinvoice);
  invoice = Z_STRVAL_PP(pinvoice);
  sess = hks_ptr_stringtoptr(Z_STRVAL_PP(psess));

  r = cv_void(sess, invoice);

  RETURN_STRING(cv_ret2str(r), 1);
}
/* }}} */

/* {{{ proto string ccvs_status(string session, string invoice)
   Check the status of an invoice */
PHP_FUNCTION(ccvs_status) /* cv_status() */
{
  zval **psess;
  zval **pinvoice;
  void *sess;
  char *invoice;
  register int r;

  if ((ZEND_NUM_ARGS() != 2) || (zend_get_parameters_ex(2, &psess, &pinvoice) != SUCCESS))  /* accept only SUCCESS in case something weird gets returned instead of 'FAILURE' on fail */
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(psess);
  convert_to_string_ex(pinvoice);
  invoice = Z_STRVAL_PP(pinvoice);
  sess = hks_ptr_stringtoptr(Z_STRVAL_PP(psess));

  r = cv_status(sess, invoice);

  RETURN_STRING(cv_stat2str(r), 1);
}
/* }}} */

/* {{{ proto int ccvs_count(string session, string type)
   Find out how many transactions of a given type are stored in the system */
PHP_FUNCTION(ccvs_count) /* cv_count() */
{
  zval **psess;
  zval **ptype;
  void *sess;
  int type;
  register int r;

  if ((ZEND_NUM_ARGS() != 2) || (zend_get_parameters_ex(2, &psess, &ptype) != SUCCESS))  /* accept only SUCCESS in case something weird gets returned instead of 'FAILURE' on fail */
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(psess);
  convert_to_string_ex(ptype);
  type = cv_str2stat(Z_STRVAL_PP(ptype));
  sess = hks_ptr_stringtoptr(Z_STRVAL_PP(psess));

  r = cv_count(sess, type);

  RETURN_LONG(r);
}
/* }}} */

/* {{{ proto string ccvs_lookup(string session, string invoice, int inum)
   Look up an item of a particular type in the database */
PHP_FUNCTION(ccvs_lookup) /* cv_lookup() */
{
  zval **psess;
  zval **ptype;
  zval **pinum;
  void *sess;
  int type;
  long inum;
  register int r;

  if ((ZEND_NUM_ARGS() != 3) || (zend_get_parameters_ex(3, &psess, &ptype, &pinum) != SUCCESS))  /* accept only SUCCESS in case something weird gets returned instead of 'FAILURE' on fail */
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(psess);
  sess = hks_ptr_stringtoptr(Z_STRVAL_PP(psess));
  convert_to_string_ex(ptype);
  type = cv_str2stat(Z_STRVAL_PP(ptype));
  convert_to_long_ex(pinum);
  inum = Z_LVAL_PP(pinum);

  r = cv_lookup(sess, type, inum);

  RETURN_STRING(cv_textvalue(sess), 1);
}
/* }}} */

/* {{{ proto string ccvs_report(string session, string type)
   Return the status of the background communication process */
PHP_FUNCTION(ccvs_report) /* cv_report() */
{
  zval **psess;
  zval **ptype;
  void *sess;
  int type;
  long inum;
  register int r;

  if ((ZEND_NUM_ARGS() != 2) || (zend_get_parameters_ex(2, &psess, &ptype) != SUCCESS))  /* accept only SUCCESS in case something weird gets returned instead of 'FAILURE' on fail */
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(psess);
  sess = hks_ptr_stringtoptr(Z_STRVAL_PP(psess));
  convert_to_string_ex(ptype);
  type = cv_str2rep(Z_STRVAL_PP(ptype));

  r = cv_report(sess, type);

  RETURN_STRING(cv_stat2str(r), 1);
}
/* }}} */

/* {{{ proto string ccvs_command(string session, string type, string argval)
   Performs a command which is peculiar to a single protocol, and thus is not available in the general CCVS API */
PHP_FUNCTION(ccvs_command) /* cv_command() */
{
  zval **psess;
  zval **ptype;
  zval **pargval;
  void *sess;
  int type;
  register int r;
  char *argval;

  if ((ZEND_NUM_ARGS() != 3) || (zend_get_parameters_ex(3, &psess, &ptype, &pargval) != SUCCESS))  /* accept only SUCCESS in case something weird gets returned instead of 'FAILURE' on fail */
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(psess);
  sess = hks_ptr_stringtoptr(Z_STRVAL_PP(psess));
  convert_to_string_ex(ptype);
  type = cv_str2cmd(Z_STRVAL_PP(ptype));
  convert_to_string_ex(pargval);
  argval = Z_STRVAL_PP(pargval);

  r = cv_command(sess, type, argval);

  RETURN_STRING(cv_stat2str(r), 1);
}
/* }}} */

/* {{{ proto string ccvs_textvalue(string session)
   Get text return value for previous function call */
PHP_FUNCTION(ccvs_textvalue) /* cv_textvalue() */
{
  zval **psess;
  void *sess;

  if ((ZEND_NUM_ARGS() != 1) || (zend_get_parameters_ex(1, &psess) != SUCCESS))  /* accept only SUCCESS in case something weird gets returned instead of 'FAILURE' on fail */
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(psess);
  sess = hks_ptr_stringtoptr(Z_STRVAL_PP(psess));

  RETURN_STRING(cv_textvalue(sess), 1);
}
/* }}} */

/*
*	Our Info Function which reports info on this module out to PHP's phpinfo() function
*	Brendan W. McAdams <bmcadams@php.net> on 2000.07.26@16:22.EST
*/

PHP_MINFO_FUNCTION(ccvs)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "RedHat CCVS support", "enabled");
    php_info_print_table_row(2, "CCVS Support by", "Brendan W. McAdams &lt;bmcadams@php.net&gt;<br />&amp; Doug DeJulio &lt;ddj@redhat.com&gt;");
    php_info_print_table_row(2, "Release ID", cvsid);
    php_info_print_table_row(2, "This Release Certified For CCVS Versions", "3.0 and greater");
    php_info_print_table_end();

    /*  DISPLAY_INI_ENTRIES(); */

    /*
    *	In the future, we will probably have entries in php.ini for runtime config, in which case we will
    *  Uncomment the DISPLAY_INI_ENTRIES call...
    */

}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
