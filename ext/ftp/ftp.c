/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   |          Andrew Skalski      <askalski@chek.com>                     |
   +----------------------------------------------------------------------+
 */

#include "php.h"

#if HAVE_FTP

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "ftp.h"


/* reads an ftp response, returns true on success, false on error */
static int		ftp_getresp(ftpbuf_t *ftp);

/* sets the ftp transfer type */
static int		ftp_type(ftpbuf_t *ftp, ftptype_t type);

/* opens up a port for ftp transfer */
static databuf_t*	ftp_port(ftpbuf_t *ftp);

/* accepts the data connection, returns updated data buffer */
static databuf_t*	data_accept(databuf_t *data);

/* closes the data connection, returns NULL */
static databuf_t*	data_close(databuf_t *data);

/* generic file lister */
static char**		ftp_genlist(ftpbuf_t *ftp,
				const char *cmd, const char *path);


ftpbuf_t*
ftp_open(const char *host, short port)
{
	int			fd = -1;
	ftpbuf_t		*ftp;
	struct sockaddr_in	addr;
	struct hostent		*he;
	int			size;


	/* set up the address */
	if ((he = gethostbyname(host)) == NULL) {
		herror("gethostbyname");
		return NULL;
	}

	memset(&addr, 0, sizeof(addr));
	memcpy(&addr.sin_addr, he->h_addr, he->h_length);
	addr.sin_port = port ? port : htons(21);


	/* alloc the ftp structure */
	ftp = calloc(1, sizeof(*ftp));
	if (ftp == NULL) {
		perror("calloc");
		return NULL;
	}

	/* connect */
	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		goto bail;
	}

	if (connect(fd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
		perror("connect");
		goto bail;
	}

	size = sizeof(addr);
	if (getsockname(fd, (struct sockaddr*) &addr, &size) == -1) {
		perror("getsockname");
		goto bail;
	}

	ftp->localaddr = addr.sin_addr;

	if ((ftp->fp = fdopen(fd, "r+")) == NULL) {
		perror("fdopen");
		goto bail;
	}

	if (!ftp_getresp(ftp) || ftp->resp != 220) {
		goto bail;
	}

	return ftp;

bail:
	if (ftp->fp)
		fclose(ftp->fp);
	else if (fd != -1)
		close(fd);
	free(ftp);
	return NULL;
}


ftpbuf_t*
ftp_close(ftpbuf_t *ftp)
{
	if (ftp == NULL)
		return NULL;
	if (ftp->fp)
		fclose(ftp->fp);
	ftp_gc(ftp);
	free(ftp);
	return NULL;
}


void
ftp_gc(ftpbuf_t *ftp)
{
	if (ftp == NULL)
		return;

	free(ftp->pwd);
	ftp->pwd = NULL;
	free(ftp->syst);
	ftp->syst = NULL;
}


int
ftp_quit(ftpbuf_t *ftp)
{
	if (ftp == NULL)
		return 0;

	fprintf(ftp->fp, "QUIT\r\n");
	if (!ftp_getresp(ftp) || ftp->resp != 221)
		return 0;

	free(ftp->pwd);
	ftp->pwd = NULL;

	return 1;
}


int
ftp_login(ftpbuf_t *ftp, const char *user, const char *pass)
{
	if (ftp == NULL)
		return 0;

	fprintf(ftp->fp, "USER %s\r\n", user);
	if (!ftp_getresp(ftp))
		return 0;
	if (ftp->resp == 230)
		return 1;
	if (ftp->resp != 331)
		return 0;
	fprintf(ftp->fp, "PASS %s\r\n", pass);
	if (!ftp_getresp(ftp))
		return 0;
	return (ftp->resp == 230);
}


int
ftp_reinit(ftpbuf_t *ftp)
{
	if (ftp == NULL)
		return 0;

	ftp_gc(ftp);

	fprintf(ftp->fp, "REIN\r\n");
	if (!ftp_getresp(ftp) || ftp->resp != 220)
		return 0;

	return 1;
}


