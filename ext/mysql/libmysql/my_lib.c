/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/* TODO: check for overun of memory for names. */
/*	 Convert MSDOS-TIME to standar time_t */

#define USES_TYPES		/* sys/types is included */
#include	"mysys_priv.h"
#include	<m_string.h>
#include	<my_dir.h>	/* Structs used by my_dir,includes sys/types */
#include	"mysys_err.h"
#if defined(HAVE_DIRENT_H)
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
#ifndef OS2
# define dirent direct
#endif
# define NAMLEN(dirent) (dirent)->d_namlen
# if defined(HAVE_SYS_NDIR_H)
#  include <sys/ndir.h>
# endif
# if defined(HAVE_SYS_DIR_H)
#  include <sys/dir.h>
# endif
# if defined(HAVE_NDIR_H)
#  include <ndir.h>
# endif
# if defined(MSDOS) || defined(__WIN__)
# include <dos.h>
# ifdef __BORLANDC__
# include <dir.h>
# endif
# endif
#endif
#ifdef VMS
#include <rms.h>
#include <iodef.h>
#include <descrip.h>
#endif

#ifdef OS2
#include "my_os2dirsrch.h"
#endif

#if defined(THREAD) && defined(HAVE_READDIR_R)
#define READDIR(A,B,C) ((errno=readdir_r(A,B,&C)) != 0 || !C)
#else
#define READDIR(A,B,C) (!(C=readdir(A)))
#endif


#define STARTSIZE	ONCE_ALLOC_INIT*8  /* some mallocmargin */

static int	comp_names(struct fileinfo *a,struct fileinfo *b);


	/* We need this because program don't know with malloc we used */

void my_dirend(MY_DIR *buffer)
{
  DBUG_ENTER("my_dirend");
  if (buffer)
    my_free((gptr) buffer,MYF(0));
  DBUG_VOID_RETURN;
} /* my_dirend */


	/* Compare in sort of filenames */

static int comp_names(struct fileinfo *a, struct fileinfo *b)
{
  return (strcmp(a->name,b->name));
} /* comp_names */


#if !defined(MSDOS) && !defined(__WIN__)

MY_DIR	*my_dir(const char *path, myf MyFlags)
{
  DIR		*dirp;
  struct dirent *dp;
  struct fileinfo *fnames;
  char	       *buffer, *obuffer, *tempptr;
  uint		fcnt,i,size,firstfcnt, maxfcnt,length;
  char		tmp_path[FN_REFLEN+1],*tmp_file;
  my_ptrdiff_t	diff;
  bool		eof;
#ifdef THREAD
  char	dirent_tmp[sizeof(struct dirent)+_POSIX_PATH_MAX+1];
#endif
  DBUG_ENTER("my_dir");
  DBUG_PRINT("my",("path: '%s' stat: %d  MyFlags: %d",path,MyFlags));

#if defined(THREAD) && !defined(HAVE_READDIR_R)
  pthread_mutex_lock(&THR_LOCK_open);
#endif

  dirp = opendir(directory_file_name(tmp_path,(my_string) path));
  size = STARTSIZE;
  if (dirp == NULL || ! (buffer = (char *) my_malloc(size, MyFlags)))
    goto error;

  fcnt = 0;
  tmp_file=strend(tmp_path);
  firstfcnt = maxfcnt = (size - sizeof(MY_DIR)) /
    (sizeof(struct fileinfo) + FN_LEN);
  fnames=   (struct fileinfo *) (buffer + sizeof(MY_DIR));
  tempptr = (char *) (fnames + maxfcnt);

#ifdef THREAD
  dp= (struct dirent*) dirent_tmp;
#else
  dp=0;
#endif
  eof=0;
  for (;;)
  {
    while (fcnt < maxfcnt &&
	   !(eof= READDIR(dirp,(struct dirent*) dirent_tmp,dp)))
    {
      bzero((gptr) (fnames+fcnt),sizeof(fnames[0])); /* for purify */
      fnames[fcnt].name = tempptr;
      tempptr = strmov(tempptr,dp->d_name) + 1;
      if (MyFlags & MY_WANT_STAT)
      {
	VOID(strmov(tmp_file,dp->d_name));
	VOID(my_stat(tmp_path, &fnames[fcnt].mystat, MyFlags));
      }
      ++fcnt;
    }
    if (eof)
      break;
    size += STARTSIZE; obuffer = buffer;
    if (!(buffer = (char *) my_realloc((gptr) buffer, size,
				       MyFlags | MY_FREE_ON_ERROR)))
      goto error;			/* No memory */
    length= (uint) (sizeof(struct fileinfo ) * firstfcnt);
    diff=    PTR_BYTE_DIFF(buffer , obuffer) + (int) length;
    fnames=  (struct fileinfo *) (buffer + sizeof(MY_DIR));
    tempptr= ADD_TO_PTR(tempptr,diff,char*);
    for (i = 0; i < maxfcnt; i++)
      fnames[i].name = ADD_TO_PTR(fnames[i].name,diff,char*);

    /* move filenames upp a bit */
    maxfcnt += firstfcnt;
    bmove_upp(tempptr,tempptr-length,
	      (uint) (tempptr- (char*) (fnames+maxfcnt)));
  }

  (void) closedir(dirp);
  {
    MY_DIR * s = (MY_DIR *) buffer;
    s->number_off_files = (uint) fcnt;
    s->dir_entry = fnames;
  }
  if (!(MyFlags & MY_DONT_SORT))
    qsort((void *) fnames, (size_s) fcnt, sizeof(struct fileinfo),
	  (qsort_cmp) comp_names);
#if defined(THREAD) && !defined(HAVE_READDIR_R)
  pthread_mutex_unlock(&THR_LOCK_open);
#endif
  DBUG_RETURN((MY_DIR *) buffer);

 error:
#if defined(THREAD) && !defined(HAVE_READDIR_R)
  pthread_mutex_unlock(&THR_LOCK_open);
#endif
  my_errno=errno;
  if (dirp)
    (void) closedir(dirp);
  if (MyFlags & (MY_FAE+MY_WME))
    my_error(EE_DIR,MYF(ME_BELL+ME_WAITTANG),path,my_errno);
  DBUG_RETURN((MY_DIR *) NULL);
} /* my_dir */


