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

    When used as part of the PHP project this file, as present in the PHP
    CVS and original source distribution is to be considered part of the
    PHP package and subject to version 2.02 of the PHP license.
    http://www.php.net/license/2_02.txt. 

*/ 
#ifndef _MCACHE_API_
#define _MCACHE_API_

#include "reqclient.h"

#ifdef timeout
#undef timeout
#endif

#ifdef __cplusplus
class MCache_conn : public MSock
{
	protected:
	REQB	m_reqb;
	int	m_errno;
	public:
	MCache_conn(char *host, int port);
	~MCache_conn();
	int Request(int stat, char *session, char *name, char *value, int param=0);
	int RequestData(int stat, char *session, char *name, char *value, char *data, int cbdata, int param=0);
	int RequestStrings(int stat, char *session, char *name, char *value, int n, char **strings);
	char *	ResultToStr();
	char ** ResultToArray();
	char ** ResultToPairs();
	Boolean create(char *session, char *classname, char *strdata);
	Boolean create_pairs(char *session, char *classname, char *strdata, int count, char **pairs);
	Boolean destroy(char *session);
	int set(char *session, char *name, char *value);
	int set_array(char *session, int count, char **pairs);
	int get_int(char *session, char *name, int def);
	char *get_text(char *session, char *name, char *def);
	char * uniq(int cbstr, char *classname, char *strdata);
	char * randstr(int cbstr);
	char **get_array(char *session);
	char **get_object(char *session);
	char **listvar(char *name);
	char **find(char *name, char *value);
	char **list();
	int count();
	int lock(char *session);
	int unlock(char *session, int key);
	Boolean timeout(char *session, int timeout);
	int inc_int(char *session, char *name);
	char *inc_text(char *session, char *name);
	int setdata(char *session, char *data);
	char *getdata(char *session);
	int setdata(char *session, char *data, int len);
	char *getdata(char *session, int *len);
	int call_int(char *fn,int n, char **strings);
	char *call_text(char *fn,int n, char **strings);
	int ctl(char *session,int type);
	int plugin_int(char *session, char *nparam, char *sparam, int param);
	char *plugin_text(char *session, char *nparam, char *sparam, int param);
	int Sessionflush(int seconds);
	int admin(int fn, char *opt);
	int serialize(char *szparam);
	char *exec(char *szcommand);
	char *add(char *session, char *name, char *value);
	char *muldiv(char *session, char *name, char *mul, char *div);
	int ping(void);
	static void free(void *result);
	static int ctl_type(char *str);
	inline Boolean StatOK(void)	{return (m_reqb.req->stat == REQ_OK) ? TRUE : FALSE; }
	inline int param()		{return m_reqb.param; }
	inline int merrno()		{return m_errno;}
	inline char *errtext()		{return ReqbErr(&m_reqb); }
	inline char *reqtext(void)	{return (m_reqb.req->len > 1) ? m_reqb.req->datum : NULL; }

};

typedef MCache_conn *MSCONN;