const char*
ftp_syst(ftpbuf_t *ftp)
{
	char		*syst, *end;

	if (ftp == NULL)
		return NULL;

	/* default to cached value */
	if (ftp->syst)
		return ftp->syst;

	fprintf(ftp->fp, "SYST\r\n");
	if (!ftp_getresp(ftp) || ftp->resp != 215)
		return NULL;

	syst = ftp->inbuf;
	if ((end = strchr(syst, ' ')))
		*end = 0;
	ftp->syst = strdup(syst);
	if (end)
		*end = ' ';

	return ftp->syst;
}


const char*
ftp_pwd(ftpbuf_t *ftp)
{
	char		*pwd, *end;

	if (ftp == NULL)
		return NULL;

	/* default to cached value */
	if (ftp->pwd)
		return ftp->pwd;

	fprintf(ftp->fp, "PWD\r\n");
	if (!ftp_getresp(ftp) || ftp->resp != 257)
		return NULL;

	/* copy out the pwd from response */
	if ((pwd = strchr(ftp->inbuf, '"')) == NULL)
		return NULL;
	end = strrchr(++pwd, '"');
	*end = 0;
	ftp->pwd = strdup(pwd);
	*end = '"';

	return ftp->pwd;
}


int
ftp_chdir(ftpbuf_t *ftp, const char *dir)
{
	if (ftp == NULL)
		return 0;

	free(ftp->pwd);
	ftp->pwd = NULL;

	fprintf(ftp->fp, "CWD %s\r\n", dir);
	if (!ftp_getresp(ftp) || ftp->resp != 250)
		return 0;

	return 1;
}


int
ftp_cdup(ftpbuf_t *ftp)
{
	if (ftp == NULL)
		return 0;

	free(ftp->pwd);
	ftp->pwd = NULL;

	fprintf(ftp->fp, "CDUP\r\n");
	if (!ftp_getresp(ftp) || ftp->resp != 250)
		return 0;

	return 1;
}


char*
ftp_mkdir(ftpbuf_t *ftp, const char *dir)
{
	char		*mkd, *end;

	if (ftp == NULL)
		return NULL;

	fprintf(ftp->fp, "MKD %s\r\n", dir);
	if (!ftp_getresp(ftp) || ftp->resp != 257)
		return NULL;

	/* copy out the dir from response */
	if ((mkd = strchr(ftp->inbuf, '"')) == NULL)
		return NULL;
	end = strrchr(++mkd, '"');
	*end = 0;
	mkd = strdup(mkd);
	*end = '"';

	return mkd;
}


int
ftp_rmdir(ftpbuf_t *ftp, const char *dir)
{
	if (ftp == NULL)
		return 0;

	fprintf(ftp->fp, "RMD %s\r\n", dir);
	if (!ftp_getresp(ftp) || ftp->resp != 250)
		return 0;

	return 1;
}


char**
ftp_nlist(ftpbuf_t *ftp, const char *path)
{
	return ftp_genlist(ftp, "NLST", path);
}


char**
ftp_list(ftpbuf_t *ftp, const char *path)
{
	return ftp_genlist(ftp, "LIST", path);
}


int
ftp_getresp(ftpbuf_t *ftp)
{
	char		tag[4];
	int		ch;
	char		*buf;
	char		*ptr;

	if (ftp == NULL)
		return 0;
	buf = ftp->inbuf;
	ftp->resp = 0;

	do {
		if (!fread(tag, 4, 1, ftp->fp))
			return 0;

		if (tag[3] == '-') {
			while ((ch = getc(ftp->fp)) != '\n')
				if (ch == EOF) {
					return 0;
				}
		}
		else if (tag[3] == ' ') {
			ptr = fgets(buf, FTP_BUFSIZE, ftp->fp);
			if (!ptr || !(ptr = strchr(buf, '\n')))
				return 0;
			if (ptr > buf && ptr[-1] == '\r')
				ptr--;
			*ptr = 0;
		}
		else {
			return 0;
		}
	} while (tag[3] == '-');


	/* translate the tag */
	if (!isdigit(tag[0]) || !isdigit(tag[1]) || !isdigit(tag[2]))
		return 0;

	ftp->resp =	100 * (tag[0] - '0') +
			10 * (tag[1] - '0') +
			(tag[2] - '0');
	return 1;
}


