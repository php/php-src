/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Kirti Velankar <kirtig@yahoo-inc.com>                       |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unicode/ustring.h>
#include <unicode/udata.h>
#include <unicode/putil.h>
#include <unicode/ures.h>

#include "php_intl.h"
#include "locale.h"
#include "locale_class.h"
#include "locale_methods.h"
#include "intl_convert.h"
#include "intl_data.h"

#include <zend_API.h>
#include <zend.h>
#include <php.h>
#include "main/php_ini.h"
#include "zend_smart_str.h"

ZEND_EXTERN_MODULE_GLOBALS( intl )

/* Sizes required for the strings "variant15" , "extlang11", "private12" etc. */
#define SEPARATOR "_"
#define SEPARATOR1 "-"
#define DELIMITER "-_"
#define EXTLANG_PREFIX "a"
#define PRIVATE_PREFIX "x"
#define DISP_NAME "name"

#define MAX_NO_VARIANT  15
#define MAX_NO_EXTLANG  3
#define MAX_NO_PRIVATE  15
#define MAX_NO_LOOKUP_LANG_TAG  100

#define LOC_NOT_FOUND 1

/* Sizes required for the strings "variant15" , "extlang3", "private12" etc. */
#define VARIANT_KEYNAME_LEN  11
#define EXTLANG_KEYNAME_LEN  10
#define PRIVATE_KEYNAME_LEN  11

/* Based on IANA registry at the time of writing this code
*
*/
static const char * const LOC_GRANDFATHERED[] = {
	"art-lojban",		"i-klingon",		"i-lux",			"i-navajo",		"no-bok",		"no-nyn",
	"cel-gaulish",		"en-GB-oed",		"i-ami",
	"i-bnn",		"i-default",		"i-enochian",
	"i-mingo",		"i-pwn", 		"i-tao",
	"i-tay",		"i-tsu",		"sgn-BE-fr",
	"sgn-BE-nl",		"sgn-CH-de", 		"zh-cmn",
 	"zh-cmn-Hans", 		"zh-cmn-Hant",		"zh-gan" ,
	"zh-guoyu", 		"zh-hakka", 		"zh-min",
	"zh-min-nan", 		"zh-wuu", 		"zh-xiang",
	"zh-yue",		NULL
};

/* Based on IANA registry at the time of writing this code
*  This array lists the preferred values for the grandfathered tags if applicable
*  This is in sync with the array LOC_GRANDFATHERED
*  e.g. the offsets of the grandfathered tags match the offset of the preferred  value
*/
static const int 		LOC_PREFERRED_GRANDFATHERED_LEN = 6;
static const char * const 	LOC_PREFERRED_GRANDFATHERED[]  = {
	"jbo",			"tlh",			"lb",
	"nv", 			"nb",			"nn",
	NULL
};

/*returns TRUE if a is an ID separator FALSE otherwise*/
#define isIDSeparator(a) (a == '_' || a == '-')
#define isKeywordSeparator(a) (a == '@' )
#define isEndOfTag(a) (a == '\0' )

#define isPrefixLetter(a) ((a=='x')||(a=='X')||(a=='i')||(a=='I'))

/*returns TRUE if one of the special prefixes is here (s=string)
  'x-' or 'i-' */
#define isIDPrefix(s) (isPrefixLetter(s[0])&&isIDSeparator(s[1]))
#define isKeywordPrefix(s) ( isKeywordSeparator(s[0]) )

/* Dot terminates it because of POSIX form  where dot precedes the codepage
 * except for variant */
#define isTerminator(a)  ((a==0)||(a=='.')||(a=='@'))

/* {{{ return the offset of 'key' in the array 'list'.
 * returns -1 if not present */
static int16_t findOffset(const char* const* list, const char* key)
{
	const char* const* anchor = list;
	while (*list != NULL) {
		if (strcmp(key, *list) == 0) {
			return (int16_t)(list - anchor);
		}
		list++;
	}

	return -1;

}
/*}}}*/

static char* getPreferredTag(const char* gf_tag)
{
	char* result = NULL;
	int grOffset = 0;

	grOffset = findOffset( LOC_GRANDFATHERED ,gf_tag);
	if(grOffset < 0) {
		return NULL;
	}
	if( grOffset < LOC_PREFERRED_GRANDFATHERED_LEN ){
		/* return preferred tag */
		result = estrdup( LOC_PREFERRED_GRANDFATHERED[grOffset] );
	} else {
		/* Return correct grandfathered language tag */
		result = estrdup( LOC_GRANDFATHERED[grOffset] );
	}
	return result;
}

/* {{{
* returns the position of next token for lookup
* or -1 if no token
* strtokr equivalent search for token in reverse direction
*/
static int getStrrtokenPos(char* str, int savedPos)
{
	int result =-1;
	int i;

	for(i=savedPos-1; i>=0; i--) {
		if(isIDSeparator(*(str+i)) ){
			/* delimiter found; check for singleton */
			if(i>=2 && isIDSeparator(*(str+i-2)) ){
				/* a singleton; so send the position of token before the singleton */
				result = i-2;
			} else {
				result = i;
			}
			break;
		}
	}
	if(result < 1){
		/* Just in case inavlid locale e.g. '-x-xyz' or '-sl_Latn' */
		result =-1;
	}
	return result;
}
/* }}} */

/* {{{
* returns the position of a singleton if present
* returns -1 if no singleton
* strtok equivalent search for singleton
*/
static int getSingletonPos(const char* str)
{
	int result =-1;
	int i=0;
	int len = 0;

	if( str && ((len=strlen(str))>0) ){
		for( i=0; i<len ; i++){
			if( isIDSeparator(*(str+i)) ){
				if( i==1){
					/* string is of the form x-avy or a-prv1 */
					result =0;
					break;
				} else {
					/* delimiter found; check for singleton */
					if( isIDSeparator(*(str+i+2)) ){
						/* a singleton; so send the position of separator before singleton */
						result = i+1;
						break;
					}
				}
			}
		}/* end of for */

	}
	return result;
}
/* }}} */

/* {{{ proto static string Locale::getDefault(  )
   Get default locale */
/* }}} */
/* {{{ proto static string locale_get_default( )
   Get default locale */
PHP_NAMED_FUNCTION(zif_locale_get_default)
{
	RETURN_STRING( intl_locale_get_default(  ) );
}

/* }}} */

/* {{{ proto static string Locale::setDefault( string $locale )
   Set default locale */
/* }}} */
/* {{{ proto static string locale_set_default( string $locale )
   Set default locale */
