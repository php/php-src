/***************************************************************************/
/*									   */
/* ftplib.h - header file for callable ftp access routines                 */
/* Copyright (C) 1996, 1997 Thomas Pfau, pfau@cnj.digex.net                */
/*	73 Catherine Street, South Bound Brook, NJ, 08880		   */
/*									   */
/* This library is free software; you can redistribute it and/or	   */
/* modify it under the terms of the GNU Library General Public		   */
/* License as published by the Free Software Foundation; either		   */
/* version 2 of the License, or (at your option) any later version.	   */
/* 									   */
/* This library is distributed in the hope that it will be useful,	   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of	   */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU	   */
/* Library General Public License for more details.			   */
/* 									   */
/* You should have received a copy of the GNU Library General Public	   */
/* License along with this progam; if not, write to the			   */
/* Free Software Foundation, Inc., 59 Temple Place - Suite 330,		   */
/* Boston, MA 02111-1307, USA.						   */
/*									   */
/***************************************************************************/

/* Adapted by Andrew Skalski <askalski@chek.com> for use with PHP. */

#if !defined(__FTPLIB_H)
#define __FTPLIB_H

#if defined(__unix__) || defined(VMS)
#define GLOBALDEF
#define GLOBALREF extern
#elif defined(_WIN32)
#if defined BUILDING_LIBRARY
#define GLOBALDEF __declspec(dllexport)
#define GLOBALREF __declspec(dllexport)
#else
#define GLOBALREF __declspec(dllimport)
#endif
#endif

/* FtpAccess() type codes */
#define FTPLIB_DIR 1
#define FTPLIB_DIR_VERBOSE 2
#define FTPLIB_FILE_READ 3
#define FTPLIB_FILE_WRITE 4

/* FtpAccess() mode codes */
#define FTPLIB_ASCII 'A'
#define FTPLIB_IMAGE 'I'
#define FTPLIB_TEXT FTPLIB_ASCII
#define FTPLIB_BINARY FTPLIB_IMAGE

/* connection modes */
#define FTPLIB_PASSIVE 1
#define FTPLIB_PORT 2

/* connection option names */
#define FTPLIB_CONNMODE 1
#define FTPLIB_CALLBACK 2
#define FTPLIB_IDLETIME 3
#define FTPLIB_CALLBACKARG 4
#define FTPLIB_CALLBACKBYTES 5

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NetBuf netbuf;
typedef int (*FtpCallback)(netbuf *nControl, int xfered, void *arg);

#define	_FTPLIB_NO_COMPAT 1

/* v1 compatibility stuff */
#if !defined(_FTPLIB_NO_COMPAT)
netbuf *DefaultNetbuf;

#define ftplib_lastresp FtpLastResponse(DefaultNetbuf)
#define ftpInit FtpInit
#define ftpOpen(x) FtpConnect(x, &DefaultNetbuf)
#define ftpLogin(x,y) FtpLogin(x, y, DefaultNetbuf)
#define ftpSite(x) FtpSite(x, DefaultNetbuf)
#define ftpMkdir(x) FtpMkdir(x, DefaultNetbuf)
#define ftpChdir(x) FtpChdir(x, DefaultNetbuf)
#define ftpRmdir(x) FtpRmdir(x, DefaultNetbuf)
#define ftpNlst(x, y) FtpNlst(x, y, DefaultNetbuf)
#define ftpDir(x, y) FtpDir(x, y, DefaultNetbuf)
#define ftpGet(x, y, z) FtpGet(x, y, z, DefaultNetbuf)
#define ftpPut(x, y, z) FtpPut(x, y, z, DefaultNetbuf)
#define ftpRename(x, y) FtpRename(x, y, DefaultNetbuf)
#define ftpDelete(x) FtpDelete(x, DefaultNetbuf)
#define ftpQuit() FtpQuit(DefaultNetbuf)
#endif /* (_FTPLIB_NO_COMPAT) */
/* end v1 compatibility stuff */

GLOBALREF int ftplib_debug;
GLOBALREF void FtpInit(void);
GLOBALREF char *FtpLastResponse(netbuf *nControl);
GLOBALREF int FtpConnect(const char *host, netbuf **nControl);
GLOBALREF int FtpOptions(int opt, long val, netbuf *nControl);
GLOBALREF int FtpLogin(const char *user, const char *pass, netbuf *nControl);
GLOBALREF int FtpAccess(const char *path, int typ, int mode, netbuf *nControl,
    netbuf **nData);
GLOBALREF int FtpRead(void *buf, int max, netbuf *nData);
GLOBALREF int FtpWrite(void *buf, int len, netbuf *nData);
GLOBALREF int FtpClose(netbuf *nData, int readResp);
GLOBALREF int FtpSite(const char *cmd, netbuf *nControl);
GLOBALREF int FtpSysType(char *buf, int max, netbuf *nControl);
GLOBALREF int FtpMkdir(const char *path, netbuf *nControl);
GLOBALREF int FtpChdir(const char *path, netbuf *nControl);
GLOBALREF int FtpCDUp(netbuf *nControl);
GLOBALREF int FtpRmdir(const char *path, netbuf *nControl);
GLOBALREF int FtpPwd(char *path, int max, netbuf *nControl);
GLOBALREF int FtpNlst(FILE *outfp, const char *path, netbuf *nControl);
GLOBALREF int FtpDir(FILE *outfp, const char *path, netbuf *nControl);
GLOBALREF int FtpSize(const char *path, int *size, char mode, netbuf *nControl);
GLOBALREF int FtpModDate(const char *path, char *dt, int max, netbuf *nControl);
GLOBALREF int FtpGet(FILE *outfp, const char *path, char mode,
	netbuf *nControl);
GLOBALREF int FtpPut(FILE *infp, const char *path, char mode,
	netbuf *nControl);
GLOBALREF int FtpRename(const char *src, const char *dst, netbuf *nControl);
GLOBALREF int FtpDelete(const char *fnm, netbuf *nControl);
GLOBALREF void FtpQuit(netbuf *nControl);

#ifdef __cplusplus
};
#endif

#endif /* __FTPLIB_H */
