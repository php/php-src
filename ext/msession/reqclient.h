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
#ifndef _REQCLIENT_
#define _REQCLIENT_

#ifndef REQCLIENT_VER	
#define REQCLIENT_VER 051222
#endif

#ifndef _PHOENIX_
typedef unsigned char Boolean;
#endif

#if defined (__cplusplus)
extern "C" {
#endif

// Standard request transport errors.
enum REQ_ERRORS
{
	REQE_NOERROR=0,
	REQE_ERR,
	REQE_NOSESSION,
	REQE_DUPSESSION,
	REQE_NOWLOCK,
	REQE_NOVALUE,
	REQE_LOCKED,
	REQE_NOSEND,
	REQE_NOCONNECT,
	REQE_BADCONN,
	REQE_BADHDR,
	REQE_BADVER,
	REQE_BADLEN,
	REQE_BUFFER,
	REQE_DENIED,
	REQE_NOFN,
	REQE_UNKNOWN,
	REQE_PLUGIN1,
	REQE_PLUGIN2,
	REQE_PLUGIN3,
	REQE_PLUGIN4
};

#define REQ_OK		0
#define REQ_ERR		1
#define REQ_UNHANDLED	3

#define REQ_INTERNAL_BEGIN	1024
#define	REQ_POPEN		1024
#define REQ_PCLOSE		1025
#define REQ_PING		1026
#define REQ_INTERNAL_END	2048

/** Packet version */
#define REQV_VERSION 		0x42
#define REQV_VERMASK		0xFF

/** Packet has array of strings */
#define REQVF_STRINGS		0x0200
/** Packet has binary information */
#define REQVF_BVAL		0x0400


#define REQV_ISVER(V)		(((V) & REQV_VERMASK) == REQV_VERSION)
#define REQVF_TEST(V,F)		(((V) & (F)) == (F))
#define REQVSZ_VERSION		"MV42"
#define REQVSZ_STRINGS		"MS42"
#define REQVSZ_BVAL		"MB42"

#define REQFMT_BINARY	0
#define REQFMT_ASCII	1

// An application can use this to change the default
extern unsigned int g_defReqFmt;

/** Binary request packet */
/** If sizeof(int) > 4 this will need to be fixed. */
typedef struct _requestPacket
{
	int	version;
	int	stat;
	int	len;
	int	session;
	int	name;
	int	value;
	int 	param;
	int	data;
	char	datum[0];
}REQ;

/*** Ascii string version of request packet */
typedef struct _REQSZ
{
	char	version[4];
	char	stat[4];
	char	len[4];
	char	session[4];
	char	name[4];
	char	value[4];
	char	param[4];
	char 	misc[4];
	char	datum[0];
}REQSZ;
typedef struct _requestBuf
{
	unsigned int type;	// Type of packet, dynamic/static
	unsigned int size;	// size of memory block
#if (REQCLIENT_VER >= 030113)
	unsigned int fmt;	// format, binary/ascii
	unsigned int reserved;	// Just in case
#else
#warning You are using an old Phoenix definition (pre 030113), this will have problems with a newer version
#endif
#if (REQCLIENT_VER >= 051222)
	int	cmd;
	int 	param;
	char *	session;
	char *	name;
	char * 	value;
	char *	data;
	int	cbdata;

	// To be used by library layer for bookeeping
	int	cb;
	int	count;
#endif
	REQ	*req;
}REQB;

typedef struct ListWalkDataStruct
{
	REQB	*reqb;
	char	*Key;
	char 	*Data;
	int 	cb;
	int	count;
}ListWalkData;

#define MAX_REQ		16384

#define REQB_STATIC	1
#define REQB_DYNAMIC	2


#define STATIC_REQB( len )	\
	char buffer [ len ]; 	\
	REQB *preq = StaticRequestBuffer(buffer, len);

#define ASCII_REQB(reqb) (REQB)->fmt = REQFMT_ASCII
#define BINARY_REQB(reqb) (REQB)->fmt = REQFMT_BINARY

REQB *AllocateRequestBuffer(REQB *reqb, unsigned size);
void FreeRequestBuffer(REQB *req);
REQB *SizeRequestBuffer(REQB *req, unsigned int size);
REQB *StaticRequestBuffer(char *buffer, unsigned int cb);

int FilterRequest(REQB *preq);
int FormatRequest(REQB *preq, int stat, char *session, char *name, char *value, int param);
int FormatRequestData(REQB *preq, int stat, char *session, char *name, char *value, void *data, int cbdata, int param);
int FormatRequestStrings(REQB *preq, int stat, char *session, char *name, char *value, int n, char **strings);
int FormatRequestf(REQB *preq, int stat, char *session, char *name, char *value, int param, char *fmt, ...);

void *GetReqbDatumPtr(REQB *preq);

int DoSingleRequest(char *hostname, int port, REQB *preq);
void *OpenReqConn(char *hostname, int port);
Boolean ReopenReqConn(void *conn);
void CloseReqConn(void *conn);
int DoRequest(void *conn, REQB *preq);
char *ReqbErr(REQB *reqb);
int LWDaddNameVal(ListWalkData *plw, char *name, char *value);
int LWDaddValue(ListWalkData *plw, char *value);


#define ASSERT_STAT(PREQ) if(PREQ->stat != REQ_OK) \
	{fprintf(stderr, "Error in Request %s %d %s\n", \
		__FILE__,__LINE__, ReqErr(PREQ->param)); exit(-1); }

#if defined (__cplusplus)
	// C API but with class definitions
	int ReadRequestTimeout(REQB *preq, MSock *sock, int timeout);
	int ReadRequest(REQB *reqb, MSock *sock);
	int WriteRequest(REQB *reqb, MSock *sock);
	Boolean OpenReqSock(REQB *reqb, MSock *sock, char *hostname);
	void CloseReqSock(REQB *reqb, MSock *sock);
}

#endif
#endif