PHP_NAMED_FUNCTION(zif_locale_set_default)
{
	zend_string* locale_name;
	zend_string *ini_name;
	char *default_locale = NULL;

	if(zend_parse_parameters( ZEND_NUM_ARGS(),  "S", &locale_name) == FAILURE)
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 	"locale_set_default: unable to parse input params", 0 );

		RETURN_FALSE;
	}

	if (ZSTR_LEN(locale_name) == 0) {
		default_locale = (char *)uloc_getDefault();
		locale_name = zend_string_init(default_locale, strlen(default_locale), 0);
	}

	ini_name = zend_string_init(LOCALE_INI_NAME, sizeof(LOCALE_INI_NAME) - 1, 0);
	zend_alter_ini_entry(ini_name, locale_name, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	zend_string_release(ini_name);
	if (default_locale != NULL) {
		zend_string_release(locale_name);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{
* Gets the value from ICU
* common code shared by get_primary_language,get_script or get_region or get_variant
* result = 0 if error, 1 if successful , -1 if no value
*/
static zend_string* get_icu_value_internal( const char* loc_name , char* tag_name, int* result , int fromParseLocale)
{
	zend_string*	tag_value	= NULL;
	int32_t     	tag_value_len   = 512;

	int		singletonPos   	= 0;
	char*       	mod_loc_name	= NULL;
	int 		grOffset	= 0;

	int32_t     	buflen          = 512;
	UErrorCode  	status          = U_ZERO_ERROR;


	if( strcmp(tag_name, LOC_CANONICALIZE_TAG) != 0 ){
		/* Handle  grandfathered languages */
		grOffset =  findOffset( LOC_GRANDFATHERED , loc_name );
		if( grOffset >= 0 ){
			if( strcmp(tag_name , LOC_LANG_TAG)==0 ){
				return zend_string_init(loc_name, strlen(loc_name), 0);
			} else {
				/* Since Grandfathered , no value , do nothing , retutn NULL */
				return NULL;
			}
		}

	if( fromParseLocale==1 ){
		/* Handle singletons */
		if( strcmp(tag_name , LOC_LANG_TAG)==0 ){
			if( strlen(loc_name)>1 && (isIDPrefix(loc_name) == 1) ){
				return zend_string_init(loc_name, strlen(loc_name), 0);
			}
		}

		singletonPos = getSingletonPos( loc_name );
		if( singletonPos == 0){
			/* singleton at start of script, region , variant etc.
			 * or invalid singleton at start of language */
			return NULL;
		} else if( singletonPos > 0 ){
			/* singleton at some position except at start
			 * strip off the singleton and rest of the loc_name */
			mod_loc_name = estrndup ( loc_name , singletonPos-1);
		}
	} /* end of if fromParse */

	} /* end of if != LOC_CANONICAL_TAG */

	if( mod_loc_name == NULL){
		mod_loc_name = estrdup(loc_name );
	}

	/* Proceed to ICU */
	do{
		if (tag_value) {
			tag_value = zend_string_realloc( tag_value , buflen, 0);
		} else {
			tag_value = zend_string_alloc( buflen, 0);
		}
		tag_value_len = buflen;

		if( strcmp(tag_name , LOC_SCRIPT_TAG)==0 ){
			buflen = uloc_getScript ( mod_loc_name , tag_value->val , tag_value_len , &status);
		}
		if( strcmp(tag_name , LOC_LANG_TAG )==0 ){
			buflen = uloc_getLanguage ( mod_loc_name , tag_value->val , tag_value_len , &status);
		}
		if( strcmp(tag_name , LOC_REGION_TAG)==0 ){
			buflen = uloc_getCountry ( mod_loc_name , tag_value->val , tag_value_len , &status);
		}
		if( strcmp(tag_name , LOC_VARIANT_TAG)==0 ){
			buflen = uloc_getVariant ( mod_loc_name , tag_value->val , tag_value_len , &status);
		}
		if( strcmp(tag_name , LOC_CANONICALIZE_TAG)==0 ){
			buflen = uloc_canonicalize ( mod_loc_name , tag_value->val , tag_value_len , &status);
		}

		if( U_FAILURE( status ) ) {
			if( status == U_BUFFER_OVERFLOW_ERROR ) {
				status = U_ZERO_ERROR;
				buflen++; /* add space for \0 */
				continue;
			}

			/* Error in retriving data */
			*result = 0;
			if( tag_value ){
				zend_string_release( tag_value );
			}
			if( mod_loc_name ){
				efree( mod_loc_name);
			}
			return NULL;
		}
	} while( buflen > tag_value_len );

	if(  buflen ==0 ){
		/* No value found */
		*result = -1;
		if( tag_value ){
			zend_string_release( tag_value );
		}
		if( mod_loc_name ){
			efree( mod_loc_name);
		}
		return NULL;
	} else {
		*result = 1;
	}

	if( mod_loc_name ){
		efree( mod_loc_name);
	}

	tag_value->len = strlen(tag_value->val);
	return tag_value;
}
/* }}} */

/* {{{
* Gets the value from ICU , called when PHP userspace function is called
* common code shared by get_primary_language,get_script or get_region or get_variant
*/
static void get_icu_value_src_php( char* tag_name, INTERNAL_FUNCTION_PARAMETERS)
{

	const char* loc_name        	= NULL;
	size_t         loc_name_len    	= 0;

	zend_string*   tag_value		= NULL;
	char*       empty_result	= "";

	int         result    		= 0;
	char*       msg        		= NULL;

	UErrorCode  status          	= U_ZERO_ERROR;

	intl_error_reset( NULL );

	if(zend_parse_parameters( ZEND_NUM_ARGS(), "s",
	&loc_name ,&loc_name_len ) == FAILURE) {
		spprintf(&msg , 0, "locale_get_%s : unable to parse input params", tag_name );
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,  msg , 1 );
		efree(msg);

		RETURN_FALSE;
    }

	if(loc_name_len == 0) {
		loc_name = intl_locale_get_default();
	}

	/* Call ICU get */
	tag_value = get_icu_value_internal( loc_name , tag_name , &result ,0);

	/* No value found */
	if( result == -1 ) {
		if( tag_value){
			zend_string_release( tag_value);
		}
		RETURN_STRING( empty_result);
	}

	/* value found */
	if( tag_value){
		RETVAL_STR( tag_value );
		return;
	}

	/* Error encountered while fetching the value */
	if( result ==0) {
		spprintf(&msg , 0, "locale_get_%s : unable to get locale %s", tag_name , tag_name );
		intl_error_set( NULL, status, msg , 1 );
		efree(msg);
		RETURN_NULL();
	}

}
/* }}} */

/* {{{ proto static string Locale::getScript($locale)
 * gets the script for the $locale
 }}} */
/* {{{ proto static string locale_get_script($locale)
 * gets the script for the $locale
 */
PHP_FUNCTION( locale_get_script )
{
	get_icu_value_src_php( LOC_SCRIPT_TAG , INTERNAL_FUNCTION_PARAM_PASSTHRU );
}
/* }}} */

/* {{{ proto static string Locale::getRegion($locale)
 * gets the region for the $locale
 }}} */
/* {{{ proto static string locale_get_region($locale)
 * gets the region for the $locale
 */
PHP_FUNCTION( locale_get_region )
{
	get_icu_value_src_php( LOC_REGION_TAG , INTERNAL_FUNCTION_PARAM_PASSTHRU );
}
/* }}} */

/* {{{ proto static string Locale::getPrimaryLanguage($locale)
 * gets the primary language for the $locale
 }}} */
/* {{{ proto static string locale_get_primary_language($locale)
 * gets the primary language for the $locale
 */
PHP_FUNCTION(locale_get_primary_language )
{
	get_icu_value_src_php( LOC_LANG_TAG , INTERNAL_FUNCTION_PARAM_PASSTHRU );
}
/* }}} */


/* {{{
 * common code shared by display_xyz functions to  get the value from ICU
 }}} */
static void get_icu_disp_value_src_php( char* tag_name, INTERNAL_FUNCTION_PARAMETERS)
{
	const char* loc_name        	= NULL;
	size_t         loc_name_len    	= 0;

	const char* disp_loc_name       = NULL;
	size_t      disp_loc_name_len   = 0;
	int         free_loc_name       = 0;

	UChar*      disp_name      	= NULL;
	int32_t     disp_name_len  	= 0;

	char*       mod_loc_name        = NULL;

	int32_t     buflen          	= 512;
	UErrorCode  status          	= U_ZERO_ERROR;

	zend_string* u8str;

  	char*       msg             	= NULL;
	int         grOffset    	= 0;

	intl_error_reset( NULL );

	if(zend_parse_parameters( ZEND_NUM_ARGS(), "s|s",
		&loc_name, &loc_name_len ,
		&disp_loc_name ,&disp_loc_name_len ) == FAILURE)
	{
		spprintf(&msg , 0, "locale_get_display_%s : unable to parse input params", tag_name );
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,  msg , 1 );
		efree(msg);
		RETURN_FALSE;
	}

    if(loc_name_len > ULOC_FULLNAME_CAPACITY) {
        /* See bug 67397: overlong locale names cause trouble in uloc_getDisplayName */
		spprintf(&msg , 0, "locale_get_display_%s : name too long", tag_name );
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,  msg , 1 );
		efree(msg);
		RETURN_FALSE;
    }

	if(loc_name_len == 0) {
		loc_name = intl_locale_get_default();
	}

	if( strcmp(tag_name, DISP_NAME) != 0 ){
		/* Handle grandfathered languages */
		grOffset = findOffset( LOC_GRANDFATHERED , loc_name );
		if( grOffset >= 0 ){
			if( strcmp(tag_name , LOC_LANG_TAG)==0 ){
				mod_loc_name = getPreferredTag( loc_name );
			} else {
				/* Since Grandfathered, no value, do nothing, retutn NULL */
				RETURN_FALSE;
			}
		}
	} /* end of if != LOC_CANONICAL_TAG */

	if( mod_loc_name==NULL ){
		mod_loc_name = estrdup( loc_name );
	}

	/* Check if disp_loc_name passed , if not use default locale */
	if( !disp_loc_name){
		disp_loc_name = estrdup(intl_locale_get_default());
		free_loc_name = 1;
	}

    /* Get the disp_value for the given locale */
    do{
        disp_name = erealloc( disp_name , buflen * sizeof(UChar)  );
        disp_name_len = buflen;

		if( strcmp(tag_name , LOC_LANG_TAG)==0 ){
			buflen = uloc_getDisplayLanguage ( mod_loc_name , disp_loc_name , disp_name , disp_name_len , &status);
		} else if( strcmp(tag_name , LOC_SCRIPT_TAG)==0 ){
			buflen = uloc_getDisplayScript ( mod_loc_name , disp_loc_name , disp_name , disp_name_len , &status);
		} else if( strcmp(tag_name , LOC_REGION_TAG)==0 ){
			buflen = uloc_getDisplayCountry ( mod_loc_name , disp_loc_name , disp_name , disp_name_len , &status);
		} else if( strcmp(tag_name , LOC_VARIANT_TAG)==0 ){
			buflen = uloc_getDisplayVariant ( mod_loc_name , disp_loc_name , disp_name , disp_name_len , &status);
		} else if( strcmp(tag_name , DISP_NAME)==0 ){
			buflen = uloc_getDisplayName ( mod_loc_name , disp_loc_name , disp_name , disp_name_len , &status);
		}

		/* U_STRING_NOT_TERMINATED_WARNING is admissible here; don't look for it */
		if( U_FAILURE( status ) )
		{
			if( status == U_BUFFER_OVERFLOW_ERROR )
			{
				status = U_ZERO_ERROR;
				continue;
			}

			spprintf(&msg, 0, "locale_get_display_%s : unable to get locale %s", tag_name , tag_name );
			intl_error_set( NULL, status, msg , 1 );
			efree(msg);
			if( disp_name){
				efree( disp_name );
			}
			if( mod_loc_name){
				efree( mod_loc_name );
			}
			if (free_loc_name) {
				efree((void *)disp_loc_name);
				disp_loc_name = NULL;
			}
			RETURN_FALSE;
		}
	} while( buflen > disp_name_len );

	if( mod_loc_name){
		efree( mod_loc_name );
	}
	if (free_loc_name) {
		efree((void *)disp_loc_name);
		disp_loc_name = NULL;
	}
	/* Convert display locale name from UTF-16 to UTF-8. */
	u8str = intl_convert_utf16_to_utf8(disp_name, buflen, &status );
	efree( disp_name );
	if( !u8str )
	{
		spprintf(&msg, 0, "locale_get_display_%s :error converting display name for %s to UTF-8", tag_name , tag_name );
		intl_error_set( NULL, status, msg , 1 );
		efree(msg);
		RETURN_FALSE;
	}

	RETVAL_NEW_STR( u8str );
}
/* }}} */