int
ftp_type(ftpbuf_t *ftp, ftptype_t type)
{
	char		typechar;

	if (ftp == NULL)
		return 0;

	if (type == ftp->type)
		return 1;

	if (type == FTPTYPE_ASCII)
		typechar = 'A';
	else if (type == FTPTYPE_IMAGE)
		typechar = 'I';
	else
		return 0;

	fprintf(ftp->fp, "TYPE %c\r\n", typechar);
	if (!ftp_getresp(ftp) || ftp->resp != 200)
		return 0;

	ftp->type = type;

	return 1;
}


int
ftp_get(ftpbuf_t *ftp, FILE *outfp, const char *path, ftptype_t type)
{
	databuf_t		*data = NULL;
	int			ch, lastch;

	if (ftp == NULL)
		return 0;

	if (!ftp_type(ftp, type))
		goto bail;

	if ((data = ftp_port(ftp)) == NULL)
		goto bail;

	fprintf(ftp->fp, "RETR %s\r\n", path);
	if (!ftp_getresp(ftp) || ftp->resp != 150)
		goto bail;

	if ((data = data_accept(data)) == NULL)
		goto bail;

	lastch = 0;
	while ((ch = getc(data->fp)) != EOF) {
		if (type == FTPTYPE_ASCII) {
			if (lastch == '\r' && ch != '\n')
				putc('\r', outfp);
			if (ch != '\r')
				putc(ch, outfp);
			lastch = ch;
		}
		else {
			putc(ch, outfp);
		}
	}
	if (type == FTPTYPE_ASCII && lastch == '\r')
		putc('\r', outfp);

	if (ferror(data->fp) || ferror(outfp))
		goto bail;

	data = data_close(data);

	if (!ftp_getresp(ftp) || ftp->resp != 226)
		goto bail;

	return 1;
bail:
	data_close(data);
	return 0;
}


int
ftp_put(ftpbuf_t *ftp, const char *path, FILE *infp, ftptype_t type)
{
	databuf_t		*data = NULL;
	int			ch;

	if (ftp == NULL)
		return 0;

	if (!ftp_type(ftp, type))
		goto bail;

	if ((data = ftp_port(ftp)) == NULL)
		goto bail;

	fprintf(ftp->fp, "STOR %s\r\n", path);
	if (!ftp_getresp(ftp) || ftp->resp != 150)
		goto bail;

	if ((data = data_accept(data)) == NULL)
		goto bail;

	while ((ch = getc(infp)) != EOF) {
		if (type == FTPTYPE_ASCII && ch == '\n')
			putc('\r', data->fp);
		putc(ch, data->fp);
	}

	if (ferror(data->fp) || ferror(infp))
		goto bail;

	data = data_close(data);

	if (!ftp_getresp(ftp) || ftp->resp != 226)
		goto bail;

	return 1;
bail:
	data_close(data);
	return 0;
}


