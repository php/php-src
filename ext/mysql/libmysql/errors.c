/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

#include "mysys_priv.h"
#include "mysys_err.h"

#ifndef SHARED_LIBRARY

const char * NEAR globerrs[GLOBERRS]=
{
  "File '%s' not found (Errcode: %d)",
  "Can't create/write to file '%s' (Errcode: %d)",
  "Error reading file '%s' (Errcode: %d)",
  "Error writing file '%s' (Errcode: %d)",
  "Error on close of '%s' (Errcode: %d)",
  "Out of memory (Needed %u bytes)",
  "Error on delete of '%s' (Errcode: %d)",
  "Error on rename of '%s' to '%s' (Errcode: %d)",
  "",
  "Unexpected eof found when reading file '%s' (Errcode: %d)",
  "Can't lock file (Errcode: %d)",
  "Can't unlock file (Errcode: %d)",
  "Can't read dir of '%s' (Errcode: %d)",
  "Can't get stat of '%s' (Errcode: %d)",
  "Can't change size of file (Errcode: %d)",
  "Can't open stream from handle (Errcode: %d)",
  "Can't get working dirctory (Errcode: %d)",
  "Can't change dir to '%s' (Errcode: %d)",
  "Warning: '%s' had %d links",
  "%d files and %d streams is left open\n",
  "Disk is full writing '%s'. Waiting for someone to free space...",
  "Can't create directory '%s' (Errcode: %d)",
  "Character set '%s' is not a compiled character set and is not specified in the '%s' file",
  "Out of resources when opening file '%s' (Errcode: %d)",
  "Can't read value for symlink '%s' (Error %d)",
  "Can't create symlink '%s' pointing at '%s' (Error %d)",
  "Error on realpath() on '%s' (Error %d)",
};

void init_glob_errs(void)
{
  my_errmsg[GLOB] = & globerrs[0];
} /* init_glob_errs */

#else

void init_glob_errs()
{
  my_errmsg[GLOB] = & globerrs[0];

  EE(EE_FILENOTFOUND)	= "File '%s' not found (Errcode: %d)";
  EE(EE_CANTCREATEFILE) = "Can't create/write to file '%s' (Errcode: %d)";
  EE(EE_READ)		= "Error reading file '%s' (Errcode: %d)";
  EE(EE_WRITE)		= "Error writing file '%s' (Errcode: %d)";
  EE(EE_BADCLOSE)	= "Error on close of '%'s (Errcode: %d)";
  EE(EE_OUTOFMEMORY)	= "Out of memory (Needed %u bytes)";
  EE(EE_DELETE)		= "Error on delete of '%s' (Errcode: %d)";
  EE(EE_LINK)		= "Error on rename of '%s' to '%s' (Errcode: %d)";
  EE(EE_EOFERR)		= "Unexpected eof found when reading file '%s' (Errcode: %d)";
  EE(EE_CANTLOCK)	= "Can't lock file (Errcode: %d)";
  EE(EE_CANTUNLOCK)	= "Can't unlock file (Errcode: %d)";
  EE(EE_DIR)		= "Can't read dir of '%s' (Errcode: %d)";
  EE(EE_STAT)		= "Can't get stat of '%s' (Errcode: %d)";
  EE(EE_CANT_CHSIZE)	= "Can't change size of file (Errcode: %d)";
  EE(EE_CANT_OPEN_STREAM)= "Can't open stream from handle (Errcode: %d)";
  EE(EE_GETWD)		= "Can't get working dirctory (Errcode: %d)";
  EE(EE_SETWD)		= "Can't change dir to '%s' (Errcode: %d)";
  EE(EE_LINK_WARNING)	= "Warning: '%s' had %d links";
  EE(EE_OPEN_WARNING)	= "%d files and %d streams is left open\n";
  EE(EE_DISK_FULL)	= "Disk is full writing '%s'. Waiting for someone to free space...";
  EE(EE_CANT_MKDIR)	="Can't create directory '%s' (Errcode: %d)";
  EE(EE_UNKNOWN_CHARSET)= "Character set is not a compiled character set and is not specified in the %s file";
  EE(EE_OUT_OF_FILERESOURCES)="Out of resources when opening file '%s' (Errcode: %d)";
  EE(EE_CANT_READLINK)="Can't read value for symlink '%s' (Error %d)";
  EE(EE_CANT_SYMLINK)="Can't create symlink '%s' pointing at '%s' (Error %d)";
  EE(EE_REALPATH)="Error on realpath() on '%s' (Error %d)";
}
#endif
