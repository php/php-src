/*
 *    PHP Sendmail for Windows.
 *
 *  This file is rewritten specifically for PHPFI.  Some functionality
 *  has been removed (MIME and file attachments).  This code was
 *  modified from code based on code written by Jarle Aase.
 *
 *  This class is based on the original code by Jarle Aase, see below:
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

#include "php.h"				/*php specific */
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include "time.h"
# include <Ws2tcpip.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <winbase.h>
#include "sendmail.h"
#include "php_ini.h"

#include "php_win32_globals.h"

#include "Zend/zend_smart_str.h"
#include "ext/pcre/php_pcre.h"
#include "ext/standard/php_string.h"
#include "ext/date/php_date.h"

#define SENDMAIL_DEBUG 0

/*enum
   {
   DO_CONNECT = WM_USER +1
   };
 */

/* '*error_message' has to be passed around from php_mail() */
#define SMTP_ERROR_RESPONSE_SPEC	"SMTP server response: %s"
/* Convenient way to handle error messages from the SMTP server.
   response is ecalloc()d in Ack() itself and efree()d here
   because the content is in *error_message now */
#define SMTP_ERROR_RESPONSE(response)	{ \
											if (response && error_message) { \
												*error_message = ecalloc(1, sizeof(SMTP_ERROR_RESPONSE_SPEC) + strlen(response)); \
												snprintf(*error_message, sizeof(SMTP_ERROR_RESPONSE_SPEC) + strlen(response), SMTP_ERROR_RESPONSE_SPEC, response); \
												efree(response); \
											} \
										}
#define SMTP_SKIP_SPACE(str)	{ while (isspace(*str)) { str++; } }


char seps[] = " ,\t\n";
char *php_mailer = "PHP 7 WIN32";

/* Error messages */
static const char *ErrorMessages[] =
{
	"Success", /* 0 */
	"Bad arguments from form", /* 1 */
	"Unable to open temporary mailfile for read",
	"Failed to Start Sockets",
	"Failed to Resolve Host",
	"Failed to obtain socket handle", /* 5 */
	"Failed to connect to mailserver, verify your \"SMTP\" setting in php.ini",
	"Failed to Send",
	"Failed to Receive",
	"Server Error",
	"Failed to resolve the host IP name", /* 10 */
	"Out of memory",
	"Unknown error",
	"Bad Message Contents",
	"Bad Message Subject",
	"Bad Message destination", /* 15 */
	"Bad Message Return Path",
	"Bad Mail Host",
	"Bad Message File",
	"\"sendmail_from\" not set in php.ini or custom \"From:\" header missing",
	"Mailserver rejected our \"sendmail_from\" setting", /* 20 */
	"Error while trimming mail header with PCRE, please file a bug report at https://github.com/php/php-src/issues" /* 21 */
};

/* This pattern converts all single occurrences of \n (Unix)
 * without a leading \r to \r\n and all occurrences of \r (Mac)
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

static int SendText(_In_ const char *host, _In_ const char *RPath, const char *Subject, const char *mailTo, const char *data,
                    zend_string *headers, zend_string *headers_lc, char **error_message);
static int MailConnect(_In_ const char *host);
static int PostHelo(char **error_message);
static bool PostHeader(const char *RPath, const char *Subject, const char *mailTo, zend_string *xheaders);
static bool Post(LPCSTR msg);
static int Ack(char **server_response);
static unsigned long GetAddr(const char *szHost);
static int FormatEmailAddress(char* Buf, const char* EmailAddress, const char* FormatString);

/* This function is meant to unify the headers passed to mail()
 * This means, use PCRE to transform single occurrences of \n or \r in \r\n
 * As a second step we also eliminate all \r\n occurrences which are:
 * 1) At the start of the header
 * 2) At the end of the header
 * 3) Two or more occurrences in the header are removed so only one is left
 *
 * Returns NULL on error, or the new char* buffer on success.
 * You have to take care and efree() the buffer on your own.
 */