/* {{{ proto static string Locale::getDisplayName($locale[, $in_locale = null])
* gets the name for the $locale in $in_locale or default_locale
 }}} */
/* {{{ proto static string get_display_name($locale[, $in_locale = null])
* gets the name for the $locale in $in_locale or default_locale
*/
PHP_FUNCTION(locale_get_display_name)
{
    get_icu_disp_value_src_php( DISP_NAME , INTERNAL_FUNCTION_PARAM_PASSTHRU );
}
/* }}} */

/* {{{ proto static string Locale::getDisplayLanguage($locale[, $in_locale = null])
* gets the language for the $locale in $in_locale or default_locale
 }}} */
/* {{{ proto static string get_display_language($locale[, $in_locale = null])
* gets the language for the $locale in $in_locale or default_locale
*/
PHP_FUNCTION(locale_get_display_language)
{
    get_icu_disp_value_src_php( LOC_LANG_TAG , INTERNAL_FUNCTION_PARAM_PASSTHRU );
}
/* }}} */

/* {{{ proto static string Locale::getDisplayScript($locale, $in_locale = null)
* gets the script for the $locale in $in_locale or default_locale
 }}} */
/* {{{ proto static string get_display_script($locale, $in_locale = null)
* gets the script for the $locale in $in_locale or default_locale
*/
PHP_FUNCTION(locale_get_display_script)
{
    get_icu_disp_value_src_php( LOC_SCRIPT_TAG , INTERNAL_FUNCTION_PARAM_PASSTHRU );
}
/* }}} */