/*
 * Convert from directory name to filename.
 * On VMS:
 *	 xyzzy:[mukesh.emacs] => xyzzy:[mukesh]emacs.dir.1
 *	 xyzzy:[mukesh] => xyzzy:[000000]mukesh.dir.1
 * On UNIX, it's simple: just make sure there is a terminating /

 * Returns pointer to dst;
 */

my_string directory_file_name (my_string dst, const char *src)
{
#ifndef VMS

  /* Process as Unix format: just remove test the final slash. */

  my_string end;

  if (src[0] == 0)
    src= (char*) ".";				/* Use empty as current */
  end=strmov(dst, src);
  if (end[-1] != FN_LIBCHAR)
  {
    end[0]=FN_LIBCHAR;				/* Add last '/' */
    end[1]='\0';
  }
  return dst;

#else	/* VMS */

  long slen;
  long rlen;
  my_string ptr, rptr;
  char bracket;
  struct FAB fab = cc$rms_fab;
  struct NAM nam = cc$rms_nam;
  char esa[NAM$C_MAXRSS];

  if (! src[0])
    src="[.]";					/* Empty is == current dir */

  slen = strlen (src) - 1;
  if (src[slen] == FN_C_AFTER_DIR || src[slen] == FN_C_AFTER_DIR_2 ||
      src[slen] == FN_DEVCHAR)
  {
	/* VMS style - convert [x.y.z] to [x.y]z, [x] to [000000]x */
    fab.fab$l_fna = src;
    fab.fab$b_fns = slen + 1;
    fab.fab$l_nam = &nam;
    fab.fab$l_fop = FAB$M_NAM;

    nam.nam$l_esa = esa;
    nam.nam$b_ess = sizeof esa;
    nam.nam$b_nop |= NAM$M_SYNCHK;

    /* We call SYS$PARSE to handle such things as [--] for us. */
    if (SYS$PARSE(&fab, 0, 0) == RMS$_NORMAL)
    {
      slen = nam.nam$b_esl - 1;
      if (esa[slen] == ';' && esa[slen - 1] == '.')
	slen -= 2;
      esa[slen + 1] = '\0';
      src = esa;
    }
    if (src[slen] != FN_C_AFTER_DIR && src[slen] != FN_C_AFTER_DIR_2)
    {
	/* what about when we have logical_name:???? */
      if (src[slen] == FN_DEVCHAR)
      {				/* Xlate logical name and see what we get */
	VOID(strmov(dst,src));
	dst[slen] = 0;				/* remove colon */
	if (!(src = getenv (dst)))
	  return dst;				/* Can't translate */

	/* should we jump to the beginning of this procedure?
	   Good points: allows us to use logical names that xlate
	   to Unix names,
	   Bad points: can be a problem if we just translated to a device
	   name...
	   For now, I'll punt and always expect VMS names, and hope for
	   the best! */

	slen = strlen (src) - 1;
	if (src[slen] != FN_C_AFTER_DIR && src[slen] != FN_C_AFTER_DIR_2)
	{					/* no recursion here! */
	  VOID(strmov(dst, src));
	  return(dst);
	}
      }
      else
      {						/* not a directory spec */
	VOID(strmov(dst, src));
	return(dst);
      }
    }

    bracket = src[slen];			/* End char */
    if (!(ptr = strchr (src, bracket - 2)))
    {						/* no opening bracket */
      VOID(strmov (dst, src));
      return dst;
    }
    if (!(rptr = strrchr (src, '.')))
      rptr = ptr;
    slen = rptr - src;
    VOID(strmake (dst, src, slen));

    if (*rptr == '.')
    {						/* Put bracket and add */
      dst[slen++] = bracket;			/* (rptr+1) after this */
    }
    else
    {
      /* If we have the top-level of a rooted directory (i.e. xx:[000000]),
	 then translate the device and recurse. */

      if (dst[slen - 1] == ':'
	  && dst[slen - 2] != ':' 	/* skip decnet nodes */
	  && strcmp(src + slen, "[000000]") == 0)
      {
	dst[slen - 1] = '\0';
	if ((ptr = getenv (dst))
	    && (rlen = strlen (ptr) - 1) > 0
	    && (ptr[rlen] == FN_C_AFTER_DIR || ptr[rlen] == FN_C_AFTER_DIR_2)
	    && ptr[rlen - 1] == '.')
	{
	  VOID(strmov(esa,ptr));
	  esa[rlen - 1] = FN_C_AFTER_DIR;
	  esa[rlen] = '\0';
	  return (directory_file_name (dst, esa));
	}
	else
	  dst[slen - 1] = ':';
      }
      VOID(strmov(dst+slen,"[000000]"));
      slen += 8;
    }
    VOID(strmov(strmov(dst+slen,rptr+1)-1,".DIR.1"));
    return dst;
  }
  VOID(strmov(dst, src));
  if (dst[slen] == '/' && slen > 1)
    dst[slen] = 0;
  return dst;
#endif	/* VMS */
} /* directory_file_name */

