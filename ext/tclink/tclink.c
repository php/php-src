/* tclink.c - Library code for the TCLink client API.
 *
 * TCLink Copyright (c) 2002 TrustCommerce.
 * http://www.trustcommerce.com
 * developer@trustcommerce.com
 * (626) 744-7700
 */

#include "php_tclink.h"

#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#define DEFAULT_HOST    "gateway.trustcommerce.com"
#define TIMEOUT         40     /* seconds */

#define TC_BUFF_MAX     16000
#define TC_LINE_MAX     ((PARAM_MAX_LEN * 2) + 2)

char *tclink_version    = "3.3-PHP";
char *tclink_host       = DEFAULT_HOST;
int tclink_port         = 443;

/*************************************************/
/* Data structures used only within this module. */
/*************************************************/

/* Variables used for transaction data. */

typedef struct param_data
{
	char *name;
	char *value;
	struct param_data *next;
} param;

typedef struct _TCLinkCon
{
	/* Connection data */
	int *ip;
	int num_ips;
	int sd;

	/* SSL encryption */
	X509 *tc_cert;
	SSL_METHOD *meth;
	SSL_CTX *ctx;
	SSL *ssl;

	/* Transaction parameters, sent and received */
	param *send_param_list, *send_param_tail;
	param *recv_param_list;

	/* Connection status */
	int is_error;
	int pass;
	time_t start_time;
	int dns;

} TCLinkCon;

