/* Copyright Abandoned 1997 TCX DataKonsult AB & Monty Program KB & Detron HB
   This file is public domain and comes with NO WARRANTY of any kind */

/* Error messages for MySQL clients */
/* error messages for the demon is in share/language/errmsg.sys */

#include <global.h>
#include <my_sys.h>
#include "errmsg.h"

#ifdef GERMAN
const char *client_errors[]=
{
  "Unbekannter MySQL Fehler",
  "Kann UNIX-Socket nicht anlegen (%d)",
  "Keine Verbindung zu lokalem MySQL Server, socket: '%-.64s' (%d)",
  "Keine Verbindung zu MySQL Server auf %-.64s (%d)",
  "Kann TCP/IP-Socket nicht anlegen (%d)",
  "Unbekannter MySQL Server Host (%-.64s) (%d)",
  "MySQL Server nicht vorhanden",
  "Protokolle ungleich. Server Version = % d Client Version = %d",
  "MySQL client got out of memory",
  "Wrong host info",
  "Localhost via UNIX socket",
  "%s via TCP/IP",
  "Error in server handshake",
  "Lost connection to MySQL server during query",
  "Commands out of sync; You can't run this command now",
  "Verbindung ueber Named Pipe; Host: %-.64s",
  "Kann nicht auf Named Pipe warten. Host: %-.64s  pipe: %-.32s (%lu)",
  "Kann Named Pipe nicht oeffnen. Host: %-.64s  pipe: %-.32s (%lu)",
  "Kann den Status der Named Pipe nicht setzen.  Host: %-.64s  pipe: %-.32s (%lu)"
};

#else /* ENGLISH */
const char *client_errors[]=
{
  "Unknown MySQL error",
  "Can't create UNIX socket (%d)",
  "Can't connect to local MySQL server through socket '%-.64s' (%d)",
  "Can't connect to MySQL server on '%-.64s' (%d)",
  "Can't create TCP/IP socket (%d)",
  "Unknown MySQL Server Host '%-.64s' (%d)",
  "MySQL server has gone away",
  "Protocol mismatch. Server Version = %d Client Version = %d",
  "MySQL client run out of memory",
  "Wrong host info",
  "Localhost via UNIX socket",
  "%s via TCP/IP",
  "Error in server handshake",
  "Lost connection to MySQL server during query",
  "Commands out of sync;  You can't run this command now",
  "%s via named pipe",
  "Can't wait for named pipe to host: %-.64s  pipe: %-.32s (%lu)",
  "Can't open named pipe to host: %-.64s  pipe: %-.32s (%lu)",
  "Can't set state of named pipe to host: %-.64s  pipe: %-.32s (%lu)",
};
#endif


void init_client_errs(void)
{
  errmsg[CLIENT_ERRMAP] = &client_errors[0];
}
