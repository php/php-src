/*
 *    PHP Sendmail for Windows.
 *
 *  This file is rewriten specificly for PHPFI.  Some functionality
 *  has been removed (MIME and file attachments).  This code was
 *  modified from code based on code written by Jarle Aase.
 *
 *  This class is based on the original code by Jarle Aase, see bellow:
 *  wSendmail.cpp  It has been striped of some functionality to match
 *  the requirements of phpfi.
 *
 *  Very simple SMTP Send-mail program for sending command-line level
 *  emails and CGI-BIN form response for the Windows platform.
 *
 *  The complete wSendmail package with source code can be located
 *  from http://www.jgaa.com
 *
 */

/* $Id$ */

#include "php.h"				/*php specific */
#include <stdio.h>
#include <stdlib.h>
#ifndef NETWARE
#include <winsock2.h>
#include "time.h"
# include <Ws2tcpip.h>
#else	/* NETWARE */
#include <netware/sendmail_nw.h>
#endif	/* NETWARE */
#include <string.h>
#include <math.h>
#ifndef NETWARE
#include <malloc.h>
#include <memory.h>
#include <winbase.h>
#endif	/* NETWARE */
#include "sendmail.h"
#include "php_ini.h"
#include "inet.h"

#include "php_win32_globals.h"

#if HAVE_PCRE || HAVE_BUNDLED_PCRE
#include "ext/pcre/php_pcre.h"
#endif

#include "ext/standard/php_string.h"
#include "ext/date/php_date.h"

/*enum
   {
   DO_CONNECT = WM_USER +1
   };
 */

/* '*error_message' has to be passed around from php_mail() */
#define SMTP_ERROR_RESPONSE_SPEC	"SMTP server response: %s"
/* Convinient way to handle error messages from the SMTP server.
   response is ecalloc()d in Ack() itself and efree()d here
   because the content is in *error_message now */
#define SMTP_ERROR_RESPONSE(response)	{ \
											if (response && error_message) { \
												if (NULL != (*error_message = ecalloc(1, sizeof(SMTP_ERROR_RESPONSE_SPEC) + strlen(response)))) { \
													snprintf(*error_message, sizeof(SMTP_ERROR_RESPONSE_SPEC) + strlen(response), SMTP_ERROR_RESPONSE_SPEC, response); \
												} \
												efree(response); \
											} \
										}
#define SMTP_SKIP_SPACE(str)	{ while (isspace(*str)) { str++; } }


char seps[] = " ,\t\n";
#ifndef NETWARE
char *php_mailer = "PHP 7 WIN32";
#else
char *php_mailer = "PHP 7 NetWare";
#endif	/* NETWARE */

/* Error messages */
static char *ErrorMessages[] =
{
	{"Success"}, /* 0 */
	{"Bad arguments from form"}, /* 1 */
	{"Unable to open temporary mailfile for read"},
	{"Failed to Start Sockets"},
	{"Failed to Resolve Host"},
	{"Failed to obtain socket handle"}, /* 5 */
	{"Failed to connect to mailserver, verify your \"SMTP\" setting in php.ini"},
	{"Failed to Send"},
	{"Failed to Receive"},
	{"Server Error"},
	{"Failed to resolve the host IP name"}, /* 10 */
	{"Out of memory"},
	{"Unknown error"},
	{"Bad Message Contents"},
	{"Bad Message Subject"},
	{"Bad Message destination"}, /* 15 */
	{"Bad Message Return Path"},
	{"Bad Mail Host"},
	{"Bad Message File"},
	{"\"sendmail_from\" not set in php.ini or custom \"From:\" header missing"},
	{"Mailserver rejected our \"sendmail_from\" setting"}, /* 20 */
	{"Error while trimming mail header with PCRE, please file a bug report at http://bugs.php.net/"} /* 21 */
};

/* This pattern converts all single occurrences of \n (Unix)
 * withour a leading \r to \r\n and all occurrences of \r (Mac)
 * without a trailing \n to \r\n
 * Thx to Nibbler from ircnet/#linuxger
 */
#define PHP_WIN32_MAIL_UNIFY_PATTERN	"/(\r\n?)|\n/"
#define PHP_WIN32_MAIL_UNIFY_REPLACE	"\r\n"

