/* Copyright (C) 2000 MySQL AB & MySQL Finland AB & TCX DataKonsult AB
   
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
   MA 02111-1307, USA */

/* Error messages for mysql clients */
/* error messages for the demon is in share/language/errmsg.sys */

#ifdef	__cplusplus
extern "C" {
#endif
void	init_client_errs(void);
extern const char *client_errors[];	/* Error messages */
#ifdef	__cplusplus
}
#endif

#define CR_MIN_ERROR		2000	/* For easier client code */
#define CR_MAX_ERROR		2999
#define ER(X) client_errors[(X)-CR_MIN_ERROR]
#define CLIENT_ERRMAP		2	/* Errormap used by my_error() */

#define CR_UNKNOWN_ERROR	2000
#define CR_SOCKET_CREATE_ERROR	2001
#define CR_CONNECTION_ERROR	2002
#define CR_CONN_HOST_ERROR	2003
#define CR_IPSOCK_ERROR		2004
#define CR_UNKNOWN_HOST		2005
#define CR_SERVER_GONE_ERROR	2006
#define CR_VERSION_ERROR	2007
#define CR_OUT_OF_MEMORY	2008
#define CR_WRONG_HOST_INFO	2009
#define CR_LOCALHOST_CONNECTION 2010
#define CR_TCP_CONNECTION	2011
#define CR_SERVER_HANDSHAKE_ERR 2012
#define CR_SERVER_LOST		2013
#define CR_COMMANDS_OUT_OF_SYNC 2014
#define CR_NAMEDPIPE_CONNECTION 2015
#define CR_NAMEDPIPEWAIT_ERROR 2016
#define CR_NAMEDPIPEOPEN_ERROR 2017
#define CR_NAMEDPIPESETSTATE_ERROR 2018
#define CR_CANT_READ_CHARSET	2019
#define CR_NET_PACKET_TOO_LARGE 2020
