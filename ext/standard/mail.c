/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rasmus Lerdorf <rasmus@php.net>                              |
   +----------------------------------------------------------------------+
 */

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include "php.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "ext/standard/basic_functions.h"
#include "ext/date/php_date.h"
#include "zend_smart_str.h"

#ifdef HAVE_SYSEXITS_H
# include <sysexits.h>
#endif
#ifdef HAVE_SYS_SYSEXITS_H
# include <sys/sysexits.h>
#endif

#if PHP_SIGCHILD
# include <signal.h>
#endif

#include "php_syslog.h"
#include "php_mail.h"
#include "php_ini.h"
#include "php_string.h"
#include "exec.h"

#ifdef PHP_WIN32
# include "win32/sendmail.h"
#endif

#define SKIP_LONG_HEADER_SEP(str, pos)																	\
	if (str[pos] == '\r' && str[pos + 1] == '\n' && (str[pos + 2] == ' ' || str[pos + 2] == '\t')) {	\
		pos += 2;																						\
		while (str[pos + 1] == ' ' || str[pos + 1] == '\t') {											\
			pos++;																						\
		}																								\
		continue;																						\
	}																									\

extern zend_long php_getuid(void);

static bool php_mail_build_headers_check_field_value(zval *val)
{
	size_t len = 0;
	zend_string *value = Z_STR_P(val);

	/* https://tools.ietf.org/html/rfc2822#section-2.2.1 */
	/* https://tools.ietf.org/html/rfc2822#section-2.2.3 */
	while (len < value->len) {
		if (*(value->val+len) == '\r') {
			if (value->len - len >= 3
				&&  *(value->val+len+1) == '\n'
				&& (*(value->val+len+2) == ' '  || *(value->val+len+2) == '\t')) {
				len += 3;
				continue;
			}
			return FAILURE;
		}
		if (*(value->val+len) == '\0') {
			return FAILURE;
		}
		len++;
	}
	return SUCCESS;
}


static bool php_mail_build_headers_check_field_name(zend_string *key)
{
	size_t len = 0;

	/* https://tools.ietf.org/html/rfc2822#section-2.2 */
	while (len < key->len) {
		if (*(key->val+len) < 33 || *(key->val+len) > 126 || *(key->val+len) == ':') {
			return FAILURE;
		}
		len++;
	}
	return SUCCESS;
}


static void php_mail_build_headers_elems(smart_str *s, zend_string *key, zval *val);

static void php_mail_build_headers_elem(smart_str *s, zend_string *key, zval *val)
{
	switch(Z_TYPE_P(val)) {
		case IS_STRING:
			if (php_mail_build_headers_check_field_name(key) != SUCCESS) {
				zend_value_error("Header name \"%s\" contains invalid characters", ZSTR_VAL(key));
				return;
			}
			if (php_mail_build_headers_check_field_value(val) != SUCCESS) {
				zend_value_error("Header \"%s\" has invalid format, or contains invalid characters", ZSTR_VAL(key));
				return;
			}
			smart_str_append(s, key);
			smart_str_appendl(s, ": ", 2);
			smart_str_appends(s, Z_STRVAL_P(val));
			smart_str_appendl(s, "\r\n", 2);
			break;
		case IS_ARRAY:
			php_mail_build_headers_elems(s, key, val);
			break;
		default:
			zend_type_error("Header \"%s\" must be of type array|string, %s given", ZSTR_VAL(key), zend_zval_value_name(val));
	}
}


static void php_mail_build_headers_elems(smart_str *s, zend_string *key, zval *val)
{
	zend_string *tmp_key;
	zval *tmp_val;

	ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(val), tmp_key, tmp_val) {
		if (tmp_key) {
			zend_type_error("Header \"%s\" must only contain numeric keys, \"%s\" found", ZSTR_VAL(key), ZSTR_VAL(tmp_key));
			break;
		}
		ZVAL_DEREF(tmp_val);
		if (Z_TYPE_P(tmp_val) != IS_STRING) {
			zend_type_error("Header \"%s\" must only contain values of type string, %s found", ZSTR_VAL(key), zend_zval_value_name(tmp_val));
			break;
		}
		php_mail_build_headers_elem(s, key, tmp_val);
	} ZEND_HASH_FOREACH_END();
}