/* The TrustCommerce certificate. */
unsigned char cert_data[540]={
0x30,0x82,0x02,0x18,0x30,0x82,0x01,0x81,0x02,0x01,0x02,0x30,0x0D,0x06,0x09,0x2A,
0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x04,0x05,0x00,0x30,0x55,0x31,0x0B,0x30,0x09,
0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x31,0x14,0x30,0x12,0x06,0x03,0x55,
0x04,0x08,0x13,0x0B,0x4C,0x6F,0x73,0x20,0x41,0x6E,0x67,0x65,0x6C,0x65,0x73,0x31,
0x17,0x30,0x15,0x06,0x03,0x55,0x04,0x0A,0x13,0x0E,0x54,0x72,0x75,0x73,0x74,0x20,
0x43,0x6F,0x6D,0x6D,0x65,0x72,0x63,0x65,0x31,0x17,0x30,0x15,0x06,0x03,0x55,0x04,
0x03,0x13,0x0E,0x50,0x43,0x41,0x20,0x28,0x31,0x30,0x32,0x34,0x20,0x62,0x69,0x74,
0x29,0x30,0x1E,0x17,0x0D,0x30,0x30,0x30,0x34,0x32,0x39,0x30,0x35,0x30,0x39,0x30,
0x34,0x5A,0x17,0x0D,0x30,0x34,0x30,0x34,0x32,0x39,0x30,0x35,0x30,0x39,0x30,0x34,
0x5A,0x30,0x54,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,
0x31,0x14,0x30,0x12,0x06,0x03,0x55,0x04,0x08,0x13,0x0B,0x4C,0x6F,0x73,0x20,0x41,
0x6E,0x67,0x65,0x6C,0x65,0x73,0x31,0x17,0x30,0x15,0x06,0x03,0x55,0x04,0x0A,0x13,
0x0E,0x54,0x72,0x75,0x73,0x74,0x20,0x43,0x6F,0x6D,0x6D,0x65,0x72,0x63,0x65,0x31,
0x16,0x30,0x14,0x06,0x03,0x55,0x04,0x03,0x13,0x0D,0x43,0x41,0x20,0x28,0x31,0x30,
0x32,0x34,0x20,0x62,0x69,0x74,0x29,0x30,0x81,0x9F,0x30,0x0D,0x06,0x09,0x2A,0x86,
0x48,0x86,0xF7,0x0D,0x01,0x01,0x01,0x05,0x00,0x03,0x81,0x8D,0x00,0x30,0x81,0x89,
0x02,0x81,0x81,0x00,0xBD,0x7C,0x7B,0x6F,0x77,0x46,0xE3,0x0F,0xF8,0x50,0x89,0x06,
0xFC,0x54,0x5A,0x59,0x30,0x55,0xC6,0x00,0x34,0x6F,0x6B,0x64,0x8E,0x11,0x3C,0xDD,
0xA9,0x0D,0xC5,0xE1,0x1C,0x49,0xF7,0x0A,0x6B,0x3E,0xAA,0x98,0xA4,0xA2,0x8D,0xEF,
0x9A,0xCB,0xA2,0x40,0x87,0x3B,0x4B,0x13,0x73,0xE6,0x6C,0x39,0x1C,0x48,0xBE,0x6C,
0x1C,0x78,0x0F,0x8E,0x40,0x27,0xAD,0x61,0x0E,0x5E,0x1F,0x94,0xD7,0xAB,0x61,0x3C,
0xB1,0xF4,0xC9,0xE2,0x0D,0x05,0x83,0xE8,0x75,0xAB,0x64,0x12,0x39,0xAB,0xEF,0x79,
0x53,0x49,0x48,0xA0,0x9C,0x55,0xD2,0xE3,0xD0,0x25,0x94,0x78,0x69,0x03,0x95,0xBA,
0x68,0xC6,0x35,0xFB,0x54,0x3D,0x05,0x6D,0xAD,0x50,0x5F,0xE7,0x63,0xB9,0x4A,0x28,
0xB0,0xB3,0xE2,0x07,0x02,0x03,0x01,0x00,0x01,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,
0x86,0xF7,0x0D,0x01,0x01,0x04,0x05,0x00,0x03,0x81,0x81,0x00,0x34,0xED,0xF6,0x9D,
0x32,0x51,0xCA,0x22,0xD6,0x8C,0x81,0x6F,0x64,0x41,0x7D,0xC0,0xD8,0x66,0xB7,0x2C,
0x65,0x54,0x8A,0xE4,0x21,0x6A,0xF2,0x0F,0x3F,0xD0,0x85,0xDC,0x15,0xC1,0x5C,0x72,
0x9A,0x0F,0x00,0x8D,0x38,0x59,0x70,0x7A,0x9C,0x40,0xA6,0x9A,0x2D,0x0E,0xA0,0x31,
0x61,0x2E,0xA2,0x77,0x11,0xBB,0x20,0xF8,0xF9,0x28,0x10,0x1E,0x12,0xB1,0x9D,0x29,
0xF7,0x86,0x12,0x05,0x83,0x83,0xE3,0xC3,0x82,0x65,0x97,0xE9,0xC2,0x5B,0x09,0x11,
0x1D,0xF1,0x01,0x37,0x20,0x2E,0xC5,0x69,0x9C,0xED,0xE3,0xC1,0x29,0x1B,0x3D,0x47,
0x72,0xED,0xA1,0x7B,0xE4,0x8B,0x2B,0x18,0x39,0xEA,0xDE,0x54,0x69,0xE7,0x35,0xDB,
0x8F,0xFB,0x34,0xC7,0xF7,0xB3,0x6A,0x9A,0xE5,0x27,0xA4,0x0F};


/*************************************
 * Internal functions, not exported. *
 *************************************/

/* Random number from min to max. */
static int number(int min, int max)
{
	return (rand() % (max - min + 1)) + min;
}

/* Safe string copy and append functions. */
#define SAFE_COPY(d, s)    safe_copy((d), (s), sizeof(d));
#define SAFE_APPEND(d, s)  safe_append((d), (s), sizeof(d));

void safe_copy(char *dst, const char *src, int size)
{
  int len = strlen(src);
  if (len < size)
    strcpy(dst, src);
  else {
    strncpy(dst, src, size - 1);
    dst[size-1] = 0;
  }
}

void safe_append(char *dst, const char *src, int size)
{
  int dlen = strlen(dst);
  int slen = strlen(src);
	int avail = size - dlen;
	if (avail < 1)
		return;

  if (slen < avail)
    strcpy(dst+dlen, src);
  else {
    strncpy(dst+dlen, src, avail - 1);
    dst[size-1] = 0;
  }
}

