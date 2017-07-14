#if !defined(sendmail_h)		/* Sentry, use file only if it's not already included. */
#define sendmail_h
#include <windows.h>

#define HOST_NAME_LEN	256
#define MAX_APPNAME_LENGTH 100
#define MAIL_BUFFER_SIZE		(1024*4)	/* 4k buffer */
/* Return values */
#define MIN_ERROR_INDEX					0 /* Always 0 like SUCCESS */
#define SUCCESS							0
#define FAILED_TO_PARSE_ARGUMENTS		1
#define FAILED_TO_OPEN_MAILFILE			2
#define FAILED_TO_START_SOCKETS			3
#define FAILED_TO_RESOLVE_HOST			4
#define FAILED_TO_OBTAIN_SOCKET_HANDLE	5
#define FAILED_TO_CONNECT				6
#define FAILED_TO_SEND					7
#define FAILED_TO_RECEIVE				8
#define SMTP_SERVER_ERROR				9
#define FAILED_TO_GET_HOSTNAME			10
#define OUT_OF_MEMORY					11
#define UNKNOWN_ERROR					12
#define BAD_MSG_CONTENTS				13
#define BAD_MSG_SUBJECT					14
#define BAD_MSG_DESTINATION				15
#define BAD_MSG_RPATH					16
#define BAD_MAIL_HOST					17
#define BAD_MSG_FILE					18
#define W32_SM_SENDMAIL_FROM_NOT_SET	19
#define W32_SM_SENDMAIL_FROM_MALFORMED	20
#define W32_SM_PCRE_ERROR				21
#define MAX_ERROR_INDEX					22 /* Always last error message + 1 */


PHPAPI int TSendMail(char *smtpaddr, int *returnerror, char **error_message,
			  char *RPath, char *Subject, char *mailTo, char *data,
			  char *mailCc, char *mailBcc, char *mailRPath);
PHPAPI void TSMClose(void);
static int SendText(char *RPath, char *Subject, char *mailTo, char *mailCc, char *mailBcc, char *data,
			 char *headers, char *headers_lc, char **error_message);
PHPAPI char *GetSMErrorText(int index);

static int MailConnect();
static int PostHeader(char *RPath, char *Subject, char *mailTo, char *xheaders);
static int Post(LPCSTR msg);
static int Ack(char **server_response);
static unsigned long GetAddr(LPSTR szHost);
static int FormatEmailAddress(char* Buf, char* EmailAddress, char* FormatString);
#endif							/* sendmail_h */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