/* {{{ proto static string Locale::getDisplayRegion($locale, $in_locale = null)
* gets the region for the $locale in $in_locale or default_locale
 }}} */
/* {{{ proto static string get_display_region($locale, $in_locale = null)
* gets the region for the $locale in $in_locale or default_locale
*/
PHP_FUNCTION(locale_get_display_region)
{
    get_icu_disp_value_src_php( LOC_REGION_TAG , INTERNAL_FUNCTION_PARAM_PASSTHRU );
}
/* }}} */

/* {{{
* proto static string Locale::getDisplayVariant($locale, $in_locale = null)
* gets the variant for the $locale in $in_locale or default_locale
 }}} */
/* {{{
* proto static string get_display_variant($locale, $in_locale = null)
* gets the variant for the $locale in $in_locale or default_locale
*/
PHP_FUNCTION(locale_get_display_variant)
{
    get_icu_disp_value_src_php( LOC_VARIANT_TAG , INTERNAL_FUNCTION_PARAM_PASSTHRU );
}
/* }}} */

 /* {{{ proto static array getKeywords(string $locale) {
 * return an associative array containing keyword-value
 * pairs for this locale. The keys are keys to the array (doh!)
 * }}}*/
 /* {{{ proto static array locale_get_keywords(string $locale) {
 * return an associative array containing keyword-value
 * pairs for this locale. The keys are keys to the array (doh!)
 */
PHP_FUNCTION( locale_get_keywords )
{
    UEnumeration*   e        = NULL;
    UErrorCode      status   = U_ZERO_ERROR;

    const char*	 	kw_key        = NULL;
    int32_t         kw_key_len    = 0;

    const char*       	loc_name        = NULL;
    size_t        	 	loc_name_len    = 0;

/*
	ICU expects the buffer to be allocated  before calling the function
	and so the buffer size has been explicitly specified
	ICU uloc.h #define 	ULOC_KEYWORD_AND_VALUES_CAPACITY   100
	hence the kw_value buffer size is 100
*/
	zend_string *kw_value_str;
    int32_t     kw_value_len = 100;

    intl_error_reset( NULL );

    if(zend_parse_parameters( ZEND_NUM_ARGS(), "s",
        &loc_name, &loc_name_len ) == FAILURE)
    {
        intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
             "locale_get_keywords: unable to parse input params", 0 );

        RETURN_FALSE;
    }

    if(loc_name_len == 0) {
        loc_name = intl_locale_get_default();
    }

	/* Get the keywords */
    e = uloc_openKeywords( loc_name, &status );
    if( e != NULL )
    {
		/* Traverse it, filling the return array. */
    	array_init( return_value );

    	while( ( kw_key = uenum_next( e, &kw_key_len, &status ) ) != NULL ){
    		kw_value_len = 100;
			kw_value_str = zend_string_alloc(kw_value_len, 0);

			/* Get the keyword value for each keyword */
			kw_value_len=uloc_getKeywordValue( loc_name, kw_key, ZSTR_VAL(kw_value_str), kw_value_len, &status );
			if (status == U_BUFFER_OVERFLOW_ERROR) {
				status = U_ZERO_ERROR;
				kw_value_str = zend_string_extend(kw_value_str, kw_value_len, 0);
				kw_value_len=uloc_getKeywordValue( loc_name,kw_key, ZSTR_VAL(kw_value_str), kw_value_len+1, &status );
			} else if(!U_FAILURE(status)) {
				kw_value_str = zend_string_truncate(kw_value_str, kw_value_len, 0);
			}
			if (U_FAILURE(status)) {
				intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "locale_get_keywords: Error encountered while getting the keyword  value for the  keyword", 0 );
				if( kw_value_str){
					zend_string_free( kw_value_str );
				}
				zval_dtor(return_value);
        		RETURN_FALSE;
			}

       		add_assoc_str( return_value, (char *)kw_key, kw_value_str);
		} /* end of while */

	} /* end of if e!=NULL */

    uenum_close( e );
}
/* }}} */

 /* {{{ proto static string Locale::canonicalize($locale)
 * @return string the canonicalized locale
 * }}} */
 /* {{{ proto static string locale_canonicalize(Locale $loc, string $locale)
 * @param string $locale	The locale string to canonicalize
 */
PHP_FUNCTION(locale_canonicalize)
{
	get_icu_value_src_php( LOC_CANONICALIZE_TAG , INTERNAL_FUNCTION_PARAM_PASSTHRU );
}
/* }}} */

/* {{{ append_key_value
* Internal function which is called from locale_compose
* gets the value for the key_name and appends to the loc_name
* returns 1 if successful , -1 if not found ,
* 0 if array element is not a string , -2 if buffer-overflow
*/
static int append_key_value(smart_str* loc_name, HashTable* hash_arr, char* key_name)
{
	zval *ele_value;

	if ((ele_value = zend_hash_str_find(hash_arr , key_name, strlen(key_name))) != NULL ) {
		if(Z_TYPE_P(ele_value)!= IS_STRING ){
			/* element value is not a string */
			return FAILURE;
		}
		if(strcmp(key_name, LOC_LANG_TAG) != 0 &&
		   strcmp(key_name, LOC_GRANDFATHERED_LANG_TAG)!=0 ) {
			/* not lang or grandfathered tag */
			smart_str_appendl(loc_name, SEPARATOR , sizeof(SEPARATOR)-1);
		}
		smart_str_appendl(loc_name, Z_STRVAL_P(ele_value) , Z_STRLEN_P(ele_value));
		return SUCCESS;
	}

	return LOC_NOT_FOUND;
}
/* }}} */

/* {{{ append_prefix , appends the prefix needed
* e.g. private adds 'x'
*/
static void add_prefix(smart_str* loc_name, char* key_name)
{
	if( strncmp(key_name , LOC_PRIVATE_TAG , 7) == 0 ){
		smart_str_appendl(loc_name, SEPARATOR , sizeof(SEPARATOR)-1);
		smart_str_appendl(loc_name, PRIVATE_PREFIX , sizeof(PRIVATE_PREFIX)-1);
	}
}
/* }}} */