/* Add a parameter-value pair to the recieved list. */
static void AddRecvParam(TCLinkCon *c, const char *name, const char *value)
{
	param *p;

	if (name[0] == 0 || value[0] == 0)
		return;

	p = (param *)malloc(sizeof(param));
	p->name = strdup(name);
	p->value = strdup(value);
	p->next = c->recv_param_list;
	c->recv_param_list = p;
}

/* Add a string to the received list. */
static int AddRecvString(TCLinkCon *c, char *string)
{
	char *ptr = strchr(string, '=');
	if (ptr == NULL)
		return 0;

	*ptr = 0;
	AddRecvParam(c, string, ptr+1);

	return 1;
}

/* Deallocate the send list. */
static void ClearSendList(TCLinkCon *c)
{
	param *p, *next;
	for (p = c->send_param_list; p; p = next)
	{
		next = p->next;
		free(p->name);
		free(p->value);
		free(p);
	}

	c->send_param_list = c->send_param_tail = NULL;
}

/* Deallocate the recv list. */
static void ClearRecvList(TCLinkCon *c)
{
	param *p, *next;
	for (p = c->recv_param_list; p; p = next)
	{
		next = p->next;
		free(p->name);
		free(p->value);
		free(p);
	}

	c->recv_param_list = NULL;
}

/* Open a socket to the host_ip specified.  Returns the socket's file
 * descriptor on success (the open attempt is underway) or -1 for failure
 * (should never happen in practice).  Note that this function DOES NOT block
 * and wait for the connection; you'll need to select() on the socket later to see
 * if it opened successfully.
 */
static int BeginConnection(TCLinkCon *c, int host_ip)
{
	struct sockaddr_in sa;
	int sd;

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0)
		return -1;

	fcntl(sd, F_SETFL, O_NONBLOCK);

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = host_ip;
	sa.sin_port = htons(tclink_port);

	connect(sd, (struct sockaddr *) &sa, sizeof(sa));

	return sd;
}

/* This function is called on a socket file descriptor once the connection has been
 * established and we're ready to negotiate SSL.  If the SSL handshake fails for some
 * reason (such as the host on the other end not using SSL), it will return 0 for
 * failure.  Success returns 1.
 */
static int FinishConnection(TCLinkCon *c, int sd)
{
	int ssl_connected, is_error, errcode, res, n;
	struct pollfd pfd;
	X509 *server_cert;
	time_t start, remaining;

	/* check if socket has connected successfully */
	int val;
	int /*socklen_t*/ size = 4;
	getsockopt(sd, SOL_SOCKET, SO_ERROR, &val, &size);
	if (val != 0)
		return 0;

	c->ssl = SSL_new(c->ctx);
	if (!c->ssl)
		return 0;

	SSL_set_fd(c->ssl, sd);

	ssl_connected = 0;
	is_error = 0;
	start = time(0);

	while (!ssl_connected && !is_error)
	{
		remaining = 5 - (time(0) - start);
		if (remaining <= 0) {
			is_error = 1;
			break;
		}

		res = SSL_connect(c->ssl);

		ssl_connected = ((res == 1) && SSL_is_init_finished(c->ssl));

		if (!ssl_connected)
		{
			errcode = SSL_get_error(c->ssl, res);
			switch (errcode)
			{
				case SSL_ERROR_NONE:
					/* no error, we should have a connection, check again */
					break;

				case SSL_ERROR_WANT_READ:
				case SSL_ERROR_WANT_WRITE:
					/* no error, just wait for more data */
					pfd.fd = sd; pfd.events = POLLOUT|POLLIN; pfd.revents = 0;
					while ((n = poll(&pfd, 1, remaining * 1000)) < 0)
						if (errno != EINTR) {
							is_error = 1;
							break;
						}

					if (!n || !(pfd.revents & (POLLOUT|POLLIN)))
						is_error = 1;

					break;

				case SSL_ERROR_ZERO_RETURN: /* peer closed the connection */
				case SSL_ERROR_SSL:         /* error in SSL handshake */
				default:
					is_error = 1;
			}
		}
	}

	if (is_error) {
		SSL_free(c->ssl);
		return 0;
	}
   
	fcntl(sd, F_SETFL, 0);           /* make the socket blocking again */

	/* verify that server certificate is authentic */
	server_cert = SSL_get_peer_certificate(c->ssl);
	if (!server_cert || (X509_cmp(server_cert, c->tc_cert) != 0)) {
		SSL_free(c->ssl);
		return 0;
	}

	X509_free(server_cert);

	return 1;
}