PHPAPI zend_string *php_mail_build_headers(HashTable *headers)
{
	zend_ulong idx;
	zend_string *key;
	zval *val;
	smart_str s = {0};

	ZEND_HASH_FOREACH_KEY_VAL(headers, idx, key, val) {
		if (!key) {
			zend_type_error("Header name cannot be numeric, " ZEND_LONG_FMT " given", idx);
			break;
		}
		ZVAL_DEREF(val);
		/* https://tools.ietf.org/html/rfc2822#section-3.6 */
		if (zend_string_equals_literal_ci(key, "orig-date")) {
			PHP_MAIL_BUILD_HEADER_CHECK("orig-date", s, key, val);
		} else if (zend_string_equals_literal_ci(key, "from")) {
			PHP_MAIL_BUILD_HEADER_CHECK("from", s, key, val);
		} else if (zend_string_equals_literal_ci(key, "sender")) {
			PHP_MAIL_BUILD_HEADER_CHECK("sender", s, key, val);
		} else if (zend_string_equals_literal_ci(key, "reply-to")) {
			PHP_MAIL_BUILD_HEADER_CHECK("reply-to", s, key, val);
		} else if (zend_string_equals_literal_ci(key, "to")) {
			zend_value_error("The additional headers cannot contain the \"To\" header");
		} else if (zend_string_equals_literal_ci(key, "cc")) {
			PHP_MAIL_BUILD_HEADER_CHECK("cc", s, key, val);
		} else if (zend_string_equals_literal_ci(key, "bcc")) {
			PHP_MAIL_BUILD_HEADER_CHECK("bcc", s, key, val);
		} else if (zend_string_equals_literal_ci(key, "message-id")) {
			PHP_MAIL_BUILD_HEADER_CHECK("message-id", s, key, val);
		} else if (zend_string_equals_literal_ci(key, "references")) {
			PHP_MAIL_BUILD_HEADER_CHECK("references", s, key, val);
		} else if (zend_string_equals_literal_ci(key, "in-reply-to")) {
			PHP_MAIL_BUILD_HEADER_CHECK("in-reply-to", s, key, val);
		} else if (zend_string_equals_literal_ci(key, "subject")) {
			zend_value_error("The additional headers cannot contain the \"Subject\" header");
		} else {
			PHP_MAIL_BUILD_HEADER_DEFAULT(s, key, val);
		}

		if (EG(exception)) {
			smart_str_free(&s);
			return NULL;
		}
	} ZEND_HASH_FOREACH_END();

	/* Remove the last \r\n */
	if (s.s) s.s->len -= 2;
	smart_str_0(&s);

	return s.s;
}


/* {{{ Send an email message */
PHP_FUNCTION(mail)
{
	char *to=NULL, *message=NULL;
	char *subject=NULL;
	zend_string *extra_cmd=NULL;
	zend_string *headers_str = NULL;
	HashTable *headers_ht = NULL;
	size_t to_len, message_len;
	size_t subject_len, i;
	char *force_extra_parameters = INI_STR("mail.force_extra_parameters");
	char *to_r, *subject_r;

	ZEND_PARSE_PARAMETERS_START(3, 5)
		Z_PARAM_PATH(to, to_len)
		Z_PARAM_PATH(subject, subject_len)
		Z_PARAM_PATH(message, message_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT_OR_STR(headers_ht, headers_str)
		Z_PARAM_PATH_STR(extra_cmd)
	ZEND_PARSE_PARAMETERS_END();

	if (headers_str) {
		if (strlen(ZSTR_VAL(headers_str)) != ZSTR_LEN(headers_str)) {
			zend_argument_value_error(4, "must not contain any null bytes");
			RETURN_THROWS();
		}
		headers_str = php_trim(headers_str, NULL, 0, 2);
	} else if (headers_ht) {
		headers_str = php_mail_build_headers(headers_ht);
		if (EG(exception)) {
			RETURN_THROWS();
		}
	}

	if (to_len > 0) {
		to_r = estrndup(to, to_len);
		for (; to_len; to_len--) {
			if (!isspace((unsigned char) to_r[to_len - 1])) {
				break;
			}
			to_r[to_len - 1] = '\0';
		}
		for (i = 0; to_r[i]; i++) {
			if (iscntrl((unsigned char) to_r[i])) {
				/* According to RFC 822, section 3.1.1 long headers may be separated into
				 * parts using CRLF followed at least one linear-white-space character ('\t' or ' ').
				 * To prevent these separators from being replaced with a space, we use the
				 * SKIP_LONG_HEADER_SEP to skip over them. */
				SKIP_LONG_HEADER_SEP(to_r, i);
				to_r[i] = ' ';
			}
		}
	} else {
		to_r = to;
	}

	if (subject_len > 0) {
		subject_r = estrndup(subject, subject_len);
		for (; subject_len; subject_len--) {
			if (!isspace((unsigned char) subject_r[subject_len - 1])) {
				break;
			}
			subject_r[subject_len - 1] = '\0';
		}
		for (i = 0; subject_r[i]; i++) {
			if (iscntrl((unsigned char) subject_r[i])) {
				SKIP_LONG_HEADER_SEP(subject_r, i);
				subject_r[i] = ' ';
			}
		}
	} else {
		subject_r = subject;
	}

	if (force_extra_parameters) {
		extra_cmd = php_escape_shell_cmd(force_extra_parameters);
	} else if (extra_cmd) {
		extra_cmd = php_escape_shell_cmd(ZSTR_VAL(extra_cmd));
	}

	if (php_mail(to_r, subject_r, message, headers_str && ZSTR_LEN(headers_str) ? ZSTR_VAL(headers_str) : NULL, extra_cmd ? ZSTR_VAL(extra_cmd) : NULL)) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}

	if (headers_str) {
		zend_string_release_ex(headers_str, 0);
	}

	if (extra_cmd) {
		zend_string_release_ex(extra_cmd, 0);
	}
	if (to_r != to) {
		efree(to_r);
	}
	if (subject_r != subject) {
		efree(subject_r);
	}
}
/* }}} */


