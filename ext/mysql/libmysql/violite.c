/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/*
  Note that we can't have assertion on file descriptors;  The reason for
  this is that during mysql shutdown, another thread can close a file
  we are working on.  In this case we should just return read errors from
  the file descriptior.
*/

#include <global.h>

#ifndef HAVE_VIO			/* is Vio suppored by the Vio lib ? */

#include <errno.h>
#include <assert.h>
#include <violite.h>
#include <my_sys.h>
#include <my_net.h>
#include <m_string.h>
#ifdef HAVE_POLL
#include <sys/poll.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#if !defined(MSDOS) && !defined(__WIN__) && !defined(HAVE_BROKEN_NETINET_INCLUDES) && !defined(__BEOS__) && !defined(__FreeBSD__)
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#if !defined(alpha_linux_port)
#include <netinet/tcp.h>
#endif
#endif

#if defined(__EMX__) || defined(OS2)
#define ioctlsocket ioctl
#endif	/* defined(__EMX__) */

#if defined(MSDOS) || defined(__WIN__)
#define O_NONBLOCK 1    /* For emulation of fcntl() */
#endif
#ifndef EWOULDBLOCK
#define SOCKET_EWOULDBLOCK SOCKET_EAGAIN
#endif

#ifndef __WIN__
#define HANDLE void *
#endif

struct st_vio
{
  my_socket		sd;		/* my_socket - real or imaginary */
  HANDLE hPipe;
  my_bool		localhost;	/* Are we from localhost? */
  int			fcntl_mode;	/* Buffered fcntl(sd,F_GETFL) */
  struct sockaddr_in	local;		/* Local internet address */
  struct sockaddr_in	remote;		/* Remote internet address */
  enum enum_vio_type	type;		/* Type of connection */
  char			desc[30];	/* String description */
};

typedef void *vio_ptr;
typedef char *vio_cstring;

/*
 * Helper to fill most of the Vio* with defaults.
 */

static void vio_reset(Vio* vio, enum enum_vio_type type,
		      my_socket sd, HANDLE hPipe,
		      my_bool localhost)
{
  bzero((char*) vio, sizeof(*vio));
  vio->type	= type;
  vio->sd	= sd;
  vio->hPipe	= hPipe;
  vio->localhost= localhost;
}

/* Open the socket or TCP/IP connection and read the fnctl() status */

Vio *vio_new(my_socket sd, enum enum_vio_type type, my_bool localhost)
{
  Vio *vio;
  DBUG_ENTER("vio_new");
  DBUG_PRINT("enter", ("sd=%d", sd));
  if ((vio = (Vio*) my_malloc(sizeof(*vio),MYF(MY_WME))))
  {
    vio_reset(vio, type, sd, 0, localhost);
    sprintf(vio->desc,
	    (vio->type == VIO_TYPE_SOCKET ? "socket (%d)" : "TCP/IP (%d)"),
	    vio->sd);
#if !defined(___WIN__) && !defined(__EMX__) && !defined(OS2)
#if !defined(NO_FCNTL_NONBLOCK)
    vio->fcntl_mode = fcntl(sd, F_GETFL);
#elif defined(HAVE_SYS_IOCTL_H)			/* hpux */
    /* Non blocking sockets doesn't work good on HPUX 11.0 */
    (void) ioctl(sd,FIOSNBIO,0);
#endif
#else /* !defined(__WIN__) && !defined(__EMX__) */
    {
      /* set to blocking mode by default */
      ulong arg=0, r;
      r = ioctlsocket(vio->sd,FIONBIO,(void*) &arg, sizeof(arg));
    }
#endif
  }
  DBUG_RETURN(vio);
}


#ifdef __WIN__

Vio *vio_new_win32pipe(HANDLE hPipe)
{
  Vio *vio;
  DBUG_ENTER("vio_new_handle");
  if ((vio = (Vio*) my_malloc(sizeof(Vio),MYF(MY_WME))))
  {
    vio_reset(vio, VIO_TYPE_NAMEDPIPE, 0, hPipe, TRUE);
    strmov(vio->desc, "named pipe");
  }
  DBUG_RETURN(vio);
}

#endif

void vio_delete(Vio * vio)
{
  /* It must be safe to delete null pointers. */
  /* This matches the semantics of C++'s delete operator. */
  if (vio)
  {
    if (vio->type != VIO_CLOSED)
      vio_close(vio);
    my_free((gptr) vio,MYF(0));
  }
}

int vio_errno(Vio *vio __attribute__((unused)))
{
  return socket_errno;		/* On Win32 this mapped to WSAGetLastError() */
}