/* This function should be called on list of socket file descriptors (sd) to determine
 * if any have opened successfully.  If so, it will return which one (index into
 * the array).  Otherwise it returns -1 if none have successfully opened.
 * This function will block for a maximum of 3 seconds.
 * As this function calls FinishConnection(), you shouldn't need to do anything special
 * after it returns success - the socket is set up and ready for use.
 */
static int CheckConnection(TCLinkCon *c, int *sd, int num_sd)
{
	fd_set wr_set, err_set;
	struct timeval tv;
	int max_sd = -1, i;

	tv.tv_sec = 3;        /* wait 3 seconds for soc->mething to happen */
	tv.tv_usec = 0;

	/* build the fd_sets used for select() */
	FD_ZERO(&wr_set);
	FD_ZERO(&err_set);
	for (i = 0; i < num_sd; i++)
	{
		if (sd[i] < 0) continue;
		FD_SET(sd[i], &wr_set);
		FD_SET(sd[i], &err_set);
		if (sd[i] > max_sd)
			max_sd = sd[i];
	}

	/* run the select and see what we have waiting for us */
	if (select(max_sd + 1, NULL, &wr_set, &err_set, &tv) < 1)
		return -1;     /* I hope this never happens */

	for (i = 0; i < num_sd; i++)
		if (sd[i] >= 0)
		{
			if (FD_ISSET(sd[i], &err_set))
			{
				/* error - close the socket and mark it defunct */
				close(sd[i]);
				sd[i] = -1;
			}
			else if (FD_ISSET(sd[i], &wr_set))
			{
				/* socket has opened! try to negotiate SSL */
				if (FinishConnection(c, sd[i])) {
					/* socket is ready to go, so return success */
					c->sd = sd[i];
					return i;
				}
				else {
					/* SSL handshake had errors, close the socket and mark it defunct */
					close(sd[i]);
					sd[i] = -1;
				}
			}
		}

	/* if we get here, nothing much interesting happened during those 3 seconds */
	return -1;
}

void do_SSL_randomize()
{
	enum { RAND_VALS = 32 };
	int randbuf[RAND_VALS];
	char fname[512];
	int use_rand_file;
	time_t t;
	int i, c;

	/* if they have a /dev/urandom we can skip this function */
	if (RAND_status() != 0)
		return;

	t = time(0);
	RAND_seed((char *)&t, sizeof(time_t));

	/* have they specified a random file with RANDFILE environment variable? */
	use_rand_file = RAND_file_name(fname, sizeof(fname)) ? 1 : 0;
	if (use_rand_file)
		RAND_load_file(fname, 4096);

	/* stuff it with packets of random numbers until it is satisfied */
	for (i = 0; i < 256 && RAND_status() == 0; i++)
	{
		for (c = 0; c < RAND_VALS; c++)
			randbuf[c] = rand();
		RAND_seed((char *)randbuf, sizeof(int) * RAND_VALS);
	}
}

