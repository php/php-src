/*
   +----------------------------------------------------------------------+
   | msession 1.0                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Mark Woodward <markw@mohawksoft.com>                        |
   |    Portions copyright the PHP group.                                 |
   +----------------------------------------------------------------------+
 */

/*
--------------------------------------------------------------------------
	These are definitions are pulled from Phoenix.
 	It would probably be easier to maintain one file, 
	but some phoenix header files conflict with other
	project header files common to PHP. Besides, this
	allows this header to be PHP license and Phoenix to
	be LGPL with no conflicts.

	MAKE NO CHANGES TO THIS FILE IT. MUST REMAIN CONSTANT
	WITH PHOENIX OR IT WILL NOT WORK.
--------------------------------------------------------------------------
*/
/* Required to use the new version of Phoenix 1.2 */

#ifndef REQCLIENT_VER	
#define REQCLIENT_VER 030113
#endif

enum REQ_TYPES
{
	REQ_ERR,
	REQ_OK,
	REQ_CTL,
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
	REQ_RANDSTR,
	REQ_PLUGIN,
	REQ_CALL,
	REQ_SERIALIZE,
	REQ_RESTORE,
	REQ_EXEC,
	REQ_LAST,
	REQ_INTERNAL_BEGIN=1023,
	REQ_INTERNALLAST,
};
enum REQ_ERRORS
{
	REQE_NOERROR=0,
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
	REQE_UNKNOWN
};


#define REQ_POPEN               1024
#define REQ_PCLOSE              1025
#define REQ_PING                1026

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
	unsigned int type;	// Type of packet, dynamic/static
	unsigned int size;	// size of memory block
#if (REQCLIENT_VER >= 030113)
	unsigned int fmt;	// format, binary/ascii
	unsigned int reserved;	// Just in case
#else
#warning You are using an old Phoenix definition, this will have problems with a newer version
#endif
	REQ	req;
}REQB;

#define MAX_REQ		16384

#define REQB_STATIC	1
#define REQB_DYNAMIC	2

#define REQ_STAT_EXIST	0
#define REQ_STAT_TTL	1	
#define REQ_STAT_AGE	2
#define REQ_STAT_TLA	3
#define REQ_STAT_CTIM	4
#define REQ_STAT_TOUCH	5
#define REQ_STAT_NOW	6


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

int FormatRequest(REQB **buffer, int stat, const char *session, const char *name, const char *value, int param);
int FormatRequestMulti(REQB **buffer, int stat, char *session, int n, char **pairs, int param);
int FormatRequestStrings(REQB **ppreq, int stat, char *session, int n, char **strings);
int DoSingleRequest(char *hostname, int port, REQB **preq);
void *OpenReqConn(char *hostname, int port);
void CloseReqConn(void *conn);
void DeleteReqConn(void *conn);
unsigned char ReopenReqConn(void *conn);
int DoRequest(void *conn, REQB **preq);
char *ReqbErr(REQB *reqb);

#define ASSERT_STAT(PREQ) if(PREQ->stat != REQ_OK) \
	{fprintf(stderr, "Error in Request %s %d %s\n", \
		__FILE__,__LINE__, ReqErr(PREQ->param)); exit(-1); }

#if defined (__cplusplus)
	/* C API but with class definitions */
	int ReadRequestTimeout(REQB **ppreq, MSock *sock, int timeout);
	int ReadRequest(REQB **preq, MSock *sock);
	int WriteRequest(REQB *preq, MSock *sock);
}
#endif