int vio_read(Vio * vio, gptr buf, int size)
{
  int r;
  DBUG_ENTER("vio_read");
  DBUG_PRINT("enter", ("sd=%d  size=%d", vio->sd, size));
#if defined( __WIN__) || defined(OS2)
  if (vio->type == VIO_TYPE_NAMEDPIPE)
  {
    DWORD length;
#ifdef OS2
    if (!DosRead((HFILE)vio->hPipe, buf, size, &length))
      DBUG_RETURN(-1);
#else
    if (!ReadFile(vio->hPipe, buf, size, &length, NULL))
      DBUG_RETURN(-1);
#endif
    DBUG_RETURN(length);
  }
  r = recv(vio->sd, buf, size,0);
#else
  errno=0;					/* For linux */
  r = read(vio->sd, buf, size);
#endif /* __WIN__ */
#ifndef DBUG_OFF
  if (r < 0)
  {
    DBUG_PRINT("vio_error", ("Got error %d during read",socket_errno));
  }
#endif /* DBUG_OFF */
  DBUG_PRINT("exit", ("%d", r));
  DBUG_RETURN(r);
}


int vio_write(Vio * vio, const gptr buf, int size)
{
  int r;
  DBUG_ENTER("vio_write");
  DBUG_PRINT("enter", ("sd=%d  size=%d", vio->sd, size));
#if defined( __WIN__) || defined(OS2)
  if ( vio->type == VIO_TYPE_NAMEDPIPE)
  {
    DWORD length;
#ifdef OS2
    if (!DosWrite((HFILE)vio->hPipe, (char*) buf, size, &length))
      DBUG_RETURN(-1);
#else
    if (!WriteFile(vio->hPipe, (char*) buf, size, &length, NULL))
      DBUG_RETURN(-1);
#endif
    DBUG_RETURN(length);
  }
  r = send(vio->sd, buf, size,0);
#else
  r = write(vio->sd, buf, size);
#endif /* __WIN__ */
#ifndef DBUG_OFF
  if (r < 0)
  {
    DBUG_PRINT("vio_error", ("Got error on write: %d",socket_errno));
  }
#endif /* DBUG_OFF */
  DBUG_PRINT("exit", ("%d", r));
  DBUG_RETURN(r);
}


int vio_blocking(Vio * vio, my_bool set_blocking_mode)
{
  int r=0;
  DBUG_ENTER("vio_blocking");
  DBUG_PRINT("enter", ("set_blocking_mode: %d", (int) set_blocking_mode));

#if !defined(___WIN__) && !defined(__EMX__) && !defined(OS2)
#if !defined(NO_FCNTL_NONBLOCK)

  if (vio->sd >= 0)
  {
    int old_fcntl=vio->fcntl_mode;
    if (set_blocking_mode)
      vio->fcntl_mode &= ~O_NONBLOCK; /* clear bit */
    else
      vio->fcntl_mode |= O_NONBLOCK; /* set bit */
    if (old_fcntl != vio->fcntl_mode)
      r = fcntl(vio->sd, F_SETFL, vio->fcntl_mode);
  }
#endif /* !defined(NO_FCNTL_NONBLOCK) */
#else /* !defined(__WIN__) && !defined(__EMX__) */
#ifndef __EMX__
  if (vio->type != VIO_TYPE_NAMEDPIPE)  
#endif
  { 
    ulong arg;
    int old_fcntl=vio->fcntl_mode;
    if (set_blocking_mode)
    {
      arg = 0;
      vio->fcntl_mode &= ~O_NONBLOCK; /* clear bit */
    }
    else
    {
      arg = 1;
      vio->fcntl_mode |= O_NONBLOCK; /* set bit */
    }
    if (old_fcntl != vio->fcntl_mode)
      r = ioctlsocket(vio->sd,FIONBIO,(void*) &arg, sizeof(arg));
  }
#endif /* !defined(__WIN__) && !defined(__EMX__) */
  DBUG_RETURN(r);
}

my_bool
vio_is_blocking(Vio * vio)
{
  my_bool r;
  DBUG_ENTER("vio_is_blocking");
  r = !(vio->fcntl_mode & O_NONBLOCK);
  DBUG_PRINT("exit", ("%d", (int) r));
  DBUG_RETURN(r);
}