/* Open a connection to one of the TrustCommerce gateway servers. */
static int Connect(TCLinkCon *c, int host_hash)
{
	struct hostent default_he;
	char *addr_list[3]; int addr[2];
	struct hostent *he;
	unsigned int **gw;

	enum { MAX_HOSTS = 32 };
	time_t last_connect[MAX_HOSTS];
	int sd[MAX_HOSTS];
	int num_sd = 0;
	int host;

	int i, j, sort, sort_val;

	unsigned char *cert_data_ptr = cert_data;

	c->sd = -1;
	c->is_error = 0;

	srand(time(0));

	/* These are used as BACKUP ONLY if the DNS if offline. */
	addr[0] = inet_addr("204.212.133.5");
	addr[1] = inet_addr("216.34.194.254");
	addr_list[0] = (char *)&addr[0];
	addr_list[1] = (char *)&addr[1];
	addr_list[2] = 0;
	default_he.h_addr_list = addr_list;

	/* determine IP addresses of gateway */
	if (!c->ip) 
	{
		he = gethostbyname(tclink_host);
		if (he)
			c->dns = 1;
		else {
			/* fall back to hardcoded IPs in an emergency */
			c->dns = 0;
			he = &default_he;
		}

		for (c->num_ips = 0; he->h_addr_list[c->num_ips]; c->num_ips++)
			;

		c->ip = (int *)malloc(c->num_ips * sizeof(int));
		gw = (int unsigned **)he->h_addr_list;

		/* sort the IP address list before storing it */
		for (i = 0; i < c->num_ips; i++)
		{
			sort = 0; sort_val = *gw[0];
			for (j = 1; j < c->num_ips; j++)
				if (*gw[j] > sort_val)
				{
					sort = j;
					sort_val = *gw[j];
				}

			c->ip[i] = sort_val;
			*gw[sort] = 0;
		}
	}

	/* do some SSL setup */
	if (!c->meth)
	{
		do_SSL_randomize();        /* handle systems without /dev/urandom */
		SSLeay_add_ssl_algorithms();
		c->meth = SSLv3_client_method();
	}

	if (!c->ctx)
	{
		c->ctx = SSL_CTX_new(c->meth);
		if (!c->ctx) return 0;
	}

	/* create the valid certificate */
	if (c->tc_cert == NULL) {
		c->tc_cert = d2i_X509(NULL, &cert_data_ptr, 540);
		if (!c->tc_cert) return 0;
	}

	/* This loop works as follows:
	 * Grab the first host.  Try to open a connection to it.  If there was an
	 * error (host down or unreachable) go to the next one.  If nothing has happened
	 * after 3 seconds, open a second socket (the first one is still open!) and try
	 * with the next fail-over host.  Continue to do this for a maximum of MAX_HOSTS
	 * sockets, or until our TIMEOUT value runs out.  We also keep track of how recently
	 * we tried to connect to a given host, so that we avoid saturating the machines
	 * in a heavy-load situation (which could be caused by anything from heavy internet
	 * lag between the local host and the TrustCommerce servers, to heavy load on the
	 * servers themselves due to half a million people trying to run credit card
	 * transactions in the same half second - unlikely, but certainly possible.)
	 */
	c->start_time = time(0);
	c->pass = 1;
	memset(last_connect, 0, MAX_HOSTS * sizeof(time_t));
	host = host_hash % c->num_ips;

	for ( ; time(0) < (c->start_time + TIMEOUT); c->pass++)
	{
		/* retry the first host at least once */
		if (c->pass > 2) host += 1;
		if (host >= c->num_ips) host = 0;

		/* only connect if we haven't tried this host before, or it's been a little
		 * while (note random modifier to help stagger network traffic) */
		if (last_connect[host] == 0 ||
		    (time(0) - last_connect[host]) >= number(TIMEOUT / 4, TIMEOUT))
		{
			if (num_sd < MAX_HOSTS)
			{
				/* fire up a new connection to this host */
				if (c->pass != 1)
					last_connect[host] = time(0);

				sd[num_sd] = BeginConnection(c, c->ip[host]);
				if (sd[num_sd] >= 0)
					num_sd++;
			}

			/* scan all current sockets and see if we've made a successful connection
			 * somewhere.  note that this also includes SSL and all that sort of fun,
			 * so once it returns success, we're all done. */
			if (num_sd > 0)
				if (CheckConnection(c, sd, num_sd) >= 0)
				{
					/* Success: close all other file handles and return */
					for (i = 0; i < num_sd; i++)
						if (sd[i] >= 0 && sd[i] != c->sd)
							close(sd[i]);

					return 1;
				}
		}
	}

	return 0;
}

/* Send a chunk of data through a connection previously opened with Connect(). */
static int Send(TCLinkCon *c, const char *string)
{
	if (SSL_write(c->ssl, string, strlen(string)) < 0)
		return 0;

	return 1;
}