/* This pattern removes \r\n from the start of the string,
 * \r\n from the end of the string and also makes sure every line
 * is only wrapped with a single \r\n (thus reduces multiple
 * occurrences of \r\n between lines to a single \r\n) */
#define PHP_WIN32_MAIL_RMVDBL_PATTERN	"/^\r\n|(\r\n)+$/m"
#define PHP_WIN32_MAIL_RMVDBL_REPLACE	""

/* This pattern escapes \n. inside the message body. It prevents
 * premature end of message if \n.\n or \r\n.\r\n is encountered
 * and ensures that \n. sequences are properly displayed in the
 * message body. */
#define PHP_WIN32_MAIL_DOT_PATTERN	"\n."
#define PHP_WIN32_MAIL_DOT_REPLACE	"\n.."

/* This function is meant to unify the headers passed to to mail()
 * This means, use PCRE to transform single occurrences of \n or \r in \r\n
 * As a second step we also eleminate all \r\n occurrences which are:
 * 1) At the start of the header
 * 2) At the end of the header
 * 3) Two or more occurrences in the header are removed so only one is left
 *
 * Returns NULL on error, or the new char* buffer on success.
 * You have to take care and efree() the buffer on your own.
 */
static zend_string *php_win32_mail_trim_header(char *header)
{

#if HAVE_PCRE || HAVE_BUNDLED_PCRE

	zend_string *result, *result2;
	zval replace;
	zend_string *regex;

	if (!header) {
		return NULL;
	}

	ZVAL_STRINGL(&replace, PHP_WIN32_MAIL_UNIFY_REPLACE, strlen(PHP_WIN32_MAIL_UNIFY_REPLACE));
	regex = zend_string_init(PHP_WIN32_MAIL_UNIFY_PATTERN, sizeof(PHP_WIN32_MAIL_UNIFY_PATTERN)-1, 0);

	result = php_pcre_replace(regex,
				  NULL, header, (int)strlen(header),
				  &replace,
				  0,
				  -1,
				  NULL);

	zval_ptr_dtor(&replace);
	zend_string_release(regex);

	if (NULL == result) {
		return NULL;
	}

	ZVAL_STRING(&replace, PHP_WIN32_MAIL_RMVDBL_PATTERN);
	regex = zend_string_init(PHP_WIN32_MAIL_RMVDBL_PATTERN, sizeof(PHP_WIN32_MAIL_RMVDBL_PATTERN)-1, 0);

	result2 = php_pcre_replace(regex,
				   result, result->val, (int)result->len,
				   &replace,
				  0,
				  -1,
				  NULL);
	zval_ptr_dtor(&replace);
	zend_string_release(regex);
	zend_string_release(result);

	return result2;
#else
	/* In case we don't have PCRE support (for whatever reason...) simply do nothing and return the unmodified header */
	return estrdup(header);
#endif
}