#elif defined(WIN32)

/*
*****************************************************************************
** Read long filename using windows rutines
*****************************************************************************
*/

MY_DIR	*my_dir(const char *path, myf MyFlags)
{
  struct fileinfo *fnames;
  char	       *buffer, *obuffer, *tempptr;
  int		eof,i,fcnt,firstfcnt,length,maxfcnt;
  uint		size;
#ifdef __BORLANDC__
  struct ffblk       find;
#else
  struct _finddata_t find;
#endif
  ushort	mode;
  char		tmp_path[FN_REFLEN],*tmp_file,attrib;
  my_ptrdiff_t	diff;
#ifdef _WIN64
  __int64       handle;
#else
  long		handle;
#endif
  DBUG_ENTER("my_dir");
  DBUG_PRINT("my",("path: '%s' stat: %d  MyFlags: %d",path,MyFlags));

  /* Put LIB-CHAR as last path-character if not there */

  tmp_file=tmp_path;
  if (!*path)
    *tmp_file++ ='.';				/* From current dir */
  tmp_file= strmov(tmp_file,path);
  if (tmp_file[-1] == FN_DEVCHAR)
    *tmp_file++= '.';				/* From current dev-dir */
  if (tmp_file[-1] != FN_LIBCHAR)
    *tmp_file++ =FN_LIBCHAR;
  tmp_file[0]='*';				/* MSDOS needs this !??? */
  tmp_file[1]='.';
  tmp_file[2]='*';
  tmp_file[3]='\0';

#ifdef __BORLANDC__
  if ((handle= findfirst(tmp_path,&find,0)) == -1L)
    goto error;
#else
  if ((handle=_findfirst(tmp_path,&find)) == -1L)
    goto error;
#endif

  size = STARTSIZE;
  firstfcnt = maxfcnt = (size - sizeof(MY_DIR)) /
    (sizeof(struct fileinfo) + FN_LEN);
  if ((buffer = (char *) my_malloc(size, MyFlags)) == 0)
    goto error;
  fnames=   (struct fileinfo *) (buffer + sizeof(MY_DIR));
  tempptr = (char *) (fnames + maxfcnt);

  fcnt = 0;
  for (;;)
  {
    do
    {
      fnames[fcnt].name = tempptr;
#ifdef __BORLANDC__
      tempptr = strmov(tempptr,find.ff_name) + 1;
      fnames[fcnt].mystat.st_size=find.ff_fsize;
      fnames[fcnt].mystat.st_uid=fnames[fcnt].mystat.st_gid=0;
      mode=MY_S_IREAD; attrib=find.ff_attrib;
#else
      tempptr = strmov(tempptr,find.name) + 1;
      fnames[fcnt].mystat.st_size=find.size;
      fnames[fcnt].mystat.st_uid=fnames[fcnt].mystat.st_gid=0;
      mode=MY_S_IREAD; attrib=find.attrib;
#endif
      if (!(attrib & _A_RDONLY))
	mode|=MY_S_IWRITE;
      if (attrib & _A_SUBDIR)
	mode|=MY_S_IFDIR;
      fnames[fcnt].mystat.st_mode=mode;
#ifdef __BORLANDC__
      fnames[fcnt].mystat.st_mtime=((uint32) find.ff_ftime);
#else
      fnames[fcnt].mystat.st_mtime=((uint32) find.time_write);
#endif
      ++fcnt;
#ifdef __BORLANDC__
    } while ((eof= findnext(&find)) == 0 && fcnt < maxfcnt);
#else
    } while ((eof= _findnext(handle,&find)) == 0 && fcnt < maxfcnt);
#endif

    DBUG_PRINT("test",("eof: %d  errno: %d",eof,errno));
    if (eof)
      break;
    size += STARTSIZE; obuffer = buffer;
    if (!(buffer = (char *) my_realloc((gptr) buffer, size,
				       MyFlags | MY_FREE_ON_ERROR)))
      goto error;
    length= sizeof(struct fileinfo ) * firstfcnt;
    diff=    PTR_BYTE_DIFF(buffer , obuffer) +length;
    fnames=  (struct fileinfo *) (buffer + sizeof(MY_DIR));
    tempptr= ADD_TO_PTR(tempptr,diff,char*);
    for (i = 0; i < maxfcnt; i++)
      fnames[i].name = ADD_TO_PTR(fnames[i].name,diff,char*);

    /* move filenames upp a bit */
    maxfcnt += firstfcnt;
    bmove_upp(tempptr,ADD_TO_PTR(tempptr,-length,char*),
	      (int) PTR_BYTE_DIFF(tempptr,fnames+maxfcnt));
  }
  {
    MY_DIR * s = (MY_DIR *) buffer;
    s->number_off_files = (uint) fcnt;
    s->dir_entry = fnames;
  }
  if (!(MyFlags & MY_DONT_SORT))
    qsort(fnames,fcnt,sizeof(struct fileinfo),(qsort_cmp) comp_names);
