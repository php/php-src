/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef HW_ERROR_H
#define HW_ERROR_H

#if HYPERWAVE

#define NOACCESS 1 /* Access denied */
#define NODOCS 2 /* No documents */
#define NONAME 3 /* No collection name */
#define NODOC 4 /* Object is not a document */
#define NOOBJ 5 /* No object received */
#define NOCOLLS 6 /* No collections received */
#define DBSTUBNG 7 /* Connection to low-level database failed */
#define NOTFOUND 8 /* Object not found */
#define EXIST 9 /* Collection already exists */
#define FATHERDEL 10 /* parent collection disappeared */
#define FATHNOCOLL 11 /* parent collection not a collection */
#define NOTEMPTY 12 /* Collection not empty */
#define DESTNOCOLL 13 /* Destination not a collection */
#define SRCEQDEST 14 /* Source equals destination */
#define REQPEND 15 /* Request pending */
#define TIMEOUT 16 /* Timeout */
#define NAMENOTUNIQUE 17 /* Name not unique */
#define WRITESTOPPED 18 /* Database now read-only; try again later */
#define LOCKED 19 /* Object locked; try again later */
#define CHANGEBASEFLD 20 /* Change of base-attribute */
#define NOTREMOVED 21 /* Attribute not removed */
#define FLDEXISTS 22 /* Attribute exists */
#define CMDSYNTAX 23 /* Syntax error in command */
#define NOLANGUAGE 24 /* No or unknown language specified */
#define WRGTYPE 25 /* Wrong type in object */
#define WRGVERSION 26 /* Client version too old */
#define CONNECTION 27 /* No connection to other server */
#define SYNC 28 /* Synchronization error */
#define NOPATH 29 /* No path entry */
#define WRGPATH 30 /* Wrong path entry */
#define PASSWD 31 /* Wrong password (server-to-server server authentication) */
#define LC_NO_MORE_USERS 32 /* No more users for license */
#define LC_NO_MORE_DOCS 33 /* No more documents for this session and license */
#define RSERV_NRESP 34 /* Remote server not responding */
#define Q_OVERFLOW 35 /* Query overflow */
#define USR_BREAK 36 /* Break by user */
#define N_IMPL 37 /* Not implemented */
#define WRG_VALUE 38 /* Wrong value */
#define INSUFF_FUNDS 39 /* Insufficient funds */
#define REORG 40 /* Reorganization in progress */
#define USER_LIMIT 41 /* Limit of simultaneous users reached */
#define FTCONNECT 513 /* No connection to fulltext server */
#define FTTIMEOUT 514 /* Connection timed out */
#define FTINDEX 515 /* Something wrong with fulltext index */
#define FTSYNTAX 516 /* Query syntax error */
#define REQUESTPENDING 1025 /* Request pending */
#define NOCONNECTION 1026 /* No connection to document server */
#define WRONGVERSION 1027 /* Wrong protocol version */
#define NOTINITIALIZED 1028 /* Not initialized */
#define BADREQUEST 1029 /* Bad request */
#define BADLRN 1030 /* Bad document number */
#define OPENSTORE_WRITE 1031 /* Cannot write to local store */
#define OPENSTORE_READ 1032 /* Cannot read from local store */
#define READSTORE 1033 /* Store read error */
#define WRITESTORE 1034 /* Write error */
#define CLOSESTORE 1035 /* Close error */
#define BADPATH 1036 /* Bad path */
#define NOPATHDC 1037 /* No path */
#define OPENFILE 1038 /* Cannot open file */
#define READFILE 1039 /* Cannot read from file // same */
#define WRITEFILE 1040 /* Cannot write to file */
#define CONNECTCLIENT 1041 /* Could not connect to client */
#define ACCEPT 1042 /* Could not accept connection */
#define READSOCKET 1043 /* Could not read from socket */
#define WRITESOCKET 1044 /* Could not write to socket */
#define TOOMUCHDATA 1046 /* Received too much data */
#define TOOFEWDATA 1047 /* Received too few data // ... */
#define NOTIMPLEMENTED 1049 /* Not implemented */
#define USERBREAK 1050 /* User break  */
#define INTERNAL 1051 /* Internal error */
#define INVALIDOBJECT 1052 /* Invalid object */
#define JOBTIMEOUT 1053 /* Job timed out */
#define OPENPORT 1054 /* Cannot open port // ... for several resons */
#define NODATA 1055 /* Received no data  */
#define NOPORT 1056 /* No port to handle this request */
#define NOTCACHED 1057 /* Document not cached */
#define BADCACHETYPE 1058 /* Bad cache type  */
#define OPENCACHE_WRITE 1059 /* Cannot write to cache */
#define OPENCACHE_READ 1060 /* Cannot read from cache // same */
#define NOSOURCE 1061 /* Do not know what to read */
#define CLOSECACHE 1062 /* Could not insert into cache  */
#define CONNECTREMOTE 1063 /* Could not connect to remote server */
#define LOCKREFUSED 1064 /* Lock refused // could not lock the stores */

#endif
#endif
