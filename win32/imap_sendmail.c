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
   | Authors: Frank M. Kromann <frank@kromann.info>                       |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php.h"				/*php specific */
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include "time.h"
#include <string.h>
#include <malloc.h>
#include <memory.h>
#include <winbase.h>
#include "ext/imap/php_imap.h"
#include "sendmail.h"
#include "imap_sendmail.h"
#include "php_ini.h"

char Buffer[MAIL_BUFFER_SIZE];

extern int WinsockStarted;
extern char MailHost[HOST_NAME_LEN];
extern char LocalHost[HOST_NAME_LEN];


/********************************************************************
 Name:  imap_TSendMail
 Input:   1) host:    Name of the mail host where the SMTP server resides
                      max accepted length of name = 256
          2) appname: Name of the application to use in the X-mailer
                      field of the message. if NULL is given the application
                      name is used as given by the GetCommandLine() function
                      max accespted length of name = 100
 Output:  1) error:   Returns the error code if something went wrong or
                      SUCCESS otherwise.

 See SendText() for additional args!
********************************************************************/
int imap_TSendMail(char *host, int *error,
			  char *headers, char *Subject, char *mailTo, char *data, char *mailCc, char *mailBcc, char *mailRPath)
{
	int ret;
	char *RPath = NULL;

	WinsockStarted = FALSE;

	if (host == NULL) {
		*error = BAD_MAIL_HOST;
		return BAD_MAIL_HOST;
	} else if (strlen(host) >= HOST_NAME_LEN) {
		*error = BAD_MAIL_HOST;
		return BAD_MAIL_HOST;
	} else {
		strcpy(MailHost, host);
	}

	if (mailRPath)
		RPath = estrdup(mailRPath);
	else if (INI_STR("sendmail_from")){
		RPath = estrdup(INI_STR("sendmail_from"));
	} else {
		return 19;
	}

	// attempt to connect with mail host
	*error = MailConnect();
	if (*error != 0) {
		if(RPath)efree(RPath);
		return *error;
	} else {
		ret = imap_SendText(RPath, Subject, mailTo, data, headers, mailCc, mailBcc);
		TSMClose();
		if (ret != SUCCESS) {
			*error = ret;
		}
		if(RPath)efree(RPath);
		return ret;
	}
}

/********************************************************************
 Name:  imap_TSendText
 Input:       1) RPath:   return path of the message
                          Is used to fill the "Return-Path" and the
                          "X-Sender" fields of the message.
              2) Subject: Subject field of the message. If NULL is given
                          the subject is set to "No Subject"
              3) mailTo:  Destination address
              4) data:    Null terminated string containing the data to be send.
			  5) headers: Null terminated string containing extra headers
			  6) mailCc:  Additional destination addresses
			  7) mailBcc: Send the mail to these addresses as well, but do not 
			              list these in the headers
 Output:      Error code or SUCCESS
 Description:
 Author/Date:  fmk/12-13-2001
 History:
********************************************************************/
int imap_SendText(char *RPath, char *Subject, char *mailTo, char *data, char *headers, char *mailCc, char *mailBcc)
{
	int res, i;
	char *p;
	char *tempMailTo;
	ADDRESS *addr;

	/* check for NULL parameters */
	if (data == NULL)
		return (BAD_MSG_CONTENTS);
	if (mailTo == NULL)
		return (BAD_MSG_DESTINATION);
	if (RPath == NULL)
		return (BAD_MSG_RPATH);

	/* simple checks for the mailto address */
	/* have ampersand ? */
	if (strchr(mailTo, '@') == NULL)
		return (BAD_MSG_DESTINATION);

	sprintf(Buffer, "HELO %s\r\n", LocalHost);

	/* in the beggining of the dialog */
	/* attempt reconnect if the first Post fail */
	if ((res = Post(Buffer)) != SUCCESS) {
		MailConnect();
		if ((res = Post(Buffer)) != SUCCESS)
			return (res);
	}
	if ((res = Ack(NULL)) != SUCCESS)
		return (res);

	sprintf(Buffer, "MAIL FROM:<%s>\r\n", RPath);
	if ((res = Post(Buffer)) != SUCCESS)
		return (res);
	if ((res = Ack(NULL)) != SUCCESS)
		return (res);

	// Send mail to all rcpt's
	tempMailTo = estrdup(mailTo);	// rfc822_parse_adrlist will change the string
	addr = NULL;
	rfc822_parse_adrlist(&addr, tempMailTo, NULL);
	while (addr) {
		if (strcmp(addr->host, ERRHOST) == 0)
			return (BAD_MSG_DESTINATION);
		else {
			sprintf(Buffer, "RCPT TO:<%s@%s>\r\n", addr->mailbox, addr->host);
			if ((res = Post(Buffer)) != SUCCESS)
				return (res);
			if ((res = Ack(NULL)) != SUCCESS)
				return (res);
		}
		addr = addr->next;
	}
	efree(tempMailTo);

	/* Send mail to all Cc rcpt's */
	if (mailCc && *mailCc) {
		tempMailTo = estrdup(mailCc);
		addr = NULL;
		rfc822_parse_adrlist(&addr, tempMailTo, NULL);
		while (addr) {
			if (strcmp(addr->host, ERRHOST) == 0)
				return (BAD_MSG_DESTINATION);
			else {
				sprintf(Buffer, "RCPT TO:<%s@%s>\r\n", addr->mailbox, addr->host);
				if ((res = Post(Buffer)) != SUCCESS)
					return (res);
				if ((res = Ack(NULL)) != SUCCESS)
					return (res);
			}
			addr = addr->next;
		}
		efree(tempMailTo);
	}

	if (mailBcc && *mailBcc) {
		tempMailTo = estrdup(mailBcc);
		addr = NULL;
		rfc822_parse_adrlist(&addr, tempMailTo, NULL);
		while (addr) {
			if (strcmp(addr->host, ERRHOST) == 0)
				return (BAD_MSG_DESTINATION);
			else {
				sprintf(Buffer, "RCPT TO:<%s@%s>\r\n", addr->mailbox, addr->host);
				if ((res = Post(Buffer)) != SUCCESS)
					return (res);
				if ((res = Ack(NULL)) != SUCCESS)
					return (res);
			}
			addr = addr->next;
		}
		efree(tempMailTo);
	}

	if ((res = Post("DATA\r\n")) != SUCCESS)
		return (res);
	if ((res = Ack(NULL)) != SUCCESS)
		return (res);


	// send message header
	if (Subject == NULL)
		res = PostHeader(RPath, "No Subject", mailTo, headers, mailCc);
	else
		res = PostHeader(RPath, Subject, mailTo, headers, mailCc);
	if (res != SUCCESS)
		return (res);


	/* send message contents in 1024 chunks */
	if (strlen(data) <= 1024) {
		if ((res = Post(data)) != SUCCESS)
			return (res);
	} else {
		p = data;
		while (1) {
			if (*p == '\0')
				break;
			if (strlen(p) >= 1024)
				i = 1024;
			else
				i = strlen(p);

			/* put next chunk in buffer */
			strncpy(Buffer, p, i);
			Buffer[i] = '\0';
			p += i;

			/* send chunk */
			if ((res = Post(Buffer)) != SUCCESS)
				return (res);
		}
	}

	/*send termination dot */
	if ((res = Post("\r\n.\r\n")) != SUCCESS)
		return (res);
	if ((res = Ack(NULL)) != SUCCESS)
		return (res);

	return (SUCCESS);
}