#ifndef __BORLANDC__
  _findclose(handle);
#endif
  DBUG_RETURN((MY_DIR *) buffer);

error:
  my_errno=errno;
#ifndef __BORLANDC__
  if (handle != -1)
      _findclose(handle);
#endif
  if (MyFlags & MY_FAE+MY_WME)
    my_error(EE_DIR,MYF(ME_BELL+ME_WAITTANG),path,errno);
  DBUG_RETURN((MY_DIR *) NULL);
} /* my_dir */

#else /* MSDOS and not WIN32 */


/******************************************************************************
** At MSDOS you always get stat of files, but time is in packed MSDOS-format
******************************************************************************/

MY_DIR	*my_dir(const char* path, myf MyFlags)
{
  struct fileinfo *fnames;
  char	       *buffer, *obuffer, *tempptr;
  int		eof,i,fcnt,firstfcnt,length,maxfcnt;
  uint		size;
  struct find_t find;
  ushort	mode;
  char		tmp_path[FN_REFLEN],*tmp_file,attrib;
  my_ptrdiff_t	diff;
  DBUG_ENTER("my_dir");
  DBUG_PRINT("my",("path: '%s' stat: %d  MyFlags: %d",path,MyFlags));

  /* Put LIB-CHAR as last path-character if not there */

  tmp_file=tmp_path;
  if (!*path)
    *tmp_file++ ='.';				/* From current dir */
  tmp_file= strmov(tmp_file,path);
  if (tmp_file[-1] == FN_DEVCHAR)
    *tmp_file++= '.';				/* From current dev-dir */
  if (tmp_file[-1] != FN_LIBCHAR)
    *tmp_file++ =FN_LIBCHAR;
  tmp_file[0]='*';				/* MSDOS needs this !??? */
  tmp_file[1]='.';
  tmp_file[2]='*';
  tmp_file[3]='\0';

  if (_dos_findfirst(tmp_path,_A_NORMAL | _A_SUBDIR, &find))
    goto error;

  size = STARTSIZE;
  firstfcnt = maxfcnt = (size - sizeof(MY_DIR)) /
    (sizeof(struct fileinfo) + FN_LEN);
  if ((buffer = (char *) my_malloc(size, MyFlags)) == 0)
    goto error;
  fnames=   (struct fileinfo *) (buffer + sizeof(MY_DIR));
  tempptr = (char *) (fnames + maxfcnt);

  fcnt = 0;
  for (;;)
  {
    do
    {
      fnames[fcnt].name = tempptr;
      tempptr = strmov(tempptr,find.name) + 1;
      fnames[fcnt].mystat.st_size=find.size;
      fnames[fcnt].mystat.st_uid=fnames[fcnt].mystat.st_gid=0;
      mode=MY_S_IREAD; attrib=find.attrib;
      if (!(attrib & _A_RDONLY))
	mode|=MY_S_IWRITE;
      if (attrib & _A_SUBDIR)
	mode|=MY_S_IFDIR;
      fnames[fcnt].mystat.st_mode=mode;
      fnames[fcnt].mystat.st_mtime=((uint32) find.wr_date << 16) +
					     find.wr_time;
      ++fcnt;
    } while ((eof= _dos_findnext(&find)) == 0 && fcnt < maxfcnt);

    DBUG_PRINT("test",("eof: %d  errno: %d",eof,errno));
    if (eof)
      break;
    size += STARTSIZE; obuffer = buffer;
    if (!(buffer = (char *) my_realloc((gptr) buffer, size,
				       MyFlags | MY_FREE_ON_ERROR)))
      goto error;
    length= sizeof(struct fileinfo ) * firstfcnt;
    diff=    PTR_BYTE_DIFF(buffer , obuffer) +length;
    fnames=  (struct fileinfo *) (buffer + sizeof(MY_DIR));
    tempptr= ADD_TO_PTR(tempptr,diff,char*);
    for (i = 0; i < maxfcnt; i++)
      fnames[i].name = ADD_TO_PTR(fnames[i].name,diff,char*);

    /* move filenames upp a bit */
    maxfcnt += firstfcnt;
    bmove_upp(tempptr,ADD_TO_PTR(tempptr,-length,char*),
	      (int) PTR_BYTE_DIFF(tempptr,fnames+maxfcnt));
  }
  {
    MY_DIR * s = (MY_DIR *) buffer;
    s->number_off_files = (uint) fcnt;
    s->dir_entry = fnames;
  }
  if (!(MyFlags & MY_DONT_SORT))
    qsort(fnames,fcnt,sizeof(struct fileinfo),(qsort_cmp) comp_names);
  DBUG_RETURN((MY_DIR *) buffer);

error:
  if (MyFlags & MY_FAE+MY_WME)
    my_error(EE_DIR,MYF(ME_BELL+ME_WAITTANG),path,errno);
  DBUG_RETURN((MY_DIR *) NULL);
} /* my_dir */