/*********************************************************************
// Name:  TSendMail
// Input:   1) host:    Name of the mail host where the SMTP server resides
//                      max accepted length of name = 256
//          2) appname: Name of the application to use in the X-mailer
//                      field of the message. if NULL is given the application
//                      name is used as given by the GetCommandLine() function
//                      max accespted length of name = 100
// Output:  1) error:   Returns the error code if something went wrong or
//                      SUCCESS otherwise.
//
//  See SendText() for additional args!
//********************************************************************/
PHPAPI int TSendMail(char *host, int *error, char **error_message,
			  char *headers, char *Subject, char *mailTo, char *data,
			  char *mailCc, char *mailBcc, char *mailRPath)
{
	int ret;
	char *RPath = NULL;
	zend_string *headers_lc = NULL; /* headers_lc is only created if we've a header at all */
	char *pos1 = NULL, *pos2 = NULL;

	if (host == NULL) {
		*error = BAD_MAIL_HOST;
		return FAILURE;
	} else if (strlen(host) >= HOST_NAME_LEN) {
		*error = BAD_MAIL_HOST;
		return FAILURE;
	} else {
		strcpy(PW32G(mail_host), host);
	}

	if (headers) {
		char *pos = NULL;
		size_t i;

		/* Use PCRE to trim the header into the right format */
		if (NULL == (headers_lc = php_win32_mail_trim_header(headers))) {
			*error = W32_SM_PCRE_ERROR;
			return FAILURE;
		}

		/* Create a lowercased header for all the searches so we're finally case
		 * insensitive when searching for a pattern. */
		for (i = 0; i < headers_lc->len; i++) {
			headers_lc->val[i] = tolower(headers_lc->val[i]);
		}
	}

	/* Fall back to sendmail_from php.ini setting */
	if (mailRPath && *mailRPath) {
		RPath = estrdup(mailRPath);
	} else if (INI_STR("sendmail_from")) {
		RPath = estrdup(INI_STR("sendmail_from"));
	} else if (headers_lc) {
		int found = 0;
		char *lookup = headers_lc->val;

		while (lookup) {
			pos1 = strstr(lookup, "from:");

			if (!pos1) {
				break;
			} else if (pos1 != headers_lc->val && *(pos1-1) != '\n') {
				if (strlen(pos1) >= sizeof("from:")) {
					lookup = pos1 + sizeof("from:");
					continue;
				} else {
					break;
				}
			}

			found = 1;

			/* Real offset is memaddress from the original headers + difference of
			 * string found in the lowercase headrs + 5 characters to jump over
			 * the from: */
			pos1 = headers + (pos1 - lookup) + 5;
			if (NULL == (pos2 = strstr(pos1, "\r\n"))) {
				RPath = estrndup(pos1, strlen(pos1));
			} else {
				RPath = estrndup(pos1, pos2 - pos1);
			}

			break;
		}

		if (!found) {
			if (headers_lc) {
				zend_string_free(headers_lc);
			}
			*error = W32_SM_SENDMAIL_FROM_NOT_SET;
			return FAILURE;
		}
	}

	/* attempt to connect with mail host */
	*error = MailConnect();
	if (*error != 0) {
		if (RPath) {
			efree(RPath);
		}
		if (headers) {
			zend_string_free(headers_lc);
		}
		/* 128 is safe here, the specifier in snprintf isn't longer than that */
		if (NULL == (*error_message = ecalloc(1, HOST_NAME_LEN + 128))) {
			return FAILURE;
		}
		snprintf(*error_message, HOST_NAME_LEN + 128,
			"Failed to connect to mailserver at \"%s\" port %d, verify your \"SMTP\" "
			"and \"smtp_port\" setting in php.ini or use ini_set()",
			PW32G(mail_host), !INI_INT("smtp_port") ? 25 : INI_INT("smtp_port"));
		return FAILURE;
	} else {
		ret = SendText(RPath, Subject, mailTo, mailCc, mailBcc, data, headers, headers_lc->val, error_message);
		TSMClose();
		if (RPath) {
			efree(RPath);
		}
		if (headers) {
			zend_string_free(headers_lc);
		}
		if (ret != SUCCESS) {
			*error = ret;
			return FAILURE;
		}
		return SUCCESS;
	}
}

//********************************************************************
// Name:  TSendMail::~TSendMail
// Input:
// Output:
// Description: DESTRUCTOR
// Author/Date:  jcar 20/9/96
// History:
//********************************************************************/
PHPAPI void TSMClose()
{
	Post("QUIT\r\n");
	Ack(NULL);
	/* to guarantee that the cleanup is not made twice and
	   compomise the rest of the application if sockets are used
	   elesewhere
	*/

	shutdown(PW32G(mail_socket), 0);
	closesocket(PW32G(mail_socket));
}


/*********************************************************************
// Name:  char *GetSMErrorText
// Input:   Error index returned by the menber functions
// Output:  pointer to a string containing the error description
// Description:
// Author/Date:  jcar 20/9/96
// History:
//*******************************************************************/
PHPAPI char *GetSMErrorText(int index)
{
	if (MIN_ERROR_INDEX <= index && index < MAX_ERROR_INDEX) {
		return (ErrorMessages[index]);

	} else {
		return (ErrorMessages[UNKNOWN_ERROR]);

	}
}

PHPAPI zend_string *php_str_to_str(char *haystack, size_t length, char *needle,
		size_t needle_len, char *str, size_t str_len);


