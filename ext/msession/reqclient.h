/*
    Mohawk Software Framework by Mohawk Software
    Copyright (C) 1998-2001 Mark L. Woodward
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
 
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
    MA 02111-1307, USA
 
    If you want support or to professionally license this library, the author
    can be reached at info@mohawksoft.com
*/ 

enum REQ_TYPES
{
	REQ_ERR,
	REQ_OK,
	REQ_STAT,
	REQ_SETVAL,
	REQ_GETVAL,
	REQ_CREATE,
	REQ_DROP,
	REQ_GETALL,
	REQ_FIND,
	REQ_COUNT,
	REQ_FLUSH,
	REQ_SLOCK,
	REQ_SUNLOCK,
	REQ_TIMEOUT,
	REQ_INC,
	REQ_DATAGET,
	REQ_DATASET,
	REQ_LIST,
	REQ_LISTVAR,
	REQ_UNIQ,
	REQ_LAST,
	REQ_POPEN=1024,
	REQ_PCLOSE
};

typedef struct _requestPacket
{
	int	version;
	int	stat;
	int	len;
	int	session;
	int	name;
	int	value;
	int 	param;
	char	datum[0];
}REQ;

typedef struct _requestBuf
{
	unsigned int type;
	unsigned int size;
	REQ	req;
}REQB;

#define MAX_REQ		16384

#define REQB_STATIC	1
#define REQB_DYNAMIC	2

#define STATIC_REQB( len )	\
	char buffer [ len ]; 	\
	REQB *preq = StaticRequestBuffer(buffer, len);

#if defined (__cplusplus)
extern "C" {
#endif

#define SIZEREQB(REQB,SIZE) (((REQB)->size >= (unsigned int) SIZE) ? REQB : SizeRequestBuffer(REQB,SIZE))
REQB *AllocateRequestBuffer(unsigned size);
void FreeRequestBuffer(REQB *req);
REQB *SizeRequestBuffer(REQB *req, unsigned int size);
REQB *StaticRequestBuffer(char *buffer, unsigned int cb);

int FormatRequest(REQB **buffer, int stat, char *session, char *name, char *value, int param);
int FormatRequestMulti(REQB **buffer, int stat, char *session, int n, char **pairs, int param);
int DoSingleRequest(char *hostname, int port, REQB **preq);
void *OpenReqConn(char *hostname, int port);
void CloseReqConn(void *conn);
int DoRequest(void *conn, REQB **preq);
char *ReqErr(int param);

#define ASSERT_STAT(PREQ) if(PREQ->stat != REQ_OK) \
	{fprintf(stderr, "Error in Request %s %d %s\n", \
		__FILE__,__LINE__, ReqErr(PREQ->param)); exit(-1); }

#if defined (__cplusplus)
	// C API but with class definitions
	int ReadRequestTimeout(REQB **ppreq, MSock *sock, int timeout);
	int ReadRequest(REQB **preq, MSock *sock);
	int WriteRequest(REQB *preq, MSock *sock);
}
#endif

