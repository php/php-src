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
   | Authors: Novell, Inc.                                                |
   +----------------------------------------------------------------------+
*/


#ifndef __sys_stat_h__
#define __sys_stat_h__


#include <time.h>
#include <stddef.h>
#include <sys/mode.h>
#include <sys/time.h>

#include <pshpack1.h>


#ifdef CLIB_STAT_PATCH

/***************** stat structure taken from CLib and modified ***************/
struct stat 
{
   long           st_dev;        /* volume number                         */    /* dev_t replaced by long */
   MACHINE_WORD   st_ino;        /* directory entry number of the st_name */    /* ino_t replaced by MACHINE_WORD */
   unsigned short st_mode;       /* emulated file mode                    */
   unsigned short st_pad1;       /* reserved for alignment                */
   unsigned long  st_nlink;      /* count of hard links (always 1)        */
   unsigned long  st_uid;        /* object id of owner                    */
   unsigned long  st_gid;        /* group-id (always 0)                   */
   long           st_rdev;       /* device type (always 0)                */    /* dev_t replaced by long */
   off_t          st_size;       /* total file size--files only           */
   time_t         st_atime;      /* last access date--files only          */
   time_t         st_mtime;      /* last modify date and time             */
   time_t         st_ctime;      /* POSIX: last status change time...     */
                                 /* ...NetWare: creation date/time        */
   time_t         st_btime;      /* last archived date and time           */
   unsigned long  st_attr;       /* file attributes                       */
   unsigned long  st_archivedID; /* user/object ID of last archive        */
   unsigned long  st_updatedID;  /* user/object ID of last update         */
   unsigned short st_inheritedRightsMask;  /* inherited rights mask       */
   unsigned short st_pad2;       /* reserved for alignment                */
   unsigned int   st_originatingNameSpace; /* namespace of creation       */
   size_t         st_blksize;    /* block size for allocation--files only */
   size_t         st_blocks;     /* count of blocks allocated to file     */
   unsigned int   st_flags;      /* user-defined flags                    */
   unsigned long  st_spare[4];   /* for future use                        */
   unsigned char  st_name[255+1];/* TARGET_NAMESPACE name                 */
};
/***************** stat structure taken from CLib and modified ***************/

struct stat_libc
{
   uint32_t   st_userspec; /* untouched by stat()                   */
   uint32_t   st_flags;    /* flags for this entry                  */
   mode_t     st_mode;     /* emulated file mode                    */
   uint32_t   st_spare1;
   uint64_t   st_gen;      /* generation number of inode            */
   ino_t      st_ino;      /* directory entry number                */
   dev_t      st_dev;      /* volume number                         */
   dev_t      st_rdev;     /* device type (always 0)                */
   off64_t    st_size;     /* total file size                       */
   uint64_t   st_spare2;
   blkcnt_t   st_blocks;   /* count of blocks allocated to file     */
   blksize_t  st_blksize;  /* block size for allocation--files only */
   nlink_t    st_nlink;    /* count of hard links (always 1)        */
   uint32_t   st_spare3[3];
   uid_t      st_uid;      /* owner (object) identity               */
   gid_t      st_gid;      /* group-id (always 0)                   */
   uid_t      st_bid;      /* identity of last archiver             */
   uid_t      st_mid;      /* identity of last updator              */
   timespec_t st_atime;    /* last access date--files only          */
   timespec_t st_mtime;    /* last modify date and time             */
   timespec_t st_ctime;    /* last file attributes modification     */
   timespec_t st_btime;    /* last archived date and time           */
   uint64_t   st_spare4[44];
};                         /* sizeof(struct dirent) == 0x200 (512.) */

#else	/* CLIB_STAT_PATCH */

struct stat
{
   uint32_t   st_userspec; /* untouched by stat()                   */
   uint32_t   st_flags;    /* flags for this entry                  */
   mode_t     st_mode;     /* emulated file mode                    */
   uint32_t   st_spare1;
   uint64_t   st_gen;      /* generation number of inode            */
   ino_t      st_ino;      /* directory entry number                */
   dev_t      st_dev;      /* volume number                         */
   dev_t      st_rdev;     /* device type (always 0)                */
   off64_t    st_size;     /* total file size                       */
   uint64_t   st_spare2;
   blkcnt_t   st_blocks;   /* count of blocks allocated to file     */
   blksize_t  st_blksize;  /* block size for allocation--files only */
   nlink_t    st_nlink;    /* count of hard links (always 1)        */
   uint32_t   st_spare3[3];
   uid_t      st_uid;      /* owner (object) identity               */
   gid_t      st_gid;      /* group-id (always 0)                   */
   uid_t      st_bid;      /* identity of last archiver             */
   uid_t      st_mid;      /* identity of last updator              */
   timespec_t st_atime;    /* last access date--files only          */
   timespec_t st_mtime;    /* last modify date and time             */
   timespec_t st_ctime;    /* last file attributes modification     */
   timespec_t st_btime;    /* last archived date and time           */
   uint64_t   st_spare4[44];
};                         /* sizeof(struct dirent) == 0x200 (512.) */

#endif	/* CLIB_STAT_PATCH */


#include <poppack.h>

#ifdef __cplusplus
extern "C"
{
#endif

int    chmod ( const char *path,     mode_t mode        );
int    fchmod( int fildes,           mode_t mode        );
int    fstat ( int fildes,           struct stat *buf );
int    mkdir ( const char *pathname, mode_t mode        );
int    mkfifo( const char *pathname, mode_t mode        );
int    stat  ( const char *path,     struct stat *buf );
mode_t umask (                       mode_t cmask       );

#ifdef __cplusplus
}
#endif

#define stat64  stat       /* same structure and function do both   */
#define fstat64 fstat      /* same function does both               */


#endif	/* __sys_stat_h__ */
