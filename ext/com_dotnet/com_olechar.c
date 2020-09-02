/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   |         Harald Radi <h.radi@nme.at>                                  |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_com_dotnet.h"
#include "php_com_dotnet_internal.h"


PHP_COM_DOTNET_API OLECHAR *php_com_string_to_olestring(const char *string, size_t string_len, int codepage)
{
	OLECHAR *olestring = NULL;
	DWORD flags = codepage == CP_UTF8 ? 0 : MB_PRECOMPOSED | MB_ERR_INVALID_CHARS;
	BOOL ok;

	if (string_len == -1) {
		/* determine required length for the buffer (includes NUL terminator) */
		string_len = MultiByteToWideChar(codepage, flags, string, -1, NULL, 0);
	} else {
		/* allow room for NUL terminator */
		string_len++;
	}

	if (string_len > 0) {
		olestring = (OLECHAR*)safe_emalloc(string_len, sizeof(OLECHAR), 0);
		/* XXX if that's a real multibyte string, olestring is obviously allocated excessively.
		This should be fixed by reallocating the olestring, but as emalloc is used, that doesn't
		matter much. */
		ok = MultiByteToWideChar(codepage, flags, string, (int)string_len, olestring, (int)string_len);
		if (ok > 0 && (size_t)ok < string_len) {
			olestring[ok] = '\0';
		}
	} else {
		ok = FALSE;
		olestring = (OLECHAR*)emalloc(sizeof(OLECHAR));
		*olestring = 0;
	}

	if (!ok) {
		char *msg = php_win32_error_to_msg(GetLastError());

		php_error_docref(NULL, E_WARNING,
			"Could not convert string to unicode: `%s'", msg);

		php_win32_error_msg_free(msg);
	}

	return olestring;
}

PHP_COM_DOTNET_API char *php_com_olestring_to_string(OLECHAR *olestring, size_t *string_len, int codepage)
{
	char *string;
	uint32_t length = 0;
	BOOL ok;

	length = WideCharToMultiByte(codepage, 0, olestring, -1, NULL, 0, NULL, NULL);

	if (length) {
		string = (char*)safe_emalloc(length, sizeof(char), 0);
		length = WideCharToMultiByte(codepage, 0, olestring, -1, string, length, NULL, NULL);
		ok = length > 0;
	} else {
		string = (char*)emalloc(sizeof(char));
		*string = '\0';
		ok = FALSE;
		length = 0;
	}

	if (!ok) {
		char *msg = php_win32_error_to_msg(GetLastError());

		php_error_docref(NULL, E_WARNING,
			"Could not convert string from unicode: `%s'", msg);

		php_win32_error_msg_free(msg);
	}

	if (string_len) {
		*string_len = length-1;
	}

	return string;
}

BSTR php_com_string_to_bstr(zend_string *string, int codepage)
{
	BSTR bstr = NULL;
	DWORD flags = codepage == CP_UTF8 ? 0 : MB_PRECOMPOSED | MB_ERR_INVALID_CHARS;
	size_t mb_len = ZSTR_LEN(string);
	int wc_len;

	if ((wc_len = MultiByteToWideChar(codepage, flags, ZSTR_VAL(string), (int)mb_len + 1, NULL, 0)) <= 0) {
		goto fail;
	}
	if ((bstr = SysAllocStringLen(NULL, (UINT)(wc_len - 1))) == NULL) {
		goto fail;
	}
	if ((wc_len = MultiByteToWideChar(codepage, flags, ZSTR_VAL(string), (int)mb_len + 1, bstr, wc_len)) <= 0) {
		goto fail;
	}
	return bstr;

fail:
	char *msg = php_win32_error_to_msg(GetLastError());
	php_error_docref(NULL, E_WARNING,
		"Could not convert string to unicode: `%s'", msg);
	LocalFree(msg);
	SysFreeString(bstr);
	return SysAllocString(L"");
}

zend_string *php_com_bstr_to_string(BSTR bstr, int codepage)
{
	zend_string *string = NULL;
	UINT wc_len = SysStringLen(bstr);
	int mb_len;

	mb_len = WideCharToMultiByte(codepage, 0, bstr, wc_len + 1, NULL, 0, NULL, NULL);
	if (mb_len > 0) {
		string = zend_string_alloc(mb_len - 1, 0);
		mb_len = WideCharToMultiByte(codepage, 0, bstr, wc_len + 1, ZSTR_VAL(string), mb_len, NULL, NULL);
	}

	if (mb_len <= 0) {
		char *msg = php_win32_error_to_msg(GetLastError());

		php_error_docref(NULL, E_WARNING,
			"Could not convert string from unicode: `%s'", msg);
		LocalFree(msg);

		if (string != NULL) {
			zend_string_release(string);
		}
		string = ZSTR_EMPTY_ALLOC();
	}

	return string;
}