void php_mail_log_crlf_to_spaces(char *message) {
	/* Find all instances of carriage returns or line feeds and
	 * replace them with spaces. Thus, a log line is always one line
	 * long
	 */
	char *p = message;
	while ((p = strpbrk(p, "\r\n"))) {
		*p = ' ';
	}
}

void php_mail_log_to_syslog(char *message) {
	/* Write 'message' to syslog. */
#ifdef HAVE_SYSLOG_H
	php_syslog(LOG_NOTICE, "%s", message);
#endif
}


void php_mail_log_to_file(char *filename, char *message, size_t message_size) {
	/* Write 'message' to the given file. */
	uint32_t flags = REPORT_ERRORS | STREAM_DISABLE_OPEN_BASEDIR;
	php_stream *stream = php_stream_open_wrapper(filename, "a", flags, NULL);
	if (stream) {
		php_stream_write(stream, message, message_size);
		php_stream_close(stream);
	}
}


static int php_mail_detect_multiple_crlf(const char *hdr) {
	/* This function detects multiple/malformed multiple newlines. */

	if (!hdr || !strlen(hdr)) {
		return 0;
	}

	/* Should not have any newlines at the beginning. */
	/* RFC 2822 2.2. Header Fields */
	if (*hdr < 33 || *hdr > 126 || *hdr == ':') {
		return 1;
	}

	while(*hdr) {
		if (*hdr == '\r') {
			if (*(hdr+1) == '\0' || *(hdr+1) == '\r' || (*(hdr+1) == '\n' && (*(hdr+2) == '\0' || *(hdr+2) == '\n' || *(hdr+2) == '\r'))) {
				/* Malformed or multiple newlines. */
				return 1;
			} else {
				hdr += 2;
			}
		} else if (*hdr == '\n') {
			if (*(hdr+1) == '\0' || *(hdr+1) == '\r' || *(hdr+1) == '\n') {
				/* Malformed or multiple newlines. */
				return 1;
			} else {
				hdr += 2;
			}
		} else {
			hdr++;
		}
	}

	return 0;
}