/* {{{ append_multiple_key_values
* Internal function which is called from locale_compose
* gets the multiple values for the key_name and appends to the loc_name
* used for 'variant','extlang','private'
* returns 1 if successful , -1 if not found ,
* 0 if array element is not a string , -2 if buffer-overflow
*/
static int append_multiple_key_values(smart_str* loc_name, HashTable* hash_arr, char* key_name)
{
	zval	*ele_value;
	int 	i 		= 0;
	int 	isFirstSubtag 	= 0;
	int 	max_value 	= 0;

	/* Variant/ Extlang/Private etc. */
	if ((ele_value = zend_hash_str_find( hash_arr , key_name , strlen(key_name))) != NULL) {
		if( Z_TYPE_P(ele_value) == IS_STRING ){
			add_prefix( loc_name , key_name);

			smart_str_appendl(loc_name, SEPARATOR , sizeof(SEPARATOR)-1);
			smart_str_appendl(loc_name, Z_STRVAL_P(ele_value) , Z_STRLEN_P(ele_value));
			return SUCCESS;
		} else if(Z_TYPE_P(ele_value) == IS_ARRAY ) {
			HashTable *arr = Z_ARRVAL_P(ele_value);
			zval *data;

			ZEND_HASH_FOREACH_VAL(arr, data) {
				if(Z_TYPE_P(data) != IS_STRING) {
					return FAILURE;
				}
				if (isFirstSubtag++ == 0){
					add_prefix(loc_name , key_name);
				}
				smart_str_appendl(loc_name, SEPARATOR , sizeof(SEPARATOR)-1);
				smart_str_appendl(loc_name, Z_STRVAL_P(data) , Z_STRLEN_P(data));
			} ZEND_HASH_FOREACH_END();
			return SUCCESS;
		} else {
			return FAILURE;
		}
	} else {
		char cur_key_name[31];
		/* Decide the max_value: the max. no. of elements allowed */
		if( strcmp(key_name , LOC_VARIANT_TAG) ==0 ){
			max_value  = MAX_NO_VARIANT;
		}
		if( strcmp(key_name , LOC_EXTLANG_TAG) ==0 ){
			max_value  = MAX_NO_EXTLANG;
		}
		if( strcmp(key_name , LOC_PRIVATE_TAG) ==0 ){
			max_value  = MAX_NO_PRIVATE;
		}

		/* Multiple variant values as variant0, variant1 ,variant2 */
		isFirstSubtag = 0;
		for( i=0 ; i< max_value; i++ ){
			snprintf( cur_key_name , 30, "%s%d", key_name , i);
			if ((ele_value = zend_hash_str_find( hash_arr , cur_key_name , strlen(cur_key_name))) != NULL) {
				if( Z_TYPE_P(ele_value)!= IS_STRING ){
					/* variant is not a string */
					return FAILURE;
				}
				/* Add the contents */
				if (isFirstSubtag++ == 0){
					add_prefix(loc_name , cur_key_name);
				}
				smart_str_appendl(loc_name, SEPARATOR , sizeof(SEPARATOR)-1);
				smart_str_appendl(loc_name, Z_STRVAL_P(ele_value) , Z_STRLEN_P(ele_value));
			}
		} /* end of for */
	} /* end of else */

	return SUCCESS;
}
/* }}} */

/*{{{
* If applicable sets error message and aborts locale_compose gracefully
* returns 0  if locale_compose needs to be aborted
* otherwise returns 1
*/
static int handleAppendResult( int result, smart_str* loc_name)
{
	intl_error_reset( NULL );
	if( result == FAILURE) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "locale_compose: parameter array element is not a string", 0 );
		smart_str_free(loc_name);
		return 0;
	}
	return 1;
}
/* }}} */

#define RETURN_SMART_STR(str) smart_str_0((str)); RETURN_NEW_STR((str)->s)
/* {{{ proto static string Locale::composeLocale($array)
* Creates a locale by combining the parts of locale-ID passed
* }}} */
/* {{{ proto static string compose_locale($array)
* Creates a locale by combining the parts of locale-ID passed
* }}} */
PHP_FUNCTION(locale_compose)
{
	smart_str      	loc_name_s = {0};
	smart_str *loc_name = &loc_name_s;
	zval*			arr	= NULL;
	HashTable*		hash_arr = NULL;
	int 			result = 0;

	intl_error_reset( NULL );

	if(zend_parse_parameters( ZEND_NUM_ARGS(), "a",
		&arr) == FAILURE)
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "locale_compose: unable to parse input params", 0 );
		RETURN_FALSE;
	}

	hash_arr = Z_ARRVAL_P( arr );

	if( !hash_arr || zend_hash_num_elements( hash_arr ) == 0 )
		RETURN_FALSE;

	/* Check for grandfathered first */
	result = append_key_value(loc_name, hash_arr,  LOC_GRANDFATHERED_LANG_TAG);
	if( result == SUCCESS){
		RETURN_SMART_STR(loc_name);
	}
	if( !handleAppendResult( result, loc_name)){
		RETURN_FALSE;
	}

	/* Not grandfathered */
	result = append_key_value(loc_name, hash_arr , LOC_LANG_TAG);
	if( result == LOC_NOT_FOUND ){
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
		"locale_compose: parameter array does not contain 'language' tag.", 0 );
		smart_str_free(loc_name);
		RETURN_FALSE;
	}
	if( !handleAppendResult( result, loc_name)){
		RETURN_FALSE;
	}

	/* Extlang */
	result = append_multiple_key_values(loc_name, hash_arr , LOC_EXTLANG_TAG);
	if( !handleAppendResult( result, loc_name)){
		RETURN_FALSE;
	}

	/* Script */
	result = append_key_value(loc_name, hash_arr , LOC_SCRIPT_TAG);
	if( !handleAppendResult( result, loc_name)){
		RETURN_FALSE;
	}

	/* Region */
	result = append_key_value( loc_name, hash_arr , LOC_REGION_TAG);
	if( !handleAppendResult( result, loc_name)){
		RETURN_FALSE;
	}

	/* Variant */
	result = append_multiple_key_values( loc_name, hash_arr , LOC_VARIANT_TAG);
	if( !handleAppendResult( result, loc_name)){
		RETURN_FALSE;
	}

	/* Private */
	result = append_multiple_key_values( loc_name, hash_arr , LOC_PRIVATE_TAG);
	if( !handleAppendResult( result, loc_name)){
		RETURN_FALSE;
	}

	RETURN_SMART_STR(loc_name);
}
/* }}} */


/*{{{
* Parses the locale and returns private subtags  if existing
* else returns NULL
* e.g. for locale='en_US-x-prv1-prv2-prv3'
* returns a pointer to the string 'prv1-prv2-prv3'
*/
static zend_string* get_private_subtags(const char* loc_name)
{
	zend_string*	result =NULL;
	int 	singletonPos = 0;
	int 	len =0;
	const char* 	mod_loc_name =NULL;

	if( loc_name && (len = strlen(loc_name)>0 ) ){
		mod_loc_name = loc_name ;
		len   = strlen(mod_loc_name);
		while( (singletonPos = getSingletonPos(mod_loc_name))!= -1){

			if( singletonPos!=-1){
				if( (*(mod_loc_name+singletonPos)=='x') || (*(mod_loc_name+singletonPos)=='X') ){
					/* private subtag start found */
					if( singletonPos + 2 ==  len){
						/* loc_name ends with '-x-' ; return  NULL */
					}
					else{
						/* result = mod_loc_name + singletonPos +2; */
						result = zend_string_init(mod_loc_name + singletonPos+2  , (len -( singletonPos +2) ), 0);
					}
					break;
				}
				else{
					if( singletonPos + 1 >=  len){
						/* String end */
						break;
					} else {
						/* singleton found but not a private subtag , hence check further in the string for the private subtag */
						mod_loc_name = mod_loc_name + singletonPos +1;
						len = strlen(mod_loc_name);
					}
				}
			}

		} /* end of while */
	}

	return result;
}
/* }}} */