/*********************************************************************
// Name:  SendText
// Input:       1) RPath:   return path of the message
//                                  Is used to fill the "Return-Path" and the
//                                  "X-Sender" fields of the message.
//                  2) Subject: Subject field of the message. If NULL is given
//                                  the subject is set to "No Subject"
//                  3) mailTo:  Destination address
//                  4) data:        Null terminated string containing the data to be send.
//                  5,6) headers of the message. Note that the second
//                  parameter, headers_lc, is actually a lowercased version of
//                  headers. The should match exactly (in terms of length),
//                  only differ in case
// Output:      Error code or SUCCESS
// Description:
// Author/Date:  jcar 20/9/96
// History:
//*******************************************************************/
static int SendText(char *RPath, char *Subject, char *mailTo, char *mailCc, char *mailBcc, char *data,
			 char *headers, char *headers_lc, char **error_message)
{
	int res;
	char *p;
	char *tempMailTo, *token, *pos1, *pos2;
	char *server_response = NULL;
	char *stripped_header  = NULL;
	zend_string *data_cln;

	/* check for NULL parameters */
	if (data == NULL)
		return (BAD_MSG_CONTENTS);
	if (mailTo == NULL)
		return (BAD_MSG_DESTINATION);
	if (RPath == NULL)
		return (BAD_MSG_RPATH);

	/* simple checks for the mailto address */
	/* have ampersand ? */
	/* mfischer, 20020514: I commented this out because it really
	   seems bogus. Only a username for example may still be a
	   valid address at the destination system.
	if (strchr(mailTo, '@') == NULL)
		return (BAD_MSG_DESTINATION);
	*/

	snprintf(PW32G(mail_buffer), sizeof(PW32G(mail_buffer)), "HELO %s\r\n", PW32G(mail_local_host));

	/* in the beginning of the dialog */
	/* attempt reconnect if the first Post fail */
	if ((res = Post(PW32G(mail_buffer))) != SUCCESS) {
		int err = MailConnect();
		if (0 != err) {
			return (FAILED_TO_SEND);
		}

		if ((res = Post(PW32G(mail_buffer))) != SUCCESS) {
			return (res);
		}
	}
	if ((res = Ack(&server_response)) != SUCCESS) {
		SMTP_ERROR_RESPONSE(server_response);
		return (res);
	}

	SMTP_SKIP_SPACE(RPath);
	FormatEmailAddress(PW32G(mail_buffer), RPath, "MAIL FROM:<%s>\r\n");
	if ((res = Post(PW32G(mail_buffer))) != SUCCESS) {
		return (res);
	}
	if ((res = Ack(&server_response)) != SUCCESS) {
		SMTP_ERROR_RESPONSE(server_response);
		return W32_SM_SENDMAIL_FROM_MALFORMED;
	}

	tempMailTo = estrdup(mailTo);
	/* Send mail to all rcpt's */
	token = strtok(tempMailTo, ",");
	while (token != NULL)
	{
		SMTP_SKIP_SPACE(token);
		FormatEmailAddress(PW32G(mail_buffer), token, "RCPT TO:<%s>\r\n");
		if ((res = Post(PW32G(mail_buffer))) != SUCCESS) {
			efree(tempMailTo);
			return (res);
		}
		if ((res = Ack(&server_response)) != SUCCESS) {
			SMTP_ERROR_RESPONSE(server_response);
			efree(tempMailTo);
			return (res);
		}
		token = strtok(NULL, ",");
	}
	efree(tempMailTo);

	if (mailCc && *mailCc) {
		tempMailTo = estrdup(mailCc);
		/* Send mail to all rcpt's */
		token = strtok(tempMailTo, ",");
		while (token != NULL)
		{
			SMTP_SKIP_SPACE(token);
			FormatEmailAddress(PW32G(mail_buffer), token, "RCPT TO:<%s>\r\n");
			if ((res = Post(PW32G(mail_buffer))) != SUCCESS) {
				efree(tempMailTo);
				return (res);
			}
			if ((res = Ack(&server_response)) != SUCCESS) {
				SMTP_ERROR_RESPONSE(server_response);
				efree(tempMailTo);
				return (res);
			}
			token = strtok(NULL, ",");
		}
		efree(tempMailTo);
	}
	/* Send mail to all Cc rcpt's */
	else if (headers && (pos1 = strstr(headers_lc, "cc:")) && ((pos1 == headers_lc) || (*(pos1-1) == '\n'))) {
		/* Real offset is memaddress from the original headers + difference of
		 * string found in the lowercase headrs + 3 characters to jump over
		 * the cc: */
		pos1 = headers + (pos1 - headers_lc) + 3;
		if (NULL == (pos2 = strstr(pos1, "\r\n"))) {
			tempMailTo = estrndup(pos1, strlen(pos1));
		} else {
			tempMailTo = estrndup(pos1, pos2 - pos1);
		}

		token = strtok(tempMailTo, ",");
		while (token != NULL)
		{
			SMTP_SKIP_SPACE(token);
			FormatEmailAddress(PW32G(mail_buffer), token, "RCPT TO:<%s>\r\n");
			if ((res = Post(PW32G(mail_buffer))) != SUCCESS) {
				efree(tempMailTo);
				return (res);
			}
			if ((res = Ack(&server_response)) != SUCCESS) {
				SMTP_ERROR_RESPONSE(server_response);
				efree(tempMailTo);
				return (res);
			}
			token = strtok(NULL, ",");
		}
		efree(tempMailTo);
	}

	/* Send mail to all Bcc rcpt's
	   This is basically a rip of the Cc code above.
	   Just don't forget to remove the Bcc: from the header afterwards. */
	if (mailBcc && *mailBcc) {
		tempMailTo = estrdup(mailBcc);
		/* Send mail to all rcpt's */
		token = strtok(tempMailTo, ",");
		while (token != NULL)
		{
			SMTP_SKIP_SPACE(token);
			FormatEmailAddress(PW32G(mail_buffer), token, "RCPT TO:<%s>\r\n");
			if ((res = Post(PW32G(mail_buffer))) != SUCCESS) {
				efree(tempMailTo);
				return (res);
			}
			if ((res = Ack(&server_response)) != SUCCESS) {
				SMTP_ERROR_RESPONSE(server_response);
				efree(tempMailTo);
				return (res);
			}
			token = strtok(NULL, ",");
		}
		efree(tempMailTo);
	}
	else if (headers) {
		if (pos1 = strstr(headers_lc, "bcc:")) {
			/* Real offset is memaddress from the original headers + difference of
			 * string found in the lowercase headrs + 4 characters to jump over
			 * the bcc: */
			pos1 = headers + (pos1 - headers_lc) + 4;
			if (NULL == (pos2 = strstr(pos1, "\r\n"))) {
				tempMailTo = estrndup(pos1, strlen(pos1));
				/* Later, when we remove the Bcc: out of the
				   header we know it was the last thing. */
				pos2 = pos1;
			} else {
				tempMailTo = estrndup(pos1, pos2 - pos1);
			}

			token = strtok(tempMailTo, ",");
			while (token != NULL)
			{
				SMTP_SKIP_SPACE(token);
				FormatEmailAddress(PW32G(mail_buffer), token, "RCPT TO:<%s>\r\n");
				if ((res = Post(PW32G(mail_buffer))) != SUCCESS) {
					efree(tempMailTo);
					return (res);
				}
				if ((res = Ack(&server_response)) != SUCCESS) {
					SMTP_ERROR_RESPONSE(server_response);
					efree(tempMailTo);
					return (res);
				}
				token = strtok(NULL, ",");
			}
			efree(tempMailTo);

			/* Now that we've identified that we've a Bcc list,
			   remove it from the current header. */
			if (NULL == (stripped_header = ecalloc(1, strlen(headers)))) {
				return OUT_OF_MEMORY;
			}
			/* headers = point to string start of header
			   pos1    = pointer IN headers where the Bcc starts
			   '4'     = Length of the characters 'bcc:'
			   Because we've added +4 above for parsing the Emails
			   we've to subtract them here. */
			memcpy(stripped_header, headers, pos1 - headers - 4);
			if (pos1 != pos2) {
				/* if pos1 != pos2 , pos2 points to the rest of the headers.
				   Since pos1 != pos2 if "\r\n" was found, we know those characters
				   are there and so we jump over them (else we would generate a new header
				   which would look like "\r\n\r\n". */
				memcpy(stripped_header + (pos1 - headers - 4), pos2 + 2, strlen(pos2) - 2);
			}
		}
	}

	/* Simplify the code that we create a copy of stripped_header no matter if
	   we actually strip something or not. So we've a single efree() later. */
	if (headers && !stripped_header) {
		if (NULL == (stripped_header = estrndup(headers, strlen(headers)))) {
			return OUT_OF_MEMORY;
		}
	}

	if ((res = Post("DATA\r\n")) != SUCCESS) {
		if (stripped_header) {
			efree(stripped_header);
		}
		return (res);
	}
	if ((res = Ack(&server_response)) != SUCCESS) {
		SMTP_ERROR_RESPONSE(server_response);
		if (stripped_header) {
			efree(stripped_header);
		}
		return (res);
	}

	/* send message header */
	if (Subject == NULL) {
		res = PostHeader(RPath, "No Subject", mailTo, stripped_header);
	} else {
		res = PostHeader(RPath, Subject, mailTo, stripped_header);
	}
	if (stripped_header) {
		efree(stripped_header);
	}
	if (res != SUCCESS) {
		return (res);
	}

	/* Escape \n. sequences
	 * We use php_str_to_str() and not php_str_replace_in_subject(), since the latter
	 * uses ZVAL as it's parameters */
	data_cln = php_str_to_str(data, strlen(data), PHP_WIN32_MAIL_DOT_PATTERN, sizeof(PHP_WIN32_MAIL_DOT_PATTERN) - 1,
					PHP_WIN32_MAIL_DOT_REPLACE, sizeof(PHP_WIN32_MAIL_DOT_REPLACE) - 1);
	if (!data_cln) {
		data_cln = ZSTR_EMPTY_ALLOC();
	}

	/* send message contents in 1024 chunks */
	{
		char c, *e2, *e = data_cln->val + data_cln->len;
		p = data_cln->val;

		while (e - p > 1024) {
			e2 = p + 1024;
			c = *e2;
			*e2 = '\0';
			if ((res = Post(p)) != SUCCESS) {
				zend_string_free(data_cln);
				return(res);
			}
			*e2 = c;
			p = e2;
		}
		if ((res = Post(p)) != SUCCESS) {
			zend_string_free(data_cln);
			return(res);
		}
	}

	zend_string_free(data_cln);

	/*send termination dot */
	if ((res = Post("\r\n.\r\n")) != SUCCESS)
		return (res);
	if ((res = Ack(&server_response)) != SUCCESS) {
		SMTP_ERROR_RESPONSE(server_response);
		return (res);
	}

	return (SUCCESS);
}