int vio_fastsend(Vio * vio __attribute__((unused)))
{
  int r=0;
  DBUG_ENTER("vio_fastsend");

#ifdef IPTOS_THROUGHPUT
  {
#ifndef __EMX__
    int tos = IPTOS_THROUGHPUT;
    if (!setsockopt(vio->sd, IPPROTO_IP, IP_TOS, (void *) &tos, sizeof(tos)))
#endif				/* !__EMX__ */
    {
      int nodelay = 1;
      if (setsockopt(vio->sd, IPPROTO_TCP, TCP_NODELAY, (void *) &nodelay,
		     sizeof(nodelay))) {
	DBUG_PRINT("warning",
		   ("Couldn't set socket option for fast send"));
	r= -1;
      }
    }
  }
#endif	/* IPTOS_THROUGHPUT */
  DBUG_PRINT("exit", ("%d", r));
  DBUG_RETURN(r);
}

int vio_keepalive(Vio* vio, my_bool set_keep_alive)
{
  int r=0;
  uint opt = 0;
  DBUG_ENTER("vio_keepalive");
  DBUG_PRINT("enter", ("sd=%d  set_keep_alive=%d", vio->sd, (int)
		       set_keep_alive));
  if (vio->type != VIO_TYPE_NAMEDPIPE)
  {
    if (set_keep_alive)
      opt = 1;
    r = setsockopt(vio->sd, SOL_SOCKET, SO_KEEPALIVE, (char *) &opt,
		   sizeof(opt));
  }
  DBUG_RETURN(r);
}


my_bool
vio_should_retry(Vio * vio __attribute__((unused)))
{
  int en = socket_errno;
  return en == SOCKET_EAGAIN || en == SOCKET_EINTR || en == SOCKET_EWOULDBLOCK;
}


int vio_close(Vio * vio)
{
  int r;
  DBUG_ENTER("vio_close");
#ifdef __WIN__
  if (vio->type == VIO_TYPE_NAMEDPIPE)
  {
#if defined(__NT__) && defined(MYSQL_SERVER)
    CancelIo(vio->hPipe);
    DisconnectNamedPipe(vio->hPipe);
#endif
    r=CloseHandle(vio->hPipe);
  }
  else if (vio->type != VIO_CLOSED)
#endif /* __WIN__ */
  {
    r=0;
    if (shutdown(vio->sd,2))
      r= -1;
    if (closesocket(vio->sd))
      r= -1;
  }
  if (r)
  {
    DBUG_PRINT("vio_error", ("close() failed, error: %d",socket_errno));
    /* FIXME: error handling (not critical for MySQL) */
  }
  vio->type= VIO_CLOSED;
  vio->sd=   -1;
  DBUG_RETURN(r);
}


const char *vio_description(Vio * vio)
{
  return vio->desc;
}

enum enum_vio_type vio_type(Vio* vio)
{
  return vio->type;
}

my_socket vio_fd(Vio* vio)
{
  return vio->sd;
}


my_bool vio_peer_addr(Vio * vio, char *buf)
{
  DBUG_ENTER("vio_peer_addr");
  DBUG_PRINT("enter", ("sd=%d", vio->sd));
  if (vio->localhost)
  {
    strmov(buf,"127.0.0.1");
  }
  else
  {
    size_socket addrLen = sizeof(struct sockaddr);
    if (getpeername(vio->sd, (struct sockaddr *) (& (vio->remote)),
		    &addrLen) != 0)
    {
      DBUG_PRINT("exit", ("getpeername, error: %d", socket_errno));
      DBUG_RETURN(1);
    }
    my_inet_ntoa(vio->remote.sin_addr,buf);
  }
  DBUG_PRINT("exit", ("addr=%s", buf));
  DBUG_RETURN(0);
}


void vio_in_addr(Vio *vio, struct in_addr *in)
{
  DBUG_ENTER("vio_in_addr");
  if (vio->localhost)
    bzero((char*) in, sizeof(*in));	/* This should never be executed */
  else
    *in=vio->remote.sin_addr;
  DBUG_VOID_RETURN;
}


/* Return 0 if there is data to be read */

my_bool vio_poll_read(Vio *vio,uint timeout)
{
#ifndef HAVE_POLL
  return 0;
#else
  struct pollfd fds;
  int res;
  DBUG_ENTER("vio_poll");
  fds.fd=vio->sd;
  fds.events=POLLIN;
  fds.revents=0;
  if ((res=poll(&fds,1,(int) timeout*1000)) <= 0)
  {
    DBUG_RETURN(res < 0 ? 0 : 1);		/* Don't return 1 on errors */
  }
  DBUG_RETURN(fds.revents & POLLIN ? 0 : 1);
#endif
}

#endif /* HAVE_VIO */