/* Peel a line off the current input.  Note that this DOESN'T necessarily wait for all
 * input to come in, only up to a "\n".  -1 is returned for a network error, otherwise
 * it returns the length of the line read.  If there is not a complete line pending
 * for read this will block until there is, or an error occurs.
 */
static int ReadLine(TCLinkCon *c, char *buffer, char *destbuf)
{
	struct timeval tv;
	fd_set read;
	fd_set error;

	while (1)      /* we wait for a line to come in or an error to occur */
	{
		char *eol = strchr(buffer, '\n');
		if (eol != NULL)
		{
			/* peel off the line and return it */
			*eol++ = 0;
			safe_copy(destbuf, buffer, TC_LINE_MAX);
			memmove(buffer, eol, strlen(eol)+1);
			return strlen(destbuf);
		}
		else
		{
			if (c->is_error == 1)
				return -1;

			/* do socket work to grab the most recent chunk of incoming data */
			FD_ZERO(&read);   FD_SET(c->sd, &read);
			FD_ZERO(&error);  FD_SET(c->sd, &error);
			tv.tv_sec = TIMEOUT;
			tv.tv_usec = 0;

			if (select(c->sd + 1, &read, NULL, &error, &tv) < 1)
				c->is_error = 1;
			else if (FD_ISSET(c->sd, &error))
				c->is_error = 1;
			else if (FD_ISSET(c->sd, &read))
			{
				int buffer_end = strlen(buffer);
				int size = SSL_read(c->ssl, buffer + buffer_end, TC_BUFF_MAX-1 - buffer_end);
				if (size < 0)
					c->is_error = 1;
				else
					buffer[buffer_end + size] = 0;
			}
		}
	}
}

/* Closes a connection opened with Connect() and frees memory associated with it.
 * You ONLY need to Close() connections which opened successfully; those that don't
 * clean up after themselves before Connect() returns.
 */
static int Close(TCLinkCon *c)
{
	if (c->ssl) SSL_shutdown(c->ssl);

	if (c->sd >= 0) {
		close(c->sd);
		c->sd = -1;
	}

	if (c->ssl) {
		SSL_free(c->ssl);
		c->ssl = NULL;
	}

	if (c->ctx) {
		SSL_CTX_free(c->ctx);
		c->ctx = NULL;
	}

	/* We DON'T free c->meth or c->tc_cert here, because they can be *
	 * reused by other transactions run on this same TCLinkHandle.   */

	return 1;
}

/**********************************************
 * API functions exported to the user client. *
 **********************************************/

TCLinkHandle TCLinkCreate()
{
	TCLinkCon *c = (TCLinkCon *)malloc(sizeof(TCLinkCon));

	c->ip = NULL;
	c->num_ips = 0;
	c->sd = -1;

	c->tc_cert = NULL;
	c->meth = NULL;
	c->ctx = NULL;
	c->ssl = NULL;

	c->send_param_list = NULL;
	c->send_param_tail = NULL;
	c->recv_param_list = NULL;

	c->is_error = 0;
	c->pass = 0;
	c->start_time = 0;
	c->dns = -1;

	return (TCLinkHandle)c;
}

void TCLinkPushParam(TCLinkHandle handle, const char *name, const char *value)
{
	param *p;
	char *ch;

	TCLinkCon *c = (TCLinkCon *)handle;

	if (name && value)
	{
		p = (param *)malloc(sizeof(param));
		p->name = strdup(name);
		p->value = strdup(value);
		p->next = NULL;
		if (c->send_param_tail)
			c->send_param_tail->next = p;
		else
			c->send_param_list = p;
		c->send_param_tail = p;

		/* remove newlines and equals signs from the parameter name */
		for (ch = p->name; *ch; ch++)
			if (*ch == '=' || *ch == '\n') *ch = ' ';

		/* remove newlines from the value */
		for (ch = p->value; *ch; ch++)
			if (*ch == '\n') *ch = ' ';
	}
}