/* {{{ php_mail */
PHPAPI int php_mail(const char *to, const char *subject, const char *message, const char *headers, const char *extra_cmd)
{
#ifdef PHP_WIN32
	int tsm_err;
	char *tsm_errmsg = NULL;
#endif
	FILE *sendmail;
	int ret;
	char *sendmail_path = INI_STR("sendmail_path");
	char *sendmail_cmd = NULL;
	char *mail_log = INI_STR("mail.log");
	const char *hdr = headers;
	char *ahdr = NULL;
#if PHP_SIGCHILD
	void (*sig_handler)() = NULL;
#endif

#define MAIL_RET(val) \
	if (ahdr != NULL) {	\
		efree(ahdr);	\
	}	\
	return val;	\

	if (mail_log && *mail_log) {
		char *logline;

		spprintf(&logline, 0, "mail() on [%s:%d]: To: %s -- Headers: %s -- Subject: %s", zend_get_executed_filename(), zend_get_executed_lineno(), to, hdr ? hdr : "", subject);

		if (hdr) {
			php_mail_log_crlf_to_spaces(logline);
		}

		if (!strcmp(mail_log, "syslog")) {
			php_mail_log_to_syslog(logline);
		} else {
			/* Add date when logging to file */
			char *tmp;
			time_t curtime;
			zend_string *date_str;
			size_t len;


			time(&curtime);
			date_str = php_format_date("d-M-Y H:i:s e", 13, curtime, 1);
			len = spprintf(&tmp, 0, "[%s] %s%s", date_str->val, logline, PHP_EOL);

			php_mail_log_to_file(mail_log, tmp, len);

			zend_string_free(date_str);
			efree(tmp);
		}

		efree(logline);
	}

	if (EG(exception)) {
		MAIL_RET(0);
	}

	char *line_sep = PG(mail_mixed_lf_and_crlf) ? "\n" : "\r\n";

	if (PG(mail_x_header)) {
		const char *tmp = zend_get_executed_filename();
		zend_string *f;

		f = php_basename(tmp, strlen(tmp), NULL, 0);

		if (headers != NULL && *headers) {
			spprintf(&ahdr, 0, "X-PHP-Originating-Script: " ZEND_LONG_FMT ":%s%s%s", php_getuid(), ZSTR_VAL(f), line_sep, headers);
		} else {
			spprintf(&ahdr, 0, "X-PHP-Originating-Script: " ZEND_LONG_FMT ":%s", php_getuid(), ZSTR_VAL(f));
		}
		hdr = ahdr;
		zend_string_release_ex(f, 0);
	}

	if (hdr && php_mail_detect_multiple_crlf(hdr)) {
		php_error_docref(NULL, E_WARNING, "Multiple or malformed newlines found in additional_header");
		MAIL_RET(0);
	}

	if (!sendmail_path) {
#ifdef PHP_WIN32
		/* handle old style win smtp sending */
		if (TSendMail(INI_STR("SMTP"), &tsm_err, &tsm_errmsg, hdr, subject, to, message, NULL, NULL, NULL) == FAILURE) {
			if (tsm_errmsg) {
				php_error_docref(NULL, E_WARNING, "%s", tsm_errmsg);
				efree(tsm_errmsg);
			} else {
				php_error_docref(NULL, E_WARNING, "%s", GetSMErrorText(tsm_err));
			}
			MAIL_RET(0);
		}
		MAIL_RET(1);
#else
		MAIL_RET(0);
#endif
	}
	if (extra_cmd != NULL) {
		spprintf(&sendmail_cmd, 0, "%s %s", sendmail_path, extra_cmd);
	} else {
		sendmail_cmd = sendmail_path;
	}

#if PHP_SIGCHILD
	/* Set signal handler of SIGCHLD to default to prevent other signal handlers
	 * from being called and reaping the return code when our child exits.
	 * The original handler needs to be restored after pclose() */
	sig_handler = (void *)signal(SIGCHLD, SIG_DFL);
	if (sig_handler == SIG_ERR) {
		sig_handler = NULL;
	}
#endif

#ifdef PHP_WIN32
	sendmail = popen_ex(sendmail_cmd, "wb", NULL, NULL);
#else
	/* Since popen() doesn't indicate if the internal fork() doesn't work
	 * (e.g. the shell can't be executed) we explicitly set it to 0 to be
	 * sure we don't catch any older errno value. */
	errno = 0;
	sendmail = popen(sendmail_cmd, "w");
#endif
	if (extra_cmd != NULL) {
		efree (sendmail_cmd);
	}

	if (sendmail) {
#ifndef PHP_WIN32
		if (EACCES == errno) {
			php_error_docref(NULL, E_WARNING, "Permission denied: unable to execute shell to run mail delivery binary '%s'", sendmail_path);
			pclose(sendmail);
#if PHP_SIGCHILD
			/* Restore handler in case of error on Windows
			   Not sure if this applicable on Win but just in case. */
			if (sig_handler) {
				signal(SIGCHLD, sig_handler);
			}
#endif
			MAIL_RET(0);
		}
#endif
		fprintf(sendmail, "To: %s%s", to, line_sep);
		fprintf(sendmail, "Subject: %s%s", subject, line_sep);
		if (hdr != NULL) {
			fprintf(sendmail, "%s%s", hdr, line_sep);
		}
		fprintf(sendmail, "%s%s%s", line_sep, message, line_sep);
		ret = pclose(sendmail);

#if PHP_SIGCHILD
		if (sig_handler) {
			signal(SIGCHLD, sig_handler);
		}
#endif

#ifdef PHP_WIN32
		if (ret == -1)
#else
#if defined(EX_TEMPFAIL)
		if ((ret != EX_OK)&&(ret != EX_TEMPFAIL))
#elif defined(EX_OK)
		if (ret != EX_OK)
#else
		if (ret != 0)
#endif
#endif
		{
			MAIL_RET(0);
		} else {
			MAIL_RET(1);
		}
	} else {
		php_error_docref(NULL, E_WARNING, "Could not execute mail delivery program '%s'", sendmail_path);
#if PHP_SIGCHILD
		if (sig_handler) {
			signal(SIGCHLD, sig_handler);
		}
#endif
		MAIL_RET(0);
	}

	MAIL_RET(1); /* never reached */
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(mail)
{
	char *sendmail_path = INI_STR("sendmail_path");

#ifdef PHP_WIN32
	if (!sendmail_path) {
		php_info_print_table_row(2, "Internal Sendmail Support for Windows", "enabled");
	} else {
		php_info_print_table_row(2, "Path to sendmail", sendmail_path);
	}
#else
	php_info_print_table_row(2, "Path to sendmail", sendmail_path);
#endif
}
/* }}} */
