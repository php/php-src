/*
 * nanohttp.c: minimalist FTP implementation to fetch external subsets.
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */
 
#ifndef __NANO_FTP_H__
#define __NANO_FTP_H__

#include <libxml/xmlversion.h>

#ifdef LIBXML_FTP_ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ftpListCallback: 
 * @userData:  user provided data for the callback
 * @filename:  the file name (including "->" when links are shown)
 * @attrib:  the attribute string
 * @owner:  the owner string
 * @group:  the group string
 * @size:  the file size
 * @links:  the link count
 * @year:  the year
 * @month:  the month
 * @day:  the day
 * @hour:  the hour
 * @minute:  the minute
 *
 * A callback for the xmlNanoFTPList command.
 * Note that only one of year and day:minute are specified.
 */
typedef void (*ftpListCallback) (void *userData,
	                         const char *filename, const char *attrib,
	                         const char *owner, const char *group,
				 unsigned long size, int links, int year,
				 const char *month, int day, int hour,
				 int minute);
/**
 * ftpDataCallback: 
 * @userData: the user provided context
 * @data: the data received
 * @len: its size in bytes
 *
 * A callback for the xmlNanoFTPGet command.
 */
typedef void (*ftpDataCallback) (void *userData,
				 const char *data,
				 int len);

/*
 * Init
 */
void	xmlNanoFTPInit		(void);
void	xmlNanoFTPCleanup	(void);

/*
 * Creating/freeing contexts.
 */
void *	xmlNanoFTPNewCtxt	(const char *URL);
void	xmlNanoFTPFreeCtxt	(void * ctx);
void * 	xmlNanoFTPConnectTo	(const char *server,
				 int port);
/*
 * Opening/closing session connections.
 */
void * 	xmlNanoFTPOpen		(const char *URL);
int	xmlNanoFTPConnect	(void *ctx);
int	xmlNanoFTPClose		(void *ctx);
int	xmlNanoFTPQuit		(void *ctx);
void	xmlNanoFTPScanProxy	(const char *URL);
void	xmlNanoFTPProxy		(const char *host,
				 int port,
				 const char *user,
				 const char *passwd,
				 int type);
int	xmlNanoFTPUpdateURL	(void *ctx,
				 const char *URL);

/*
 * Rather internal commands.
 */
int	xmlNanoFTPGetResponse	(void *ctx);
int	xmlNanoFTPCheckResponse	(void *ctx);

/*
 * CD/DIR/GET handlers.
 */
int	xmlNanoFTPCwd		(void *ctx,
				 char *directory);

int	xmlNanoFTPGetConnection	(void *ctx);
int	xmlNanoFTPCloseConnection(void *ctx);
int	xmlNanoFTPList		(void *ctx,
				 ftpListCallback callback,
				 void *userData,
				 char *filename);
int	xmlNanoFTPGetSocket	(void *ctx,
				 const char *filename);
int	xmlNanoFTPGet		(void *ctx,
				 ftpDataCallback callback,
				 void *userData,
				 const char *filename);
int	xmlNanoFTPRead		(void *ctx,
				 void *dest,
				 int len);

#ifdef __cplusplus
}
#endif /* LIBXML_FTP_ENABLED */
#endif
#endif /* __NANO_FTP_H__ */