databuf_t*
ftp_port(ftpbuf_t *ftp)
{
	int			fd = -1;
	databuf_t		*data;
	struct sockaddr_in	addr;
	int			size;
	union {
		unsigned long	l[1];
		unsigned short	s[2];
		unsigned char	c[4];
	}			ipbox;

	/* alloc the data structure */
	data = calloc(1, sizeof(*data));
	if (data == NULL) {
		perror("calloc");
		return NULL;
	}
	data->listener = -1;
	data->type = ftp->type;

	/* bind/listen */
	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		goto bail;
	}

	/* bind to a local address */
	memset(&addr, 0, sizeof(addr));
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = 0;

	if (bind(fd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
		perror("bind");
		goto bail;
	}

	size = sizeof(addr);
	if (getsockname(fd, (struct sockaddr*) &addr, &size) == -1) {
		perror("getsockname");
		goto bail;
	}

	if (listen(fd, 5) == -1) {
		perror("listen");
		goto bail;
	}

	data->listener = fd;

	/* send the PORT */
	ipbox.l[0] = ftp->localaddr.s_addr;
	fprintf(ftp->fp, "PORT %u,%u,%u,%u,",
		ipbox.c[0], ipbox.c[1], ipbox.c[2], ipbox.c[3]);
	ipbox.s[0] = addr.sin_port;
	fprintf(ftp->fp, "%u,%u\r\n",
		ipbox.c[0], ipbox.c[1]);

	if (!ftp_getresp(ftp) || ftp->resp != 200)
		goto bail;

	return data;

bail:
	if (fd != -1)
		close(fd);
	free(data);
	return NULL;
}


databuf_t*
data_accept(databuf_t *data)
{
	struct sockaddr_in	addr;
	int			size;
	int			fd;

	size = sizeof(addr);
	fd = accept(data->listener, (struct sockaddr*) &addr, &size);
	close(data->listener);
	data->listener = -1;

	if (fd == -1) {
		free(data);
		return NULL;
	}

	if ((data->fp = fdopen(fd, "r+")) == NULL) {
		close(fd);
		free(data);
		return NULL;
	}

	return data;
}


databuf_t*
data_close(databuf_t *data)
{
	if (data == NULL)
		return NULL;
	if (data->listener != -1)
		close(data->listener);
	if (data->fp)
		fclose(data->fp);
	free(data);
	return NULL;
}


char**
ftp_genlist(ftpbuf_t *ftp, const char *cmd, const char *path)
{
	FILE		*tmpfp = NULL;
	databuf_t	*data = NULL;
	int		ch, lastch;
	int		size;
	int		lines;
	char		**ret = NULL;
	char		**entry;
	char		*text;


	if ((tmpfp = tmpfile()) == NULL)
		return NULL;

	if (!ftp_type(ftp, FTPTYPE_ASCII))
		goto bail;

	if ((data = ftp_port(ftp)) == NULL)
		goto bail;

	if (path)
		fprintf(ftp->fp, "%s %s\r\n", cmd, path);
	else
		fprintf(ftp->fp, "%s\r\n", cmd);
	if (!ftp_getresp(ftp) || ftp->resp != 150)
		goto bail;

	/* pull data buffer into tmpfile */
	if ((data = data_accept(data)) == NULL)
		goto bail;

	size = 0;
	lines = 0;
	lastch = 0;
	while ((ch = getc(data->fp)) != EOF) {
		if (ch == '\n' && lastch == '\r')
			lines++;
		else
			size++;
		putc(ch, tmpfp);
		lastch = ch;
	}
	data = data_close(data);

	if (ferror(tmpfp))
		goto bail;

	rewind(tmpfp);

	ret = malloc((lines + 1) * sizeof(char**) + size * sizeof(char*));
	if (ret == NULL) {
		perror("malloc");
		goto bail;
	}

	entry = ret;
	text = (char*) (ret + lines + 1);
	*entry = text;
	lastch = 0;
	while ((ch = getc(tmpfp)) != EOF) {
		if (ch == '\n' && lastch == '\r') {
			*(text - 1) = 0;
			*++entry = text;
		}
		else {
			*text++ = ch;
		}
		lastch = ch;
	}
	*entry = NULL;

	if (ferror(tmpfp))
		goto bail;

	fclose(tmpfp);

	if (!ftp_getresp(ftp) || ftp->resp != 226) {
		free(ret);
		return NULL;
	}

	return ret;
bail:
	data_close(data);
	fclose(tmpfp);
	free(ret);
	return NULL;
}

#endif /* HAVE_FTP */