extern "C" {
#else
typedef void * MSCONN;
#endif

#define MAX_REQ_STRINGS	1024
#define MREQ_TOPHANDLER	-42

enum REQ_TYPES
{
	MREQ_CTL=2,
	MREQ_SETVAL,
	MREQ_GETVAL,
	MREQ_CREATE,
	MREQ_DROP,
	MREQ_GETALL,
	MREQ_FIND,
	MREQ_COUNT,
	MREQ_FLUSH,
	MREQ_SLOCK,
	MREQ_SUNLOCK,
	MREQ_TIMEOUT,
	MREQ_ADD,
	MREQ_DATAGET,
	MREQ_DATASET,
	MREQ_LIST,
	MREQ_LISTVAR,
	MREQ_UNIQ,
	MREQ_RANDSTR,
	MREQ_PLUGIN,
	MREQ_CALL,
	MREQ_SERIALIZE,
	MREQ_RESTORE,
	MREQ_EXEC,
	MREQ_FIND_CACHEID,
	MREQ_SET_CACHEID,
	MREQ_RENAME,
	MREQ_GETOBJECT,
	MREQ_MULDIV,
	MREQ_LAST,
// These are handled by the server, not the library.
	MREQ_ADMIN = REQ_INTERNAL_END,
};
#define MREQ_OK		REQ_OK
#define MREQ_ERR	REQ_ERR

#ifdef PLUGMSG_START
enum MREQ_PLUGIN_MSG
{
	MREQ_PLUGIN_PULSE=PLUGMSG_START,
	MREQ_PLUGIN_SESSION_FLUSH,
	MREQ_PLUGIN_SESSION_CREATE,
	MREQ_PLUGIN_SESSION_INIT,
	MREQ_PLUGIN_SESSION_DESTROY,
	MREQ_PLUGIN_ESCAPE,
	MREQ_PLUGIN_RELAY,
	MREQ_PLUGIN_SERIALIZE,
	MREQ_PLUGIN_RESTORE,
	MREQ_PLUGIN_SESSION_SETDATA,
	MREQ_PLUGIN_SESSION_SETVALUE,
	MREQ_PLUGIN_SESSION_SETBLOB,
	MREQ_PLUGIN_SESSION_RENAME,
	MREQ_PLUGIN_LAST
};

#define MREQ_SESSION_INIT_FAILED	-1
#define MREQ_SESSION_INIT_HANDLED	1
#define MREQ_SESSION_INIT_NOTHANDLED	0
#endif

enum MCACHE_ADMIN
{
	MADMIN_SHUTDOWN,
	MADMIN_LOADDLL,
	MADMIN_SETSERIAL,
	MADMIN_SETVERBOSE,
	MADMIN_SETMAXTHREADS,
	MADMIN_SERIALIZE,
	MADMIN_AUTOSAVE,
	MADMIN_AUTODROP,
	MADMIN_STATUS
};

#define MREQ_CTL_EXIST  0
#define MREQ_CTL_TTL    1
#define MREQ_CTL_AGE    2
#define MREQ_CTL_TLA    3
#define MREQ_CTL_CTIM   4
#define MREQ_CTL_TOUCH  5
#define MREQ_CTL_NOW    6


#define MPARM_TIME_MASK			0x0000FFFF
#define MPARM_TIME_FIXED		0x0000FFFF
#define MPARAM_CBSTR_MASK		0x00FF0000


#define MPARM_TIME(parm)		((parm)&MPARM_TIME_MASK)
#define MPARM_CBSTR(parm)		(((parm) >> 16) & 0xFF)

#define MPARM_CREATE(TIME, CBSTR) \
	((TIME) | (CBSTR << 16)) 

/* Added for MCache objects */
#define MSTAT_TYPE_SESSION		0x00000000
#define MSTAT_TYPE_OBJECT		0x00100000
#define MSTAT_TYPE_FAST			0x00200000
#define MSTAT_TYPE_FIXED		0x00400000
#define MSTAT_TYPE_MASK			0x00F00000
#define MSTAT_CMD_MASK			0x000FFFFF

#define MSTAT_TYPE_TEST(stat, TY)	(((stat) & MSTAT_TYPE_MASK)==(TY))
#define MSTAT_CMD(stat)			((stat) & (MSTAT_CMD_MASK))

// Private session flags
#define MCACHE_FIXED	MSTAT_TYPE_FIXED	// Fixed object, no expiration
#define MCACHE_NORMAL	MSTAT_TYPE_SESSION
#define MCACHE_OBJECT	MSTAT_TYPE_OBJECT
#define MCACHE_FAST	MSTAT_TYPE_FAST

#ifdef WIN32
#define EXTAPI	extern "C" __declspec(dllexport) 
#else
#define EXTAPI	extern "C"
#endif

#ifndef MCACHEAPI_INTERNAL
MSCONN 	mcache_connect(char *host, int port);

/** Disconnects a session from an mcache daemon. Returns nothing. */
void 	mcache_disconnect(MSCONN conn);

/** Creates a new session. Returns TRUE is successful, or FALSE if an error occured.
An error occurs if the session already exists. mcache_errno() will return REQE_DUPSESSION
if the session exists */

Boolean mcache_create(MSCONN conn, char *session, char *classname, char *strdata);

/** Behaves exactly like mcache_create, except that initial data is passed to populate the session.
"pairs" is an array of string data. pairs[0] contains a name, pairs[1] contains its value, pairs[2]
contains the second name, pairs[3] containes the second value, and so on. */

Boolean mcache_create_pairs(MSCONN conn, char *session, char *classname, char *strdata, int count, char **pairs);

/** Destroys a session. Returns TRUE on success, FALSE on error. */
Boolean mcache_destroy(MSCONN conn, char *session);

/** Sets a value in a session. Returns TRUE on success, FALSE on error */
int 	mcache_set(MSCONN conn, char *session, char *name, char *value);

int mcache_set_array(MSCONN conn, char *session, int count, char **pairs);
/** Gets a value in a session as an integer. Returns the value from the session, or
def from the argument list, if no value was found. */
int 	mcache_get_int(MSCONN conn, char *session, char *name, int def);


/** Gets a value in a session as a string. Returns the value from the session, or
def from the argument list, if no value was found. */
char *	mcache_get_text(MSCONN conn, char *session, char *name, char *def);


/** Create a new session with a guaranteed unique name. Returns a string of the name */
char * mcache_uniq(MSCONN conn, int cbstr, char *classname, char *strdata);

/** Create a pseudo-random alphanumeric string */
char * mcache_randstr(MSCONN conn, int cbstr);

/** Gets all the name/value pairs from a session into a character array. The array is 
formatted as an array of string data. array[0] contains a name, array[1] contains its 
value, array[2] contains the second name, array[3] containes the second value, and so on.
Returns NULL on error. */

char ** mcache_get_array(MSCONN conn, char *session);

char **mcache_get_object(MSCONN conn, char *session);

/** Finds sessions which have the particular name/value property. Returns NULL
if none were found. */
char ** mcache_find(MSCONN conn, char *name, char *value);

int mcache_inc_int(MSCONN conn, char *session, char *name);
char *mcache_inc_text(MSCONN conn, char *session, char *name);

char *mcache_add(MSCONN conn, char *session, char *name, char *value);
char *mcache_muldiv(MSCONN conn,char *sn,char *nam,char *mul,char *div);

/** Lists all active sessions. */
char ** mcache_list(MSCONN conn);

/** Returns an associative array of session:var for all sessions which have
a setting of name. */
char **mcache_listvar(MSCONN conn, char *name);

/** Returns the number of active sessions */
int 	mcache_count(MSCONN conn);

/** Locks a session. Returns an integer key for the lock.
Locks remain exclusive for 30 seconds, after which someone else 
can lock the session. Locks are coperative, no enforcement
is attempted. */

int	mcache_lock(MSCONN conn, char *session);

/** Unlocks a session */
int	mcache_unlock(MSCONN conn, char *session, int key);


/** Sets the timeout of a session.
 * timeout = 0 means never timeout
 * timeout > 0 sets session expire time in seconds
 * timeout < 0 resets to default timeout
*/
Boolean mcache_timeout(MSCONN conn, char *session, int timeout);

/** Sets the session data string, creates a session if one does not exist */
int mcache_setdata(MSCONN conn, char *session, char *data);

/** Gets the session data string from a session */
char *mcache_getdata(MSCONN conn, char *session);

/** Returns the last error.  */
int mcache_errno(MSCONN conn);

/** Call a function plugin, return an int */
int mcache_call_int(MSCONN conn, char *fn,int n, char **strings);

/** Call a function plugin, return a char */
char *mcache_call_text(MSCONN conn, char *fn,int n, char **strings);

/** Call to get info about a session */
int mcache_ctl(MSCONN conn, char *session, int type);

/** Convert a string ctl to a type for mcache_ctl */
int mcache_ctl_type(char *str);

/** Call the personality plugin escape function, return int */
int mcache_plugin_int(MSCONN conn, char *session, char *nparam, char *sparam, int param);

/** Call the personality plugin escape function, return char * */
char *mcache_plugin_text(MSCONN conn, char *session, char *nparam, char *sparam, int param);

int mcache_flush(MSCONN conn, int seconds);
int mcache_loaddll(MSCONN conn, char *dll);
int mcache_shutdown(MSCONN conn, char *timeout);
int mcache_serialize(MSCONN conn, char *szparam);
char * mcache_exec(MSCONN conn, char *szparam);
int mcache_setserial(MSCONN conn, char *serialize);
int mcache_setmaxthreads(MSCONN conn, char *maxt);
int mcache_setverbose(MSCONN conn, char *verbose);
int mcache_setautosave(MSCONN conn, char *autsave);
int mcache_setautodrop(MSCONN conn, char *autdrop);
int mcache_ping(MSCONN conn);
int mcache_status(MSCONN conn);
Boolean mcache_statok(MSCONN conn);

char *mcache_text(MSCONN conn);

/** Returns a static character string describing an error. */
char *	mcache_errtext(MSCONN conn);
#endif

#ifdef __cplusplus
}
#endif
#endif