void TCLinkSend(TCLinkHandle handle)
{
	param *p, *next;
	char buf[TC_BUFF_MAX], destbuf[TC_LINE_MAX];
	char buf2[1024];
	int host_hash = 1;
	int retval = 0;

	TCLinkCon *c = (TCLinkCon *)handle;

	ClearRecvList(c);

	/* build most of the string we will send to the processor */
	sprintf(buf, "BEGIN\nversion=%s\n", tclink_version);

	for (p = c->send_param_list; p; p = next)
	{
		next = p->next;
		SAFE_COPY(buf2, p->name);
		SAFE_APPEND(buf2, "=");
		SAFE_APPEND(buf2, p->value);
		SAFE_APPEND(buf2, "\n");
		SAFE_APPEND(buf, buf2);
		if (!strcasecmp(p->name, "custid")) {
			host_hash = atoi(p->value);
			host_hash = (host_hash / 100) + (host_hash % 100);
		}
		free(p->name);
		free(p->value);
		free(p);
	}

	c->send_param_list = c->send_param_tail = NULL;

	/* try to make the connection */
	if (!Connect(c, host_hash))
	{
		Close(c);  // clean up any memory Connect() may have left lying around
		AddRecvParam(c, "status", "error");
		AddRecvParam(c, "errortype", "cantconnect");
		return;
	}

	/* append some data about the connection */
	sprintf(buf+strlen(buf), "pass=%d\ntime=%ld\n", c->pass, time(0) - c->start_time);
	if (c->dns != 1) SAFE_APPEND(buf, "dns=n\n");
	SAFE_APPEND(buf, "END\n");

	/* send the data */
	if (Send(c, buf))
	{
		int state = 0;
		buf[0] = destbuf[0] = 0;          /* recycle buf */
		c->is_error = 0;
		while (1)
		{
			int len = ReadLine(c, buf, destbuf);
			if (len == 0) continue;
			if (len < 0) break;
			if (strcasecmp(destbuf, "BEGIN") == 0)
			{
				if (state != 0)
					{ state = -1; break; }
				state = 1;
			}
			else if (strcasecmp(destbuf, "END") == 0)
			{
				if (state != 1)
					state = -1;
				else
					state = 2;
				break;
			}
			else
			{
				if (state != 1 || !AddRecvString(c, destbuf))
					{ state = -1; break; }
			}
		}
		if (state == 2)
			retval = 1;
	}

	Close(c);

	if (!retval)
	{
		ClearRecvList(c);
		AddRecvParam(c, "status", "error");
		AddRecvParam(c, "errortype", "linkfailure");
	}
}
 
char *TCLinkGetResponse(TCLinkHandle handle, const char *name, char *value)
{
	param *p;
	TCLinkCon *c = (TCLinkCon *)handle;

	for (p = c->recv_param_list; p; p = p->next)
		if (strcasecmp(name, p->name) == 0)
		{
			safe_copy(value, p->value, PARAM_MAX_LEN);
			return value;
		}

	return NULL;
}

static void stuff_string(char *buf, int *len, int size, const char *add)
{
	int newlen = strlen(add);
	if ((*len + newlen) >= size)
		newlen = size - *len - 1;
	if (newlen < 1) return;
	strncpy(buf + *len, add, newlen);
	*len += newlen;
	buf[*len] = 0;
}

char *TCLinkGetEntireResponse(TCLinkHandle handle, char *buf, int size)
{
	param *p;
	int len = 0;
	TCLinkCon *c = (TCLinkCon *)handle;

	for (p = c->recv_param_list; p; p = p->next) {
		stuff_string(buf, &len, size, p->name);
		stuff_string(buf, &len, size, "=");
		stuff_string(buf, &len, size, p->value);
		stuff_string(buf, &len, size, "\n");
	}

	return buf;
}

void TCLinkDestroy(TCLinkHandle handle)
{
	TCLinkCon *c = (TCLinkCon *)handle;
	if (!c) return;

	ClearSendList(c);
	ClearRecvList(c);
	Close(c);

	if (c->ip)
		free(c->ip);

	if (c->tc_cert)
		X509_free(c->tc_cert);

	free(c);
}

char *TCLinkGetVersion(char *buf)
{
	strcpy(buf, tclink_version);
	return buf;
}