static int addToHeader(char **header_buffer, const char *specifier, char *string)
{
	if (NULL == (*header_buffer = erealloc(*header_buffer, strlen(*header_buffer) + strlen(specifier) + strlen(string) + 1))) {
		return 0;
	}
	sprintf(*header_buffer + strlen(*header_buffer), specifier, string);
	return 1;
}

/*********************************************************************
// Name:  PostHeader
// Input:       1) return path
//              2) Subject
//              3) destination address
//              4) headers
// Output:      Error code or Success
// Description:
// Author/Date:  jcar 20/9/96
// History:
//********************************************************************/
static int PostHeader(char *RPath, char *Subject, char *mailTo, char *xheaders)
{
	/* Print message header according to RFC 822 */
	/* Return-path, Received, Date, From, Subject, Sender, To, cc */

	int res;
	char *header_buffer;
	char *headers_lc = NULL;
	size_t i;

	if (xheaders) {
		if (NULL == (headers_lc = estrdup(xheaders))) {
			return OUT_OF_MEMORY;
		}
		for (i = 0; i < strlen(headers_lc); i++) {
			headers_lc[i] = tolower(headers_lc[i]);
		}
	}

	header_buffer = ecalloc(1, MAIL_BUFFER_SIZE);

	if (!xheaders || !strstr(headers_lc, "date:")) {
		time_t tNow = time(NULL);
		zend_string *dt = php_format_date("r", 1, tNow, 1);

		snprintf(header_buffer, MAIL_BUFFER_SIZE, "Date: %s\r\n", dt->val);
		zend_string_free(dt);
	}

	if (!headers_lc || !strstr(headers_lc, "from:")) {
		if (!addToHeader(&header_buffer, "From: %s\r\n", RPath)) {
			goto PostHeader_outofmem;
		}
	}
	if (!addToHeader(&header_buffer, "Subject: %s\r\n", Subject)) {
		goto PostHeader_outofmem;
	}

	/* Only add the To: field from the $to parameter if isn't in the custom headers */
	if ((headers_lc && (!strstr(headers_lc, "\r\nto:") && (strncmp(headers_lc, "to:", 3) != 0))) || !headers_lc) {
		if (!addToHeader(&header_buffer, "To: %s\r\n", mailTo)) {
			goto PostHeader_outofmem;
		}
	}
	if (xheaders) {
		if (!addToHeader(&header_buffer, "%s\r\n", xheaders)) {
			goto PostHeader_outofmem;
		}
	}

	if (headers_lc) {
		efree(headers_lc);
	}
	if ((res = Post(header_buffer)) != SUCCESS) {
		efree(header_buffer);
		return (res);
	}
	efree(header_buffer);

	if ((res = Post("\r\n")) != SUCCESS) {
		return (res);
	}

	return (SUCCESS);

PostHeader_outofmem:
	if (headers_lc) {
		efree(headers_lc);
	}
	return OUT_OF_MEMORY;
}