/* {{{ code used by locale_parse
*/
static int add_array_entry(const char* loc_name, zval* hash_arr, char* key_name)
{
	zend_string*   key_value 	= NULL;
	char*   cur_key_name	= NULL;
	char*   token        	= NULL;
	char*   last_ptr  	= NULL;

	int	result		= 0;
	int 	cur_result  	= 0;
	int 	cnt  		= 0;


	if( strcmp(key_name , LOC_PRIVATE_TAG)==0 ){
		key_value = get_private_subtags( loc_name );
		result = 1;
	} else {
		key_value = get_icu_value_internal( loc_name , key_name , &result,1 );
	}
	if( (strcmp(key_name , LOC_PRIVATE_TAG)==0) ||
		( strcmp(key_name , LOC_VARIANT_TAG)==0) ){
		if( result > 0 && key_value){
			/* Tokenize on the "_" or "-"  */
			token = php_strtok_r( key_value->val , DELIMITER ,&last_ptr);
			if( cur_key_name ){
				efree( cur_key_name);
			}
			cur_key_name = (char*)ecalloc( 25,  25);
			sprintf( cur_key_name , "%s%d", key_name , cnt++);
			add_assoc_string( hash_arr, cur_key_name , token);
			/* tokenize on the "_" or "-" and stop  at singleton if any */
			while( (token = php_strtok_r(NULL , DELIMITER , &last_ptr)) && (strlen(token)>1) ){
				sprintf( cur_key_name , "%s%d", key_name , cnt++);
				add_assoc_string( hash_arr, cur_key_name , token);
			}
/*
			if( strcmp(key_name, LOC_PRIVATE_TAG) == 0 ){
			}
*/
		}
		if (key_value) {
			zend_string_release(key_value);
		}
	} else {
		if( result == 1 ){
			add_assoc_str( hash_arr, key_name , key_value);
			cur_result = 1;
		} else if (key_value) {
			zend_string_release(key_value);
		}
	}

	if( cur_key_name ){
		efree( cur_key_name);
	}
	/*if( key_name != LOC_PRIVATE_TAG && key_value){*/
	return cur_result;
}
/* }}} */

/* {{{ proto static array Locale::parseLocale($locale)
* parses a locale-id into an array the different parts of it
 }}} */
/* {{{ proto static array parse_locale($locale)
* parses a locale-id into an array the different parts of it
*/
PHP_FUNCTION(locale_parse)
{
    const char* loc_name        = NULL;
    size_t         loc_name_len    = 0;
    int         grOffset    	= 0;

    intl_error_reset( NULL );

    if(zend_parse_parameters( ZEND_NUM_ARGS(), "s",
        &loc_name, &loc_name_len ) == FAILURE)
    {
        intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
             "locale_parse: unable to parse input params", 0 );

        RETURN_FALSE;
    }

    if(loc_name_len == 0) {
        loc_name = intl_locale_get_default();
    }

	array_init( return_value );

	grOffset =  findOffset( LOC_GRANDFATHERED , loc_name );
	if( grOffset >= 0 ){
		add_assoc_string( return_value , LOC_GRANDFATHERED_LANG_TAG, (char *)loc_name);
	}
	else{
		/* Not grandfathered */
		add_array_entry( loc_name , return_value , LOC_LANG_TAG);
		add_array_entry( loc_name , return_value , LOC_SCRIPT_TAG);
		add_array_entry( loc_name , return_value , LOC_REGION_TAG);
		add_array_entry( loc_name , return_value , LOC_VARIANT_TAG);
		add_array_entry( loc_name , return_value , LOC_PRIVATE_TAG);
	}
}
/* }}} */

/* {{{ proto static array Locale::getAllVariants($locale)
* gets an array containing the list of variants, or null
 }}} */
/* {{{ proto static array locale_get_all_variants($locale)
* gets an array containing the list of variants, or null
*/
PHP_FUNCTION(locale_get_all_variants)
{
	const char*  	loc_name        = NULL;
	size_t    		loc_name_len    = 0;

	int	result		= 0;
	char*	token		= NULL;
	zend_string*	variant		= NULL;
	char*	saved_ptr	= NULL;

	intl_error_reset( NULL );

	if(zend_parse_parameters( ZEND_NUM_ARGS(), "s",
	&loc_name, &loc_name_len ) == FAILURE)
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
	     "locale_parse: unable to parse input params", 0 );

		RETURN_FALSE;
	}

	if(loc_name_len == 0) {
		loc_name = intl_locale_get_default();
	}


	array_init( return_value );

	/* If the locale is grandfathered, stop, no variants */
	if( findOffset( LOC_GRANDFATHERED , loc_name ) >=  0 ){
		/* ("Grandfathered Tag. No variants."); */
	}
	else {
	/* Call ICU variant */
		variant = get_icu_value_internal( loc_name , LOC_VARIANT_TAG , &result ,0);
		if( result > 0 && variant){
			/* Tokenize on the "_" or "-" */
			token = php_strtok_r( variant->val , DELIMITER , &saved_ptr);
			add_next_index_stringl( return_value, token , strlen(token));
			/* tokenize on the "_" or "-" and stop  at singleton if any	*/
			while( (token = php_strtok_r(NULL , DELIMITER, &saved_ptr)) && (strlen(token)>1) ){
 				add_next_index_stringl( return_value, token , strlen(token));
			}
		}
		if( variant ){
			zend_string_release( variant );
		}
	}


}
/* }}} */

/*{{{
* Converts to lower case and also replaces all hyphens with the underscore
*/
static int strToMatch(const char* str ,char *retstr)
{
	char* 	anchor 	= NULL;
	const char* 	anchor1 = NULL;
	int 	result 	= 0;

    if( (!str) || str[0] == '\0'){
        return result;
    } else {
	anchor = retstr;
	anchor1 = str;
        while( (*str)!='\0' ){
		if( *str == '-' ){
			*retstr =  '_';
		} else {
			*retstr = tolower(*str);
		}
            str++;
            retstr++;
	}
	*retstr = '\0';
	retstr=  anchor;
	str=  anchor1;
	result = 1;
    }

    return(result);
}
/* }}} */

