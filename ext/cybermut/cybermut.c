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
   | Authors: Sylvain PAGES <spages@free.fr>                              |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_cybermut.h"

#include "cm-mac.h"

/* If you declare any globals in php_cybermut.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(cybermut)
*/

/* True global resources - no need for thread safety here */
static int le_cybermut;

/* Every user visible function must have an entry in cybermut_functions[].
*/
function_entry cybermut_functions[] = {
	PHP_FE(confirm_cybermut_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(cybermut_creerformulairecm,	NULL)
	PHP_FE(cybermut_testmac,	NULL)
	PHP_FE(cybermut_creerreponsecm,	NULL)
	{NULL, NULL, NULL}	/* Must be the last line in cybermut_functions[] */
};

zend_module_entry cybermut_module_entry = {
	"cybermut",
	cybermut_functions,
	PHP_MINIT(cybermut),
	PHP_MSHUTDOWN(cybermut),
	PHP_RINIT(cybermut),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(cybermut),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(cybermut),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_CYBERMUT
ZEND_GET_MODULE(cybermut)
#endif

/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
PHP_INI_END()
*/

PHP_MINIT_FUNCTION(cybermut)
{
/* Remove comments if you have entries in php.ini
	REGISTER_INI_ENTRIES();
*/
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(cybermut)
{
/* Remove comments if you have entries in php.ini
	UNREGISTER_INI_ENTRIES();
*/
	return SUCCESS;
}

/* Remove if there's nothing to do at request start */
PHP_RINIT_FUNCTION(cybermut)
{
	return SUCCESS;
}

/* Remove if there's nothing to do at request end */
PHP_RSHUTDOWN_FUNCTION(cybermut)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(cybermut)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "cybermut support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}

/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_cybermut_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_cybermut_compiled)
{
	zval **arg;
	int len;
	char string[256];

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg);

	len = sprintf(string, "Congratulations, you have successfully modified ext/cybermut/config.m4, module %s is compiled into PHP", Z_STRVAL_PP(arg));
	RETURN_STRINGL(string, len, 1);
}
/* }}} */
/* The previous line is meant for emacs, so it can correctly fold and unfold
   functions in source code. See the corresponding marks just before function
   definition, where the functions purpose is also documented. Please follow
   this convention for the convenience of others editing your code.
*/

/* {{{ proto string cybermut_creerformulairecm(string url_CM, string version, string TPE, string montant, string ref_commande, string texte_libre, string url_retour, string url_retour_ok, string url_retour_err, string langue, string code_societe, string texte_bouton)
   Return a string containing source HTML of the form of request for payment.
   This result corresponds to the last parameter "formulaire" of the original function 
   which was removed */
PHP_FUNCTION(cybermut_creerformulairecm)
{
	zval **url_CM, **version, **TPE, **montant, **ref_commande, **texte_libre, **url_retour, **url_retour_ok, **url_retour_err, **langue, **code_societe, **texte_bouton;

	 char   formulaire [10000];

	if (ZEND_NUM_ARGS() != 12 || zend_get_parameters_ex(12, &url_CM, &version, &TPE, &montant, &ref_commande, &texte_libre, &url_retour, &url_retour_ok, &url_retour_err, &langue, &code_societe, &texte_bouton) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(url_CM);
	convert_to_string_ex(version);
	convert_to_string_ex(TPE);
	convert_to_string_ex(montant);
	convert_to_string_ex(ref_commande);
	convert_to_string_ex(texte_libre);
	convert_to_string_ex(url_retour);
	convert_to_string_ex(url_retour_ok);
	convert_to_string_ex(url_retour_err);
	convert_to_string_ex(langue);
	convert_to_string_ex(code_societe);
	convert_to_string_ex(texte_bouton);

	CreerFormulaireCM ( Z_STRVAL_PP(url_CM), Z_STRVAL_PP(version), Z_STRVAL_PP(TPE), Z_STRVAL_PP(montant), Z_STRVAL_PP(ref_commande), Z_STRVAL_PP(texte_libre), Z_STRVAL_PP(url_retour), Z_STRVAL_PP(url_retour_ok), Z_STRVAL_PP(url_retour_err), Z_STRVAL_PP(langue), Z_STRVAL_PP(code_societe), Z_STRVAL_PP(texte_bouton), formulaire);

	
	RETURN_STRING(formulaire, 1);

}
/* }}} */

/* {{{ proto bool cybermut_testmac(string code_MAC, string version, string TPE, string cdate, string montant, string ref_commande, string texte_libre, string code_retour)
   Return a boolean attesting that the authentification proceeded well 
   true : the received message is authenticated 
   false: if not */
PHP_FUNCTION(cybermut_testmac)
{
	zval **code_MAC, **version, **TPE, **cdate, **montant, **ref_commande, **texte_libre, **code_retour;
	int cdr_test;

	if (ZEND_NUM_ARGS() != 8 || zend_get_parameters_ex(8, &code_MAC, &version, &TPE, &cdate, &montant, &ref_commande, &texte_libre, &code_retour) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(code_MAC);
	convert_to_string_ex(version);
	convert_to_string_ex(TPE);
	convert_to_string_ex(cdate);
	convert_to_string_ex(montant);
	convert_to_string_ex(ref_commande);
	convert_to_string_ex(texte_libre);
	convert_to_string_ex(code_retour);

	cdr_test = TestMAC ( Z_STRVAL_PP(code_MAC), Z_STRVAL_PP(version), Z_STRVAL_PP(TPE), Z_STRVAL_PP(cdate), Z_STRVAL_PP(montant), Z_STRVAL_PP(ref_commande), Z_STRVAL_PP(texte_libre), Z_STRVAL_PP(code_retour));

	if (cdr_test == 1) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}

}
/* }}} */

/* {{{ proto string cybermut_creerreponsecm(string phrase)
   Return a string containing the message of acknowledgement of delivery 
	(headers and body of the message).
   This result corresponds to the last parameter "reponse" of the original function
   which was removed. */
PHP_FUNCTION(cybermut_creerreponsecm)
{
	zval **phrase;
	char buf[5000];

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &phrase) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(phrase);
	
	CreerReponseCM( Z_STRVAL_PP(phrase), buf);
	
	RETURN_STRING (buf,1);
}
/* }}} */



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