static zend_string *php_win32_mail_trim_header(const char *header)
{
	zend_string *result, *result2;
	zend_string *replace;
	zend_string *regex;

	if (!header) {
		return NULL;
	}

	replace = ZSTR_INIT_LITERAL(PHP_WIN32_MAIL_UNIFY_REPLACE, false);
	regex = ZSTR_INIT_LITERAL(PHP_WIN32_MAIL_UNIFY_PATTERN, false);

	result = php_pcre_replace(regex,
				  NULL, header, strlen(header),
				  replace,
				  -1,
				  NULL);

	zend_string_release_ex(replace, false);
	zend_string_release_ex(regex, false);

	if (NULL == result) {
		return NULL;
	}

	replace = ZSTR_INIT_LITERAL(PHP_WIN32_MAIL_RMVDBL_PATTERN, false);
	regex = ZSTR_INIT_LITERAL(PHP_WIN32_MAIL_RMVDBL_PATTERN, false);

	result2 = php_pcre_replace(regex,
				   result, ZSTR_VAL(result), ZSTR_LEN(result),
				   replace,
				  -1,
				  NULL);
	zend_string_release_ex(replace, false);
	zend_string_release_ex(regex, false);
	zend_string_release_ex(result, false);

	return result2;
}

//*********************************************************************
// Name:  TSendMail
// Input:   1) host:    Name of the mail host where the SMTP server resides
//                      max accepted length of name = 256
//          2) appname: Name of the application to use in the X-mailer
//                      field of the message. if NULL is given the application
//                      name is used as given by the GetCommandLine() function
//                      max accepted length of name = 100
// Output:  1) error:   Returns the error code if something went wrong or
//                      SUCCESS otherwise.
//
//  See SendText() for additional args!
//*********************************************************************
PHPAPI int TSendMail(const char *host, int *error, char **error_message,
			  const char *headers, const char *Subject, const char *mailTo, const char *data)
{
	int ret;
	char *RPath = NULL;
	zend_string *headers_lc = NULL, *headers_trim = NULL; /* headers_lc is only created if we've a header at all */
	const char *pos1 = NULL, *pos2 = NULL;

	if (host == NULL) {
		*error = BAD_MAIL_HOST;
		return FAILURE;
	}

	if (headers) {
		/* Use PCRE to trim the header into the right format */
		if (NULL == (headers_trim = php_win32_mail_trim_header(headers))) {
			*error = W32_SM_PCRE_ERROR;
			return FAILURE;
		}

		/* Create a lowercased header for all the searches so we're finally case
		 * insensitive when searching for a pattern. */
		headers_lc = zend_string_tolower(headers_trim);
	}

	/* Fall back to sendmail_from php.ini setting */
	if (INI_STR("sendmail_from")) {
		RPath = estrdup(INI_STR("sendmail_from"));
	} else if (headers_lc) {
		bool found = false;
		const char *lookup = ZSTR_VAL(headers_lc);

		while (lookup) {
			pos1 = strstr(lookup, "from:");

			if (!pos1) {
				break;
			} else if (pos1 != ZSTR_VAL(headers_lc) && *(pos1-1) != '\n') {
				if (strlen(pos1) >= sizeof("from:")) {
					lookup = pos1 + sizeof("from:");
					continue;
				} else {
					break;
				}
			}

			found = true;

			/* Real offset is memaddress from the original headers + difference of
			 * string found in the lowercase headers + 5 characters to jump over
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
			if (headers) {
				zend_string_release(headers_trim);
				zend_string_release(headers_lc);
			}
			*error = W32_SM_SENDMAIL_FROM_NOT_SET;
			return FAILURE;
		}
	}

	 ret = SendText(host, RPath, Subject, mailTo, data, headers_trim, headers_lc, error_message);
	TSMClose();
	efree(RPath);

	if (headers) {
		zend_string_release(headers_trim);
		zend_string_release(headers_lc);
	}
	if (ret != SUCCESS) {
		*error = ret;
		return FAILURE;
	}
	return SUCCESS;
}

//*********************************************************************
// Name:  TSendMail::~TSendMail
// Input:
// Output:
// Description: DESTRUCTOR
// Author/Date:  jcar 20/9/96
// History:
//*********************************************************************
PHPAPI void TSMClose(void)
{
	Post("QUIT\r\n");
	Ack(NULL);
	/* to guarantee that the cleanup is not made twice and
	   compromise the rest of the application if sockets are used
	   elsewhere
	*/

	shutdown(PW32G(mail_socket), 0);
	closesocket(PW32G(mail_socket));
}


//*********************************************************************
// Name:  char *GetSMErrorText
// Input:   Error index returned by the member functions
// Output:  pointer to a string containing the error description
// Description:
// Author/Date:  jcar 20/9/96
// History:
//*********************************************************************
PHPAPI const char *GetSMErrorText(int index)
{
	if (MIN_ERROR_INDEX <= index && index < MAX_ERROR_INDEX) {
		return (ErrorMessages[index]);

	} else {
		return (ErrorMessages[UNKNOWN_ERROR]);

	}
}

/* strtok_r like, but recognizes quoted-strings */
static char *find_address(char *list, char **state)
{
	bool in_quotes = 0;
	char *p = list;

	if (list == NULL) {
		if (*state == NULL) {
			return NULL;
		}
		p = list = *state;
	}
	*state = NULL;
	while ((p = strpbrk(p, ",\"\\")) != NULL) {
		if (*p == '\\' && in_quotes) {
			if (p[1] == '\0') {
				/* invalid address; let SMTP server deal with it */
				break;
			}
			p++;
		} else if (*p == '"') {
			in_quotes = !in_quotes;
		} else if (*p == ',' && !in_quotes) {
			*p = '\0';
			*state = p + 1;
			break;
		}
		p++;
	}
	return list;
}

//*********************************************************************
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
//*********************************************************************
static int SendText(_In_ const char *host, _In_ const char *RPath, const char *Subject, const char *mailTo, const char *data,
			 zend_string *headers, zend_string *headers_lc, char **error_message)
{
	int res;
	char *p;
	char *tempMailTo, *token, *token_state;
	const char *pos1, *pos2;
	char *server_response = NULL;
	zend_string *stripped_header  = NULL;
	zend_string *data_cln;

	/* check for NULL parameters */
	if (data == NULL)
		return (BAD_MSG_CONTENTS);
	if (mailTo == NULL)
		return (BAD_MSG_DESTINATION);

	/* simple checks for the mailto address */
	/* have ampersand ? */
	/* mfischer, 20020514: I commented this out because it really
	   seems bogus. Only a username for example may still be a
	   valid address at the destination system.
	if (strchr(mailTo, '@') == NULL)
		return (BAD_MSG_DESTINATION);
	*/

	/* attempt to connect with mail host */
	res = MailConnect(host);
	if (res != 0) {
		/* 128 is safe here, the specifier in snprintf isn't longer than that */
		*error_message = ecalloc(1, HOST_NAME_LEN + 128);
		snprintf(*error_message, HOST_NAME_LEN + 128,
			"Failed to connect to mailserver at \"%s\" port " ZEND_ULONG_FMT ", verify your \"SMTP\" "
			"and \"smtp_port\" setting in php.ini or use ini_set()",
			host, !INI_INT("smtp_port") ? 25 : INI_INT("smtp_port"));
		return res;
	}

	res = PostHelo(error_message);
	if (res != SUCCESS) {
		return res;
	}

	SMTP_SKIP_SPACE(RPath);
	FormatEmailAddress(PW32G(mail_buffer), RPath, "MAIL FROM:<%s>\r\n");
	if (!Post(PW32G(mail_buffer))) {
		return (FAILED_TO_SEND);
	}
	if ((res = Ack(&server_response)) != SUCCESS) {
		SMTP_ERROR_RESPONSE(server_response);
		return W32_SM_SENDMAIL_FROM_MALFORMED;
	}

	tempMailTo = estrdup(mailTo);
	/* Send mail to all rcpt's */
	token = find_address(tempMailTo, &token_state);
	while (token != NULL)
	{
		SMTP_SKIP_SPACE(token);
		FormatEmailAddress(PW32G(mail_buffer), token, "RCPT TO:<%s>\r\n");
		if (!Post(PW32G(mail_buffer))) {
			efree(tempMailTo);
			return (FAILED_TO_SEND);
		}
		if ((res = Ack(&server_response)) != SUCCESS) {
			SMTP_ERROR_RESPONSE(server_response);
			efree(tempMailTo);
			return (res);
		}
		token = find_address(NULL, &token_state);
	}
	efree(tempMailTo);

	/* Send mail to all Cc rcpt's */
	if (headers && (pos1 = strstr(ZSTR_VAL(headers_lc), "cc:")) && ((pos1 == ZSTR_VAL(headers_lc)) || (*(pos1-1) == '\n'))) {
		/* Real offset is memaddress from the original headers + difference of
		 * string found in the lowercase headers + 3 characters to jump over
		 * the cc: */
		pos1 = ZSTR_VAL(headers) + (pos1 - ZSTR_VAL(headers_lc)) + 3;
		if (NULL == (pos2 = strstr(pos1, "\r\n"))) {
			tempMailTo = estrndup(pos1, strlen(pos1));
		} else {
			char *pos3;
			while (pos2[2] == ' ' || pos2[2] == '\t') {
				pos3 = strstr(pos2 + 2, "\r\n");
				if (pos3 != NULL) {
					pos2 = pos3;
				} else {
					pos2 += strlen(pos2);
					break;
				}
			}
			tempMailTo = estrndup(pos1, pos2 - pos1);
		}

		token = find_address(tempMailTo, &token_state);
		while (token != NULL)
		{
			SMTP_SKIP_SPACE(token);
			FormatEmailAddress(PW32G(mail_buffer), token, "RCPT TO:<%s>\r\n");
			if (!Post(PW32G(mail_buffer))) {
				efree(tempMailTo);
				return (FAILED_TO_SEND);
			}
			if ((res = Ack(&server_response)) != SUCCESS) {
				SMTP_ERROR_RESPONSE(server_response);
				efree(tempMailTo);
				return (res);
			}
			token = find_address(NULL,&token_state);
		}
		efree(tempMailTo);
	}

	if (!Post("DATA\r\n")) {
		return (FAILED_TO_SEND);
	}
	if ((res = Ack(&server_response)) != SUCCESS) {
		SMTP_ERROR_RESPONSE(server_response);
		return (res);
	}

	/* Send mail to all Bcc rcpt's
	   This is basically a rip of the Cc code above.
	   Just don't forget to remove the Bcc: from the header afterwards. */
	if (headers) {
		if ((pos1 = strstr(ZSTR_VAL(headers_lc), "bcc:")) && (pos1 == ZSTR_VAL(headers_lc) || *(pos1-1) == '\n')) {
			/* Real offset is memaddress from the original headers + difference of
			 * string found in the lowercase headers + 4 characters to jump over
			 * the bcc: */
			pos1 = ZSTR_VAL(headers) + (pos1 - ZSTR_VAL(headers_lc)) + 4;
			if (NULL == (pos2 = strstr(pos1, "\r\n"))) {
				tempMailTo = estrndup(pos1, strlen(pos1));
				/* Later, when we remove the Bcc: out of the
				   header we know it was the last thing. */
				pos2 = pos1;
			} else {
				const char *pos3 = pos2;
				while (pos2[2] == ' ' || pos2[2] == '\t') {
					pos3 = strstr(pos2 + 2, "\r\n");
					if (pos3 != NULL) {
						pos2 = pos3;
					} else {
						pos2 += strlen(pos2);
						break;
					}
				}
				tempMailTo = estrndup(pos1, pos2 - pos1);
				if (pos3 == NULL) {
					/* Later, when we remove the Bcc: out of the
					   header we know it was the last thing. */
					pos2 = pos1;
				}
			}

			token = find_address(tempMailTo, &token_state);
			while (token != NULL)
			{
				SMTP_SKIP_SPACE(token);
				FormatEmailAddress(PW32G(mail_buffer), token, "RCPT TO:<%s>\r\n");
				if (!Post(PW32G(mail_buffer))) {
					efree(tempMailTo);
					return (FAILED_TO_SEND);
				}
				if ((res = Ack(&server_response)) != SUCCESS) {
					SMTP_ERROR_RESPONSE(server_response);
					efree(tempMailTo);
					return (res);
				}
				token = find_address(NULL, &token_state);
			}
			efree(tempMailTo);

			/* Now that we've identified that we've a Bcc list,
			   remove it from the current header. */
			/* headers = point to string start of header
			   pos1    = pointer IN headers where the Bcc starts
			   '4'     = Length of the characters 'bcc:'
			   Because we've added +4 above for parsing the Emails
			   we've to subtract them here. */
			size_t header_length_prior_to_bcc = pos1 - ZSTR_VAL(headers) - 4;
			if (pos1 != pos2) {
				/* if pos1 != pos2 , pos2 points to the rest of the headers.
				   Since pos1 != pos2 if "\r\n" was found, we know those characters
				   are there and so we jump over them (else we would generate a new header
				   which would look like "\r\n\r\n". */
				stripped_header = zend_string_concat2(ZSTR_VAL(headers), header_length_prior_to_bcc, pos2 + 2, strlen(pos2) - 2);
			} else {
				stripped_header = zend_string_truncate(headers, header_length_prior_to_bcc, false);
				ZSTR_VAL(stripped_header)[ZSTR_LEN(stripped_header)] = '\0';
			}
		} else {
			/* Simplify the code that we create a copy of stripped_header no matter if
			   we actually strip something or not. So we've a single zend_string_release() later. */
			stripped_header = zend_string_copy(headers);
		}
	}

	/* send message header */
	bool PostHeaderIsSuccessful = false;
	if (Subject == NULL) {
		PostHeaderIsSuccessful = PostHeader(RPath, "No Subject", mailTo, stripped_header);
	} else {
		PostHeaderIsSuccessful = PostHeader(RPath, Subject, mailTo, stripped_header);
	}
	if (stripped_header) {
		zend_string_release_ex(stripped_header, false);
	}
	if (!PostHeaderIsSuccessful) {
		return FAILED_TO_SEND;
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
		char c, *e2, *e = ZSTR_VAL(data_cln) + ZSTR_LEN(data_cln);
		p = ZSTR_VAL(data_cln);

		while (e - p > 1024) {
			e2 = p + 1024;
			c = *e2;
			*e2 = '\0';
			if (!Post(p)) {
				zend_string_free(data_cln);
				return(FAILED_TO_SEND);
			}
			*e2 = c;
			p = e2;
		}
		if (!Post(p)) {
			zend_string_free(data_cln);
			return(FAILED_TO_SEND);
		}
	}

	zend_string_free(data_cln);

	/*send termination dot */
	if (!Post("\r\n.\r\n"))
		return (FAILED_TO_SEND);
	if ((res = Ack(&server_response)) != SUCCESS) {
		SMTP_ERROR_RESPONSE(server_response);
		return (res);
	}

	return (SUCCESS);
}

static int PostHelo(char **error_message)
{
	size_t namelen;
	struct hostent *ent;
	IN_ADDR addr;
#ifdef HAVE_IPV6
	IN6_ADDR addr6;
#endif
	char mail_local_host[HOST_NAME_LEN];

#if SENDMAIL_DEBUG
	return 0;
#endif

	/* Get our own host name */
	if (gethostname(mail_local_host, HOST_NAME_LEN)) {
		return (FAILED_TO_GET_HOSTNAME);
	}

	ent = gethostbyname(mail_local_host);

	if (!ent) {
		return (FAILED_TO_GET_HOSTNAME);
	}

	namelen = strlen(ent->h_name);

#ifdef HAVE_IPV6
	if (inet_pton(AF_INET, ent->h_name, &addr) == 1 || inet_pton(AF_INET6, ent->h_name, &addr6) == 1)
#else
	if (inet_pton(AF_INET, ent->h_name, &addr) == 1)
#endif
	{
		if (namelen + 2 >= HOST_NAME_LEN) {
			return (FAILED_TO_GET_HOSTNAME);
		}

		strcpy(mail_local_host, "[");
		strcpy(mail_local_host + 1, ent->h_name);
		strcpy(mail_local_host + namelen + 1, "]");
	} else {
		if (namelen >= HOST_NAME_LEN) {
			return (FAILED_TO_GET_HOSTNAME);
		}

		strcpy(mail_local_host, ent->h_name);
	}

	snprintf(PW32G(mail_buffer), sizeof(PW32G(mail_buffer)), "HELO %s\r\n", mail_local_host);

	if (!Post(PW32G(mail_buffer))) {
		return (FAILED_TO_SEND);
	}

	char *server_response = NULL;
	int res = Ack(&server_response);
	if (res != SUCCESS) {
		SMTP_ERROR_RESPONSE(server_response);
		return (res);
	}
	return SUCCESS;
}

//*********************************************************************
// Name:  PostHeader
// Input:       1) return path
//              2) Subject
//              3) destination address
//              4) headers
// Output:      Error code or Success
// Description:
// Author/Date:  jcar 20/9/96
// History:
//*********************************************************************
static bool PostHeader(const char *RPath, const char *Subject, const char *mailTo, zend_string *xheaders)
{
	/* Print message header according to RFC 822 */
	/* Return-path, Received, Date, From, Subject, Sender, To, cc */

	zend_string *headers_lc = NULL;
	smart_str combined_headers = {0};

	if (xheaders) {
		headers_lc = zend_string_tolower(xheaders);
	}

	if (!xheaders || !strstr(ZSTR_VAL(headers_lc), "date:")) {
		time_t tNow = time(NULL);
		zend_string *dt = php_format_date("r", 1, tNow, 1);

		smart_str_appends(&combined_headers, "Date: ");
		smart_str_append(&combined_headers, dt);
		smart_str_appends(&combined_headers, "\r\n");
		zend_string_free(dt);
	}

	if (!headers_lc || !strstr(ZSTR_VAL(headers_lc), "from:")) {
		smart_str_appends(&combined_headers, "From: ");
		smart_str_appends(&combined_headers, RPath);
		smart_str_appends(&combined_headers, "\r\n");
	}
	smart_str_appends(&combined_headers, "Subject: ");
	smart_str_appends(&combined_headers, Subject);
	smart_str_appends(&combined_headers, "\r\n");

	/* Only add the To: field from the $to parameter if isn't in the custom headers */
	if (!headers_lc || (!strstr(ZSTR_VAL(headers_lc), "\r\nto:") && (strncmp(ZSTR_VAL(headers_lc), "to:", 3) != 0))) {
		smart_str_appends(&combined_headers, "To: ");
		smart_str_appends(&combined_headers, mailTo);
		smart_str_appends(&combined_headers, "\r\n");
	}
	if (xheaders) {
		smart_str_append(&combined_headers, xheaders);
		smart_str_appends(&combined_headers, "\r\n");
	}
	/* End of headers */
	smart_str_appends(&combined_headers, "\r\n");
	zend_string *combined_headers_str = smart_str_extract(&combined_headers);

	if (headers_lc) {
		zend_string_release_ex(headers_lc, false);
	}

	bool header_post_status = Post(ZSTR_VAL(combined_headers_str));
	zend_string_efree(combined_headers_str);
	return header_post_status;
}



//*********************************************************************
// Name:  MailConnect
// Input:   None
// Output:  None
// Description: Connect to the mail host and receive the welcome message.
// Author/Date:  jcar 20/9/96
// History:
//*********************************************************************
static int MailConnect(_In_ const char *host)
{

	int res;
	short portnum;
	SOCKADDR_IN sock_in;

	/* Create Socket */
	if ((PW32G(mail_socket) = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		return (FAILED_TO_OBTAIN_SOCKET_HANDLE);
	}

	/* Resolve the servers IP */
	unsigned long server_addr = GetAddr(host);

	portnum = (short) INI_INT("smtp_port");
	if (!portnum) {
		portnum = 25;
	}

	/* Connect to server */
	sock_in.sin_family = AF_INET;
	sock_in.sin_port = htons(portnum);
	sock_in.sin_addr.S_un.S_addr = server_addr;

	if (connect(PW32G(mail_socket), (LPSOCKADDR) & sock_in, sizeof(sock_in))) {
		closesocket(PW32G(mail_socket));
		return (FAILED_TO_CONNECT);
	}

	/* receive Server welcome message */
	res = Ack(NULL);
	return (res);
}


//*********************************************************************
// Name:  Post
// Input:
// Output:
// Description:
// Author/Date:  jcar 20/9/96
// History:
//*********************************************************************
static bool Post(LPCSTR msg)
{
	int len = (int)strlen(msg);
	int slen;
	int index = 0;

#if SENDMAIL_DEBUG
	if (msg)
		printf("POST: '%s'\n", msg);
	return true;
#endif

	while (len > 0) {
		if ((slen = send(PW32G(mail_socket), msg + index, len, 0)) < 1)
			return false;
		len -= slen;
		index += slen;
	}
	return true;
}



//*********************************************************************
// Name:  Ack
// Input:
// Output:
// Description:
// Get the response from the server. We only want to know if the
// last command was successful.
// Author/Date:  jcar 20/9/96
// History:
//*********************************************************************
static int Ack(char **server_response)
{
	ZEND_TLS char buf[MAIL_BUFFER_SIZE];
	int rlen;
	int Index = 0;
	int Received = 0;

#if SENDMAIL_DEBUG
	return (SUCCESS);
#endif

again:

	if ((rlen = recv(PW32G(mail_socket), buf + Index, ((MAIL_BUFFER_SIZE) - 1) - Received, 0)) < 1) {
		return (FAILED_TO_RECEIVE);
	}
	Received += rlen;
	buf[Received] = 0;
	/*err_msg   fprintf(stderr,"Received: (%d bytes) %s", rlen, buf + Index); */

	/* Check for newline */
	Index += rlen;

	/* SMTP RFC says \r\n is the only valid line ending, who are we to argue ;)
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


//*********************************************************************
// Name:  unsigned long GetAddr (const char *szHost)
// Input:
// Output:
// Description: Given a string, it will return an IP address.
//   - first it tries to convert the string directly
//   - if that fails, it tries o resolve it as a hostname
//
// WARNING: gethostbyname() is a blocking function
// Author/Date:  jcar 20/9/96
// History:
//*********************************************************************
static unsigned long GetAddr(const char *szHost)
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

/* returns the contents of an angle-addr (caller needs to efree) or NULL */
static char *get_angle_addr(const char *address)
{
	bool in_quotes = 0;
	const char *p1 = address, *p2;

	while ((p1 = strpbrk(p1, "<\"\\")) != NULL) {
		if (*p1 == '\\' && in_quotes) {
			if (p1[1] == '\0') {
				/* invalid address; let SMTP server deal with it */
				return NULL;
			}
			p1++;
		} else if (*p1 == '"') {
			in_quotes = !in_quotes;
		} else if (*p1 == '<' && !in_quotes) {
			break;
		}
		p1++;
	}
	if (p1 == NULL) return NULL;
	p2 = ++p1;
	while ((p2 = strpbrk(p2, ">\"\\")) != NULL) {
		if (*p2 == '\\' && in_quotes) {
			if (p2[1] == '\0') {
				/* invalid address; let SMTP server deal with it */
				return NULL;
			}
			p2++;
		} else if (*p2 == '"') {
			in_quotes = !in_quotes;
		} else if (*p2 == '>' && !in_quotes) {
			break;
		}
		p2++;
	}
	if (p2 == NULL) return NULL;

	return estrndup(p1, p2 - p1);
}

//*********************************************************************
// Name:  int FormatEmailAddress
// Input:
// Output:
// Description: Formats the email address to remove any content outside
//   of the angle brackets < > as per RFC 2821.
//
//   Returns the invalidly formatted mail address if the < > are
//   unbalanced (the SMTP server should reject it if it's out of spec.)
//
// Author/Date:  garretts 08/18/2009
// History:
//*********************************************************************
static int FormatEmailAddress(char* Buf, const char* EmailAddress, const char* FormatString) {
	char *tmpAddress;
	int result;

	if ((tmpAddress = get_angle_addr(EmailAddress)) != NULL) {
		result = snprintf(Buf, MAIL_BUFFER_SIZE, FormatString, tmpAddress);
		efree(tmpAddress);
		return result;
	}
	return snprintf(Buf, MAIL_BUFFER_SIZE , FormatString , EmailAddress );
} /* end FormatEmailAddress() */
