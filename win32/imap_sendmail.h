#if !defined(imap_sendmail_h)		// Sentry, use file only if it's not already included.
#define imap_sendmail_h

int imap_TSendMail(char *host, int *error,
			  char *headers, char *Subject, char *mailTo, char *data, char *mailCc, char *mailBcc, char *mailRPath);
int imap_SendText(char *RPath, char *Subject, char *mailTo, char *data, char *headers, char *mailCc, char *mailBcc);

int imap_PostHeader(char *, char *, char *, char *, char *);

extern char *GetSMErrorText(int index);
#endif