/*********************************************************************
// Name:  MailConnect
// Input:   None
// Output:  None
// Description: Connect to the mail host and receive the welcome message.
// Author/Date:  jcar 20/9/96
// History:
//********************************************************************/
static int MailConnect()
{

	int res, namelen;
	short portnum;
	struct hostent *ent;
	IN_ADDR addr;
#ifdef HAVE_IPV6
	IN6_ADDR addr6;
#endif
	SOCKADDR_IN sock_in;

	/* Create Socket */
	if ((PW32G(mail_socket) = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		return (FAILED_TO_OBTAIN_SOCKET_HANDLE);
	}

	/* Get our own host name */
	if (gethostname(PW32G(mail_local_host), HOST_NAME_LEN)) {
		closesocket(PW32G(mail_socket));
		return (FAILED_TO_GET_HOSTNAME);
	}

	ent = gethostbyname(PW32G(mail_local_host));

	if (!ent) {
		closesocket(PW32G(mail_socket));
		return (FAILED_TO_GET_HOSTNAME);
	}

	namelen = (int)strlen(ent->h_name);

#ifdef HAVE_IPV6
	if (inet_pton(AF_INET, ent->h_name, &addr) == 1 || inet_pton(AF_INET6, ent->h_name, &addr6) == 1)
#else
	if (inet_pton(AF_INET, ent->h_name, &addr) == 1)
#endif
	{
		if (namelen + 2 >= HOST_NAME_LEN) {
			closesocket(PW32G(mail_socket));
			return (FAILED_TO_GET_HOSTNAME);
		}

		strcpy(PW32G(mail_local_host), "[");
		strcpy(PW32G(mail_local_host) + 1, ent->h_name);
		strcpy(PW32G(mail_local_host) + namelen + 1, "]");
	} else {
		if (namelen >= HOST_NAME_LEN) {
			closesocket(PW32G(mail_socket));
			return (FAILED_TO_GET_HOSTNAME);
		}

		strcpy(PW32G(mail_local_host), ent->h_name);
	}

	/* Resolve the servers IP */
	/*
	if (!isdigit(PW32G(mail_host)[0])||!gethostbyname(PW32G(mail_host)))
	{
		return (FAILED_TO_RESOLVE_HOST);
	}
	*/

	portnum = (short) INI_INT("smtp_port");
	if (!portnum) {
		portnum = 25;
	}

	/* Connect to server */
	sock_in.sin_family = AF_INET;
	sock_in.sin_port = htons(portnum);
	sock_in.sin_addr.S_un.S_addr = GetAddr(PW32G(mail_host));

	if (connect(PW32G(mail_socket), (LPSOCKADDR) & sock_in, sizeof(sock_in))) {
		closesocket(PW32G(mail_socket));
		return (FAILED_TO_CONNECT);
	}

	/* receive Server welcome message */
	res = Ack(NULL);
	return (res);
}


/*********************************************************************
// Name:  Post
// Input:
// Output:
// Description:
// Author/Date:  jcar 20/9/96
// History:
//********************************************************************/
static int Post(LPCSTR msg)
{
	int len = (int)strlen(msg);
	int slen;
	int index = 0;

	while (len > 0) {
		if ((slen = send(PW32G(mail_socket), msg + index, len, 0)) < 1)
			return (FAILED_TO_SEND);
		len -= slen;
		index += slen;
	}
	return (SUCCESS);
}



/*********************************************************************
// Name:  Ack
// Input:
// Output:
// Description:
// Get the response from the server. We only want to know if the
// last command was successful.
// Author/Date:  jcar 20/9/96
// History:
//********************************************************************/
static int Ack(char **server_response)
{
	ZEND_TLS char buf[MAIL_BUFFER_SIZE];
	int rlen;
	int Index = 0;
	int Received = 0;

again:

	if ((rlen = recv(PW32G(mail_socket), buf + Index, ((MAIL_BUFFER_SIZE) - 1) - Received, 0)) < 1) {
		return (FAILED_TO_RECEIVE);
	}
	Received += rlen;
	buf[Received] = 0;
	/*err_msg   fprintf(stderr,"Received: (%d bytes) %s", rlen, buf + Index); */

	/* Check for newline */
	Index += rlen;

	/* SMPT RFC says \r\n is the only valid line ending, who are we to argue ;)
	 * The response code must contain at least 5 characters ex. 220\r\n */
	if (Received < 5 || buf[Received - 1] != '\n' || buf[Received - 2] != '\r') {
		goto again;
	}

	if (buf[0] > '3') {
		/* If we've a valid pointer, return the SMTP server response so the error message contains more information */
		if (server_response) {
			int dec = 0;
			/* See if we have something like \r, \n, \r\n or \n\r at the end of the message and chop it off */
			if (Received > 2) {
				if (buf[Received-1] == '\n' || buf[Received-1] == '\r') {
					dec++;
					if (buf[Received-2] == '\r' || buf[Received-2] == '\n') {
						dec++;
					}
				}

			}
			*server_response = estrndup(buf, Received - dec);
		}
		return (SMTP_SERVER_ERROR);
	}

	return (SUCCESS);
}


/*********************************************************************
// Name:  unsigned long GetAddr (LPSTR szHost)
// Input:
// Output:
// Description: Given a string, it will return an IP address.
//   - first it tries to convert the string directly
//   - if that fails, it tries o resolve it as a hostname
//
// WARNING: gethostbyname() is a blocking function
// Author/Date:  jcar 20/9/96
// History:
//********************************************************************/
static unsigned long GetAddr(LPSTR szHost)
{
	LPHOSTENT lpstHost;
	u_long lAddr = INADDR_ANY;

	/* check that we have a string */
	if (*szHost) {

		/* check for a dotted-IP address string */
		lAddr = inet_addr(szHost);

		/* If not an address, then try to resolve it as a hostname */
		if ((lAddr == INADDR_NONE) && (strcmp(szHost, "255.255.255.255"))) {

			lpstHost = gethostbyname(szHost);
			if (lpstHost) {		/* success */
				lAddr = *((u_long FAR *) (lpstHost->h_addr));
			} else {
				lAddr = INADDR_ANY;		/* failure */
			}
		}
	}
	return (lAddr);
} /* end GetAddr() */


/*********************************************************************
// Name:  int FormatEmailAddress
// Input:
// Output:
// Description: Formats the email address to remove any content ouside
//   of the angle brackets < > as per RFC 2821.
//
//   Returns the invalidly formatted mail address if the < > are
//   unbalanced (the SMTP server should reject it if it's out of spec.)
//
// Author/Date:  garretts 08/18/2009
// History:
//********************************************************************/
static int FormatEmailAddress(char* Buf, char* EmailAddress, char* FormatString) {
	char *tmpAddress1, *tmpAddress2;
	int result;

	if( (tmpAddress1 = strchr(EmailAddress, '<')) && (tmpAddress2 = strchr(tmpAddress1, '>'))  ) {
		*tmpAddress2 = 0; // terminate the string temporarily.
		result = snprintf(Buf, MAIL_BUFFER_SIZE, FormatString , tmpAddress1+1);
		*tmpAddress2 = '>'; // put it back the way it was.
		return result;
	}
	return snprintf(Buf, MAIL_BUFFER_SIZE , FormatString , EmailAddress );
} /* end FormatEmailAddress() */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
