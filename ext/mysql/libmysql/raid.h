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

/* Parser needs these defines  always, even if USE_RAID is not defined */
#define RAID_TYPE_0 1       // Striping
#define RAID_TYPE_x 2       // Some new modes
#define RAID_TYPE_y 3       // 

#define RAID_DEFAULT_CHUNKS 4
#define RAID_DEFAULT_CHUNKSIZE 256*1024 /* 256kB */

extern const char *raid_type_string[];

#if defined(USE_RAID)

#ifdef __GNUC__
#pragma interface			/* gcc class implementation */
#endif
#include "my_dir.h"

/* Trap all occurences of my_...() in source and use our wrapper around this function */

#ifdef MAP_TO_USE_RAID
#define my_read(A,B,C,D)     my_raid_read(A,B,C,D)
#define my_write(A,B,C,D)    my_raid_write(A,B,C,D)
#define my_pwrite(A,B,C,D,E) my_raid_pwrite(A,B,C,D,E)
#define my_pread(A,B,C,D,E)  my_raid_pread(A,B,C,D,E)
#define my_chsize(A,B,C)     my_raid_chsize(A,B,C)
#define my_close(A,B)        my_raid_close(A,B)
#define my_tell(A,B)         my_raid_tell(A,B)
#define my_seek(A,B,C,D)     my_raid_seek(A,B,C,D)
#define my_lock(A,B,C,D,E)     my_raid_lock(A,B,C,D,E)
#define my_fstat(A,B,C)     my_raid_fstat(A,B,C)
#endif /* MAP_TO_USE_RAID */

#ifdef __cplusplus
extern "C" {
#endif

  void init_raid(void);
  void end_raid(void);

  bool is_raid(File fd);
  File my_raid_create(const char *FileName, int CreateFlags, int access_flags,
		      uint raid_type, uint raid_chunks, ulong raid_chunksize,
		      myf MyFlags);
  File my_raid_open(const char *FileName, int Flags,
		    uint raid_type, uint raid_chunks, ulong raid_chunksize,
		    myf MyFlags);
  int my_raid_rename(const char *from, const char *to, uint raid_chunks,
		     myf MyFlags);
  int my_raid_delete(const char *from, uint raid_chunks, myf MyFlags);
  int my_raid_redel(const char *old_name, const char *new_name,
		    uint raid_chunks, myf MyFlags);

  my_off_t my_raid_seek(File fd, my_off_t pos, int whence, myf MyFlags);
  my_off_t my_raid_tell(File fd, myf MyFlags);

  uint my_raid_write(File,const byte *Buffer, uint Count, myf MyFlags);
  uint my_raid_read(File Filedes, byte *Buffer, uint Count, myf MyFlags);

  uint my_raid_pread(File Filedes, byte *Buffer, uint Count, my_off_t offset,
		     myf MyFlags);
  uint my_raid_pwrite(int Filedes, const byte *Buffer, uint Count,
		      my_off_t offset, myf MyFlags);

  int my_raid_lock(File,int locktype, my_off_t start, my_off_t length,
		   myf MyFlags);
  int my_raid_chsize(File fd, my_off_t newlength, myf MyFlags);
  int my_raid_close(File, myf MyFlags);
  int my_raid_fstat(int Filedes, struct stat *buf,  myf MyFlags);

  const char *my_raid_type(int raid_type);

#ifdef __cplusplus
}

class RaidName {
  public:
    RaidName(const char *FileName);
    ~RaidName();
    bool IsRaid();
    int Rename(const char * from, const char * to, myf MyFlags);
  private:
    uint _raid_type;       // RAID_TYPE_0 or RAID_TYPE_1 or RAID_TYPE_5
    uint _raid_chunks;     // 1..n
    ulong _raid_chunksize; // 1..n in bytes
};

class RaidFd {
  public:
    RaidFd(uint raid_type, uint raid_chunks , ulong raid_chunksize);
    ~RaidFd();
    File Create(const char *FileName, int CreateFlags, int access_flags,
		myf MyFlags);
    File Open(const char *FileName, int Flags, myf MyFlags);
    my_off_t Seek(my_off_t pos,int whence,myf MyFlags);
    my_off_t Tell(myf MyFlags);
    int Write(const byte *Buffer, uint Count, myf MyFlags);
    int Read(const byte *Buffer, uint Count, myf MyFlags);
    int Lock(int locktype, my_off_t start, my_off_t length, myf MyFlags);
    int Chsize(File fd, my_off_t newlength, myf MyFlags);
    int Fstat(int fd, MY_STAT *stat_area, myf MyFlags );
    int Close(myf MyFlags);
    static bool IsRaid(File fd);
    static DYNAMIC_ARRAY _raid_map;		/* Map of RaidFD* */
  private:

    uint _raid_type;       // RAID_TYPE_0 or RAID_TYPE_1 or RAID_TYPE_5
    uint _raid_chunks;     // 1..n
    ulong _raid_chunksize; // 1..n in bytes

    ulong _total_block;    // We are operating with block no x (can be 0..many).
    uint _this_block;      // can be 0.._raid_chunks
    uint _remaining_bytes; // Maximum bytes that can be written in this block

    my_off_t _position;
    my_off_t _size;        // Cached file size for faster seek(SEEK_END)
    File _fd;
    File *_fd_vector;		/* Array of File */
    off_t *_seek_vector;	/* Array of cached seek positions */

    inline void Calculate()
    {
      DBUG_ENTER("RaidFd::_Calculate");
      DBUG_PRINT("info",("_position: %lu _raid_chunksize: %d, _size: %lu",
			 (ulong) _position, _raid_chunksize, (ulong) _size));

      _total_block = (ulong) (_position / _raid_chunksize);
      _this_block = _total_block % _raid_chunks;    // can be 0.._raid_chunks
      _remaining_bytes = (uint) (_raid_chunksize -
				 (_position - _total_block * _raid_chunksize));
      DBUG_PRINT("info",
		 ("_total_block: %d  this_block: %d  _remaining_bytes:%d",
		  _total_block, _this_block, _remaining_bytes));
      DBUG_VOID_RETURN;
    }
};

#endif /* __cplusplus */
#endif /* USE_RAID */