/* {{{ proto static boolean Locale::filterMatches(string $langtag, string $locale[, bool $canonicalize])
* Checks if a $langtag filter matches with $locale according to RFC 4647's basic filtering algorithm
*/
/* }}} */
/* {{{ proto boolean locale_filter_matches(string $langtag, string $locale[, bool $canonicalize])
* Checks if a $langtag filter matches with $locale according to RFC 4647's basic filtering algorithm
*/
PHP_FUNCTION(locale_filter_matches)
{
	char*       	lang_tag        = NULL;
	size_t         	lang_tag_len    = 0;
	const char*     loc_range       = NULL;
	size_t         	loc_range_len   = 0;

	int		result		= 0;
	char*		token		= 0;
	char*		chrcheck	= NULL;

	zend_string*   	can_lang_tag    = NULL;
	zend_string*   	can_loc_range   = NULL;

	char*       	cur_lang_tag    = NULL;
	char*       	cur_loc_range   = NULL;

	zend_bool 	boolCanonical 	= 0;
	UErrorCode	status		= U_ZERO_ERROR;

	intl_error_reset( NULL );

	if(zend_parse_parameters( ZEND_NUM_ARGS(), "ss|b",
		&lang_tag, &lang_tag_len , &loc_range , &loc_range_len ,
		&boolCanonical) == FAILURE)
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
		"locale_filter_matches: unable to parse input params", 0 );

		RETURN_FALSE;
	}

	if(loc_range_len == 0) {
		loc_range = intl_locale_get_default();
	}

	if( strcmp(loc_range,"*")==0){
		RETURN_TRUE;
	}

	if( boolCanonical ){
		/* canonicalize loc_range */
		can_loc_range=get_icu_value_internal( loc_range , LOC_CANONICALIZE_TAG , &result , 0);
		if( result ==0) {
			intl_error_set( NULL, status,
				"locale_filter_matches : unable to canonicalize loc_range" , 0 );
			RETURN_FALSE;
		}

		/* canonicalize lang_tag */
		can_lang_tag = get_icu_value_internal( lang_tag , LOC_CANONICALIZE_TAG , &result ,  0);
		if( result ==0) {
			intl_error_set( NULL, status,
				"locale_filter_matches : unable to canonicalize lang_tag" , 0 );
			RETURN_FALSE;
		}

		/* Convert to lower case for case-insensitive comparison */
		cur_lang_tag = ecalloc( 1, can_lang_tag->len + 1);

		/* Convert to lower case for case-insensitive comparison */
		result = strToMatch( can_lang_tag->val , cur_lang_tag);
		if( result == 0) {
			efree( cur_lang_tag );
			zend_string_release( can_lang_tag );
			RETURN_FALSE;
		}

		cur_loc_range = ecalloc( 1, can_loc_range->len + 1);
		result = strToMatch( can_loc_range->val , cur_loc_range );
		if( result == 0) {
			efree( cur_lang_tag );
			zend_string_release( can_lang_tag );
			efree( cur_loc_range );
			zend_string_release( can_loc_range );
			RETURN_FALSE;
		}

		/* check if prefix */
		token 	= strstr( cur_lang_tag , cur_loc_range );

		if( token && (token==cur_lang_tag) ){
			/* check if the char. after match is SEPARATOR */
			chrcheck = token + (strlen(cur_loc_range));
			if( isIDSeparator(*chrcheck) || isEndOfTag(*chrcheck) ){
				if( cur_lang_tag){
					efree( cur_lang_tag );
				}
				if( cur_loc_range){
					efree( cur_loc_range );
				}
				if( can_lang_tag){
					zend_string_release( can_lang_tag );
				}
				if( can_loc_range){
					zend_string_release( can_loc_range );
				}
				RETURN_TRUE;
			}
		}

		/* No prefix as loc_range */
		if( cur_lang_tag){
			efree( cur_lang_tag );
		}
		if( cur_loc_range){
			efree( cur_loc_range );
		}
		if( can_lang_tag){
			zend_string_release( can_lang_tag );
		}
		if( can_loc_range){
			zend_string_release( can_loc_range );
		}
		RETURN_FALSE;

	} /* end of if isCanonical */
	else{
		/* Convert to lower case for case-insensitive comparison */
		cur_lang_tag = ecalloc( 1, strlen(lang_tag ) + 1);

		result = strToMatch( lang_tag , cur_lang_tag);
		if( result == 0) {
			efree( cur_lang_tag );
			RETURN_FALSE;
		}
		cur_loc_range = ecalloc( 1, strlen(loc_range ) + 1);
		result = strToMatch( loc_range , cur_loc_range );
		if( result == 0) {
			efree( cur_lang_tag );
			efree( cur_loc_range );
			RETURN_FALSE;
		}

		/* check if prefix */
		token 	= strstr( cur_lang_tag , cur_loc_range );

		if( token && (token==cur_lang_tag) ){
			/* check if the char. after match is SEPARATOR */
			chrcheck = token + (strlen(cur_loc_range));
			if( isIDSeparator(*chrcheck) || isEndOfTag(*chrcheck) ){
				if( cur_lang_tag){
					efree( cur_lang_tag );
				}
				if( cur_loc_range){
					efree( cur_loc_range );
				}
				RETURN_TRUE;
			}
		}

		/* No prefix as loc_range */
		if( cur_lang_tag){
			efree( cur_lang_tag );
		}
		if( cur_loc_range){
			efree( cur_loc_range );
		}
		RETURN_FALSE;

	}
}
/* }}} */

static void array_cleanup( char* arr[] , int arr_size)
{
	int i=0;
	for( i=0; i< arr_size; i++ ){
		if( arr[i*2] ){
			efree( arr[i*2]);
		}
	}
	efree(arr);
}

