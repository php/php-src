#if !defined(sendmail_h)		/* Sentry, use file only if it's not already included. */
#define sendmail_h
#include <windows.h>

#define HOST_NAME_LEN	256
#define MAX_APPNAME_LENGHT 100
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


int TSendMail(char *smtpaddr, int *returnerror, char **error_message,
			  char *RPath, char *Subject, char *mailTo, char *data,
			  char *mailCc, char *mailBcc, char *mailRPath);
void TSMClose(void);
int SendText(char *RPath, char *Subject, char *mailTo, char *mailCc, char *mailBcc, char *data, 
			 char *headers, char *headers_lc, char **error_message);
char *GetSMErrorText(int index);

int MailConnect();
int PostHeader(char *, char *, char *, char *);
int Post(LPCSTR);
int Ack(char **server_response);
unsigned long GetAddr(LPSTR szHost);



#endif							/* sendmail_h */
