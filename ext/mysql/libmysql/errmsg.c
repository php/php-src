/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
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
  "%-.64s via TCP/IP",
  "Error in server handshake",
  "Lost connection to MySQL server during query",
  "Commands out of sync; You can't run this command now",
  "Verbindung ueber Named Pipe; Host: %-.64s",
  "Kann nicht auf Named Pipe warten. Host: %-.64s  pipe: %-.32s (%lu)",
  "Kann Named Pipe nicht oeffnen. Host: %-.64s  pipe: %-.32s (%lu)",
  "Kann den Status der Named Pipe nicht setzen.  Host: %-.64s  pipe: %-.32s (%lu)",
  "Can't initialize character set %-.64s (path: %-.64s)",
  "Got packet bigger than 'max_allowed_packet'"
};

/* Start of code added by Roberto M. Serqueira - martinsc@uol.com.br - 05.24.2001 */

#elif defined PORTUGUESE
const char *client_errors[]=
{
  "Erro desconhecido do MySQL",
  "Não pode criar 'UNIX socket' (%d)",
  "Não pode se conectar ao servidor MySQL local através do 'socket' '%-.64s' (%d)", 
  "Não pode se conectar ao servidor MySQL em '%-.64s' (%d)",
  "Não pode criar 'socket TCP/IP' (%d)",
  "'Host' servidor MySQL '%-.64s' (%d) desconhecido", 
  "Servidor MySQL desapareceu",
  "Incompatibilidade de protocolos. Versão do Servidor: %d - Versão do Cliente: %d",
  "Cliente do MySQL com falta de memória",
  "Informação inválida de 'host'",
  "Localhost via 'UNIX socket'",
  "%-.64s via 'TCP/IP'",
  "Erro na negociação de acesso ao servidor",
  "Conexão perdida com servidor MySQL durante 'query'",
  "Comandos fora de sincronismo. Você não pode executar este comando agora",
  "%-.64s via 'named pipe'",
  "Não pode esperar pelo 'named pipe' para o 'host' %-.64s - 'pipe' %-.32s (%lu)",
  "Não pode abrir 'named pipe' para o 'host' %-.64s - 'pipe' %-.32s (%lu)",
  "Não pode estabelecer o estado do 'named pipe' para o 'host' %-.64s - 'pipe' %-.32s (%lu)",
  "Não pode inicializar conjunto de caracteres %-.64s (caminho %-.64s)",
  "Obteve pacote maior do que 'max_allowed_packet'"
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
  "%-.64s via TCP/IP",
  "Error in server handshake",
  "Lost connection to MySQL server during query",
  "Commands out of sync;  You can't run this command now",
  "%-.64s via named pipe",
  "Can't wait for named pipe to host: %-.64s  pipe: %-.32s (%lu)",
  "Can't open named pipe to host: %-.64s  pipe: %-.32s (%lu)",
  "Can't set state of named pipe to host: %-.64s  pipe: %-.32s (%lu)",
  "Can't initialize character set %-.64s (path: %-.64s)",
  "Got packet bigger than 'max_allowed_packet'"
};
#endif


void init_client_errs(void)
{
  my_errmsg[CLIENT_ERRMAP] = &client_errors[0];
}