#endif /* WIN32 && MSDOS */

/****************************************************************************
** File status
** Note that MY_STAT is assumed to be same as struct stat
****************************************************************************/ 

int my_fstat(int Filedes, MY_STAT *stat_area, myf MyFlags )
{
  DBUG_ENTER("my_fstat");
  DBUG_PRINT("my",("fd: %d MyFlags: %d",Filedes,MyFlags));
  DBUG_RETURN(fstat(Filedes, (struct stat *) stat_area));
}

MY_STAT *my_stat(const char *path, MY_STAT *stat_area, myf my_flags)
{
  int m_used;
  DBUG_ENTER("my_stat");
  DBUG_PRINT("my", ("path: '%s', stat_area: %lx, MyFlags: %d", path,
	     (byte *) stat_area, my_flags));

  if ((m_used= (stat_area == NULL)))
    if (!(stat_area = (MY_STAT *) my_malloc(sizeof(MY_STAT), my_flags)))
      goto error;
  if ( ! stat((my_string) path, (struct stat *) stat_area) )
    DBUG_RETURN(stat_area);
  my_errno=errno;
  if (m_used)					/* Free if new area */
    my_free((gptr) stat_area,MYF(0));

error:
  if (my_flags & (MY_FAE+MY_WME))
  {
    my_error(EE_STAT, MYF(ME_BELL+ME_WAITTANG),path,my_errno);
    DBUG_RETURN((MY_STAT *) NULL);
  }
  DBUG_RETURN((MY_STAT *) NULL);
} /* my_stat */