#define LOOKUP_CLEAN_RETURN(value)	array_cleanup(cur_arr, cur_arr_len); return (value)
/* {{{
* returns the lookup result to lookup_loc_range_src_php
* internal function
*/
static zend_string* lookup_loc_range(const char* loc_range, HashTable* hash_arr, int canonicalize )
{
	int	i = 0;
	int	cur_arr_len = 0;
	int result = 0;

	zend_string* lang_tag = NULL;
	zval* ele_value = NULL;
	char** cur_arr = NULL;

	char* cur_loc_range	= NULL;
	zend_string* can_loc_range	= NULL;
	int	saved_pos = 0;

	zend_string* return_value = NULL;

	cur_arr = ecalloc(zend_hash_num_elements(hash_arr)*2, sizeof(char *));
	ZEND_HASH_FOREACH_VAL(hash_arr, ele_value) {
	/* convert the array to lowercase , also replace hyphens with the underscore and store it in cur_arr */
		if(Z_TYPE_P(ele_value)!= IS_STRING) {
			/* element value is not a string */
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "lookup_loc_range: locale array element is not a string", 0);
			LOOKUP_CLEAN_RETURN(NULL);
		}
		cur_arr[cur_arr_len*2] = estrndup(Z_STRVAL_P(ele_value), Z_STRLEN_P(ele_value));
		result = strToMatch(Z_STRVAL_P(ele_value), cur_arr[cur_arr_len*2]);
		if(result == 0) {
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "lookup_loc_range: unable to canonicalize lang_tag", 0);
			LOOKUP_CLEAN_RETURN(NULL);
		}
		cur_arr[cur_arr_len*2+1] = Z_STRVAL_P(ele_value);
		cur_arr_len++ ;
	} ZEND_HASH_FOREACH_END(); /* end of for */

	/* Canonicalize array elements */
	if(canonicalize) {
		for(i=0; i<cur_arr_len; i++) {
			lang_tag = get_icu_value_internal(cur_arr[i*2], LOC_CANONICALIZE_TAG, &result, 0);
			if(result != 1 || lang_tag == NULL || !lang_tag->val[0]) {
				if(lang_tag) {
					zend_string_release(lang_tag);
				}
				intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "lookup_loc_range: unable to canonicalize lang_tag" , 0);
				LOOKUP_CLEAN_RETURN(NULL);
			}
			cur_arr[i*2] = erealloc(cur_arr[i*2], lang_tag->len+1);
			result = strToMatch(lang_tag->val, cur_arr[i*2]);
			zend_string_release(lang_tag);
			if(result == 0) {
				intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "lookup_loc_range: unable to canonicalize lang_tag" , 0);
				LOOKUP_CLEAN_RETURN(NULL);
			}
		}

	}

	if(canonicalize) {
		/* Canonicalize the loc_range */
		can_loc_range = get_icu_value_internal(loc_range, LOC_CANONICALIZE_TAG, &result , 0);
		if( result != 1 || can_loc_range == NULL || !can_loc_range->val[0]) {
			/* Error */
			intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "lookup_loc_range: unable to canonicalize loc_range" , 0 );
			if(can_loc_range) {
				zend_string_release(can_loc_range);
			}
			LOOKUP_CLEAN_RETURN(NULL);
		} else {
			loc_range = can_loc_range->val;
		}
	}

	cur_loc_range = ecalloc(1, strlen(loc_range)+1);
	/* convert to lower and replace hyphens */
	result = strToMatch(loc_range, cur_loc_range);
	if(can_loc_range) {
		zend_string_release(can_loc_range);
	}
	if(result == 0) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "lookup_loc_range: unable to canonicalize lang_tag" , 0);
		LOOKUP_CLEAN_RETURN(NULL);
	}

	/* Lookup for the lang_tag match */
	saved_pos = strlen(cur_loc_range);
	while(saved_pos > 0) {
		for(i=0; i< cur_arr_len; i++){
			if(cur_arr[i*2] != NULL && strlen(cur_arr[i*2]) == saved_pos && strncmp(cur_loc_range, cur_arr[i*2], saved_pos) == 0) {
				/* Match found */
				char *str = canonicalize ? cur_arr[i*2] : cur_arr[i*2+1];
				return_value = zend_string_init(str, strlen(str), 0);
				efree(cur_loc_range);
				LOOKUP_CLEAN_RETURN(return_value);
			}
		}
		saved_pos = getStrrtokenPos(cur_loc_range, saved_pos);
	}

	/* Match not found */
	efree(cur_loc_range);
	LOOKUP_CLEAN_RETURN(NULL);
}
/* }}} */

/* {{{ proto string Locale::lookup(array $langtag, string $locale[, bool $canonicalize[, string $default = null]])
* Searchs the items in $langtag for the best match to the language
* range
*/
/* }}} */
/* {{{ proto string locale_lookup(array $langtag, string $locale[, bool $canonicalize[, string $default = null]])
* Searchs the items in $langtag for the best match to the language
* range
*/
PHP_FUNCTION(locale_lookup)
{
	zend_string*   	fallback_loc_str	= NULL;
	const char*    	loc_range      		= NULL;
	size_t        	loc_range_len  		= 0;

	zval*		arr				= NULL;
	HashTable*	hash_arr		= NULL;
	zend_bool	boolCanonical	= 0;
	zend_string* 	result_str	= NULL;

	intl_error_reset( NULL );

	if(zend_parse_parameters( ZEND_NUM_ARGS(), "as|bS", &arr, &loc_range, &loc_range_len,
		&boolCanonical,	&fallback_loc_str) == FAILURE) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,	"locale_lookup: unable to parse input params", 0 );
		RETURN_FALSE;
	}

	if(loc_range_len == 0) {
		if(fallback_loc_str) {
			loc_range = ZSTR_VAL(fallback_loc_str);
		} else {
			loc_range = intl_locale_get_default();
		}
	}

	hash_arr = Z_ARRVAL_P(arr);

	if( !hash_arr || zend_hash_num_elements( hash_arr ) == 0 ) {
		RETURN_EMPTY_STRING();
	}

	result_str = lookup_loc_range(loc_range, hash_arr, boolCanonical);
	if(result_str == NULL || ZSTR_VAL(result_str)[0] == '\0') {
		if( fallback_loc_str ) {
			result_str = zend_string_copy(fallback_loc_str);
		} else {
			RETURN_EMPTY_STRING();
		}
	}

	RETURN_STR(result_str);
}
/* }}} */

/* {{{ proto string Locale::acceptFromHttp(string $http_accept)
* Tries to find out best available locale based on HTTP �Accept-Language� header
*/
/* }}} */
/* {{{ proto string locale_accept_from_http(string $http_accept)
* Tries to find out best available locale based on HTTP �Accept-Language� header
*/
PHP_FUNCTION(locale_accept_from_http)
{
	UEnumeration *available;
	char *http_accept = NULL;
	size_t http_accept_len;
	UErrorCode status = 0;
	int len;
	char resultLocale[INTL_MAX_LOCALE_LEN+1];
	UAcceptResult outResult;

	if(zend_parse_parameters( ZEND_NUM_ARGS(), "s", &http_accept, &http_accept_len) == FAILURE)
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
		"locale_accept_from_http: unable to parse input parameters", 0 );
		RETURN_FALSE;
	}
	if(http_accept_len > ULOC_FULLNAME_CAPACITY) {
		/* check each fragment, if any bigger than capacity, can't do it due to bug #72533 */
		char *start = http_accept;
		char *end;
		size_t len;
		do {
			end = strchr(start, ',');
			len = end ? end-start : http_accept_len-(start-http_accept);
			if(len > ULOC_FULLNAME_CAPACITY) {
				intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
						"locale_accept_from_http: locale string too long", 0 TSRMLS_CC );
				RETURN_FALSE;
			}
			if(end) {
				start = end+1;
			}
		} while(end != NULL);
	}

	available = ures_openAvailableLocales(NULL, &status);
	INTL_CHECK_STATUS(status, "locale_accept_from_http: failed to retrieve locale list");
	len = uloc_acceptLanguageFromHTTP(resultLocale, INTL_MAX_LOCALE_LEN,
						&outResult, http_accept, available, &status);
	uenum_close(available);
	INTL_CHECK_STATUS(status, "locale_accept_from_http: failed to find acceptable locale");
	if (len < 0 || outResult == ULOC_ACCEPT_FAILED) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(resultLocale, len);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 *can_loc_len
*/
