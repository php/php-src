/*
** 2001 September 15
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This header file defines the interface that the SQLite library
** presents to client programs.  If a C-function, structure, datatype,
** or constant definition does not appear in this file, then it is
** not a published API of SQLite, is subject to change without
** notice, and should not be referenced by programs that use SQLite.
**
** Some of the definitions that are in this file are marked as
** "experimental".  Experimental interfaces are normally new
** features recently added to SQLite.  We do not anticipate changes
** to experimental interfaces but reserve the right to make minor changes
** if experience from use "in the wild" suggest such changes are prudent.
**
** The official C-language API documentation for SQLite is derived
** from comments in this file.  This file is the authoritative source
** on how SQLite interfaces are suppose to operate.
**
** The name of this file under configuration management is "sqlite.h.in".
** The makefile makes some minor changes to this file (such as inserting
** the version number) and changes its name to "sqlite3.h" as
** part of the build process.
*/
#ifndef _SQLITE3_H_
#define _SQLITE3_H_
#include <stdarg.h>     /* Needed for the definition of va_list */

/*
** Make sure we can call this stuff from C++.
*/
#ifdef __cplusplus
extern "C" {
#endif


/*
** Add the ability to override 'extern'
*/
#ifndef SQLITE_EXTERN
# define SQLITE_EXTERN extern
#endif

#ifndef SQLITE_API
# define SQLITE_API
#endif


/*
** These no-op macros are used in front of interfaces to mark those
** interfaces as either deprecated or experimental.  New applications
** should not use deprecated interfaces - they are support for backwards
** compatibility only.  Application writers should be aware that
** experimental interfaces are subject to change in point releases.
**
** These macros used to resolve to various kinds of compiler magic that
** would generate warning messages when they were used.  But that
** compiler magic ended up generating such a flurry of bug reports
** that we have taken it all out and gone back to using simple
** noop macros.
*/
#define SQLITE_DEPRECATED
#define SQLITE_EXPERIMENTAL

/*
** Ensure these symbols were not defined by some previous header file.
*/
#ifdef SQLITE_VERSION
# undef SQLITE_VERSION
#endif
#ifdef SQLITE_VERSION_NUMBER
# undef SQLITE_VERSION_NUMBER
#endif

/*
** CAPI3REF: Compile-Time Library Version Numbers {H10010} <S60100>
**
** The SQLITE_VERSION and SQLITE_VERSION_NUMBER #defines in
** the sqlite3.h file specify the version of SQLite with which
** that header file is associated.
**
** The "version" of SQLite is a string of the form "W.X.Y" or "W.X.Y.Z".
** The W value is major version number and is always 3 in SQLite3.
** The W value only changes when backwards compatibility is
** broken and we intend to never break backwards compatibility.
** The X value is the minor version number and only changes when
** there are major feature enhancements that are forwards compatible
** but not backwards compatible.
** The Y value is the release number and is incremented with
** each release but resets back to 0 whenever X is incremented.
** The Z value only appears on branch releases.
**
** The SQLITE_VERSION_NUMBER is an integer that is computed as
** follows:
**
** <blockquote><pre>
** SQLITE_VERSION_NUMBER = W*1000000 + X*1000 + Y
** </pre></blockquote>
**
** Since version 3.6.18, SQLite source code has been stored in the
** <a href="http://www.fossil-scm.org/">fossil configuration management
** system</a>.  The SQLITE_SOURCE_ID
** macro is a string which identifies a particular check-in of SQLite
** within its configuration management system.  The string contains the
** date and time of the check-in (UTC) and an SHA1 hash of the entire
** source tree.
**
** See also: [sqlite3_libversion()],
** [sqlite3_libversion_number()], [sqlite3_sourceid()],
** [sqlite_version()] and [sqlite_source_id()].
**
** Requirements: [H10011] [H10014]
*/
#define SQLITE_VERSION        "3.6.19"
#define SQLITE_VERSION_NUMBER 3006019
#define SQLITE_SOURCE_ID      "2009-10-14 11:33:55 c1d499afc50d54b376945b4efb65c56c787a073d"

/*
** CAPI3REF: Run-Time Library Version Numbers {H10020} <S60100>
** KEYWORDS: sqlite3_version
**
** These interfaces provide the same information as the [SQLITE_VERSION],
** [SQLITE_VERSION_NUMBER], and [SQLITE_SOURCE_ID] #defines in the header,
** but are associated with the library instead of the header file.  Cautious
** programmers might include assert() statements in their application to
** verify that values returned by these interfaces match the macros in
** the header, and thus insure that the application is
** compiled with matching library and header files.
**
** <blockquote><pre>
** assert( sqlite3_libversion_number()==SQLITE_VERSION_NUMBER );
** assert( strcmp(sqlite3_sourceid(),SQLITE_SOURCE_ID)==0 );
** assert( strcmp(sqlite3_libversion,SQLITE_VERSION)==0 );
** </pre></blockquote>
**
** The sqlite3_libversion() function returns the same information as is
** in the sqlite3_version[] string constant.  The function is provided
** for use in DLLs since DLL users usually do not have direct access to string
** constants within the DLL.  Similarly, the sqlite3_sourceid() function
** returns the same information as is in the [SQLITE_SOURCE_ID] #define of
** the header file.
**
** See also: [sqlite_version()] and [sqlite_source_id()].
**
** Requirements: [H10021] [H10022] [H10023]
*/
SQLITE_API SQLITE_EXTERN const char sqlite3_version[];
SQLITE_API const char *sqlite3_libversion(void);
SQLITE_API const char *sqlite3_sourceid(void);
SQLITE_API int sqlite3_libversion_number(void);

/*
** CAPI3REF: Test To See If The Library Is Threadsafe {H10100} <S60100>
**
** SQLite can be compiled with or without mutexes.  When
** the [SQLITE_THREADSAFE] C preprocessor macro is 1 or 2, mutexes
** are enabled and SQLite is threadsafe.  When the
** [SQLITE_THREADSAFE] macro is 0, 
** the mutexes are omitted.  Without the mutexes, it is not safe
** to use SQLite concurrently from more than one thread.
**
** Enabling mutexes incurs a measurable performance penalty.
** So if speed is of utmost importance, it makes sense to disable
** the mutexes.  But for maximum safety, mutexes should be enabled.
** The default behavior is for mutexes to be enabled.
**
** This interface can be used by an application to make sure that the
** version of SQLite that it is linking against was compiled with
** the desired setting of the [SQLITE_THREADSAFE] macro.
**
** This interface only reports on the compile-time mutex setting
** of the [SQLITE_THREADSAFE] flag.  If SQLite is compiled with
** SQLITE_THREADSAFE=1 then mutexes are enabled by default but
** can be fully or partially disabled using a call to [sqlite3_config()]
** with the verbs [SQLITE_CONFIG_SINGLETHREAD], [SQLITE_CONFIG_MULTITHREAD],
** or [SQLITE_CONFIG_MUTEX].  The return value of this function shows
** only the default compile-time setting, not any run-time changes
** to that setting.
**
** See the [threading mode] documentation for additional information.
**
** Requirements: [H10101] [H10102]
*/
SQLITE_API int sqlite3_threadsafe(void);

/*
** CAPI3REF: Database Connection Handle {H12000} <S40200>
** KEYWORDS: {database connection} {database connections}
**
** Each open SQLite database is represented by a pointer to an instance of
** the opaque structure named "sqlite3".  It is useful to think of an sqlite3
** pointer as an object.  The [sqlite3_open()], [sqlite3_open16()], and
** [sqlite3_open_v2()] interfaces are its constructors, and [sqlite3_close()]
** is its destructor.  There are many other interfaces (such as
** [sqlite3_prepare_v2()], [sqlite3_create_function()], and
** [sqlite3_busy_timeout()] to name but three) that are methods on an
** sqlite3 object.
*/
typedef struct sqlite3 sqlite3;

/*
** CAPI3REF: 64-Bit Integer Types {H10200} <S10110>
** KEYWORDS: sqlite_int64 sqlite_uint64
**
** Because there is no cross-platform way to specify 64-bit integer types
** SQLite includes typedefs for 64-bit signed and unsigned integers.
**
** The sqlite3_int64 and sqlite3_uint64 are the preferred type definitions.
** The sqlite_int64 and sqlite_uint64 types are supported for backwards
** compatibility only.
**
** Requirements: [H10201] [H10202]
*/
#ifdef SQLITE_INT64_TYPE
  typedef SQLITE_INT64_TYPE sqlite_int64;
  typedef unsigned SQLITE_INT64_TYPE sqlite_uint64;
#elif defined(_MSC_VER) || defined(__BORLANDC__)
  typedef __int64 sqlite_int64;
  typedef unsigned __int64 sqlite_uint64;
#else
  typedef long long int sqlite_int64;
  typedef unsigned long long int sqlite_uint64;
#endif
typedef sqlite_int64 sqlite3_int64;
typedef sqlite_uint64 sqlite3_uint64;

/*
** If compiling for a processor that lacks floating point support,
** substitute integer for floating-point.
*/
#ifdef SQLITE_OMIT_FLOATING_POINT
# define double sqlite3_int64
#endif

/*
** CAPI3REF: Closing A Database Connection {H12010} <S30100><S40200>
**
** This routine is the destructor for the [sqlite3] object.
**
** Applications should [sqlite3_finalize | finalize] all [prepared statements]
** and [sqlite3_blob_close | close] all [BLOB handles] associated with
** the [sqlite3] object prior to attempting to close the object.
** The [sqlite3_next_stmt()] interface can be used to locate all
** [prepared statements] associated with a [database connection] if desired.
** Typical code might look like this:
**
** <blockquote><pre>
** sqlite3_stmt *pStmt;
** while( (pStmt = sqlite3_next_stmt(db, 0))!=0 ){
** &nbsp;   sqlite3_finalize(pStmt);
** }
** </pre></blockquote>
**
** If [sqlite3_close()] is invoked while a transaction is open,
** the transaction is automatically rolled back.
**
** The C parameter to [sqlite3_close(C)] must be either a NULL
** pointer or an [sqlite3] object pointer obtained
** from [sqlite3_open()], [sqlite3_open16()], or
** [sqlite3_open_v2()], and not previously closed.
**
** Requirements:
** [H12011] [H12012] [H12013] [H12014] [H12015] [H12019]
*/
SQLITE_API int sqlite3_close(sqlite3 *);

/*
** The type for a callback function.
** This is legacy and deprecated.  It is included for historical
** compatibility and is not documented.
*/
typedef int (*sqlite3_callback)(void*,int,char**, char**);

/*
** CAPI3REF: One-Step Query Execution Interface {H12100} <S10000>
**
** The sqlite3_exec() interface is a convenient way of running one or more
** SQL statements without having to write a lot of C code.  The UTF-8 encoded
** SQL statements are passed in as the second parameter to sqlite3_exec().
** The statements are evaluated one by one until either an error or
** an interrupt is encountered, or until they are all done.  The 3rd parameter
** is an optional callback that is invoked once for each row of any query
** results produced by the SQL statements.  The 5th parameter tells where
** to write any error messages.
**
** The error message passed back through the 5th parameter is held
** in memory obtained from [sqlite3_malloc()].  To avoid a memory leak,
** the calling application should call [sqlite3_free()] on any error
** message returned through the 5th parameter when it has finished using
** the error message.
**
** If the SQL statement in the 2nd parameter is NULL or an empty string
** or a string containing only whitespace and comments, then no SQL
** statements are evaluated and the database is not changed.
**
** The sqlite3_exec() interface is implemented in terms of
** [sqlite3_prepare_v2()], [sqlite3_step()], and [sqlite3_finalize()].
** The sqlite3_exec() routine does nothing to the database that cannot be done
** by [sqlite3_prepare_v2()], [sqlite3_step()], and [sqlite3_finalize()].
**
** The first parameter to [sqlite3_exec()] must be an valid and open
** [database connection].
**
** The database connection must not be closed while
** [sqlite3_exec()] is running.
**
** The calling function should use [sqlite3_free()] to free
** the memory that *errmsg is left pointing at once the error
** message is no longer needed.
**
** The SQL statement text in the 2nd parameter to [sqlite3_exec()]
** must remain unchanged while [sqlite3_exec()] is running.
**
** Requirements:
** [H12101] [H12102] [H12104] [H12105] [H12107] [H12110] [H12113] [H12116]
** [H12119] [H12122] [H12125] [H12131] [H12134] [H12137] [H12138]
*/
SQLITE_API int sqlite3_exec(
  sqlite3*,                                  /* An open database */
  const char *sql,                           /* SQL to be evaluated */
  int (*callback)(void*,int,char**,char**),  /* Callback function */
  void *,                                    /* 1st argument to callback */
  char **errmsg                              /* Error msg written here */
);

/*
** CAPI3REF: Result Codes {H10210} <S10700>
** KEYWORDS: SQLITE_OK {error code} {error codes}
** KEYWORDS: {result code} {result codes}
**
** Many SQLite functions return an integer result code from the set shown
** here in order to indicates success or failure.
**
** New error codes may be added in future versions of SQLite.
**
** See also: [SQLITE_IOERR_READ | extended result codes]
*/
#define SQLITE_OK           0   /* Successful result */
/* beginning-of-error-codes */
#define SQLITE_ERROR        1   /* SQL error or missing database */
#define SQLITE_INTERNAL     2   /* Internal logic error in SQLite */
#define SQLITE_PERM         3   /* Access permission denied */
#define SQLITE_ABORT        4   /* Callback routine requested an abort */
#define SQLITE_BUSY         5   /* The database file is locked */
#define SQLITE_LOCKED       6   /* A table in the database is locked */
#define SQLITE_NOMEM        7   /* A malloc() failed */
#define SQLITE_READONLY     8   /* Attempt to write a readonly database */
#define SQLITE_INTERRUPT    9   /* Operation terminated by sqlite3_interrupt()*/
#define SQLITE_IOERR       10   /* Some kind of disk I/O error occurred */
#define SQLITE_CORRUPT     11   /* The database disk image is malformed */
#define SQLITE_NOTFOUND    12   /* NOT USED. Table or record not found */
#define SQLITE_FULL        13   /* Insertion failed because database is full */
#define SQLITE_CANTOPEN    14   /* Unable to open the database file */
#define SQLITE_PROTOCOL    15   /* NOT USED. Database lock protocol error */
#define SQLITE_EMPTY       16   /* Database is empty */
#define SQLITE_SCHEMA      17   /* The database schema changed */
#define SQLITE_TOOBIG      18   /* String or BLOB exceeds size limit */
#define SQLITE_CONSTRAINT  19   /* Abort due to constraint violation */
#define SQLITE_MISMATCH    20   /* Data type mismatch */
#define SQLITE_MISUSE      21   /* Library used incorrectly */
#define SQLITE_NOLFS       22   /* Uses OS features not supported on host */
#define SQLITE_AUTH        23   /* Authorization denied */
#define SQLITE_FORMAT      24   /* Auxiliary database format error */
#define SQLITE_RANGE       25   /* 2nd parameter to sqlite3_bind out of range */
#define SQLITE_NOTADB      26   /* File opened that is not a database file */
#define SQLITE_ROW         100  /* sqlite3_step() has another row ready */
#define SQLITE_DONE        101  /* sqlite3_step() has finished executing */
/* end-of-error-codes */

/*
** CAPI3REF: Extended Result Codes {H10220} <S10700>
** KEYWORDS: {extended error code} {extended error codes}
** KEYWORDS: {extended result code} {extended result codes}
**
** In its default configuration, SQLite API routines return one of 26 integer
** [SQLITE_OK | result codes].  However, experience has shown that many of
** these result codes are too coarse-grained.  They do not provide as
** much information about problems as programmers might like.  In an effort to
** address this, newer versions of SQLite (version 3.3.8 and later) include
** support for additional result codes that provide more detailed information
** about errors. The extended result codes are enabled or disabled
** on a per database connection basis using the
** [sqlite3_extended_result_codes()] API.
**
** Some of the available extended result codes are listed here.
** One may expect the number of extended result codes will be expand
** over time.  Software that uses extended result codes should expect
** to see new result codes in future releases of SQLite.
**
** The SQLITE_OK result code will never be extended.  It will always
** be exactly zero.
*/
#define SQLITE_IOERR_READ              (SQLITE_IOERR | (1<<8))
#define SQLITE_IOERR_SHORT_READ        (SQLITE_IOERR | (2<<8))
#define SQLITE_IOERR_WRITE             (SQLITE_IOERR | (3<<8))
#define SQLITE_IOERR_FSYNC             (SQLITE_IOERR | (4<<8))
#define SQLITE_IOERR_DIR_FSYNC         (SQLITE_IOERR | (5<<8))
#define SQLITE_IOERR_TRUNCATE          (SQLITE_IOERR | (6<<8))
#define SQLITE_IOERR_FSTAT             (SQLITE_IOERR | (7<<8))
#define SQLITE_IOERR_UNLOCK            (SQLITE_IOERR | (8<<8))
#define SQLITE_IOERR_RDLOCK            (SQLITE_IOERR | (9<<8))
#define SQLITE_IOERR_DELETE            (SQLITE_IOERR | (10<<8))
#define SQLITE_IOERR_BLOCKED           (SQLITE_IOERR | (11<<8))
#define SQLITE_IOERR_NOMEM             (SQLITE_IOERR | (12<<8))
#define SQLITE_IOERR_ACCESS            (SQLITE_IOERR | (13<<8))
#define SQLITE_IOERR_CHECKRESERVEDLOCK (SQLITE_IOERR | (14<<8))
#define SQLITE_IOERR_LOCK              (SQLITE_IOERR | (15<<8))
#define SQLITE_IOERR_CLOSE             (SQLITE_IOERR | (16<<8))
#define SQLITE_IOERR_DIR_CLOSE         (SQLITE_IOERR | (17<<8))
#define SQLITE_LOCKED_SHAREDCACHE      (SQLITE_LOCKED | (1<<8) )

/*
** CAPI3REF: Flags For File Open Operations {H10230} <H11120> <H12700>
**
** These bit values are intended for use in the
** 3rd parameter to the [sqlite3_open_v2()] interface and
** in the 4th parameter to the xOpen method of the
** [sqlite3_vfs] object.
*/
#define SQLITE_OPEN_READONLY         0x00000001  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_READWRITE        0x00000002  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_CREATE           0x00000004  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_DELETEONCLOSE    0x00000008  /* VFS only */
#define SQLITE_OPEN_EXCLUSIVE        0x00000010  /* VFS only */
#define SQLITE_OPEN_MAIN_DB          0x00000100  /* VFS only */
#define SQLITE_OPEN_TEMP_DB          0x00000200  /* VFS only */
#define SQLITE_OPEN_TRANSIENT_DB     0x00000400  /* VFS only */
#define SQLITE_OPEN_MAIN_JOURNAL     0x00000800  /* VFS only */
#define SQLITE_OPEN_TEMP_JOURNAL     0x00001000  /* VFS only */
#define SQLITE_OPEN_SUBJOURNAL       0x00002000  /* VFS only */
#define SQLITE_OPEN_MASTER_JOURNAL   0x00004000  /* VFS only */
#define SQLITE_OPEN_NOMUTEX          0x00008000  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_FULLMUTEX        0x00010000  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_SHAREDCACHE      0x00020000  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_PRIVATECACHE     0x00040000  /* Ok for sqlite3_open_v2() */

/*
** CAPI3REF: Device Characteristics {H10240} <H11120>
**
** The xDeviceCapabilities method of the [sqlite3_io_methods]
** object returns an integer which is a vector of the these
** bit values expressing I/O characteristics of the mass storage
** device that holds the file that the [sqlite3_io_methods]
** refers to.
**
** The SQLITE_IOCAP_ATOMIC property means that all writes of
** any size are atomic.  The SQLITE_IOCAP_ATOMICnnn values
** mean that writes of blocks that are nnn bytes in size and
** are aligned to an address which is an integer multiple of
** nnn are atomic.  The SQLITE_IOCAP_SAFE_APPEND value means
** that when data is appended to a file, the data is appended
** first then the size of the file is extended, never the other
** way around.  The SQLITE_IOCAP_SEQUENTIAL property means that
** information is written to disk in the same order as calls
** to xWrite().
*/
#define SQLITE_IOCAP_ATOMIC          0x00000001
#define SQLITE_IOCAP_ATOMIC512       0x00000002
#define SQLITE_IOCAP_ATOMIC1K        0x00000004
#define SQLITE_IOCAP_ATOMIC2K        0x00000008
#define SQLITE_IOCAP_ATOMIC4K        0x00000010
#define SQLITE_IOCAP_ATOMIC8K        0x00000020
#define SQLITE_IOCAP_ATOMIC16K       0x00000040
#define SQLITE_IOCAP_ATOMIC32K       0x00000080
#define SQLITE_IOCAP_ATOMIC64K       0x00000100
#define SQLITE_IOCAP_SAFE_APPEND     0x00000200
#define SQLITE_IOCAP_SEQUENTIAL      0x00000400

/*
** CAPI3REF: File Locking Levels {H10250} <H11120> <H11310>
**
** SQLite uses one of these integer values as the second
** argument to calls it makes to the xLock() and xUnlock() methods
** of an [sqlite3_io_methods] object.
*/
#define SQLITE_LOCK_NONE          0
#define SQLITE_LOCK_SHARED        1
#define SQLITE_LOCK_RESERVED      2
#define SQLITE_LOCK_PENDING       3
#define SQLITE_LOCK_EXCLUSIVE     4

/*
** CAPI3REF: Synchronization Type Flags {H10260} <H11120>
**
** When SQLite invokes the xSync() method of an
** [sqlite3_io_methods] object it uses a combination of
** these integer values as the second argument.
**
** When the SQLITE_SYNC_DATAONLY flag is used, it means that the
** sync operation only needs to flush data to mass storage.  Inode
** information need not be flushed. If the lower four bits of the flag
** equal SQLITE_SYNC_NORMAL, that means to use normal fsync() semantics.
** If the lower four bits equal SQLITE_SYNC_FULL, that means
** to use Mac OS X style fullsync instead of fsync().
*/
#define SQLITE_SYNC_NORMAL        0x00002
#define SQLITE_SYNC_FULL          0x00003
#define SQLITE_SYNC_DATAONLY      0x00010

/*
** CAPI3REF: OS Interface Open File Handle {H11110} <S20110>
**
** An [sqlite3_file] object represents an open file in the 
** [sqlite3_vfs | OS interface layer].  Individual OS interface
** implementations will
** want to subclass this object by appending additional fields
** for their own use.  The pMethods entry is a pointer to an
** [sqlite3_io_methods] object that defines methods for performing
** I/O operations on the open file.
*/
typedef struct sqlite3_file sqlite3_file;
struct sqlite3_file {
  const struct sqlite3_io_methods *pMethods;  /* Methods for an open file */
};

/*
** CAPI3REF: OS Interface File Virtual Methods Object {H11120} <S20110>
**
** Every file opened by the [sqlite3_vfs] xOpen method populates an
** [sqlite3_file] object (or, more commonly, a subclass of the
** [sqlite3_file] object) with a pointer to an instance of this object.
** This object defines the methods used to perform various operations
** against the open file represented by the [sqlite3_file] object.
**
** If the xOpen method sets the sqlite3_file.pMethods element 
** to a non-NULL pointer, then the sqlite3_io_methods.xClose method
** may be invoked even if the xOpen reported that it failed.  The
** only way to prevent a call to xClose following a failed xOpen
** is for the xOpen to set the sqlite3_file.pMethods element to NULL.
**
** The flags argument to xSync may be one of [SQLITE_SYNC_NORMAL] or
** [SQLITE_SYNC_FULL].  The first choice is the normal fsync().
** The second choice is a Mac OS X style fullsync.  The [SQLITE_SYNC_DATAONLY]
** flag may be ORed in to indicate that only the data of the file
** and not its inode needs to be synced.
**
** The integer values to xLock() and xUnlock() are one of
** <ul>
** <li> [SQLITE_LOCK_NONE],
** <li> [SQLITE_LOCK_SHARED],
** <li> [SQLITE_LOCK_RESERVED],
** <li> [SQLITE_LOCK_PENDING], or
** <li> [SQLITE_LOCK_EXCLUSIVE].
** </ul>
** xLock() increases the lock. xUnlock() decreases the lock.
** The xCheckReservedLock() method checks whether any database connection,
** either in this process or in some other process, is holding a RESERVED,
** PENDING, or EXCLUSIVE lock on the file.  It returns true
** if such a lock exists and false otherwise.
**
** The xFileControl() method is a generic interface that allows custom
** VFS implementations to directly control an open file using the
** [sqlite3_file_control()] interface.  The second "op" argument is an
** integer opcode.  The third argument is a generic pointer intended to
** point to a structure that may contain arguments or space in which to
** write return values.  Potential uses for xFileControl() might be
** functions to enable blocking locks with timeouts, to change the
** locking strategy (for example to use dot-file locks), to inquire
** about the status of a lock, or to break stale locks.  The SQLite
** core reserves all opcodes less than 100 for its own use.
** A [SQLITE_FCNTL_LOCKSTATE | list of opcodes] less than 100 is available.
** Applications that define a custom xFileControl method should use opcodes
** greater than 100 to avoid conflicts.
**
** The xSectorSize() method returns the sector size of the
** device that underlies the file.  The sector size is the
** minimum write that can be performed without disturbing
** other bytes in the file.  The xDeviceCharacteristics()
** method returns a bit vector describing behaviors of the
** underlying device:
**
** <ul>
** <li> [SQLITE_IOCAP_ATOMIC]
** <li> [SQLITE_IOCAP_ATOMIC512]
** <li> [SQLITE_IOCAP_ATOMIC1K]
** <li> [SQLITE_IOCAP_ATOMIC2K]
** <li> [SQLITE_IOCAP_ATOMIC4K]
** <li> [SQLITE_IOCAP_ATOMIC8K]
** <li> [SQLITE_IOCAP_ATOMIC16K]
** <li> [SQLITE_IOCAP_ATOMIC32K]
** <li> [SQLITE_IOCAP_ATOMIC64K]
** <li> [SQLITE_IOCAP_SAFE_APPEND]
** <li> [SQLITE_IOCAP_SEQUENTIAL]
** </ul>
**
** The SQLITE_IOCAP_ATOMIC property means that all writes of
** any size are atomic.  The SQLITE_IOCAP_ATOMICnnn values
** mean that writes of blocks that are nnn bytes in size and
** are aligned to an address which is an integer multiple of
** nnn are atomic.  The SQLITE_IOCAP_SAFE_APPEND value means
** that when data is appended to a file, the data is appended
** first then the size of the file is extended, never the other
** way around.  The SQLITE_IOCAP_SEQUENTIAL property means that
** information is written to disk in the same order as calls
** to xWrite().
**
** If xRead() returns SQLITE_IOERR_SHORT_READ it must also fill
** in the unread portions of the buffer with zeros.  A VFS that
** fails to zero-fill short reads might seem to work.  However,
** failure to zero-fill short reads will eventually lead to
** database corruption.
*/
typedef struct sqlite3_io_methods sqlite3_io_methods;
struct sqlite3_io_methods {
  int iVersion;
  int (*xClose)(sqlite3_file*);
  int (*xRead)(sqlite3_file*, void*, int iAmt, sqlite3_int64 iOfst);
  int (*xWrite)(sqlite3_file*, const void*, int iAmt, sqlite3_int64 iOfst);
  int (*xTruncate)(sqlite3_file*, sqlite3_int64 size);
  int (*xSync)(sqlite3_file*, int flags);
  int (*xFileSize)(sqlite3_file*, sqlite3_int64 *pSize);
  int (*xLock)(sqlite3_file*, int);
  int (*xUnlock)(sqlite3_file*, int);
  int (*xCheckReservedLock)(sqlite3_file*, int *pResOut);
  int (*xFileControl)(sqlite3_file*, int op, void *pArg);
  int (*xSectorSize)(sqlite3_file*);
  int (*xDeviceCharacteristics)(sqlite3_file*);
  /* Additional methods may be added in future releases */
};

/*
** CAPI3REF: Standard File Control Opcodes {H11310} <S30800>
**
** These integer constants are opcodes for the xFileControl method
** of the [sqlite3_io_methods] object and for the [sqlite3_file_control()]
** interface.
**
** The [SQLITE_FCNTL_LOCKSTATE] opcode is used for debugging.  This
** opcode causes the xFileControl method to write the current state of
** the lock (one of [SQLITE_LOCK_NONE], [SQLITE_LOCK_SHARED],
** [SQLITE_LOCK_RESERVED], [SQLITE_LOCK_PENDING], or [SQLITE_LOCK_EXCLUSIVE])
** into an integer that the pArg argument points to. This capability
** is used during testing and only needs to be supported when SQLITE_TEST
** is defined.
*/
#define SQLITE_FCNTL_LOCKSTATE        1
#define SQLITE_GET_LOCKPROXYFILE      2
#define SQLITE_SET_LOCKPROXYFILE      3
#define SQLITE_LAST_ERRNO             4

/*
** CAPI3REF: Mutex Handle {H17110} <S20130>
**
** The mutex module within SQLite defines [sqlite3_mutex] to be an
** abstract type for a mutex object.  The SQLite core never looks
** at the internal representation of an [sqlite3_mutex].  It only
** deals with pointers to the [sqlite3_mutex] object.
**
** Mutexes are created using [sqlite3_mutex_alloc()].
*/
typedef struct sqlite3_mutex sqlite3_mutex;

/*
** CAPI3REF: OS Interface Object {H11140} <S20100>
**
** An instance of the sqlite3_vfs object defines the interface between
** the SQLite core and the underlying operating system.  The "vfs"
** in the name of the object stands for "virtual file system".
**
** The value of the iVersion field is initially 1 but may be larger in
** future versions of SQLite.  Additional fields may be appended to this
** object when the iVersion value is increased.  Note that the structure
** of the sqlite3_vfs object changes in the transaction between
** SQLite version 3.5.9 and 3.6.0 and yet the iVersion field was not
** modified.
**
** The szOsFile field is the size of the subclassed [sqlite3_file]
** structure used by this VFS.  mxPathname is the maximum length of
** a pathname in this VFS.
**
** Registered sqlite3_vfs objects are kept on a linked list formed by
** the pNext pointer.  The [sqlite3_vfs_register()]
** and [sqlite3_vfs_unregister()] interfaces manage this list
** in a thread-safe way.  The [sqlite3_vfs_find()] interface
** searches the list.  Neither the application code nor the VFS
** implementation should use the pNext pointer.
**
** The pNext field is the only field in the sqlite3_vfs
** structure that SQLite will ever modify.  SQLite will only access
** or modify this field while holding a particular static mutex.
** The application should never modify anything within the sqlite3_vfs
** object once the object has been registered.
**
** The zName field holds the name of the VFS module.  The name must
** be unique across all VFS modules.
**
** SQLite will guarantee that the zFilename parameter to xOpen
** is either a NULL pointer or string obtained
** from xFullPathname().  SQLite further guarantees that
** the string will be valid and unchanged until xClose() is
** called. Because of the previous sentence,
** the [sqlite3_file] can safely store a pointer to the
** filename if it needs to remember the filename for some reason.
** If the zFilename parameter is xOpen is a NULL pointer then xOpen
** must invent its own temporary name for the file.  Whenever the 
** xFilename parameter is NULL it will also be the case that the
** flags parameter will include [SQLITE_OPEN_DELETEONCLOSE].
**
** The flags argument to xOpen() includes all bits set in
** the flags argument to [sqlite3_open_v2()].  Or if [sqlite3_open()]
** or [sqlite3_open16()] is used, then flags includes at least
** [SQLITE_OPEN_READWRITE] | [SQLITE_OPEN_CREATE]. 
** If xOpen() opens a file read-only then it sets *pOutFlags to
** include [SQLITE_OPEN_READONLY].  Other bits in *pOutFlags may be set.
**
** SQLite will also add one of the following flags to the xOpen()
** call, depending on the object being opened:
**
** <ul>
** <li>  [SQLITE_OPEN_MAIN_DB]
** <li>  [SQLITE_OPEN_MAIN_JOURNAL]
** <li>  [SQLITE_OPEN_TEMP_DB]
** <li>  [SQLITE_OPEN_TEMP_JOURNAL]
** <li>  [SQLITE_OPEN_TRANSIENT_DB]
** <li>  [SQLITE_OPEN_SUBJOURNAL]
** <li>  [SQLITE_OPEN_MASTER_JOURNAL]
** </ul>
**
** The file I/O implementation can use the object type flags to
** change the way it deals with files.  For example, an application
** that does not care about crash recovery or rollback might make
** the open of a journal file a no-op.  Writes to this journal would
** also be no-ops, and any attempt to read the journal would return
** SQLITE_IOERR.  Or the implementation might recognize that a database
** file will be doing page-aligned sector reads and writes in a random
** order and set up its I/O subsystem accordingly.
**
** SQLite might also add one of the following flags to the xOpen method:
**
** <ul>
** <li> [SQLITE_OPEN_DELETEONCLOSE]
** <li> [SQLITE_OPEN_EXCLUSIVE]
** </ul>
**
** The [SQLITE_OPEN_DELETEONCLOSE] flag means the file should be
** deleted when it is closed.  The [SQLITE_OPEN_DELETEONCLOSE]
** will be set for TEMP  databases, journals and for subjournals.
**
** The [SQLITE_OPEN_EXCLUSIVE] flag is always used in conjunction
** with the [SQLITE_OPEN_CREATE] flag, which are both directly
** analogous to the O_EXCL and O_CREAT flags of the POSIX open()
** API.  The SQLITE_OPEN_EXCLUSIVE flag, when paired with the 
** SQLITE_OPEN_CREATE, is used to indicate that file should always
** be created, and that it is an error if it already exists.
** It is <i>not</i> used to indicate the file should be opened 
** for exclusive access.
**
** At least szOsFile bytes of memory are allocated by SQLite
** to hold the  [sqlite3_file] structure passed as the third
** argument to xOpen.  The xOpen method does not have to
** allocate the structure; it should just fill it in.  Note that
** the xOpen method must set the sqlite3_file.pMethods to either
** a valid [sqlite3_io_methods] object or to NULL.  xOpen must do
** this even if the open fails.  SQLite expects that the sqlite3_file.pMethods
** element will be valid after xOpen returns regardless of the success
** or failure of the xOpen call.
**
** The flags argument to xAccess() may be [SQLITE_ACCESS_EXISTS]
** to test for the existence of a file, or [SQLITE_ACCESS_READWRITE] to
** test whether a file is readable and writable, or [SQLITE_ACCESS_READ]
** to test whether a file is at least readable.   The file can be a
** directory.
**
** SQLite will always allocate at least mxPathname+1 bytes for the
** output buffer xFullPathname.  The exact size of the output buffer
** is also passed as a parameter to both  methods. If the output buffer
** is not large enough, [SQLITE_CANTOPEN] should be returned. Since this is
** handled as a fatal error by SQLite, vfs implementations should endeavor
** to prevent this by setting mxPathname to a sufficiently large value.
**
** The xRandomness(), xSleep(), and xCurrentTime() interfaces
** are not strictly a part of the filesystem, but they are
** included in the VFS structure for completeness.
** The xRandomness() function attempts to return nBytes bytes
** of good-quality randomness into zOut.  The return value is
** the actual number of bytes of randomness obtained.
** The xSleep() method causes the calling thread to sleep for at
** least the number of microseconds given.  The xCurrentTime()
** method returns a Julian Day Number for the current date and time.
**
*/
typedef struct sqlite3_vfs sqlite3_vfs;
struct sqlite3_vfs {
  int iVersion;            /* Structure version number */
  int szOsFile;            /* Size of subclassed sqlite3_file */
  int mxPathname;          /* Maximum file pathname length */
  sqlite3_vfs *pNext;      /* Next registered VFS */
  const char *zName;       /* Name of this virtual file system */
  void *pAppData;          /* Pointer to application-specific data */
  int (*xOpen)(sqlite3_vfs*, const char *zName, sqlite3_file*,
               int flags, int *pOutFlags);
  int (*xDelete)(sqlite3_vfs*, const char *zName, int syncDir);
  int (*xAccess)(sqlite3_vfs*, const char *zName, int flags, int *pResOut);
  int (*xFullPathname)(sqlite3_vfs*, const char *zName, int nOut, char *zOut);
  void *(*xDlOpen)(sqlite3_vfs*, const char *zFilename);
  void (*xDlError)(sqlite3_vfs*, int nByte, char *zErrMsg);
  void (*(*xDlSym)(sqlite3_vfs*,void*, const char *zSymbol))(void);
  void (*xDlClose)(sqlite3_vfs*, void*);
  int (*xRandomness)(sqlite3_vfs*, int nByte, char *zOut);
  int (*xSleep)(sqlite3_vfs*, int microseconds);
  int (*xCurrentTime)(sqlite3_vfs*, double*);
  int (*xGetLastError)(sqlite3_vfs*, int, char *);
  /* New fields may be appended in figure versions.  The iVersion
  ** value will increment whenever this happens. */
};

/*
** CAPI3REF: Flags for the xAccess VFS method {H11190} <H11140>
**
** These integer constants can be used as the third parameter to
** the xAccess method of an [sqlite3_vfs] object. {END}  They determine
** what kind of permissions the xAccess method is looking for.
** With SQLITE_ACCESS_EXISTS, the xAccess method
** simply checks whether the file exists.
** With SQLITE_ACCESS_READWRITE, the xAccess method
** checks whether the file is both readable and writable.
** With SQLITE_ACCESS_READ, the xAccess method
** checks whether the file is readable.
*/
#define SQLITE_ACCESS_EXISTS    0
#define SQLITE_ACCESS_READWRITE 1
#define SQLITE_ACCESS_READ      2

/*
** CAPI3REF: Initialize The SQLite Library {H10130} <S20000><S30100>
**
** The sqlite3_initialize() routine initializes the
** SQLite library.  The sqlite3_shutdown() routine
** deallocates any resources that were allocated by sqlite3_initialize().
**
** A call to sqlite3_initialize() is an "effective" call if it is
** the first time sqlite3_initialize() is invoked during the lifetime of
** the process, or if it is the first time sqlite3_initialize() is invoked
** following a call to sqlite3_shutdown().  Only an effective call
** of sqlite3_initialize() does any initialization.  All other calls
** are harmless no-ops.
**
** A call to sqlite3_shutdown() is an "effective" call if it is the first
** call to sqlite3_shutdown() since the last sqlite3_initialize().  Only
** an effective call to sqlite3_shutdown() does any deinitialization.
** All other calls to sqlite3_shutdown() are harmless no-ops.
**
** Among other things, sqlite3_initialize() shall invoke
** sqlite3_os_init().  Similarly, sqlite3_shutdown()
** shall invoke sqlite3_os_end().
**
** The sqlite3_initialize() routine returns [SQLITE_OK] on success.
** If for some reason, sqlite3_initialize() is unable to initialize
** the library (perhaps it is unable to allocate a needed resource such
** as a mutex) it returns an [error code] other than [SQLITE_OK].
**
** The sqlite3_initialize() routine is called internally by many other
** SQLite interfaces so that an application usually does not need to
** invoke sqlite3_initialize() directly.  For example, [sqlite3_open()]
** calls sqlite3_initialize() so the SQLite library will be automatically
** initialized when [sqlite3_open()] is called if it has not be initialized
** already.  However, if SQLite is compiled with the [SQLITE_OMIT_AUTOINIT]
** compile-time option, then the automatic calls to sqlite3_initialize()
** are omitted and the application must call sqlite3_initialize() directly
** prior to using any other SQLite interface.  For maximum portability,
** it is recommended that applications always invoke sqlite3_initialize()
** directly prior to using any other SQLite interface.  Future releases
** of SQLite may require this.  In other words, the behavior exhibited
** when SQLite is compiled with [SQLITE_OMIT_AUTOINIT] might become the
** default behavior in some future release of SQLite.
**
** The sqlite3_os_init() routine does operating-system specific
** initialization of the SQLite library.  The sqlite3_os_end()
** routine undoes the effect of sqlite3_os_init().  Typical tasks
** performed by these routines include allocation or deallocation
** of static resources, initialization of global variables,
** setting up a default [sqlite3_vfs] module, or setting up
** a default configuration using [sqlite3_config()].
**
** The application should never invoke either sqlite3_os_init()
** or sqlite3_os_end() directly.  The application should only invoke
** sqlite3_initialize() and sqlite3_shutdown().  The sqlite3_os_init()
** interface is called automatically by sqlite3_initialize() and
** sqlite3_os_end() is called by sqlite3_shutdown().  Appropriate
** implementations for sqlite3_os_init() and sqlite3_os_end()
** are built into SQLite when it is compiled for Unix, Windows, or OS/2.
** When [custom builds | built for other platforms]
** (using the [SQLITE_OS_OTHER=1] compile-time
** option) the application must supply a suitable implementation for
** sqlite3_os_init() and sqlite3_os_end().  An application-supplied
** implementation of sqlite3_os_init() or sqlite3_os_end()
** must return [SQLITE_OK] on success and some other [error code] upon
** failure.
*/
SQLITE_API int sqlite3_initialize(void);
SQLITE_API int sqlite3_shutdown(void);
SQLITE_API int sqlite3_os_init(void);
SQLITE_API int sqlite3_os_end(void);

/*
** CAPI3REF: Configuring The SQLite Library {H14100} <S20000><S30200>
** EXPERIMENTAL
**
** The sqlite3_config() interface is used to make global configuration
** changes to SQLite in order to tune SQLite to the specific needs of
** the application.  The default configuration is recommended for most
** applications and so this routine is usually not necessary.  It is
** provided to support rare applications with unusual needs.
**
** The sqlite3_config() interface is not threadsafe.  The application
** must insure that no other SQLite interfaces are invoked by other
** threads while sqlite3_config() is running.  Furthermore, sqlite3_config()
** may only be invoked prior to library initialization using
** [sqlite3_initialize()] or after shutdown by [sqlite3_shutdown()].
** Note, however, that sqlite3_config() can be called as part of the
** implementation of an application-defined [sqlite3_os_init()].
**
** The first argument to sqlite3_config() is an integer
** [SQLITE_CONFIG_SINGLETHREAD | configuration option] that determines
** what property of SQLite is to be configured.  Subsequent arguments
** vary depending on the [SQLITE_CONFIG_SINGLETHREAD | configuration option]
** in the first argument.
**
** When a configuration option is set, sqlite3_config() returns [SQLITE_OK].
** If the option is unknown or SQLite is unable to set the option
** then this routine returns a non-zero [error code].
**
** Requirements:
** [H14103] [H14106] [H14120] [H14123] [H14126] [H14129] [H14132] [H14135]
** [H14138] [H14141] [H14144] [H14147] [H14150] [H14153] [H14156] [H14159]
** [H14162] [H14165] [H14168]
*/
SQLITE_API SQLITE_EXPERIMENTAL int sqlite3_config(int, ...);

/*
** CAPI3REF: Configure database connections  {H14200} <S20000>
** EXPERIMENTAL
**
** The sqlite3_db_config() interface is used to make configuration
** changes to a [database connection].  The interface is similar to
** [sqlite3_config()] except that the changes apply to a single
** [database connection] (specified in the first argument).  The
** sqlite3_db_config() interface can only be used immediately after
** the database connection is created using [sqlite3_open()],
** [sqlite3_open16()], or [sqlite3_open_v2()].  
**
** The second argument to sqlite3_db_config(D,V,...)  is the
** configuration verb - an integer code that indicates what
** aspect of the [database connection] is being configured.
** The only choice for this value is [SQLITE_DBCONFIG_LOOKASIDE].
** New verbs are likely to be added in future releases of SQLite.
** Additional arguments depend on the verb.
**
** Requirements:
** [H14203] [H14206] [H14209] [H14212] [H14215]
*/
SQLITE_API SQLITE_EXPERIMENTAL int sqlite3_db_config(sqlite3*, int op, ...);

/*
** CAPI3REF: Memory Allocation Routines {H10155} <S20120>
** EXPERIMENTAL
**
** An instance of this object defines the interface between SQLite
** and low-level memory allocation routines.
**
** This object is used in only one place in the SQLite interface.
** A pointer to an instance of this object is the argument to
** [sqlite3_config()] when the configuration option is
** [SQLITE_CONFIG_MALLOC] or [SQLITE_CONFIG_GETMALLOC].  
** By creating an instance of this object
** and passing it to [sqlite3_config]([SQLITE_CONFIG_MALLOC])
** during configuration, an application can specify an alternative
** memory allocation subsystem for SQLite to use for all of its
** dynamic memory needs.
**
** Note that SQLite comes with several [built-in memory allocators]
** that are perfectly adequate for the overwhelming majority of applications
** and that this object is only useful to a tiny minority of applications
** with specialized memory allocation requirements.  This object is
** also used during testing of SQLite in order to specify an alternative
** memory allocator that simulates memory out-of-memory conditions in
** order to verify that SQLite recovers gracefully from such
** conditions.
**
** The xMalloc and xFree methods must work like the
** malloc() and free() functions from the standard C library.
** The xRealloc method must work like realloc() from the standard C library
** with the exception that if the second argument to xRealloc is zero,
** xRealloc must be a no-op - it must not perform any allocation or
** deallocation.  SQLite guaranteeds that the second argument to
** xRealloc is always a value returned by a prior call to xRoundup.
** And so in cases where xRoundup always returns a positive number,
** xRealloc can perform exactly as the standard library realloc() and
** still be in compliance with this specification.
**
** xSize should return the allocated size of a memory allocation
** previously obtained from xMalloc or xRealloc.  The allocated size
** is always at least as big as the requested size but may be larger.
**
** The xRoundup method returns what would be the allocated size of
** a memory allocation given a particular requested size.  Most memory
** allocators round up memory allocations at least to the next multiple
** of 8.  Some allocators round up to a larger multiple or to a power of 2.
** Every memory allocation request coming in through [sqlite3_malloc()]
** or [sqlite3_realloc()] first calls xRoundup.  If xRoundup returns 0, 
** that causes the corresponding memory allocation to fail.
**
** The xInit method initializes the memory allocator.  (For example,
** it might allocate any require mutexes or initialize internal data
** structures.  The xShutdown method is invoked (indirectly) by
** [sqlite3_shutdown()] and should deallocate any resources acquired
** by xInit.  The pAppData pointer is used as the only parameter to
** xInit and xShutdown.
**
** SQLite holds the [SQLITE_MUTEX_STATIC_MASTER] mutex when it invokes
** the xInit method, so the xInit method need not be threadsafe.  The
** xShutdown method is only called from [sqlite3_shutdown()] so it does
** not need to be threadsafe either.  For all other methods, SQLite
** holds the [SQLITE_MUTEX_STATIC_MEM] mutex as long as the
** [SQLITE_CONFIG_MEMSTATUS] configuration option is turned on (which
** it is by default) and so the methods are automatically serialized.
** However, if [SQLITE_CONFIG_MEMSTATUS] is disabled, then the other
** methods must be threadsafe or else make their own arrangements for
** serialization.
**
** SQLite will never invoke xInit() more than once without an intervening
** call to xShutdown().
*/
typedef struct sqlite3_mem_methods sqlite3_mem_methods;
struct sqlite3_mem_methods {
  void *(*xMalloc)(int);         /* Memory allocation function */
  void (*xFree)(void*);          /* Free a prior allocation */
  void *(*xRealloc)(void*,int);  /* Resize an allocation */
  int (*xSize)(void*);           /* Return the size of an allocation */
  int (*xRoundup)(int);          /* Round up request size to allocation size */
  int (*xInit)(void*);           /* Initialize the memory allocator */
  void (*xShutdown)(void*);      /* Deinitialize the memory allocator */
  void *pAppData;                /* Argument to xInit() and xShutdown() */
};

/*
** CAPI3REF: Configuration Options {H10160} <S20000>
** EXPERIMENTAL
**
** These constants are the available integer configuration options that
** can be passed as the first argument to the [sqlite3_config()] interface.
**
** New configuration options may be added in future releases of SQLite.
** Existing configuration options might be discontinued.  Applications
** should check the return code from [sqlite3_config()] to make sure that
** the call worked.  The [sqlite3_config()] interface will return a
** non-zero [error code] if a discontinued or unsupported configuration option
** is invoked.
**
** <dl>
** <dt>SQLITE_CONFIG_SINGLETHREAD</dt>
** <dd>There are no arguments to this option.  This option disables
** all mutexing and puts SQLite into a mode where it can only be used
** by a single thread.</dd>
**
** <dt>SQLITE_CONFIG_MULTITHREAD</dt>
** <dd>There are no arguments to this option.  This option disables
** mutexing on [database connection] and [prepared statement] objects.
** The application is responsible for serializing access to
** [database connections] and [prepared statements].  But other mutexes
** are enabled so that SQLite will be safe to use in a multi-threaded
** environment as long as no two threads attempt to use the same
** [database connection] at the same time.  See the [threading mode]
** documentation for additional information.</dd>
**
** <dt>SQLITE_CONFIG_SERIALIZED</dt>
** <dd>There are no arguments to this option.  This option enables
** all mutexes including the recursive
** mutexes on [database connection] and [prepared statement] objects.
** In this mode (which is the default when SQLite is compiled with
** [SQLITE_THREADSAFE=1]) the SQLite library will itself serialize access
** to [database connections] and [prepared statements] so that the
** application is free to use the same [database connection] or the
** same [prepared statement] in different threads at the same time.
** See the [threading mode] documentation for additional information.</dd>
**
** <dt>SQLITE_CONFIG_MALLOC</dt>
** <dd>This option takes a single argument which is a pointer to an
** instance of the [sqlite3_mem_methods] structure.  The argument specifies
** alternative low-level memory allocation routines to be used in place of
** the memory allocation routines built into SQLite.</dd>
**
** <dt>SQLITE_CONFIG_GETMALLOC</dt>
** <dd>This option takes a single argument which is a pointer to an
** instance of the [sqlite3_mem_methods] structure.  The [sqlite3_mem_methods]
** structure is filled with the currently defined memory allocation routines.
** This option can be used to overload the default memory allocation
** routines with a wrapper that simulations memory allocation failure or
** tracks memory usage, for example.</dd>
**
** <dt>SQLITE_CONFIG_MEMSTATUS</dt>
** <dd>This option takes single argument of type int, interpreted as a 
** boolean, which enables or disables the collection of memory allocation 
** statistics. When disabled, the following SQLite interfaces become 
** non-operational:
**   <ul>
**   <li> [sqlite3_memory_used()]
**   <li> [sqlite3_memory_highwater()]
**   <li> [sqlite3_soft_heap_limit()]
**   <li> [sqlite3_status()]
**   </ul>
** </dd>
**
** <dt>SQLITE_CONFIG_SCRATCH</dt>
** <dd>This option specifies a static memory buffer that SQLite can use for
** scratch memory.  There are three arguments:  A pointer an 8-byte
** aligned memory buffer from which the scrach allocations will be
** drawn, the size of each scratch allocation (sz),
** and the maximum number of scratch allocations (N).  The sz
** argument must be a multiple of 16. The sz parameter should be a few bytes
** larger than the actual scratch space required due to internal overhead.
** The first argument should pointer to an 8-byte aligned buffer
** of at least sz*N bytes of memory.
** SQLite will use no more than one scratch buffer at once per thread, so
** N should be set to the expected maximum number of threads.  The sz
** parameter should be 6 times the size of the largest database page size.
** Scratch buffers are used as part of the btree balance operation.  If
** The btree balancer needs additional memory beyond what is provided by
** scratch buffers or if no scratch buffer space is specified, then SQLite
** goes to [sqlite3_malloc()] to obtain the memory it needs.</dd>
**
** <dt>SQLITE_CONFIG_PAGECACHE</dt>
** <dd>This option specifies a static memory buffer that SQLite can use for
** the database page cache with the default page cache implemenation.  
** This configuration should not be used if an application-define page
** cache implementation is loaded using the SQLITE_CONFIG_PCACHE option.
** There are three arguments to this option: A pointer to 8-byte aligned
** memory, the size of each page buffer (sz), and the number of pages (N).
** The sz argument should be the size of the largest database page
** (a power of two between 512 and 32768) plus a little extra for each
** page header.  The page header size is 20 to 40 bytes depending on
** the host architecture.  It is harmless, apart from the wasted memory,
** to make sz a little too large.  The first
** argument should point to an allocation of at least sz*N bytes of memory.
** SQLite will use the memory provided by the first argument to satisfy its
** memory needs for the first N pages that it adds to cache.  If additional
** page cache memory is needed beyond what is provided by this option, then
** SQLite goes to [sqlite3_malloc()] for the additional storage space.
** The implementation might use one or more of the N buffers to hold 
** memory accounting information. The pointer in the first argument must
** be aligned to an 8-byte boundary or subsequent behavior of SQLite
** will be undefined.</dd>
**
** <dt>SQLITE_CONFIG_HEAP</dt>
** <dd>This option specifies a static memory buffer that SQLite will use
** for all of its dynamic memory allocation needs beyond those provided
** for by [SQLITE_CONFIG_SCRATCH] and [SQLITE_CONFIG_PAGECACHE].
** There are three arguments: An 8-byte aligned pointer to the memory,
** the number of bytes in the memory buffer, and the minimum allocation size.
** If the first pointer (the memory pointer) is NULL, then SQLite reverts
** to using its default memory allocator (the system malloc() implementation),
** undoing any prior invocation of [SQLITE_CONFIG_MALLOC].  If the
** memory pointer is not NULL and either [SQLITE_ENABLE_MEMSYS3] or
** [SQLITE_ENABLE_MEMSYS5] are defined, then the alternative memory
** allocator is engaged to handle all of SQLites memory allocation needs.
** The first pointer (the memory pointer) must be aligned to an 8-byte
** boundary or subsequent behavior of SQLite will be undefined.</dd>
**
** <dt>SQLITE_CONFIG_MUTEX</dt>
** <dd>This option takes a single argument which is a pointer to an
** instance of the [sqlite3_mutex_methods] structure.  The argument specifies
** alternative low-level mutex routines to be used in place
** the mutex routines built into SQLite.</dd>
**
** <dt>SQLITE_CONFIG_GETMUTEX</dt>
** <dd>This option takes a single argument which is a pointer to an
** instance of the [sqlite3_mutex_methods] structure.  The
** [sqlite3_mutex_methods]
** structure is filled with the currently defined mutex routines.
** This option can be used to overload the default mutex allocation
** routines with a wrapper used to track mutex usage for performance
** profiling or testing, for example.</dd>
**
** <dt>SQLITE_CONFIG_LOOKASIDE</dt>
** <dd>This option takes two arguments that determine the default
** memory allocation lookaside optimization.  The first argument is the
** size of each lookaside buffer slot and the second is the number of
** slots allocated to each database connection.  This option sets the
** <i>default</i> lookaside size.  The [SQLITE_DBCONFIG_LOOKASIDE]
** verb to [sqlite3_db_config()] can be used to change the lookaside
** configuration on individual connections.</dd>
**
** <dt>SQLITE_CONFIG_PCACHE</dt>
** <dd>This option takes a single argument which is a pointer to
** an [sqlite3_pcache_methods] object.  This object specifies the interface
** to a custom page cache implementation.  SQLite makes a copy of the
** object and uses it for page cache memory allocations.</dd>
**
** <dt>SQLITE_CONFIG_GETPCACHE</dt>
** <dd>This option takes a single argument which is a pointer to an
** [sqlite3_pcache_methods] object.  SQLite copies of the current
** page cache implementation into that object.</dd>
**
** </dl>
*/
#define SQLITE_CONFIG_SINGLETHREAD  1  /* nil */
#define SQLITE_CONFIG_MULTITHREAD   2  /* nil */
#define SQLITE_CONFIG_SERIALIZED    3  /* nil */
#define SQLITE_CONFIG_MALLOC        4  /* sqlite3_mem_methods* */
#define SQLITE_CONFIG_GETMALLOC     5  /* sqlite3_mem_methods* */
#define SQLITE_CONFIG_SCRATCH       6  /* void*, int sz, int N */
#define SQLITE_CONFIG_PAGECACHE     7  /* void*, int sz, int N */
#define SQLITE_CONFIG_HEAP          8  /* void*, int nByte, int min */
#define SQLITE_CONFIG_MEMSTATUS     9  /* boolean */
#define SQLITE_CONFIG_MUTEX        10  /* sqlite3_mutex_methods* */
#define SQLITE_CONFIG_GETMUTEX     11  /* sqlite3_mutex_methods* */
/* previously SQLITE_CONFIG_CHUNKALLOC 12 which is now unused. */ 
#define SQLITE_CONFIG_LOOKASIDE    13  /* int int */
#define SQLITE_CONFIG_PCACHE       14  /* sqlite3_pcache_methods* */
#define SQLITE_CONFIG_GETPCACHE    15  /* sqlite3_pcache_methods* */

/*
** CAPI3REF: Configuration Options {H10170} <S20000>
** EXPERIMENTAL
**
** These constants are the available integer configuration options that
** can be passed as the second argument to the [sqlite3_db_config()] interface.
**
** New configuration options may be added in future releases of SQLite.
** Existing configuration options might be discontinued.  Applications
** should check the return code from [sqlite3_db_config()] to make sure that
** the call worked.  The [sqlite3_db_config()] interface will return a
** non-zero [error code] if a discontinued or unsupported configuration option
** is invoked.
**
** <dl>
** <dt>SQLITE_DBCONFIG_LOOKASIDE</dt>
** <dd>This option takes three additional arguments that determine the 
** [lookaside memory allocator] configuration for the [database connection].
** The first argument (the third parameter to [sqlite3_db_config()] is a
** pointer to an memory buffer to use for lookaside memory.
** The first argument may be NULL in which case SQLite will allocate the
** lookaside buffer itself using [sqlite3_malloc()].  The second argument is the
** size of each lookaside buffer slot and the third argument is the number of
** slots.  The size of the buffer in the first argument must be greater than
** or equal to the product of the second and third arguments.  The buffer
** must be aligned to an 8-byte boundary.  If the second argument is not
** a multiple of 8, it is internally rounded down to the next smaller
** multiple of 8.  See also: [SQLITE_CONFIG_LOOKASIDE]</dd>
**
** </dl>
*/
#define SQLITE_DBCONFIG_LOOKASIDE    1001  /* void* int int */


/*
** CAPI3REF: Enable Or Disable Extended Result Codes {H12200} <S10700>
**
** The sqlite3_extended_result_codes() routine enables or disables the
** [extended result codes] feature of SQLite. The extended result
** codes are disabled by default for historical compatibility considerations.
**
** Requirements:
** [H12201] [H12202]
*/
SQLITE_API int sqlite3_extended_result_codes(sqlite3*, int onoff);

/*
** CAPI3REF: Last Insert Rowid {H12220} <S10700>
**
** Each entry in an SQLite table has a unique 64-bit signed
** integer key called the [ROWID | "rowid"]. The rowid is always available
** as an undeclared column named ROWID, OID, or _ROWID_ as long as those
** names are not also used by explicitly declared columns. If
** the table has a column of type [INTEGER PRIMARY KEY] then that column
** is another alias for the rowid.
**
** This routine returns the [rowid] of the most recent
** successful [INSERT] into the database from the [database connection]
** in the first argument.  If no successful [INSERT]s
** have ever occurred on that database connection, zero is returned.
**
** If an [INSERT] occurs within a trigger, then the [rowid] of the inserted
** row is returned by this routine as long as the trigger is running.
** But once the trigger terminates, the value returned by this routine
** reverts to the last value inserted before the trigger fired.
**
** An [INSERT] that fails due to a constraint violation is not a
** successful [INSERT] and does not change the value returned by this
** routine.  Thus INSERT OR FAIL, INSERT OR IGNORE, INSERT OR ROLLBACK,
** and INSERT OR ABORT make no changes to the return value of this
** routine when their insertion fails.  When INSERT OR REPLACE
** encounters a constraint violation, it does not fail.  The
** INSERT continues to completion after deleting rows that caused
** the constraint problem so INSERT OR REPLACE will always change
** the return value of this interface.
**
** For the purposes of this routine, an [INSERT] is considered to
** be successful even if it is subsequently rolled back.
**
** Requirements:
** [H12221] [H12223]
**
** If a separate thread performs a new [INSERT] on the same
** database connection while the [sqlite3_last_insert_rowid()]
** function is running and thus changes the last insert [rowid],
** then the value returned by [sqlite3_last_insert_rowid()] is
** unpredictable and might not equal either the old or the new
** last insert [rowid].
*/
SQLITE_API sqlite3_int64 sqlite3_last_insert_rowid(sqlite3*);

/*
** CAPI3REF: Count The Number Of Rows Modified {H12240} <S10600>
**
** This function returns the number of database rows that were changed
** or inserted or deleted by the most recently completed SQL statement
** on the [database connection] specified by the first parameter.
** Only changes that are directly specified by the [INSERT], [UPDATE],
** or [DELETE] statement are counted.  Auxiliary changes caused by
** triggers or [foreign key actions] are not counted. Use the
** [sqlite3_total_changes()] function to find the total number of changes
** including changes caused by triggers and foreign key actions.
**
** Changes to a view that are simulated by an [INSTEAD OF trigger]
** are not counted.  Only real table changes are counted.
**
** A "row change" is a change to a single row of a single table
** caused by an INSERT, DELETE, or UPDATE statement.  Rows that
** are changed as side effects of [REPLACE] constraint resolution,
** rollback, ABORT processing, [DROP TABLE], or by any other
** mechanisms do not count as direct row changes.
**
** A "trigger context" is a scope of execution that begins and
** ends with the script of a [CREATE TRIGGER | trigger]. 
** Most SQL statements are
** evaluated outside of any trigger.  This is the "top level"
** trigger context.  If a trigger fires from the top level, a
** new trigger context is entered for the duration of that one
** trigger.  Subtriggers create subcontexts for their duration.
**
** Calling [sqlite3_exec()] or [sqlite3_step()] recursively does
** not create a new trigger context.
**
** This function returns the number of direct row changes in the
** most recent INSERT, UPDATE, or DELETE statement within the same
** trigger context.
**
** Thus, when called from the top level, this function returns the
** number of changes in the most recent INSERT, UPDATE, or DELETE
** that also occurred at the top level.  Within the body of a trigger,
** the sqlite3_changes() interface can be called to find the number of
** changes in the most recently completed INSERT, UPDATE, or DELETE
** statement within the body of the same trigger.
** However, the number returned does not include changes
** caused by subtriggers since those have their own context.
**
** See also the [sqlite3_total_changes()] interface and the
** [count_changes pragma].
**
** Requirements:
** [H12241] [H12243]
**
** If a separate thread makes changes on the same database connection
** while [sqlite3_changes()] is running then the value returned
** is unpredictable and not meaningful.
*/
SQLITE_API int sqlite3_changes(sqlite3*);

/*
** CAPI3REF: Total Number Of Rows Modified {H12260} <S10600>
**
** This function returns the number of row changes caused by [INSERT],
** [UPDATE] or [DELETE] statements since the [database connection] was opened.
** The count includes all changes from all [CREATE TRIGGER | trigger] 
** contexts and changes made by [foreign key actions]. However,
** the count does not include changes used to implement [REPLACE] constraints,
** do rollbacks or ABORT processing, or [DROP TABLE] processing.  The
** count does not include rows of views that fire an [INSTEAD OF trigger],
** though if the INSTEAD OF trigger makes changes of its own, those changes 
** are counted.
** The changes are counted as soon as the statement that makes them is
** completed (when the statement handle is passed to [sqlite3_reset()] or
** [sqlite3_finalize()]).
**
** See also the [sqlite3_changes()] interface and the
** [count_changes pragma].
**
** Requirements:
** [H12261] [H12263]
**
** If a separate thread makes changes on the same database connection
** while [sqlite3_total_changes()] is running then the value
** returned is unpredictable and not meaningful.
*/
SQLITE_API int sqlite3_total_changes(sqlite3*);

/*
** CAPI3REF: Interrupt A Long-Running Query {H12270} <S30500>
**
** This function causes any pending database operation to abort and
** return at its earliest opportunity. This routine is typically
** called in response to a user action such as pressing "Cancel"
** or Ctrl-C where the user wants a long query operation to halt
** immediately.
**
** It is safe to call this routine from a thread different from the
** thread that is currently running the database operation.  But it
** is not safe to call this routine with a [database connection] that
** is closed or might close before sqlite3_interrupt() returns.
**
** If an SQL operation is very nearly finished at the time when
** sqlite3_interrupt() is called, then it might not have an opportunity
** to be interrupted and might continue to completion.
**
** An SQL operation that is interrupted will return [SQLITE_INTERRUPT].
** If the interrupted SQL operation is an INSERT, UPDATE, or DELETE
** that is inside an explicit transaction, then the entire transaction
** will be rolled back automatically.
**
** The sqlite3_interrupt(D) call is in effect until all currently running
** SQL statements on [database connection] D complete.  Any new SQL statements
** that are started after the sqlite3_interrupt() call and before the 
** running statements reaches zero are interrupted as if they had been
** running prior to the sqlite3_interrupt() call.  New SQL statements
** that are started after the running statement count reaches zero are
** not effected by the sqlite3_interrupt().
** A call to sqlite3_interrupt(D) that occurs when there are no running
** SQL statements is a no-op and has no effect on SQL statements
** that are started after the sqlite3_interrupt() call returns.
**
** Requirements:
** [H12271] [H12272]
**
** If the database connection closes while [sqlite3_interrupt()]
** is running then bad things will likely happen.
*/
SQLITE_API void sqlite3_interrupt(sqlite3*);

/*
** CAPI3REF: Determine If An SQL Statement Is Complete {H10510} <S70200>
**
** These routines are useful during command-line input to determine if the
** currently entered text seems to form a complete SQL statement or
** if additional input is needed before sending the text into
** SQLite for parsing.  These routines return 1 if the input string
** appears to be a complete SQL statement.  A statement is judged to be
** complete if it ends with a semicolon token and is not a prefix of a
** well-formed CREATE TRIGGER statement.  Semicolons that are embedded within
** string literals or quoted identifier names or comments are not
** independent tokens (they are part of the token in which they are
** embedded) and thus do not count as a statement terminator.  Whitespace
** and comments that follow the final semicolon are ignored.
**
** These routines return 0 if the statement is incomplete.  If a
** memory allocation fails, then SQLITE_NOMEM is returned.
**
** These routines do not parse the SQL statements thus
** will not detect syntactically incorrect SQL.
**
** If SQLite has not been initialized using [sqlite3_initialize()] prior 
** to invoking sqlite3_complete16() then sqlite3_initialize() is invoked
** automatically by sqlite3_complete16().  If that initialization fails,
** then the return value from sqlite3_complete16() will be non-zero
** regardless of whether or not the input SQL is complete.
**
** Requirements: [H10511] [H10512]
**
** The input to [sqlite3_complete()] must be a zero-terminated
** UTF-8 string.
**
** The input to [sqlite3_complete16()] must be a zero-terminated
** UTF-16 string in native byte order.
*/
SQLITE_API int sqlite3_complete(const char *sql);
SQLITE_API int sqlite3_complete16(const void *sql);

/*
** CAPI3REF: Register A Callback To Handle SQLITE_BUSY Errors {H12310} <S40400>
**
** This routine sets a callback function that might be invoked whenever
** an attempt is made to open a database table that another thread
** or process has locked.
**
** If the busy callback is NULL, then [SQLITE_BUSY] or [SQLITE_IOERR_BLOCKED]
** is returned immediately upon encountering the lock. If the busy callback
** is not NULL, then the callback will be invoked with two arguments.
**
** The first argument to the handler is a copy of the void* pointer which
** is the third argument to sqlite3_busy_handler().  The second argument to
** the handler callback is the number of times that the busy handler has
** been invoked for this locking event.  If the
** busy callback returns 0, then no additional attempts are made to
** access the database and [SQLITE_BUSY] or [SQLITE_IOERR_BLOCKED] is returned.
** If the callback returns non-zero, then another attempt
** is made to open the database for reading and the cycle repeats.
**
** The presence of a busy handler does not guarantee that it will be invoked
** when there is lock contention. If SQLite determines that invoking the busy
** handler could result in a deadlock, it will go ahead and return [SQLITE_BUSY]
** or [SQLITE_IOERR_BLOCKED] instead of invoking the busy handler.
** Consider a scenario where one process is holding a read lock that
** it is trying to promote to a reserved lock and
** a second process is holding a reserved lock that it is trying
** to promote to an exclusive lock.  The first process cannot proceed
** because it is blocked by the second and the second process cannot
** proceed because it is blocked by the first.  If both processes
** invoke the busy handlers, neither will make any progress.  Therefore,
** SQLite returns [SQLITE_BUSY] for the first process, hoping that this
** will induce the first process to release its read lock and allow
** the second process to proceed.
**
** The default busy callback is NULL.
**
** The [SQLITE_BUSY] error is converted to [SQLITE_IOERR_BLOCKED]
** when SQLite is in the middle of a large transaction where all the
** changes will not fit into the in-memory cache.  SQLite will
** already hold a RESERVED lock on the database file, but it needs
** to promote this lock to EXCLUSIVE so that it can spill cache
** pages into the database file without harm to concurrent
** readers.  If it is unable to promote the lock, then the in-memory
** cache will be left in an inconsistent state and so the error
** code is promoted from the relatively benign [SQLITE_BUSY] to
** the more severe [SQLITE_IOERR_BLOCKED].  This error code promotion
** forces an automatic rollback of the changes.  See the
** <a href="/cvstrac/wiki?p=CorruptionFollowingBusyError">
** CorruptionFollowingBusyError</a> wiki page for a discussion of why
** this is important.
**
** There can only be a single busy handler defined for each
** [database connection].  Setting a new busy handler clears any
** previously set handler.  Note that calling [sqlite3_busy_timeout()]
** will also set or clear the busy handler.
**
** The busy callback should not take any actions which modify the
** database connection that invoked the busy handler.  Any such actions
** result in undefined behavior.
** 
** Requirements:
** [H12311] [H12312] [H12314] [H12316] [H12318]
**
** A busy handler must not close the database connection
** or [prepared statement] that invoked the busy handler.
*/
SQLITE_API int sqlite3_busy_handler(sqlite3*, int(*)(void*,int), void*);

/*
** CAPI3REF: Set A Busy Timeout {H12340} <S40410>
**
** This routine sets a [sqlite3_busy_handler | busy handler] that sleeps
** for a specified amount of time when a table is locked.  The handler
** will sleep multiple times until at least "ms" milliseconds of sleeping
** have accumulated. {H12343} After "ms" milliseconds of sleeping,
** the handler returns 0 which causes [sqlite3_step()] to return
** [SQLITE_BUSY] or [SQLITE_IOERR_BLOCKED].
**
** Calling this routine with an argument less than or equal to zero
** turns off all busy handlers.
**
** There can only be a single busy handler for a particular
** [database connection] any any given moment.  If another busy handler
** was defined  (using [sqlite3_busy_handler()]) prior to calling
** this routine, that other busy handler is cleared.
**
** Requirements:
** [H12341] [H12343] [H12344]
*/
SQLITE_API int sqlite3_busy_timeout(sqlite3*, int ms);

/*
** CAPI3REF: Convenience Routines For Running Queries {H12370} <S10000>
**
** Definition: A <b>result table</b> is memory data structure created by the
** [sqlite3_get_table()] interface.  A result table records the
** complete query results from one or more queries.
**
** The table conceptually has a number of rows and columns.  But
** these numbers are not part of the result table itself.  These
** numbers are obtained separately.  Let N be the number of rows
** and M be the number of columns.
**
** A result table is an array of pointers to zero-terminated UTF-8 strings.
** There are (N+1)*M elements in the array.  The first M pointers point
** to zero-terminated strings that  contain the names of the columns.
** The remaining entries all point to query results.  NULL values result
** in NULL pointers.  All other values are in their UTF-8 zero-terminated
** string representation as returned by [sqlite3_column_text()].
**
** A result table might consist of one or more memory allocations.
** It is not safe to pass a result table directly to [sqlite3_free()].
** A result table should be deallocated using [sqlite3_free_table()].
**
** As an example of the result table format, suppose a query result
** is as follows:
**
** <blockquote><pre>
**        Name        | Age
**        -----------------------
**        Alice       | 43
**        Bob         | 28
**        Cindy       | 21
** </pre></blockquote>
**
** There are two column (M==2) and three rows (N==3).  Thus the
** result table has 8 entries.  Suppose the result table is stored
** in an array names azResult.  Then azResult holds this content:
**
** <blockquote><pre>
**        azResult&#91;0] = "Name";
**        azResult&#91;1] = "Age";
**        azResult&#91;2] = "Alice";
**        azResult&#91;3] = "43";
**        azResult&#91;4] = "Bob";
**        azResult&#91;5] = "28";
**        azResult&#91;6] = "Cindy";
**        azResult&#91;7] = "21";
** </pre></blockquote>
**
** The sqlite3_get_table() function evaluates one or more
** semicolon-separated SQL statements in the zero-terminated UTF-8
** string of its 2nd parameter.  It returns a result table to the
** pointer given in its 3rd parameter.
**
** After the calling function has finished using the result, it should
** pass the pointer to the result table to sqlite3_free_table() in order to
** release the memory that was malloced.  Because of the way the
** [sqlite3_malloc()] happens within sqlite3_get_table(), the calling
** function must not try to call [sqlite3_free()] directly.  Only
** [sqlite3_free_table()] is able to release the memory properly and safely.
**
** The sqlite3_get_table() interface is implemented as a wrapper around
** [sqlite3_exec()].  The sqlite3_get_table() routine does not have access
** to any internal data structures of SQLite.  It uses only the public
** interface defined here.  As a consequence, errors that occur in the
** wrapper layer outside of the internal [sqlite3_exec()] call are not
** reflected in subsequent calls to [sqlite3_errcode()] or [sqlite3_errmsg()].
**
** Requirements:
** [H12371] [H12373] [H12374] [H12376] [H12379] [H12382]
*/
SQLITE_API int sqlite3_get_table(
  sqlite3 *db,          /* An open database */
  const char *zSql,     /* SQL to be evaluated */
  char ***pazResult,    /* Results of the query */
  int *pnRow,           /* Number of result rows written here */
  int *pnColumn,        /* Number of result columns written here */
  char **pzErrmsg       /* Error msg written here */
);
SQLITE_API void sqlite3_free_table(char **result);

/*
** CAPI3REF: Formatted String Printing Functions {H17400} <S70000><S20000>
**
** These routines are work-alikes of the "printf()" family of functions
** from the standard C library.
**
** The sqlite3_mprintf() and sqlite3_vmprintf() routines write their
** results into memory obtained from [sqlite3_malloc()].
** The strings returned by these two routines should be
** released by [sqlite3_free()].  Both routines return a
** NULL pointer if [sqlite3_malloc()] is unable to allocate enough
** memory to hold the resulting string.
**
** In sqlite3_snprintf() routine is similar to "snprintf()" from
** the standard C library.  The result is written into the
** buffer supplied as the second parameter whose size is given by
** the first parameter. Note that the order of the
** first two parameters is reversed from snprintf().  This is an
** historical accident that cannot be fixed without breaking
** backwards compatibility.  Note also that sqlite3_snprintf()
** returns a pointer to its buffer instead of the number of
** characters actually written into the buffer.  We admit that
** the number of characters written would be a more useful return
** value but we cannot change the implementation of sqlite3_snprintf()
** now without breaking compatibility.
**
** As long as the buffer size is greater than zero, sqlite3_snprintf()
** guarantees that the buffer is always zero-terminated.  The first
** parameter "n" is the total size of the buffer, including space for
** the zero terminator.  So the longest string that can be completely
** written will be n-1 characters.
**
** These routines all implement some additional formatting
** options that are useful for constructing SQL statements.
** All of the usual printf() formatting options apply.  In addition, there
** is are "%q", "%Q", and "%z" options.
**
** The %q option works like %s in that it substitutes a null-terminated
** string from the argument list.  But %q also doubles every '\'' character.
** %q is designed for use inside a string literal.  By doubling each '\''
** character it escapes that character and allows it to be inserted into
** the string.
**
** For example, assume the string variable zText contains text as follows:
**
** <blockquote><pre>
**  char *zText = "It's a happy day!";
** </pre></blockquote>
**
** One can use this text in an SQL statement as follows:
**
** <blockquote><pre>
**  char *zSQL = sqlite3_mprintf("INSERT INTO table VALUES('%q')", zText);
**  sqlite3_exec(db, zSQL, 0, 0, 0);
**  sqlite3_free(zSQL);
** </pre></blockquote>
**
** Because the %q format string is used, the '\'' character in zText
** is escaped and the SQL generated is as follows:
**
** <blockquote><pre>
**  INSERT INTO table1 VALUES('It''s a happy day!')
** </pre></blockquote>
**
** This is correct.  Had we used %s instead of %q, the generated SQL
** would have looked like this:
**
** <blockquote><pre>
**  INSERT INTO table1 VALUES('It's a happy day!');
** </pre></blockquote>
**
** This second example is an SQL syntax error.  As a general rule you should
** always use %q instead of %s when inserting text into a string literal.
**
** The %Q option works like %q except it also adds single quotes around
** the outside of the total string.  Additionally, if the parameter in the
** argument list is a NULL pointer, %Q substitutes the text "NULL" (without
** single quotes) in place of the %Q option.  So, for example, one could say:
**
** <blockquote><pre>
**  char *zSQL = sqlite3_mprintf("INSERT INTO table VALUES(%Q)", zText);
**  sqlite3_exec(db, zSQL, 0, 0, 0);
**  sqlite3_free(zSQL);
** </pre></blockquote>
**
** The code above will render a correct SQL statement in the zSQL
** variable even if the zText variable is a NULL pointer.
**
** The "%z" formatting option works exactly like "%s" with the
** addition that after the string has been read and copied into
** the result, [sqlite3_free()] is called on the input string. {END}
**
** Requirements:
** [H17403] [H17406] [H17407]
*/
SQLITE_API char *sqlite3_mprintf(const char*,...);
SQLITE_API char *sqlite3_vmprintf(const char*, va_list);
SQLITE_API char *sqlite3_snprintf(int,char*,const char*, ...);

/*
** CAPI3REF: Memory Allocation Subsystem {H17300} <S20000>
**
** The SQLite core  uses these three routines for all of its own
** internal memory allocation needs. "Core" in the previous sentence
** does not include operating-system specific VFS implementation.  The
** Windows VFS uses native malloc() and free() for some operations.
**
** The sqlite3_malloc() routine returns a pointer to a block
** of memory at least N bytes in length, where N is the parameter.
** If sqlite3_malloc() is unable to obtain sufficient free
** memory, it returns a NULL pointer.  If the parameter N to
** sqlite3_malloc() is zero or negative then sqlite3_malloc() returns
** a NULL pointer.
**
** Calling sqlite3_free() with a pointer previously returned
** by sqlite3_malloc() or sqlite3_realloc() releases that memory so
** that it might be reused.  The sqlite3_free() routine is
** a no-op if is called with a NULL pointer.  Passing a NULL pointer
** to sqlite3_free() is harmless.  After being freed, memory
** should neither be read nor written.  Even reading previously freed
** memory might result in a segmentation fault or other severe error.
** Memory corruption, a segmentation fault, or other severe error
** might result if sqlite3_free() is called with a non-NULL pointer that
** was not obtained from sqlite3_malloc() or sqlite3_realloc().
**
** The sqlite3_realloc() interface attempts to resize a
** prior memory allocation to be at least N bytes, where N is the
** second parameter.  The memory allocation to be resized is the first
** parameter.  If the first parameter to sqlite3_realloc()
** is a NULL pointer then its behavior is identical to calling
** sqlite3_malloc(N) where N is the second parameter to sqlite3_realloc().
** If the second parameter to sqlite3_realloc() is zero or
** negative then the behavior is exactly the same as calling
** sqlite3_free(P) where P is the first parameter to sqlite3_realloc().
** sqlite3_realloc() returns a pointer to a memory allocation
** of at least N bytes in size or NULL if sufficient memory is unavailable.
** If M is the size of the prior allocation, then min(N,M) bytes
** of the prior allocation are copied into the beginning of buffer returned
** by sqlite3_realloc() and the prior allocation is freed.
** If sqlite3_realloc() returns NULL, then the prior allocation
** is not freed.
**
** The memory returned by sqlite3_malloc() and sqlite3_realloc()
** is always aligned to at least an 8 byte boundary. {END}
**
** The default implementation of the memory allocation subsystem uses
** the malloc(), realloc() and free() provided by the standard C library.
** {H17382} However, if SQLite is compiled with the
** SQLITE_MEMORY_SIZE=<i>NNN</i> C preprocessor macro (where <i>NNN</i>
** is an integer), then SQLite create a static array of at least
** <i>NNN</i> bytes in size and uses that array for all of its dynamic
** memory allocation needs. {END}  Additional memory allocator options
** may be added in future releases.
**
** In SQLite version 3.5.0 and 3.5.1, it was possible to define
** the SQLITE_OMIT_MEMORY_ALLOCATION which would cause the built-in
** implementation of these routines to be omitted.  That capability
** is no longer provided.  Only built-in memory allocators can be used.
**
** The Windows OS interface layer calls
** the system malloc() and free() directly when converting
** filenames between the UTF-8 encoding used by SQLite
** and whatever filename encoding is used by the particular Windows
** installation.  Memory allocation errors are detected, but
** they are reported back as [SQLITE_CANTOPEN] or
** [SQLITE_IOERR] rather than [SQLITE_NOMEM].
**
** Requirements:
** [H17303] [H17304] [H17305] [H17306] [H17310] [H17312] [H17315] [H17318]
** [H17321] [H17322] [H17323]
**
** The pointer arguments to [sqlite3_free()] and [sqlite3_realloc()]
** must be either NULL or else pointers obtained from a prior
** invocation of [sqlite3_malloc()] or [sqlite3_realloc()] that have
** not yet been released.
**
** The application must not read or write any part of
** a block of memory after it has been released using
** [sqlite3_free()] or [sqlite3_realloc()].
*/
SQLITE_API void *sqlite3_malloc(int);
SQLITE_API void *sqlite3_realloc(void*, int);
SQLITE_API void sqlite3_free(void*);

/*
** CAPI3REF: Memory Allocator Statistics {H17370} <S30210>
**
** SQLite provides these two interfaces for reporting on the status
** of the [sqlite3_malloc()], [sqlite3_free()], and [sqlite3_realloc()]
** routines, which form the built-in memory allocation subsystem.
**
** Requirements:
** [H17371] [H17373] [H17374] [H17375]
*/
SQLITE_API sqlite3_int64 sqlite3_memory_used(void);
SQLITE_API sqlite3_int64 sqlite3_memory_highwater(int resetFlag);

/*
** CAPI3REF: Pseudo-Random Number Generator {H17390} <S20000>
**
** SQLite contains a high-quality pseudo-random number generator (PRNG) used to
** select random [ROWID | ROWIDs] when inserting new records into a table that
** already uses the largest possible [ROWID].  The PRNG is also used for
** the build-in random() and randomblob() SQL functions.  This interface allows
** applications to access the same PRNG for other purposes.
**
** A call to this routine stores N bytes of randomness into buffer P.
**
** The first time this routine is invoked (either internally or by
** the application) the PRNG is seeded using randomness obtained
** from the xRandomness method of the default [sqlite3_vfs] object.
** On all subsequent invocations, the pseudo-randomness is generated
** internally and without recourse to the [sqlite3_vfs] xRandomness
** method.
**
** Requirements:
** [H17392]
*/
SQLITE_API void sqlite3_randomness(int N, void *P);

/*
** CAPI3REF: Compile-Time Authorization Callbacks {H12500} <S70100>
**
** This routine registers a authorizer callback with a particular
** [database connection], supplied in the first argument.
** The authorizer callback is invoked as SQL statements are being compiled
** by [sqlite3_prepare()] or its variants [sqlite3_prepare_v2()],
** [sqlite3_prepare16()] and [sqlite3_prepare16_v2()].  At various
** points during the compilation process, as logic is being created
** to perform various actions, the authorizer callback is invoked to
** see if those actions are allowed.  The authorizer callback should
** return [SQLITE_OK] to allow the action, [SQLITE_IGNORE] to disallow the
** specific action but allow the SQL statement to continue to be
** compiled, or [SQLITE_DENY] to cause the entire SQL statement to be
** rejected with an error.  If the authorizer callback returns
** any value other than [SQLITE_IGNORE], [SQLITE_OK], or [SQLITE_DENY]
** then the [sqlite3_prepare_v2()] or equivalent call that triggered
** the authorizer will fail with an error message.
**
** When the callback returns [SQLITE_OK], that means the operation
** requested is ok.  When the callback returns [SQLITE_DENY], the
** [sqlite3_prepare_v2()] or equivalent call that triggered the
** authorizer will fail with an error message explaining that
** access is denied. 
**
** The first parameter to the authorizer callback is a copy of the third
** parameter to the sqlite3_set_authorizer() interface. The second parameter
** to the callback is an integer [SQLITE_COPY | action code] that specifies
** the particular action to be authorized. The third through sixth parameters
** to the callback are zero-terminated strings that contain additional
** details about the action to be authorized.
**
** If the action code is [SQLITE_READ]
** and the callback returns [SQLITE_IGNORE] then the
** [prepared statement] statement is constructed to substitute
** a NULL value in place of the table column that would have
** been read if [SQLITE_OK] had been returned.  The [SQLITE_IGNORE]
** return can be used to deny an untrusted user access to individual
** columns of a table.
** If the action code is [SQLITE_DELETE] and the callback returns
** [SQLITE_IGNORE] then the [DELETE] operation proceeds but the
** [truncate optimization] is disabled and all rows are deleted individually.
**
** An authorizer is used when [sqlite3_prepare | preparing]
** SQL statements from an untrusted source, to ensure that the SQL statements
** do not try to access data they are not allowed to see, or that they do not
** try to execute malicious statements that damage the database.  For
** example, an application may allow a user to enter arbitrary
** SQL queries for evaluation by a database.  But the application does
** not want the user to be able to make arbitrary changes to the
** database.  An authorizer could then be put in place while the
** user-entered SQL is being [sqlite3_prepare | prepared] that
** disallows everything except [SELECT] statements.
**
** Applications that need to process SQL from untrusted sources
** might also consider lowering resource limits using [sqlite3_limit()]
** and limiting database size using the [max_page_count] [PRAGMA]
** in addition to using an authorizer.
**
** Only a single authorizer can be in place on a database connection
** at a time.  Each call to sqlite3_set_authorizer overrides the
** previous call.  Disable the authorizer by installing a NULL callback.
** The authorizer is disabled by default.
**
** The authorizer callback must not do anything that will modify
** the database connection that invoked the authorizer callback.
** Note that [sqlite3_prepare_v2()] and [sqlite3_step()] both modify their
** database connections for the meaning of "modify" in this paragraph.
**
** When [sqlite3_prepare_v2()] is used to prepare a statement, the
** statement might be re-prepared during [sqlite3_step()] due to a 
** schema change.  Hence, the application should ensure that the
** correct authorizer callback remains in place during the [sqlite3_step()].
**
** Note that the authorizer callback is invoked only during
** [sqlite3_prepare()] or its variants.  Authorization is not
** performed during statement evaluation in [sqlite3_step()], unless
** as stated in the previous paragraph, sqlite3_step() invokes
** sqlite3_prepare_v2() to reprepare a statement after a schema change.
**
** Requirements:
** [H12501] [H12502] [H12503] [H12504] [H12505] [H12506] [H12507] [H12510]
** [H12511] [H12512] [H12520] [H12521] [H12522]
*/
SQLITE_API int sqlite3_set_authorizer(
  sqlite3*,
  int (*xAuth)(void*,int,const char*,const char*,const char*,const char*),
  void *pUserData
);

/*
** CAPI3REF: Authorizer Return Codes {H12590} <H12500>
**
** The [sqlite3_set_authorizer | authorizer callback function] must
** return either [SQLITE_OK] or one of these two constants in order
** to signal SQLite whether or not the action is permitted.  See the
** [sqlite3_set_authorizer | authorizer documentation] for additional
** information.
*/
#define SQLITE_DENY   1   /* Abort the SQL statement with an error */
#define SQLITE_IGNORE 2   /* Don't allow access, but don't generate an error */

/*
** CAPI3REF: Authorizer Action Codes {H12550} <H12500>
**
** The [sqlite3_set_authorizer()] interface registers a callback function
** that is invoked to authorize certain SQL statement actions.  The
** second parameter to the callback is an integer code that specifies
** what action is being authorized.  These are the integer action codes that
** the authorizer callback may be passed.
**
** These action code values signify what kind of operation is to be
** authorized.  The 3rd and 4th parameters to the authorization
** callback function will be parameters or NULL depending on which of these
** codes is used as the second parameter.  The 5th parameter to the
** authorizer callback is the name of the database ("main", "temp",
** etc.) if applicable.  The 6th parameter to the authorizer callback
** is the name of the inner-most trigger or view that is responsible for
** the access attempt or NULL if this access attempt is directly from
** top-level SQL code.
**
** Requirements:
** [H12551] [H12552] [H12553] [H12554]
*/
/******************************************* 3rd ************ 4th ***********/
#define SQLITE_CREATE_INDEX          1   /* Index Name      Table Name      */
#define SQLITE_CREATE_TABLE          2   /* Table Name      NULL            */
#define SQLITE_CREATE_TEMP_INDEX     3   /* Index Name      Table Name      */
#define SQLITE_CREATE_TEMP_TABLE     4   /* Table Name      NULL            */
#define SQLITE_CREATE_TEMP_TRIGGER   5   /* Trigger Name    Table Name      */
#define SQLITE_CREATE_TEMP_VIEW      6   /* View Name       NULL            */
#define SQLITE_CREATE_TRIGGER        7   /* Trigger Name    Table Name      */
#define SQLITE_CREATE_VIEW           8   /* View Name       NULL            */
#define SQLITE_DELETE                9   /* Table Name      NULL            */
#define SQLITE_DROP_INDEX           10   /* Index Name      Table Name      */
#define SQLITE_DROP_TABLE           11   /* Table Name      NULL            */
#define SQLITE_DROP_TEMP_INDEX      12   /* Index Name      Table Name      */
#define SQLITE_DROP_TEMP_TABLE      13   /* Table Name      NULL            */
#define SQLITE_DROP_TEMP_TRIGGER    14   /* Trigger Name    Table Name      */
#define SQLITE_DROP_TEMP_VIEW       15   /* View Name       NULL            */
#define SQLITE_DROP_TRIGGER         16   /* Trigger Name    Table Name      */
#define SQLITE_DROP_VIEW            17   /* View Name       NULL            */
#define SQLITE_INSERT               18   /* Table Name      NULL            */
#define SQLITE_PRAGMA               19   /* Pragma Name     1st arg or NULL */
#define SQLITE_READ                 20   /* Table Name      Column Name     */
#define SQLITE_SELECT               21   /* NULL            NULL            */
#define SQLITE_TRANSACTION          22   /* Operation       NULL            */
#define SQLITE_UPDATE               23   /* Table Name      Column Name     */
#define SQLITE_ATTACH               24   /* Filename        NULL            */
#define SQLITE_DETACH               25   /* Database Name   NULL            */
#define SQLITE_ALTER_TABLE          26   /* Database Name   Table Name      */
#define SQLITE_REINDEX              27   /* Index Name      NULL            */
#define SQLITE_ANALYZE              28   /* Table Name      NULL            */
#define SQLITE_CREATE_VTABLE        29   /* Table Name      Module Name     */
#define SQLITE_DROP_VTABLE          30   /* Table Name      Module Name     */
#define SQLITE_FUNCTION             31   /* NULL            Function Name   */
#define SQLITE_SAVEPOINT            32   /* Operation       Savepoint Name  */
#define SQLITE_COPY                  0   /* No longer used */

/*
** CAPI3REF: Tracing And Profiling Functions {H12280} <S60400>
** EXPERIMENTAL
**
** These routines register callback functions that can be used for
** tracing and profiling the execution of SQL statements.
**
** The callback function registered by sqlite3_trace() is invoked at
** various times when an SQL statement is being run by [sqlite3_step()].
** The callback returns a UTF-8 rendering of the SQL statement text
** as the statement first begins executing.  Additional callbacks occur
** as each triggered subprogram is entered.  The callbacks for triggers
** contain a UTF-8 SQL comment that identifies the trigger.
**
** The callback function registered by sqlite3_profile() is invoked
** as each SQL statement finishes.  The profile callback contains
** the original statement text and an estimate of wall-clock time
** of how long that statement took to run.
**
** Requirements:
** [H12281] [H12282] [H12283] [H12284] [H12285] [H12287] [H12288] [H12289]
** [H12290]
*/
SQLITE_API SQLITE_EXPERIMENTAL void *sqlite3_trace(sqlite3*, void(*xTrace)(void*,const char*), void*);
SQLITE_API SQLITE_EXPERIMENTAL void *sqlite3_profile(sqlite3*,
   void(*xProfile)(void*,const char*,sqlite3_uint64), void*);

/*
** CAPI3REF: Query Progress Callbacks {H12910} <S60400>
**
** This routine configures a callback function - the
** progress callback - that is invoked periodically during long
** running calls to [sqlite3_exec()], [sqlite3_step()] and
** [sqlite3_get_table()].  An example use for this
** interface is to keep a GUI updated during a large query.
**
** If the progress callback returns non-zero, the operation is
** interrupted.  This feature can be used to implement a
** "Cancel" button on a GUI progress dialog box.
**
** The progress handler must not do anything that will modify
** the database connection that invoked the progress handler.
** Note that [sqlite3_prepare_v2()] and [sqlite3_step()] both modify their
** database connections for the meaning of "modify" in this paragraph.
**
** Requirements:
** [H12911] [H12912] [H12913] [H12914] [H12915] [H12916] [H12917] [H12918]
**
*/
SQLITE_API void sqlite3_progress_handler(sqlite3*, int, int(*)(void*), void*);

/*
** CAPI3REF: Opening A New Database Connection {H12700} <S40200>
**
** These routines open an SQLite database file whose name is given by the
** filename argument. The filename argument is interpreted as UTF-8 for
** sqlite3_open() and sqlite3_open_v2() and as UTF-16 in the native byte
** order for sqlite3_open16(). A [database connection] handle is usually
** returned in *ppDb, even if an error occurs.  The only exception is that
** if SQLite is unable to allocate memory to hold the [sqlite3] object,
** a NULL will be written into *ppDb instead of a pointer to the [sqlite3]
** object. If the database is opened (and/or created) successfully, then
** [SQLITE_OK] is returned.  Otherwise an [error code] is returned.  The
** [sqlite3_errmsg()] or [sqlite3_errmsg16()] routines can be used to obtain
** an English language description of the error.
**
** The default encoding for the database will be UTF-8 if
** sqlite3_open() or sqlite3_open_v2() is called and
** UTF-16 in the native byte order if sqlite3_open16() is used.
**
** Whether or not an error occurs when it is opened, resources
** associated with the [database connection] handle should be released by
** passing it to [sqlite3_close()] when it is no longer required.
**
** The sqlite3_open_v2() interface works like sqlite3_open()
** except that it accepts two additional parameters for additional control
** over the new database connection.  The flags parameter can take one of
** the following three values, optionally combined with the 
** [SQLITE_OPEN_NOMUTEX], [SQLITE_OPEN_FULLMUTEX], [SQLITE_OPEN_SHAREDCACHE],
** and/or [SQLITE_OPEN_PRIVATECACHE] flags:
**
** <dl>
** <dt>[SQLITE_OPEN_READONLY]</dt>
** <dd>The database is opened in read-only mode.  If the database does not
** already exist, an error is returned.</dd>
**
** <dt>[SQLITE_OPEN_READWRITE]</dt>
** <dd>The database is opened for reading and writing if possible, or reading
** only if the file is write protected by the operating system.  In either
** case the database must already exist, otherwise an error is returned.</dd>
**
** <dt>[SQLITE_OPEN_READWRITE] | [SQLITE_OPEN_CREATE]</dt>
** <dd>The database is opened for reading and writing, and is creates it if
** it does not already exist. This is the behavior that is always used for
** sqlite3_open() and sqlite3_open16().</dd>
** </dl>
**
** If the 3rd parameter to sqlite3_open_v2() is not one of the
** combinations shown above or one of the combinations shown above combined
** with the [SQLITE_OPEN_NOMUTEX], [SQLITE_OPEN_FULLMUTEX],
** [SQLITE_OPEN_SHAREDCACHE] and/or [SQLITE_OPEN_SHAREDCACHE] flags,
** then the behavior is undefined.
**
** If the [SQLITE_OPEN_NOMUTEX] flag is set, then the database connection
** opens in the multi-thread [threading mode] as long as the single-thread
** mode has not been set at compile-time or start-time.  If the
** [SQLITE_OPEN_FULLMUTEX] flag is set then the database connection opens
** in the serialized [threading mode] unless single-thread was
** previously selected at compile-time or start-time.
** The [SQLITE_OPEN_SHAREDCACHE] flag causes the database connection to be
** eligible to use [shared cache mode], regardless of whether or not shared
** cache is enabled using [sqlite3_enable_shared_cache()].  The
** [SQLITE_OPEN_PRIVATECACHE] flag causes the database connection to not
** participate in [shared cache mode] even if it is enabled.
**
** If the filename is ":memory:", then a private, temporary in-memory database
** is created for the connection.  This in-memory database will vanish when
** the database connection is closed.  Future versions of SQLite might
** make use of additional special filenames that begin with the ":" character.
** It is recommended that when a database filename actually does begin with
** a ":" character you should prefix the filename with a pathname such as
** "./" to avoid ambiguity.
**
** If the filename is an empty string, then a private, temporary
** on-disk database will be created.  This private database will be
** automatically deleted as soon as the database connection is closed.
**
** The fourth parameter to sqlite3_open_v2() is the name of the
** [sqlite3_vfs] object that defines the operating system interface that
** the new database connection should use.  If the fourth parameter is
** a NULL pointer then the default [sqlite3_vfs] object is used.
**
** <b>Note to Windows users:</b>  The encoding used for the filename argument
** of sqlite3_open() and sqlite3_open_v2() must be UTF-8, not whatever
** codepage is currently defined.  Filenames containing international
** characters must be converted to UTF-8 prior to passing them into
** sqlite3_open() or sqlite3_open_v2().
**
** Requirements:
** [H12701] [H12702] [H12703] [H12704] [H12706] [H12707] [H12709] [H12711]
** [H12712] [H12713] [H12714] [H12717] [H12719] [H12721] [H12723]
*/
SQLITE_API int sqlite3_open(
  const char *filename,   /* Database filename (UTF-8) */
  sqlite3 **ppDb          /* OUT: SQLite db handle */
);
SQLITE_API int sqlite3_open16(
  const void *filename,   /* Database filename (UTF-16) */
  sqlite3 **ppDb          /* OUT: SQLite db handle */
);
SQLITE_API int sqlite3_open_v2(
  const char *filename,   /* Database filename (UTF-8) */
  sqlite3 **ppDb,         /* OUT: SQLite db handle */
  int flags,              /* Flags */
  const char *zVfs        /* Name of VFS module to use */
);

/*
** CAPI3REF: Error Codes And Messages {H12800} <S60200>
**
** The sqlite3_errcode() interface returns the numeric [result code] or
** [extended result code] for the most recent failed sqlite3_* API call
** associated with a [database connection]. If a prior API call failed
** but the most recent API call succeeded, the return value from
** sqlite3_errcode() is undefined.  The sqlite3_extended_errcode()
** interface is the same except that it always returns the 
** [extended result code] even when extended result codes are
** disabled.
**
** The sqlite3_errmsg() and sqlite3_errmsg16() return English-language
** text that describes the error, as either UTF-8 or UTF-16 respectively.
** Memory to hold the error message string is managed internally.
** The application does not need to worry about freeing the result.
** However, the error string might be overwritten or deallocated by
** subsequent calls to other SQLite interface functions.
**
** When the serialized [threading mode] is in use, it might be the
** case that a second error occurs on a separate thread in between
** the time of the first error and the call to these interfaces.
** When that happens, the second error will be reported since these
** interfaces always report the most recent result.  To avoid
** this, each thread can obtain exclusive use of the [database connection] D
** by invoking [sqlite3_mutex_enter]([sqlite3_db_mutex](D)) before beginning
** to use D and invoking [sqlite3_mutex_leave]([sqlite3_db_mutex](D)) after
** all calls to the interfaces listed here are completed.
**
** If an interface fails with SQLITE_MISUSE, that means the interface
** was invoked incorrectly by the application.  In that case, the
** error code and message may or may not be set.
**
** Requirements:
** [H12801] [H12802] [H12803] [H12807] [H12808] [H12809]
*/
SQLITE_API int sqlite3_errcode(sqlite3 *db);
SQLITE_API int sqlite3_extended_errcode(sqlite3 *db);
SQLITE_API const char *sqlite3_errmsg(sqlite3*);
SQLITE_API const void *sqlite3_errmsg16(sqlite3*);

/*
** CAPI3REF: SQL Statement Object {H13000} <H13010>
** KEYWORDS: {prepared statement} {prepared statements}
**
** An instance of this object represents a single SQL statement.
** This object is variously known as a "prepared statement" or a
** "compiled SQL statement" or simply as a "statement".
**
** The life of a statement object goes something like this:
**
** <ol>
** <li> Create the object using [sqlite3_prepare_v2()] or a related
**      function.
** <li> Bind values to [host parameters] using the sqlite3_bind_*()
**      interfaces.
** <li> Run the SQL by calling [sqlite3_step()] one or more times.
** <li> Reset the statement using [sqlite3_reset()] then go back
**      to step 2.  Do this zero or more times.
** <li> Destroy the object using [sqlite3_finalize()].
** </ol>
**
** Refer to documentation on individual methods above for additional
** information.
*/
typedef struct sqlite3_stmt sqlite3_stmt;

/*
** CAPI3REF: Run-time Limits {H12760} <S20600>
**
** This interface allows the size of various constructs to be limited
** on a connection by connection basis.  The first parameter is the
** [database connection] whose limit is to be set or queried.  The
** second parameter is one of the [limit categories] that define a
** class of constructs to be size limited.  The third parameter is the
** new limit for that construct.  The function returns the old limit.
**
** If the new limit is a negative number, the limit is unchanged.
** For the limit category of SQLITE_LIMIT_XYZ there is a 
** [limits | hard upper bound]
** set by a compile-time C preprocessor macro named 
** [limits | SQLITE_MAX_XYZ].
** (The "_LIMIT_" in the name is changed to "_MAX_".)
** Attempts to increase a limit above its hard upper bound are
** silently truncated to the hard upper limit.
**
** Run time limits are intended for use in applications that manage
** both their own internal database and also databases that are controlled
** by untrusted external sources.  An example application might be a
** web browser that has its own databases for storing history and
** separate databases controlled by JavaScript applications downloaded
** off the Internet.  The internal databases can be given the
** large, default limits.  Databases managed by external sources can
** be given much smaller limits designed to prevent a denial of service
** attack.  Developers might also want to use the [sqlite3_set_authorizer()]
** interface to further control untrusted SQL.  The size of the database
** created by an untrusted script can be contained using the
** [max_page_count] [PRAGMA].
**
** New run-time limit categories may be added in future releases.
**
** Requirements:
** [H12762] [H12766] [H12769]
*/
SQLITE_API int sqlite3_limit(sqlite3*, int id, int newVal);

/*
** CAPI3REF: Run-Time Limit Categories {H12790} <H12760>
** KEYWORDS: {limit category} {limit categories}
**
** These constants define various performance limits
** that can be lowered at run-time using [sqlite3_limit()].
** The synopsis of the meanings of the various limits is shown below.
** Additional information is available at [limits | Limits in SQLite].
**
** <dl>
** <dt>SQLITE_LIMIT_LENGTH</dt>
** <dd>The maximum size of any string or BLOB or table row.<dd>
**
** <dt>SQLITE_LIMIT_SQL_LENGTH</dt>
** <dd>The maximum length of an SQL statement.</dd>
**
** <dt>SQLITE_LIMIT_COLUMN</dt>
** <dd>The maximum number of columns in a table definition or in the
** result set of a [SELECT] or the maximum number of columns in an index
** or in an ORDER BY or GROUP BY clause.</dd>
**
** <dt>SQLITE_LIMIT_EXPR_DEPTH</dt>
** <dd>The maximum depth of the parse tree on any expression.</dd>
**
** <dt>SQLITE_LIMIT_COMPOUND_SELECT</dt>
** <dd>The maximum number of terms in a compound SELECT statement.</dd>
**
** <dt>SQLITE_LIMIT_VDBE_OP</dt>
** <dd>The maximum number of instructions in a virtual machine program
** used to implement an SQL statement.</dd>
**
** <dt>SQLITE_LIMIT_FUNCTION_ARG</dt>
** <dd>The maximum number of arguments on a function.</dd>
**
** <dt>SQLITE_LIMIT_ATTACHED</dt>
** <dd>The maximum number of [ATTACH | attached databases].</dd>
**
** <dt>SQLITE_LIMIT_LIKE_PATTERN_LENGTH</dt>
** <dd>The maximum length of the pattern argument to the [LIKE] or
** [GLOB] operators.</dd>
**
** <dt>SQLITE_LIMIT_VARIABLE_NUMBER</dt>
** <dd>The maximum number of variables in an SQL statement that can
** be bound.</dd>
**
** <dt>SQLITE_LIMIT_TRIGGER_DEPTH</dt>
** <dd>The maximum depth of recursion for triggers.</dd>
** </dl>
*/
#define SQLITE_LIMIT_LENGTH                    0
#define SQLITE_LIMIT_SQL_LENGTH                1
#define SQLITE_LIMIT_COLUMN                    2
#define SQLITE_LIMIT_EXPR_DEPTH                3
#define SQLITE_LIMIT_COMPOUND_SELECT           4
#define SQLITE_LIMIT_VDBE_OP                   5
#define SQLITE_LIMIT_FUNCTION_ARG              6
#define SQLITE_LIMIT_ATTACHED                  7
#define SQLITE_LIMIT_LIKE_PATTERN_LENGTH       8
#define SQLITE_LIMIT_VARIABLE_NUMBER           9
#define SQLITE_LIMIT_TRIGGER_DEPTH            10

/*
** CAPI3REF: Compiling An SQL Statement {H13010} <S10000>
** KEYWORDS: {SQL statement compiler}
**
** To execute an SQL query, it must first be compiled into a byte-code
** program using one of these routines.
**
** The first argument, "db", is a [database connection] obtained from a
** prior successful call to [sqlite3_open()], [sqlite3_open_v2()] or
** [sqlite3_open16()].  The database connection must not have been closed.
**
** The second argument, "zSql", is the statement to be compiled, encoded
** as either UTF-8 or UTF-16.  The sqlite3_prepare() and sqlite3_prepare_v2()
** interfaces use UTF-8, and sqlite3_prepare16() and sqlite3_prepare16_v2()
** use UTF-16.
**
** If the nByte argument is less than zero, then zSql is read up to the
** first zero terminator. If nByte is non-negative, then it is the maximum
** number of  bytes read from zSql.  When nByte is non-negative, the
** zSql string ends at either the first '\000' or '\u0000' character or
** the nByte-th byte, whichever comes first. If the caller knows
** that the supplied string is nul-terminated, then there is a small
** performance advantage to be gained by passing an nByte parameter that
** is equal to the number of bytes in the input string <i>including</i>
** the nul-terminator bytes.
**
** If pzTail is not NULL then *pzTail is made to point to the first byte
** past the end of the first SQL statement in zSql.  These routines only
** compile the first statement in zSql, so *pzTail is left pointing to
** what remains uncompiled.
**
** *ppStmt is left pointing to a compiled [prepared statement] that can be
** executed using [sqlite3_step()].  If there is an error, *ppStmt is set
** to NULL.  If the input text contains no SQL (if the input is an empty
** string or a comment) then *ppStmt is set to NULL.
** The calling procedure is responsible for deleting the compiled
** SQL statement using [sqlite3_finalize()] after it has finished with it.
** ppStmt may not be NULL.
**
** On success, [SQLITE_OK] is returned, otherwise an [error code] is returned.
**
** The sqlite3_prepare_v2() and sqlite3_prepare16_v2() interfaces are
** recommended for all new programs. The two older interfaces are retained
** for backwards compatibility, but their use is discouraged.
** In the "v2" interfaces, the prepared statement
** that is returned (the [sqlite3_stmt] object) contains a copy of the
** original SQL text. This causes the [sqlite3_step()] interface to
** behave a differently in two ways:
**
** <ol>
** <li>
** If the database schema changes, instead of returning [SQLITE_SCHEMA] as it
** always used to do, [sqlite3_step()] will automatically recompile the SQL
** statement and try to run it again.  If the schema has changed in
** a way that makes the statement no longer valid, [sqlite3_step()] will still
** return [SQLITE_SCHEMA].  But unlike the legacy behavior, [SQLITE_SCHEMA] is
** now a fatal error.  Calling [sqlite3_prepare_v2()] again will not make the
** error go away.  Note: use [sqlite3_errmsg()] to find the text
** of the parsing error that results in an [SQLITE_SCHEMA] return.
** </li>
**
** <li>
** When an error occurs, [sqlite3_step()] will return one of the detailed
** [error codes] or [extended error codes].  The legacy behavior was that
** [sqlite3_step()] would only return a generic [SQLITE_ERROR] result code
** and you would have to make a second call to [sqlite3_reset()] in order
** to find the underlying cause of the problem. With the "v2" prepare
** interfaces, the underlying reason for the error is returned immediately.
** </li>
** </ol>
**
** Requirements:
** [H13011] [H13012] [H13013] [H13014] [H13015] [H13016] [H13019] [H13021]
**
*/
SQLITE_API int sqlite3_prepare(
  sqlite3 *db,            /* Database handle */
  const char *zSql,       /* SQL statement, UTF-8 encoded */
  int nByte,              /* Maximum length of zSql in bytes. */
  sqlite3_stmt **ppStmt,  /* OUT: Statement handle */
  const char **pzTail     /* OUT: Pointer to unused portion of zSql */
);
SQLITE_API int sqlite3_prepare_v2(
  sqlite3 *db,            /* Database handle */
  const char *zSql,       /* SQL statement, UTF-8 encoded */
  int nByte,              /* Maximum length of zSql in bytes. */
  sqlite3_stmt **ppStmt,  /* OUT: Statement handle */
  const char **pzTail     /* OUT: Pointer to unused portion of zSql */
);
SQLITE_API int sqlite3_prepare16(
  sqlite3 *db,            /* Database handle */
  const void *zSql,       /* SQL statement, UTF-16 encoded */
  int nByte,              /* Maximum length of zSql in bytes. */
  sqlite3_stmt **ppStmt,  /* OUT: Statement handle */
  const void **pzTail     /* OUT: Pointer to unused portion of zSql */
);
SQLITE_API int sqlite3_prepare16_v2(
  sqlite3 *db,            /* Database handle */
  const void *zSql,       /* SQL statement, UTF-16 encoded */
  int nByte,              /* Maximum length of zSql in bytes. */
  sqlite3_stmt **ppStmt,  /* OUT: Statement handle */
  const void **pzTail     /* OUT: Pointer to unused portion of zSql */
);

/*
** CAPI3REF: Retrieving Statement SQL {H13100} <H13000>
**
** This interface can be used to retrieve a saved copy of the original
** SQL text used to create a [prepared statement] if that statement was
** compiled using either [sqlite3_prepare_v2()] or [sqlite3_prepare16_v2()].
**
** Requirements:
** [H13101] [H13102] [H13103]
*/
SQLITE_API const char *sqlite3_sql(sqlite3_stmt *pStmt);

/*
** CAPI3REF: Dynamically Typed Value Object {H15000} <S20200>
** KEYWORDS: {protected sqlite3_value} {unprotected sqlite3_value}
**
** SQLite uses the sqlite3_value object to represent all values
** that can be stored in a database table. SQLite uses dynamic typing
** for the values it stores. Values stored in sqlite3_value objects
** can be integers, floating point values, strings, BLOBs, or NULL.
**
** An sqlite3_value object may be either "protected" or "unprotected".
** Some interfaces require a protected sqlite3_value.  Other interfaces
** will accept either a protected or an unprotected sqlite3_value.
** Every interface that accepts sqlite3_value arguments specifies
** whether or not it requires a protected sqlite3_value.
**
** The terms "protected" and "unprotected" refer to whether or not
** a mutex is held.  A internal mutex is held for a protected
** sqlite3_value object but no mutex is held for an unprotected
** sqlite3_value object.  If SQLite is compiled to be single-threaded
** (with [SQLITE_THREADSAFE=0] and with [sqlite3_threadsafe()] returning 0)
** or if SQLite is run in one of reduced mutex modes 
** [SQLITE_CONFIG_SINGLETHREAD] or [SQLITE_CONFIG_MULTITHREAD]
** then there is no distinction between protected and unprotected
** sqlite3_value objects and they can be used interchangeably.  However,
** for maximum code portability it is recommended that applications
** still make the distinction between between protected and unprotected
** sqlite3_value objects even when not strictly required.
**
** The sqlite3_value objects that are passed as parameters into the
** implementation of [application-defined SQL functions] are protected.
** The sqlite3_value object returned by
** [sqlite3_column_value()] is unprotected.
** Unprotected sqlite3_value objects may only be used with
** [sqlite3_result_value()] and [sqlite3_bind_value()].
** The [sqlite3_value_blob | sqlite3_value_type()] family of
** interfaces require protected sqlite3_value objects.
*/
typedef struct Mem sqlite3_value;

/*
** CAPI3REF: SQL Function Context Object {H16001} <S20200>
**
** The context in which an SQL function executes is stored in an
** sqlite3_context object.  A pointer to an sqlite3_context object
** is always first parameter to [application-defined SQL functions].
** The application-defined SQL function implementation will pass this
** pointer through into calls to [sqlite3_result_int | sqlite3_result()],
** [sqlite3_aggregate_context()], [sqlite3_user_data()],
** [sqlite3_context_db_handle()], [sqlite3_get_auxdata()],
** and/or [sqlite3_set_auxdata()].
*/
typedef struct sqlite3_context sqlite3_context;

/*
** CAPI3REF: Binding Values To Prepared Statements {H13500} <S70300>
** KEYWORDS: {host parameter} {host parameters} {host parameter name}
** KEYWORDS: {SQL parameter} {SQL parameters} {parameter binding}
**
** In the SQL strings input to [sqlite3_prepare_v2()] and its variants,
** literals may be replaced by a [parameter] that matches one of following
** templates:
**
** <ul>
** <li>  ?
** <li>  ?NNN
** <li>  :VVV
** <li>  @VVV
** <li>  $VVV
** </ul>
**
** In the templates above, NNN represents an integer literal,
** and VVV represents an alphanumeric identifer.  The values of these
** parameters (also called "host parameter names" or "SQL parameters")
** can be set using the sqlite3_bind_*() routines defined here.
**
** The first argument to the sqlite3_bind_*() routines is always
** a pointer to the [sqlite3_stmt] object returned from
** [sqlite3_prepare_v2()] or its variants.
**
** The second argument is the index of the SQL parameter to be set.
** The leftmost SQL parameter has an index of 1.  When the same named
** SQL parameter is used more than once, second and subsequent
** occurrences have the same index as the first occurrence.
** The index for named parameters can be looked up using the
** [sqlite3_bind_parameter_index()] API if desired.  The index
** for "?NNN" parameters is the value of NNN.
** The NNN value must be between 1 and the [sqlite3_limit()]
** parameter [SQLITE_LIMIT_VARIABLE_NUMBER] (default value: 999).
**
** The third argument is the value to bind to the parameter.
**
** In those routines that have a fourth argument, its value is the
** number of bytes in the parameter.  To be clear: the value is the
** number of <u>bytes</u> in the value, not the number of characters.
** If the fourth parameter is negative, the length of the string is
** the number of bytes up to the first zero terminator.
**
** The fifth argument to sqlite3_bind_blob(), sqlite3_bind_text(), and
** sqlite3_bind_text16() is a destructor used to dispose of the BLOB or
** string after SQLite has finished with it. If the fifth argument is
** the special value [SQLITE_STATIC], then SQLite assumes that the
** information is in static, unmanaged space and does not need to be freed.
** If the fifth argument has the value [SQLITE_TRANSIENT], then
** SQLite makes its own private copy of the data immediately, before
** the sqlite3_bind_*() routine returns.
**
** The sqlite3_bind_zeroblob() routine binds a BLOB of length N that
** is filled with zeroes.  A zeroblob uses a fixed amount of memory
** (just an integer to hold its size) while it is being processed.
** Zeroblobs are intended to serve as placeholders for BLOBs whose
** content is later written using
** [sqlite3_blob_open | incremental BLOB I/O] routines.
** A negative value for the zeroblob results in a zero-length BLOB.
**
** The sqlite3_bind_*() routines must be called after
** [sqlite3_prepare_v2()] (and its variants) or [sqlite3_reset()] and
** before [sqlite3_step()].
** Bindings are not cleared by the [sqlite3_reset()] routine.
** Unbound parameters are interpreted as NULL.
**
** These routines return [SQLITE_OK] on success or an error code if
** anything goes wrong.  [SQLITE_RANGE] is returned if the parameter
** index is out of range.  [SQLITE_NOMEM] is returned if malloc() fails.
** [SQLITE_MISUSE] might be returned if these routines are called on a
** virtual machine that is the wrong state or which has already been finalized.
** Detection of misuse is unreliable.  Applications should not depend
** on SQLITE_MISUSE returns.  SQLITE_MISUSE is intended to indicate a
** a logic error in the application.  Future versions of SQLite might
** panic rather than return SQLITE_MISUSE.
**
** See also: [sqlite3_bind_parameter_count()],
** [sqlite3_bind_parameter_name()], and [sqlite3_bind_parameter_index()].
**
** Requirements:
** [H13506] [H13509] [H13512] [H13515] [H13518] [H13521] [H13524] [H13527]
** [H13530] [H13533] [H13536] [H13539] [H13542] [H13545] [H13548] [H13551]
**
*/
SQLITE_API int sqlite3_bind_blob(sqlite3_stmt*, int, const void*, int n, void(*)(void*));
SQLITE_API int sqlite3_bind_double(sqlite3_stmt*, int, double);
SQLITE_API int sqlite3_bind_int(sqlite3_stmt*, int, int);
SQLITE_API int sqlite3_bind_int64(sqlite3_stmt*, int, sqlite3_int64);
SQLITE_API int sqlite3_bind_null(sqlite3_stmt*, int);
SQLITE_API int sqlite3_bind_text(sqlite3_stmt*, int, const char*, int n, void(*)(void*));
SQLITE_API int sqlite3_bind_text16(sqlite3_stmt*, int, const void*, int, void(*)(void*));
SQLITE_API int sqlite3_bind_value(sqlite3_stmt*, int, const sqlite3_value*);
SQLITE_API int sqlite3_bind_zeroblob(sqlite3_stmt*, int, int n);

/*
** CAPI3REF: Number Of SQL Parameters {H13600} <S70300>
**
** This routine can be used to find the number of [SQL parameters]
** in a [prepared statement].  SQL parameters are tokens of the
** form "?", "?NNN", ":AAA", "$AAA", or "@AAA" that serve as
** placeholders for values that are [sqlite3_bind_blob | bound]
** to the parameters at a later time.
**
** This routine actually returns the index of the largest (rightmost)
** parameter. For all forms except ?NNN, this will correspond to the
** number of unique parameters.  If parameters of the ?NNN are used,
** there may be gaps in the list.
**
** See also: [sqlite3_bind_blob|sqlite3_bind()],
** [sqlite3_bind_parameter_name()], and
** [sqlite3_bind_parameter_index()].
**
** Requirements:
** [H13601]
*/
SQLITE_API int sqlite3_bind_parameter_count(sqlite3_stmt*);

/*
** CAPI3REF: Name Of A Host Parameter {H13620} <S70300>
**
** This routine returns a pointer to the name of the n-th
** [SQL parameter] in a [prepared statement].
** SQL parameters of the form "?NNN" or ":AAA" or "@AAA" or "$AAA"
** have a name which is the string "?NNN" or ":AAA" or "@AAA" or "$AAA"
** respectively.
** In other words, the initial ":" or "$" or "@" or "?"
** is included as part of the name.
** Parameters of the form "?" without a following integer have no name
** and are also referred to as "anonymous parameters".
**
** The first host parameter has an index of 1, not 0.
**
** If the value n is out of range or if the n-th parameter is
** nameless, then NULL is returned.  The returned string is
** always in UTF-8 encoding even if the named parameter was
** originally specified as UTF-16 in [sqlite3_prepare16()] or
** [sqlite3_prepare16_v2()].
**
** See also: [sqlite3_bind_blob|sqlite3_bind()],
** [sqlite3_bind_parameter_count()], and
** [sqlite3_bind_parameter_index()].
**
** Requirements:
** [H13621]
*/
SQLITE_API const char *sqlite3_bind_parameter_name(sqlite3_stmt*, int);

/*
** CAPI3REF: Index Of A Parameter With A Given Name {H13640} <S70300>
**
** Return the index of an SQL parameter given its name.  The
** index value returned is suitable for use as the second
** parameter to [sqlite3_bind_blob|sqlite3_bind()].  A zero
** is returned if no matching parameter is found.  The parameter
** name must be given in UTF-8 even if the original statement
** was prepared from UTF-16 text using [sqlite3_prepare16_v2()].
**
** See also: [sqlite3_bind_blob|sqlite3_bind()],
** [sqlite3_bind_parameter_count()], and
** [sqlite3_bind_parameter_index()].
**
** Requirements:
** [H13641]
*/
SQLITE_API int sqlite3_bind_parameter_index(sqlite3_stmt*, const char *zName);

/*
** CAPI3REF: Reset All Bindings On A Prepared Statement {H13660} <S70300>
**
** Contrary to the intuition of many, [sqlite3_reset()] does not reset
** the [sqlite3_bind_blob | bindings] on a [prepared statement].
** Use this routine to reset all host parameters to NULL.
**
** Requirements:
** [H13661]
*/
SQLITE_API int sqlite3_clear_bindings(sqlite3_stmt*);

/*
** CAPI3REF: Number Of Columns In A Result Set {H13710} <S10700>
**
** Return the number of columns in the result set returned by the
** [prepared statement]. This routine returns 0 if pStmt is an SQL
** statement that does not return data (for example an [UPDATE]).
**
** Requirements:
** [H13711]
*/
SQLITE_API int sqlite3_column_count(sqlite3_stmt *pStmt);

/*
** CAPI3REF: Column Names In A Result Set {H13720} <S10700>
**
** These routines return the name assigned to a particular column
** in the result set of a [SELECT] statement.  The sqlite3_column_name()
** interface returns a pointer to a zero-terminated UTF-8 string
** and sqlite3_column_name16() returns a pointer to a zero-terminated
** UTF-16 string.  The first parameter is the [prepared statement]
** that implements the [SELECT] statement. The second parameter is the
** column number.  The leftmost column is number 0.
**
** The returned string pointer is valid until either the [prepared statement]
** is destroyed by [sqlite3_finalize()] or until the next call to
** sqlite3_column_name() or sqlite3_column_name16() on the same column.
**
** If sqlite3_malloc() fails during the processing of either routine
** (for example during a conversion from UTF-8 to UTF-16) then a
** NULL pointer is returned.
**
** The name of a result column is the value of the "AS" clause for
** that column, if there is an AS clause.  If there is no AS clause
** then the name of the column is unspecified and may change from
** one release of SQLite to the next.
**
** Requirements:
** [H13721] [H13723] [H13724] [H13725] [H13726] [H13727]
*/
SQLITE_API const char *sqlite3_column_name(sqlite3_stmt*, int N);
SQLITE_API const void *sqlite3_column_name16(sqlite3_stmt*, int N);

/*
** CAPI3REF: Source Of Data In A Query Result {H13740} <S10700>
**
** These routines provide a means to determine what column of what
** table in which database a result of a [SELECT] statement comes from.
** The name of the database or table or column can be returned as
** either a UTF-8 or UTF-16 string.  The _database_ routines return
** the database name, the _table_ routines return the table name, and
** the origin_ routines return the column name.
** The returned string is valid until the [prepared statement] is destroyed
** using [sqlite3_finalize()] or until the same information is requested
** again in a different encoding.
**
** The names returned are the original un-aliased names of the
** database, table, and column.
**
** The first argument to the following calls is a [prepared statement].
** These functions return information about the Nth column returned by
** the statement, where N is the second function argument.
**
** If the Nth column returned by the statement is an expression or
** subquery and is not a column value, then all of these functions return
** NULL.  These routine might also return NULL if a memory allocation error
** occurs.  Otherwise, they return the name of the attached database, table
** and column that query result column was extracted from.
**
** As with all other SQLite APIs, those postfixed with "16" return
** UTF-16 encoded strings, the other functions return UTF-8. {END}
**
** These APIs are only available if the library was compiled with the
** [SQLITE_ENABLE_COLUMN_METADATA] C-preprocessor symbol defined.
**
** {A13751}
** If two or more threads call one or more of these routines against the same
** prepared statement and column at the same time then the results are
** undefined.
**
** Requirements:
** [H13741] [H13742] [H13743] [H13744] [H13745] [H13746] [H13748]
**
** If two or more threads call one or more
** [sqlite3_column_database_name | column metadata interfaces]
** for the same [prepared statement] and result column
** at the same time then the results are undefined.
*/
SQLITE_API const char *sqlite3_column_database_name(sqlite3_stmt*,int);
SQLITE_API const void *sqlite3_column_database_name16(sqlite3_stmt*,int);
SQLITE_API const char *sqlite3_column_table_name(sqlite3_stmt*,int);
SQLITE_API const void *sqlite3_column_table_name16(sqlite3_stmt*,int);
SQLITE_API const char *sqlite3_column_origin_name(sqlite3_stmt*,int);
SQLITE_API const void *sqlite3_column_origin_name16(sqlite3_stmt*,int);

/*
** CAPI3REF: Declared Datatype Of A Query Result {H13760} <S10700>
**
** The first parameter is a [prepared statement].
** If this statement is a [SELECT] statement and the Nth column of the
** returned result set of that [SELECT] is a table column (not an
** expression or subquery) then the declared type of the table
** column is returned.  If the Nth column of the result set is an
** expression or subquery, then a NULL pointer is returned.
** The returned string is always UTF-8 encoded. {END}
**
** For example, given the database schema:
**
** CREATE TABLE t1(c1 VARIANT);
**
** and the following statement to be compiled:
**
** SELECT c1 + 1, c1 FROM t1;
**
** this routine would return the string "VARIANT" for the second result
** column (i==1), and a NULL pointer for the first result column (i==0).
**
** SQLite uses dynamic run-time typing.  So just because a column
** is declared to contain a particular type does not mean that the
** data stored in that column is of the declared type.  SQLite is
** strongly typed, but the typing is dynamic not static.  Type
** is associated with individual values, not with the containers
** used to hold those values.
**
** Requirements:
** [H13761] [H13762] [H13763]
*/
SQLITE_API const char *sqlite3_column_decltype(sqlite3_stmt*,int);
SQLITE_API const void *sqlite3_column_decltype16(sqlite3_stmt*,int);

/*
** CAPI3REF: Evaluate An SQL Statement {H13200} <S10000>
**
** After a [prepared statement] has been prepared using either
** [sqlite3_prepare_v2()] or [sqlite3_prepare16_v2()] or one of the legacy
** interfaces [sqlite3_prepare()] or [sqlite3_prepare16()], this function
** must be called one or more times to evaluate the statement.
**
** The details of the behavior of the sqlite3_step() interface depend
** on whether the statement was prepared using the newer "v2" interface
** [sqlite3_prepare_v2()] and [sqlite3_prepare16_v2()] or the older legacy
** interface [sqlite3_prepare()] and [sqlite3_prepare16()].  The use of the
** new "v2" interface is recommended for new applications but the legacy
** interface will continue to be supported.
**
** In the legacy interface, the return value will be either [SQLITE_BUSY],
** [SQLITE_DONE], [SQLITE_ROW], [SQLITE_ERROR], or [SQLITE_MISUSE].
** With the "v2" interface, any of the other [result codes] or
** [extended result codes] might be returned as well.
**
** [SQLITE_BUSY] means that the database engine was unable to acquire the
** database locks it needs to do its job.  If the statement is a [COMMIT]
** or occurs outside of an explicit transaction, then you can retry the
** statement.  If the statement is not a [COMMIT] and occurs within a
** explicit transaction then you should rollback the transaction before
** continuing.
**
** [SQLITE_DONE] means that the statement has finished executing
** successfully.  sqlite3_step() should not be called again on this virtual
** machine without first calling [sqlite3_reset()] to reset the virtual
** machine back to its initial state.
**
** If the SQL statement being executed returns any data, then [SQLITE_ROW]
** is returned each time a new row of data is ready for processing by the
** caller. The values may be accessed using the [column access functions].
** sqlite3_step() is called again to retrieve the next row of data.
**
** [SQLITE_ERROR] means that a run-time error (such as a constraint
** violation) has occurred.  sqlite3_step() should not be called again on
** the VM. More information may be found by calling [sqlite3_errmsg()].
** With the legacy interface, a more specific error code (for example,
** [SQLITE_INTERRUPT], [SQLITE_SCHEMA], [SQLITE_CORRUPT], and so forth)
** can be obtained by calling [sqlite3_reset()] on the
** [prepared statement].  In the "v2" interface,
** the more specific error code is returned directly by sqlite3_step().
**
** [SQLITE_MISUSE] means that the this routine was called inappropriately.
** Perhaps it was called on a [prepared statement] that has
** already been [sqlite3_finalize | finalized] or on one that had
** previously returned [SQLITE_ERROR] or [SQLITE_DONE].  Or it could
** be the case that the same database connection is being used by two or
** more threads at the same moment in time.
**
** <b>Goofy Interface Alert:</b> In the legacy interface, the sqlite3_step()
** API always returns a generic error code, [SQLITE_ERROR], following any
** error other than [SQLITE_BUSY] and [SQLITE_MISUSE].  You must call
** [sqlite3_reset()] or [sqlite3_finalize()] in order to find one of the
** specific [error codes] that better describes the error.
** We admit that this is a goofy design.  The problem has been fixed
** with the "v2" interface.  If you prepare all of your SQL statements
** using either [sqlite3_prepare_v2()] or [sqlite3_prepare16_v2()] instead
** of the legacy [sqlite3_prepare()] and [sqlite3_prepare16()] interfaces,
** then the more specific [error codes] are returned directly
** by sqlite3_step().  The use of the "v2" interface is recommended.
**
** Requirements:
** [H13202] [H15304] [H15306] [H15308] [H15310]
*/
SQLITE_API int sqlite3_step(sqlite3_stmt*);

/*
** CAPI3REF: Number of columns in a result set {H13770} <S10700>
**
** Returns the number of values in the current row of the result set.
**
** Requirements:
** [H13771] [H13772]
*/
SQLITE_API int sqlite3_data_count(sqlite3_stmt *pStmt);

/*
** CAPI3REF: Fundamental Datatypes {H10265} <S10110><S10120>
** KEYWORDS: SQLITE_TEXT
**
** {H10266} Every value in SQLite has one of five fundamental datatypes:
**
** <ul>
** <li> 64-bit signed integer
** <li> 64-bit IEEE floating point number
** <li> string
** <li> BLOB
** <li> NULL
** </ul> {END}
**
** These constants are codes for each of those types.
**
** Note that the SQLITE_TEXT constant was also used in SQLite version 2
** for a completely different meaning.  Software that links against both
** SQLite version 2 and SQLite version 3 should use SQLITE3_TEXT, not
** SQLITE_TEXT.
*/
#define SQLITE_INTEGER  1
#define SQLITE_FLOAT    2
#define SQLITE_BLOB     4
#define SQLITE_NULL     5
#ifdef SQLITE_TEXT
# undef SQLITE_TEXT
#else
# define SQLITE_TEXT     3
#endif
#define SQLITE3_TEXT     3

/*
** CAPI3REF: Result Values From A Query {H13800} <S10700>
** KEYWORDS: {column access functions}
**
** These routines form the "result set query" interface.
**
** These routines return information about a single column of the current
** result row of a query.  In every case the first argument is a pointer
** to the [prepared statement] that is being evaluated (the [sqlite3_stmt*]
** that was returned from [sqlite3_prepare_v2()] or one of its variants)
** and the second argument is the index of the column for which information
** should be returned.  The leftmost column of the result set has the index 0.
**
** If the SQL statement does not currently point to a valid row, or if the
** column index is out of range, the result is undefined.
** These routines may only be called when the most recent call to
** [sqlite3_step()] has returned [SQLITE_ROW] and neither
** [sqlite3_reset()] nor [sqlite3_finalize()] have been called subsequently.
** If any of these routines are called after [sqlite3_reset()] or
** [sqlite3_finalize()] or after [sqlite3_step()] has returned
** something other than [SQLITE_ROW], the results are undefined.
** If [sqlite3_step()] or [sqlite3_reset()] or [sqlite3_finalize()]
** are called from a different thread while any of these routines
** are pending, then the results are undefined.
**
** The sqlite3_column_type() routine returns the
** [SQLITE_INTEGER | datatype code] for the initial data type
** of the result column.  The returned value is one of [SQLITE_INTEGER],
** [SQLITE_FLOAT], [SQLITE_TEXT], [SQLITE_BLOB], or [SQLITE_NULL].  The value
** returned by sqlite3_column_type() is only meaningful if no type
** conversions have occurred as described below.  After a type conversion,
** the value returned by sqlite3_column_type() is undefined.  Future
** versions of SQLite may change the behavior of sqlite3_column_type()
** following a type conversion.
**
** If the result is a BLOB or UTF-8 string then the sqlite3_column_bytes()
** routine returns the number of bytes in that BLOB or string.
** If the result is a UTF-16 string, then sqlite3_column_bytes() converts
** the string to UTF-8 and then returns the number of bytes.
** If the result is a numeric value then sqlite3_column_bytes() uses
** [sqlite3_snprintf()] to convert that value to a UTF-8 string and returns
** the number of bytes in that string.
** The value returned does not include the zero terminator at the end
** of the string.  For clarity: the value returned is the number of
** bytes in the string, not the number of characters.
**
** Strings returned by sqlite3_column_text() and sqlite3_column_text16(),
** even empty strings, are always zero terminated.  The return
** value from sqlite3_column_blob() for a zero-length BLOB is an arbitrary
** pointer, possibly even a NULL pointer.
**
** The sqlite3_column_bytes16() routine is similar to sqlite3_column_bytes()
** but leaves the result in UTF-16 in native byte order instead of UTF-8.
** The zero terminator is not included in this count.
**
** The object returned by [sqlite3_column_value()] is an
** [unprotected sqlite3_value] object.  An unprotected sqlite3_value object
** may only be used with [sqlite3_bind_value()] and [sqlite3_result_value()].
** If the [unprotected sqlite3_value] object returned by
** [sqlite3_column_value()] is used in any other way, including calls
** to routines like [sqlite3_value_int()], [sqlite3_value_text()],
** or [sqlite3_value_bytes()], then the behavior is undefined.
**
** These routines attempt to convert the value where appropriate.  For
** example, if the internal representation is FLOAT and a text result
** is requested, [sqlite3_snprintf()] is used internally to perform the
** conversion automatically.  The following table details the conversions
** that are applied:
**
** <blockquote>
** <table border="1">
** <tr><th> Internal<br>Type <th> Requested<br>Type <th>  Conversion
**
** <tr><td>  NULL    <td> INTEGER   <td> Result is 0
** <tr><td>  NULL    <td>  FLOAT    <td> Result is 0.0
** <tr><td>  NULL    <td>   TEXT    <td> Result is NULL pointer
** <tr><td>  NULL    <td>   BLOB    <td> Result is NULL pointer
** <tr><td> INTEGER  <td>  FLOAT    <td> Convert from integer to float
** <tr><td> INTEGER  <td>   TEXT    <td> ASCII rendering of the integer
** <tr><td> INTEGER  <td>   BLOB    <td> Same as INTEGER->TEXT
** <tr><td>  FLOAT   <td> INTEGER   <td> Convert from float to integer
** <tr><td>  FLOAT   <td>   TEXT    <td> ASCII rendering of the float
** <tr><td>  FLOAT   <td>   BLOB    <td> Same as FLOAT->TEXT
** <tr><td>  TEXT    <td> INTEGER   <td> Use atoi()
** <tr><td>  TEXT    <td>  FLOAT    <td> Use atof()
** <tr><td>  TEXT    <td>   BLOB    <td> No change
** <tr><td>  BLOB    <td> INTEGER   <td> Convert to TEXT then use atoi()
** <tr><td>  BLOB    <td>  FLOAT    <td> Convert to TEXT then use atof()
** <tr><td>  BLOB    <td>   TEXT    <td> Add a zero terminator if needed
** </table>
** </blockquote>
**
** The table above makes reference to standard C library functions atoi()
** and atof().  SQLite does not really use these functions.  It has its
** own equivalent internal routines.  The atoi() and atof() names are
** used in the table for brevity and because they are familiar to most
** C programmers.
**
** Note that when type conversions occur, pointers returned by prior
** calls to sqlite3_column_blob(), sqlite3_column_text(), and/or
** sqlite3_column_text16() may be invalidated.
** Type conversions and pointer invalidations might occur
** in the following cases:
**
** <ul>
** <li> The initial content is a BLOB and sqlite3_column_text() or
**      sqlite3_column_text16() is called.  A zero-terminator might
**      need to be added to the string.</li>
** <li> The initial content is UTF-8 text and sqlite3_column_bytes16() or
**      sqlite3_column_text16() is called.  The content must be converted
**      to UTF-16.</li>
** <li> The initial content is UTF-16 text and sqlite3_column_bytes() or
**      sqlite3_column_text() is called.  The content must be converted
**      to UTF-8.</li>
** </ul>
**
** Conversions between UTF-16be and UTF-16le are always done in place and do
** not invalidate a prior pointer, though of course the content of the buffer
** that the prior pointer points to will have been modified.  Other kinds
** of conversion are done in place when it is possible, but sometimes they
** are not possible and in those cases prior pointers are invalidated.
**
** The safest and easiest to remember policy is to invoke these routines
** in one of the following ways:
**
** <ul>
**  <li>sqlite3_column_text() followed by sqlite3_column_bytes()</li>
**  <li>sqlite3_column_blob() followed by sqlite3_column_bytes()</li>
**  <li>sqlite3_column_text16() followed by sqlite3_column_bytes16()</li>
** </ul>
**
** In other words, you should call sqlite3_column_text(),
** sqlite3_column_blob(), or sqlite3_column_text16() first to force the result
** into the desired format, then invoke sqlite3_column_bytes() or
** sqlite3_column_bytes16() to find the size of the result.  Do not mix calls
** to sqlite3_column_text() or sqlite3_column_blob() with calls to
** sqlite3_column_bytes16(), and do not mix calls to sqlite3_column_text16()
** with calls to sqlite3_column_bytes().
**
** The pointers returned are valid until a type conversion occurs as
** described above, or until [sqlite3_step()] or [sqlite3_reset()] or
** [sqlite3_finalize()] is called.  The memory space used to hold strings
** and BLOBs is freed automatically.  Do <b>not</b> pass the pointers returned
** [sqlite3_column_blob()], [sqlite3_column_text()], etc. into
** [sqlite3_free()].
**
** If a memory allocation error occurs during the evaluation of any
** of these routines, a default value is returned.  The default value
** is either the integer 0, the floating point number 0.0, or a NULL
** pointer.  Subsequent calls to [sqlite3_errcode()] will return
** [SQLITE_NOMEM].
**
** Requirements:
** [H13803] [H13806] [H13809] [H13812] [H13815] [H13818] [H13821] [H13824]
** [H13827] [H13830]
*/
SQLITE_API const void *sqlite3_column_blob(sqlite3_stmt*, int iCol);
SQLITE_API int sqlite3_column_bytes(sqlite3_stmt*, int iCol);
SQLITE_API int sqlite3_column_bytes16(sqlite3_stmt*, int iCol);
SQLITE_API double sqlite3_column_double(sqlite3_stmt*, int iCol);
SQLITE_API int sqlite3_column_int(sqlite3_stmt*, int iCol);
SQLITE_API sqlite3_int64 sqlite3_column_int64(sqlite3_stmt*, int iCol);
SQLITE_API const unsigned char *sqlite3_column_text(sqlite3_stmt*, int iCol);
SQLITE_API const void *sqlite3_column_text16(sqlite3_stmt*, int iCol);
SQLITE_API int sqlite3_column_type(sqlite3_stmt*, int iCol);
SQLITE_API sqlite3_value *sqlite3_column_value(sqlite3_stmt*, int iCol);

/*
** CAPI3REF: Destroy A Prepared Statement Object {H13300} <S70300><S30100>
**
** The sqlite3_finalize() function is called to delete a [prepared statement].
** If the statement was executed successfully or not executed at all, then
** SQLITE_OK is returned. If execution of the statement failed then an
** [error code] or [extended error code] is returned.
**
** This routine can be called at any point during the execution of the
** [prepared statement].  If the virtual machine has not
** completed execution when this routine is called, that is like
** encountering an error or an [sqlite3_interrupt | interrupt].
** Incomplete updates may be rolled back and transactions canceled,
** depending on the circumstances, and the
** [error code] returned will be [SQLITE_ABORT].
**
** Requirements:
** [H11302] [H11304]
*/
SQLITE_API int sqlite3_finalize(sqlite3_stmt *pStmt);

/*
** CAPI3REF: Reset A Prepared Statement Object {H13330} <S70300>
**
** The sqlite3_reset() function is called to reset a [prepared statement]
** object back to its initial state, ready to be re-executed.
** Any SQL statement variables that had values bound to them using
** the [sqlite3_bind_blob | sqlite3_bind_*() API] retain their values.
** Use [sqlite3_clear_bindings()] to reset the bindings.
**
** {H11332} The [sqlite3_reset(S)] interface resets the [prepared statement] S
**          back to the beginning of its program.
**
** {H11334} If the most recent call to [sqlite3_step(S)] for the
**          [prepared statement] S returned [SQLITE_ROW] or [SQLITE_DONE],
**          or if [sqlite3_step(S)] has never before been called on S,
**          then [sqlite3_reset(S)] returns [SQLITE_OK].
**
** {H11336} If the most recent call to [sqlite3_step(S)] for the
**          [prepared statement] S indicated an error, then
**          [sqlite3_reset(S)] returns an appropriate [error code].
**
** {H11338} The [sqlite3_reset(S)] interface does not change the values
**          of any [sqlite3_bind_blob|bindings] on the [prepared statement] S.
*/
SQLITE_API int sqlite3_reset(sqlite3_stmt *pStmt);

/*
** CAPI3REF: Create Or Redefine SQL Functions {H16100} <S20200>
** KEYWORDS: {function creation routines}
** KEYWORDS: {application-defined SQL function}
** KEYWORDS: {application-defined SQL functions}
**
** These two functions (collectively known as "function creation routines")
** are used to add SQL functions or aggregates or to redefine the behavior
** of existing SQL functions or aggregates.  The only difference between the
** two is that the second parameter, the name of the (scalar) function or
** aggregate, is encoded in UTF-8 for sqlite3_create_function() and UTF-16
** for sqlite3_create_function16().
**
** The first parameter is the [database connection] to which the SQL
** function is to be added.  If a single program uses more than one database
** connection internally, then SQL functions must be added individually to
** each database connection.
**
** The second parameter is the name of the SQL function to be created or
** redefined.  The length of the name is limited to 255 bytes, exclusive of
** the zero-terminator.  Note that the name length limit is in bytes, not
** characters.  Any attempt to create a function with a longer name
** will result in [SQLITE_ERROR] being returned.
**
** The third parameter (nArg)
** is the number of arguments that the SQL function or
** aggregate takes. If this parameter is -1, then the SQL function or
** aggregate may take any number of arguments between 0 and the limit
** set by [sqlite3_limit]([SQLITE_LIMIT_FUNCTION_ARG]).  If the third
** parameter is less than -1 or greater than 127 then the behavior is
** undefined.
**
** The fourth parameter, eTextRep, specifies what
** [SQLITE_UTF8 | text encoding] this SQL function prefers for
** its parameters.  Any SQL function implementation should be able to work
** work with UTF-8, UTF-16le, or UTF-16be.  But some implementations may be
** more efficient with one encoding than another.  An application may
** invoke sqlite3_create_function() or sqlite3_create_function16() multiple
** times with the same function but with different values of eTextRep.
** When multiple implementations of the same function are available, SQLite
** will pick the one that involves the least amount of data conversion.
** If there is only a single implementation which does not care what text
** encoding is used, then the fourth argument should be [SQLITE_ANY].
**
** The fifth parameter is an arbitrary pointer.  The implementation of the
** function can gain access to this pointer using [sqlite3_user_data()].
**
** The seventh, eighth and ninth parameters, xFunc, xStep and xFinal, are
** pointers to C-language functions that implement the SQL function or
** aggregate. A scalar SQL function requires an implementation of the xFunc
** callback only, NULL pointers should be passed as the xStep and xFinal
** parameters. An aggregate SQL function requires an implementation of xStep
** and xFinal and NULL should be passed for xFunc. To delete an existing
** SQL function or aggregate, pass NULL for all three function callbacks.
**
** It is permitted to register multiple implementations of the same
** functions with the same name but with either differing numbers of
** arguments or differing preferred text encodings.  SQLite will use
** the implementation that most closely matches the way in which the
** SQL function is used.  A function implementation with a non-negative
** nArg parameter is a better match than a function implementation with
** a negative nArg.  A function where the preferred text encoding
** matches the database encoding is a better
** match than a function where the encoding is different.  
** A function where the encoding difference is between UTF16le and UTF16be
** is a closer match than a function where the encoding difference is
** between UTF8 and UTF16.
**
** Built-in functions may be overloaded by new application-defined functions.
** The first application-defined function with a given name overrides all
** built-in functions in the same [database connection] with the same name.
** Subsequent application-defined functions of the same name only override 
** prior application-defined functions that are an exact match for the
** number of parameters and preferred encoding.
**
** An application-defined function is permitted to call other
** SQLite interfaces.  However, such calls must not
** close the database connection nor finalize or reset the prepared
** statement in which the function is running.
**
** Requirements:
** [H16103] [H16106] [H16109] [H16112] [H16118] [H16121] [H16127]
** [H16130] [H16133] [H16136] [H16139] [H16142]
*/
SQLITE_API int sqlite3_create_function(
  sqlite3 *db,
  const char *zFunctionName,
  int nArg,
  int eTextRep,
  void *pApp,
  void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
  void (*xStep)(sqlite3_context*,int,sqlite3_value**),
  void (*xFinal)(sqlite3_context*)
);
SQLITE_API int sqlite3_create_function16(
  sqlite3 *db,
  const void *zFunctionName,
  int nArg,
  int eTextRep,
  void *pApp,
  void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
  void (*xStep)(sqlite3_context*,int,sqlite3_value**),
  void (*xFinal)(sqlite3_context*)
);

/*
** CAPI3REF: Text Encodings {H10267} <S50200> <H16100>
**
** These constant define integer codes that represent the various
** text encodings supported by SQLite.
*/
#define SQLITE_UTF8           1
#define SQLITE_UTF16LE        2
#define SQLITE_UTF16BE        3
#define SQLITE_UTF16          4    /* Use native byte order */
#define SQLITE_ANY            5    /* sqlite3_create_function only */
#define SQLITE_UTF16_ALIGNED  8    /* sqlite3_create_collation only */

/*
** CAPI3REF: Deprecated Functions
** DEPRECATED
**
** These functions are [deprecated].  In order to maintain
** backwards compatibility with older code, these functions continue 
** to be supported.  However, new applications should avoid
** the use of these functions.  To help encourage people to avoid
** using these functions, we are not going to tell you what they do.
*/
#ifndef SQLITE_OMIT_DEPRECATED
SQLITE_API SQLITE_DEPRECATED int sqlite3_aggregate_count(sqlite3_context*);
SQLITE_API SQLITE_DEPRECATED int sqlite3_expired(sqlite3_stmt*);
SQLITE_API SQLITE_DEPRECATED int sqlite3_transfer_bindings(sqlite3_stmt*, sqlite3_stmt*);
SQLITE_API SQLITE_DEPRECATED int sqlite3_global_recover(void);
SQLITE_API SQLITE_DEPRECATED void sqlite3_thread_cleanup(void);
SQLITE_API SQLITE_DEPRECATED int sqlite3_memory_alarm(void(*)(void*,sqlite3_int64,int),void*,sqlite3_int64);
#endif

/*
** CAPI3REF: Obtaining SQL Function Parameter Values {H15100} <S20200>
**
** The C-language implementation of SQL functions and aggregates uses
** this set of interface routines to access the parameter values on
** the function or aggregate.
**
** The xFunc (for scalar functions) or xStep (for aggregates) parameters
** to [sqlite3_create_function()] and [sqlite3_create_function16()]
** define callbacks that implement the SQL functions and aggregates.
** The 4th parameter to these callbacks is an array of pointers to
** [protected sqlite3_value] objects.  There is one [sqlite3_value] object for
** each parameter to the SQL function.  These routines are used to
** extract values from the [sqlite3_value] objects.
**
** These routines work only with [protected sqlite3_value] objects.
** Any attempt to use these routines on an [unprotected sqlite3_value]
** object results in undefined behavior.
**
** These routines work just like the corresponding [column access functions]
** except that  these routines take a single [protected sqlite3_value] object
** pointer instead of a [sqlite3_stmt*] pointer and an integer column number.
**
** The sqlite3_value_text16() interface extracts a UTF-16 string
** in the native byte-order of the host machine.  The
** sqlite3_value_text16be() and sqlite3_value_text16le() interfaces
** extract UTF-16 strings as big-endian and little-endian respectively.
**
** The sqlite3_value_numeric_type() interface attempts to apply
** numeric affinity to the value.  This means that an attempt is
** made to convert the value to an integer or floating point.  If
** such a conversion is possible without loss of information (in other
** words, if the value is a string that looks like a number)
** then the conversion is performed.  Otherwise no conversion occurs.
** The [SQLITE_INTEGER | datatype] after conversion is returned.
**
** Please pay particular attention to the fact that the pointer returned
** from [sqlite3_value_blob()], [sqlite3_value_text()], or
** [sqlite3_value_text16()] can be invalidated by a subsequent call to
** [sqlite3_value_bytes()], [sqlite3_value_bytes16()], [sqlite3_value_text()],
** or [sqlite3_value_text16()].
**
** These routines must be called from the same thread as
** the SQL function that supplied the [sqlite3_value*] parameters.
**
** Requirements:
** [H15103] [H15106] [H15109] [H15112] [H15115] [H15118] [H15121] [H15124]
** [H15127] [H15130] [H15133] [H15136]
*/
SQLITE_API const void *sqlite3_value_blob(sqlite3_value*);
SQLITE_API int sqlite3_value_bytes(sqlite3_value*);
SQLITE_API int sqlite3_value_bytes16(sqlite3_value*);
SQLITE_API double sqlite3_value_double(sqlite3_value*);
SQLITE_API int sqlite3_value_int(sqlite3_value*);
SQLITE_API sqlite3_int64 sqlite3_value_int64(sqlite3_value*);
SQLITE_API const unsigned char *sqlite3_value_text(sqlite3_value*);
SQLITE_API const void *sqlite3_value_text16(sqlite3_value*);
SQLITE_API const void *sqlite3_value_text16le(sqlite3_value*);
SQLITE_API const void *sqlite3_value_text16be(sqlite3_value*);
SQLITE_API int sqlite3_value_type(sqlite3_value*);
SQLITE_API int sqlite3_value_numeric_type(sqlite3_value*);

/*
** CAPI3REF: Obtain Aggregate Function Context {H16210} <S20200>
**
** The implementation of aggregate SQL functions use this routine to allocate
** a structure for storing their state.
**
** The first time the sqlite3_aggregate_context() routine is called for a
** particular aggregate, SQLite allocates nBytes of memory, zeroes out that
** memory, and returns a pointer to it. On second and subsequent calls to
** sqlite3_aggregate_context() for the same aggregate function index,
** the same buffer is returned. The implementation of the aggregate can use
** the returned buffer to accumulate data.
**
** SQLite automatically frees the allocated buffer when the aggregate
** query concludes.
**
** The first parameter should be a copy of the
** [sqlite3_context | SQL function context] that is the first parameter
** to the callback routine that implements the aggregate function.
**
** This routine must be called from the same thread in which
** the aggregate SQL function is running.
**
** Requirements:
** [H16211] [H16213] [H16215] [H16217]
*/
SQLITE_API void *sqlite3_aggregate_context(sqlite3_context*, int nBytes);

/*
** CAPI3REF: User Data For Functions {H16240} <S20200>
**
** The sqlite3_user_data() interface returns a copy of
** the pointer that was the pUserData parameter (the 5th parameter)
** of the [sqlite3_create_function()]
** and [sqlite3_create_function16()] routines that originally
** registered the application defined function. {END}
**
** This routine must be called from the same thread in which
** the application-defined function is running.
**
** Requirements:
** [H16243]
*/
SQLITE_API void *sqlite3_user_data(sqlite3_context*);

/*
** CAPI3REF: Database Connection For Functions {H16250} <S60600><S20200>
**
** The sqlite3_context_db_handle() interface returns a copy of
** the pointer to the [database connection] (the 1st parameter)
** of the [sqlite3_create_function()]
** and [sqlite3_create_function16()] routines that originally
** registered the application defined function.
**
** Requirements:
** [H16253]
*/
SQLITE_API sqlite3 *sqlite3_context_db_handle(sqlite3_context*);

/*
** CAPI3REF: Function Auxiliary Data {H16270} <S20200>
**
** The following two functions may be used by scalar SQL functions to
** associate metadata with argument values. If the same value is passed to
** multiple invocations of the same SQL function during query execution, under
** some circumstances the associated metadata may be preserved. This may
** be used, for example, to add a regular-expression matching scalar
** function. The compiled version of the regular expression is stored as
** metadata associated with the SQL value passed as the regular expression
** pattern.  The compiled regular expression can be reused on multiple
** invocations of the same function so that the original pattern string
** does not need to be recompiled on each invocation.
**
** The sqlite3_get_auxdata() interface returns a pointer to the metadata
** associated by the sqlite3_set_auxdata() function with the Nth argument
** value to the application-defined function. If no metadata has been ever
** been set for the Nth argument of the function, or if the corresponding
** function parameter has changed since the meta-data was set,
** then sqlite3_get_auxdata() returns a NULL pointer.
**
** The sqlite3_set_auxdata() interface saves the metadata
** pointed to by its 3rd parameter as the metadata for the N-th
** argument of the application-defined function.  Subsequent
** calls to sqlite3_get_auxdata() might return this data, if it has
** not been destroyed.
** If it is not NULL, SQLite will invoke the destructor
** function given by the 4th parameter to sqlite3_set_auxdata() on
** the metadata when the corresponding function parameter changes
** or when the SQL statement completes, whichever comes first.
**
** SQLite is free to call the destructor and drop metadata on any
** parameter of any function at any time.  The only guarantee is that
** the destructor will be called before the metadata is dropped.
**
** In practice, metadata is preserved between function calls for
** expressions that are constant at compile time. This includes literal
** values and SQL variables.
**
** These routines must be called from the same thread in which
** the SQL function is running.
**
** Requirements:
** [H16272] [H16274] [H16276] [H16277] [H16278] [H16279]
*/
SQLITE_API void *sqlite3_get_auxdata(sqlite3_context*, int N);
SQLITE_API void sqlite3_set_auxdata(sqlite3_context*, int N, void*, void (*)(void*));


/*
** CAPI3REF: Constants Defining Special Destructor Behavior {H10280} <S30100>
**
** These are special values for the destructor that is passed in as the
** final argument to routines like [sqlite3_result_blob()].  If the destructor
** argument is SQLITE_STATIC, it means that the content pointer is constant
** and will never change.  It does not need to be destroyed.  The
** SQLITE_TRANSIENT value means that the content will likely change in
** the near future and that SQLite should make its own private copy of
** the content before returning.
**
** The typedef is necessary to work around problems in certain
** C++ compilers.  See ticket #2191.
*/
typedef void (*sqlite3_destructor_type)(void*);
#define SQLITE_STATIC      ((sqlite3_destructor_type)0)
#define SQLITE_TRANSIENT   ((sqlite3_destructor_type)-1)

/*
** CAPI3REF: Setting The Result Of An SQL Function {H16400} <S20200>
**
** These routines are used by the xFunc or xFinal callbacks that
** implement SQL functions and aggregates.  See
** [sqlite3_create_function()] and [sqlite3_create_function16()]
** for additional information.
**
** These functions work very much like the [parameter binding] family of
** functions used to bind values to host parameters in prepared statements.
** Refer to the [SQL parameter] documentation for additional information.
**
** The sqlite3_result_blob() interface sets the result from
** an application-defined function to be the BLOB whose content is pointed
** to by the second parameter and which is N bytes long where N is the
** third parameter.
**
** The sqlite3_result_zeroblob() interfaces set the result of
** the application-defined function to be a BLOB containing all zero
** bytes and N bytes in size, where N is the value of the 2nd parameter.
**
** The sqlite3_result_double() interface sets the result from
** an application-defined function to be a floating point value specified
** by its 2nd argument.
**
** The sqlite3_result_error() and sqlite3_result_error16() functions
** cause the implemented SQL function to throw an exception.
** SQLite uses the string pointed to by the
** 2nd parameter of sqlite3_result_error() or sqlite3_result_error16()
** as the text of an error message.  SQLite interprets the error
** message string from sqlite3_result_error() as UTF-8. SQLite
** interprets the string from sqlite3_result_error16() as UTF-16 in native
** byte order.  If the third parameter to sqlite3_result_error()
** or sqlite3_result_error16() is negative then SQLite takes as the error
** message all text up through the first zero character.
** If the third parameter to sqlite3_result_error() or
** sqlite3_result_error16() is non-negative then SQLite takes that many
** bytes (not characters) from the 2nd parameter as the error message.
** The sqlite3_result_error() and sqlite3_result_error16()
** routines make a private copy of the error message text before
** they return.  Hence, the calling function can deallocate or
** modify the text after they return without harm.
** The sqlite3_result_error_code() function changes the error code
** returned by SQLite as a result of an error in a function.  By default,
** the error code is SQLITE_ERROR.  A subsequent call to sqlite3_result_error()
** or sqlite3_result_error16() resets the error code to SQLITE_ERROR.
**
** The sqlite3_result_toobig() interface causes SQLite to throw an error
** indicating that a string or BLOB is to long to represent.
**
** The sqlite3_result_nomem() interface causes SQLite to throw an error
** indicating that a memory allocation failed.
**
** The sqlite3_result_int() interface sets the return value
** of the application-defined function to be the 32-bit signed integer
** value given in the 2nd argument.
** The sqlite3_result_int64() interface sets the return value
** of the application-defined function to be the 64-bit signed integer
** value given in the 2nd argument.
**
** The sqlite3_result_null() interface sets the return value
** of the application-defined function to be NULL.
**
** The sqlite3_result_text(), sqlite3_result_text16(),
** sqlite3_result_text16le(), and sqlite3_result_text16be() interfaces
** set the return value of the application-defined function to be
** a text string which is represented as UTF-8, UTF-16 native byte order,
** UTF-16 little endian, or UTF-16 big endian, respectively.
** SQLite takes the text result from the application from
** the 2nd parameter of the sqlite3_result_text* interfaces.
** If the 3rd parameter to the sqlite3_result_text* interfaces
** is negative, then SQLite takes result text from the 2nd parameter
** through the first zero character.
** If the 3rd parameter to the sqlite3_result_text* interfaces
** is non-negative, then as many bytes (not characters) of the text
** pointed to by the 2nd parameter are taken as the application-defined
** function result.
** If the 4th parameter to the sqlite3_result_text* interfaces
** or sqlite3_result_blob is a non-NULL pointer, then SQLite calls that
** function as the destructor on the text or BLOB result when it has
** finished using that result.
** If the 4th parameter to the sqlite3_result_text* interfaces or to
** sqlite3_result_blob is the special constant SQLITE_STATIC, then SQLite
** assumes that the text or BLOB result is in constant space and does not
** copy the content of the parameter nor call a destructor on the content
** when it has finished using that result.
** If the 4th parameter to the sqlite3_result_text* interfaces
** or sqlite3_result_blob is the special constant SQLITE_TRANSIENT
** then SQLite makes a copy of the result into space obtained from
** from [sqlite3_malloc()] before it returns.
**
** The sqlite3_result_value() interface sets the result of
** the application-defined function to be a copy the
** [unprotected sqlite3_value] object specified by the 2nd parameter.  The
** sqlite3_result_value() interface makes a copy of the [sqlite3_value]
** so that the [sqlite3_value] specified in the parameter may change or
** be deallocated after sqlite3_result_value() returns without harm.
** A [protected sqlite3_value] object may always be used where an
** [unprotected sqlite3_value] object is required, so either
** kind of [sqlite3_value] object can be used with this interface.
**
** If these routines are called from within the different thread
** than the one containing the application-defined function that received
** the [sqlite3_context] pointer, the results are undefined.
**
** Requirements:
** [H16403] [H16406] [H16409] [H16412] [H16415] [H16418] [H16421] [H16424]
** [H16427] [H16430] [H16433] [H16436] [H16439] [H16442] [H16445] [H16448]
** [H16451] [H16454] [H16457] [H16460] [H16463]
*/
SQLITE_API void sqlite3_result_blob(sqlite3_context*, const void*, int, void(*)(void*));
SQLITE_API void sqlite3_result_double(sqlite3_context*, double);
SQLITE_API void sqlite3_result_error(sqlite3_context*, const char*, int);
SQLITE_API void sqlite3_result_error16(sqlite3_context*, const void*, int);
SQLITE_API void sqlite3_result_error_toobig(sqlite3_context*);
SQLITE_API void sqlite3_result_error_nomem(sqlite3_context*);
SQLITE_API void sqlite3_result_error_code(sqlite3_context*, int);
SQLITE_API void sqlite3_result_int(sqlite3_context*, int);
SQLITE_API void sqlite3_result_int64(sqlite3_context*, sqlite3_int64);
SQLITE_API void sqlite3_result_null(sqlite3_context*);
SQLITE_API void sqlite3_result_text(sqlite3_context*, const char*, int, void(*)(void*));
SQLITE_API void sqlite3_result_text16(sqlite3_context*, const void*, int, void(*)(void*));
SQLITE_API void sqlite3_result_text16le(sqlite3_context*, const void*, int,void(*)(void*));
SQLITE_API void sqlite3_result_text16be(sqlite3_context*, const void*, int,void(*)(void*));
SQLITE_API void sqlite3_result_value(sqlite3_context*, sqlite3_value*);
SQLITE_API void sqlite3_result_zeroblob(sqlite3_context*, int n);

/*
** CAPI3REF: Define New Collating Sequences {H16600} <S20300>
**
** These functions are used to add new collation sequences to the
** [database connection] specified as the first argument.
**
** The name of the new collation sequence is specified as a UTF-8 string
** for sqlite3_create_collation() and sqlite3_create_collation_v2()
** and a UTF-16 string for sqlite3_create_collation16(). In all cases
** the name is passed as the second function argument.
**
** The third argument may be one of the constants [SQLITE_UTF8],
** [SQLITE_UTF16LE], or [SQLITE_UTF16BE], indicating that the user-supplied
** routine expects to be passed pointers to strings encoded using UTF-8,
** UTF-16 little-endian, or UTF-16 big-endian, respectively. The
** third argument might also be [SQLITE_UTF16] to indicate that the routine
** expects pointers to be UTF-16 strings in the native byte order, or the
** argument can be [SQLITE_UTF16_ALIGNED] if the
** the routine expects pointers to 16-bit word aligned strings
** of UTF-16 in the native byte order.
**
** A pointer to the user supplied routine must be passed as the fifth
** argument.  If it is NULL, this is the same as deleting the collation
** sequence (so that SQLite cannot call it anymore).
** Each time the application supplied function is invoked, it is passed
** as its first parameter a copy of the void* passed as the fourth argument
** to sqlite3_create_collation() or sqlite3_create_collation16().
**
** The remaining arguments to the application-supplied routine are two strings,
** each represented by a (length, data) pair and encoded in the encoding
** that was passed as the third argument when the collation sequence was
** registered. {END}  The application defined collation routine should
** return negative, zero or positive if the first string is less than,
** equal to, or greater than the second string. i.e. (STRING1 - STRING2).
**
** The sqlite3_create_collation_v2() works like sqlite3_create_collation()
** except that it takes an extra argument which is a destructor for
** the collation.  The destructor is called when the collation is
** destroyed and is passed a copy of the fourth parameter void* pointer
** of the sqlite3_create_collation_v2().
** Collations are destroyed when they are overridden by later calls to the
** collation creation functions or when the [database connection] is closed
** using [sqlite3_close()].
**
** See also:  [sqlite3_collation_needed()] and [sqlite3_collation_needed16()].
**
** Requirements:
** [H16603] [H16604] [H16606] [H16609] [H16612] [H16615] [H16618] [H16621]
** [H16624] [H16627] [H16630]
*/
SQLITE_API int sqlite3_create_collation(
  sqlite3*, 
  const char *zName, 
  int eTextRep, 
  void*,
  int(*xCompare)(void*,int,const void*,int,const void*)
);
SQLITE_API int sqlite3_create_collation_v2(
  sqlite3*, 
  const char *zName, 
  int eTextRep, 
  void*,
  int(*xCompare)(void*,int,const void*,int,const void*),
  void(*xDestroy)(void*)
);
SQLITE_API int sqlite3_create_collation16(
  sqlite3*, 
  const void *zName,
  int eTextRep, 
  void*,
  int(*xCompare)(void*,int,const void*,int,const void*)
);

/*
** CAPI3REF: Collation Needed Callbacks {H16700} <S20300>
**
** To avoid having to register all collation sequences before a database
** can be used, a single callback function may be registered with the
** [database connection] to be called whenever an undefined collation
** sequence is required.
**
** If the function is registered using the sqlite3_collation_needed() API,
** then it is passed the names of undefined collation sequences as strings
** encoded in UTF-8. {H16703} If sqlite3_collation_needed16() is used,
** the names are passed as UTF-16 in machine native byte order.
** A call to either function replaces any existing callback.
**
** When the callback is invoked, the first argument passed is a copy
** of the second argument to sqlite3_collation_needed() or
** sqlite3_collation_needed16().  The second argument is the database
** connection.  The third argument is one of [SQLITE_UTF8], [SQLITE_UTF16BE],
** or [SQLITE_UTF16LE], indicating the most desirable form of the collation
** sequence function required.  The fourth parameter is the name of the
** required collation sequence.
**
** The callback function should register the desired collation using
** [sqlite3_create_collation()], [sqlite3_create_collation16()], or
** [sqlite3_create_collation_v2()].
**
** Requirements:
** [H16702] [H16704] [H16706]
*/
SQLITE_API int sqlite3_collation_needed(
  sqlite3*, 
  void*, 
  void(*)(void*,sqlite3*,int eTextRep,const char*)
);
SQLITE_API int sqlite3_collation_needed16(
  sqlite3*, 
  void*,
  void(*)(void*,sqlite3*,int eTextRep,const void*)
);

/*
** Specify the key for an encrypted database.  This routine should be
** called right after sqlite3_open().
**
** The code to implement this API is not available in the public release
** of SQLite.
*/
SQLITE_API int sqlite3_key(
  sqlite3 *db,                   /* Database to be rekeyed */
  const void *pKey, int nKey     /* The key */
);

/*
** Change the key on an open database.  If the current database is not
** encrypted, this routine will encrypt it.  If pNew==0 or nNew==0, the
** database is decrypted.
**
** The code to implement this API is not available in the public release
** of SQLite.
*/
SQLITE_API int sqlite3_rekey(
  sqlite3 *db,                   /* Database to be rekeyed */
  const void *pKey, int nKey     /* The new key */
);

/*
** CAPI3REF: Suspend Execution For A Short Time {H10530} <S40410>
**
** The sqlite3_sleep() function causes the current thread to suspend execution
** for at least a number of milliseconds specified in its parameter.
**
** If the operating system does not support sleep requests with
** millisecond time resolution, then the time will be rounded up to
** the nearest second. The number of milliseconds of sleep actually
** requested from the operating system is returned.
**
** SQLite implements this interface by calling the xSleep()
** method of the default [sqlite3_vfs] object.
**
** Requirements: [H10533] [H10536]
*/
SQLITE_API int sqlite3_sleep(int);

/*
** CAPI3REF: Name Of The Folder Holding Temporary Files {H10310} <S20000>
**
** If this global variable is made to point to a string which is
** the name of a folder (a.k.a. directory), then all temporary files
** created by SQLite will be placed in that directory.  If this variable
** is a NULL pointer, then SQLite performs a search for an appropriate
** temporary file directory.
**
** It is not safe to read or modify this variable in more than one
** thread at a time.  It is not safe to read or modify this variable
** if a [database connection] is being used at the same time in a separate
** thread.
** It is intended that this variable be set once
** as part of process initialization and before any SQLite interface
** routines have been called and that this variable remain unchanged
** thereafter.
**
** The [temp_store_directory pragma] may modify this variable and cause
** it to point to memory obtained from [sqlite3_malloc].  Furthermore,
** the [temp_store_directory pragma] always assumes that any string
** that this variable points to is held in memory obtained from 
** [sqlite3_malloc] and the pragma may attempt to free that memory
** using [sqlite3_free].
** Hence, if this variable is modified directly, either it should be
** made NULL or made to point to memory obtained from [sqlite3_malloc]
** or else the use of the [temp_store_directory pragma] should be avoided.
*/
SQLITE_API SQLITE_EXTERN char *sqlite3_temp_directory;

/*
** CAPI3REF: Test For Auto-Commit Mode {H12930} <S60200>
** KEYWORDS: {autocommit mode}
**
** The sqlite3_get_autocommit() interface returns non-zero or
** zero if the given database connection is or is not in autocommit mode,
** respectively.  Autocommit mode is on by default.
** Autocommit mode is disabled by a [BEGIN] statement.
** Autocommit mode is re-enabled by a [COMMIT] or [ROLLBACK].
**
** If certain kinds of errors occur on a statement within a multi-statement
** transaction (errors including [SQLITE_FULL], [SQLITE_IOERR],
** [SQLITE_NOMEM], [SQLITE_BUSY], and [SQLITE_INTERRUPT]) then the
** transaction might be rolled back automatically.  The only way to
** find out whether SQLite automatically rolled back the transaction after
** an error is to use this function.
**
** If another thread changes the autocommit status of the database
** connection while this routine is running, then the return value
** is undefined.
**
** Requirements: [H12931] [H12932] [H12933] [H12934]
*/
SQLITE_API int sqlite3_get_autocommit(sqlite3*);

/*
** CAPI3REF: Find The Database Handle Of A Prepared Statement {H13120} <S60600>
**
** The sqlite3_db_handle interface returns the [database connection] handle
** to which a [prepared statement] belongs.  The [database connection]
** returned by sqlite3_db_handle is the same [database connection] that was the first argument
** to the [sqlite3_prepare_v2()] call (or its variants) that was used to
** create the statement in the first place.
**
** Requirements: [H13123]
*/
SQLITE_API sqlite3 *sqlite3_db_handle(sqlite3_stmt*);

/*
** CAPI3REF: Find the next prepared statement {H13140} <S60600>
**
** This interface returns a pointer to the next [prepared statement] after
** pStmt associated with the [database connection] pDb.  If pStmt is NULL
** then this interface returns a pointer to the first prepared statement
** associated with the database connection pDb.  If no prepared statement
** satisfies the conditions of this routine, it returns NULL.
**
** The [database connection] pointer D in a call to
** [sqlite3_next_stmt(D,S)] must refer to an open database
** connection and in particular must not be a NULL pointer.
**
** Requirements: [H13143] [H13146] [H13149] [H13152]
*/
SQLITE_API sqlite3_stmt *sqlite3_next_stmt(sqlite3 *pDb, sqlite3_stmt *pStmt);

/*
** CAPI3REF: Commit And Rollback Notification Callbacks {H12950} <S60400>
**
** The sqlite3_commit_hook() interface registers a callback
** function to be invoked whenever a transaction is [COMMIT | committed].
** Any callback set by a previous call to sqlite3_commit_hook()
** for the same database connection is overridden.
** The sqlite3_rollback_hook() interface registers a callback
** function to be invoked whenever a transaction is [ROLLBACK | rolled back].
** Any callback set by a previous call to sqlite3_commit_hook()
** for the same database connection is overridden.
** The pArg argument is passed through to the callback.
** If the callback on a commit hook function returns non-zero,
** then the commit is converted into a rollback.
**
** If another function was previously registered, its
** pArg value is returned.  Otherwise NULL is returned.
**
** The callback implementation must not do anything that will modify
** the database connection that invoked the callback.  Any actions
** to modify the database connection must be deferred until after the
** completion of the [sqlite3_step()] call that triggered the commit
** or rollback hook in the first place.
** Note that [sqlite3_prepare_v2()] and [sqlite3_step()] both modify their
** database connections for the meaning of "modify" in this paragraph.
**
** Registering a NULL function disables the callback.
**
** When the commit hook callback routine returns zero, the [COMMIT]
** operation is allowed to continue normally.  If the commit hook
** returns non-zero, then the [COMMIT] is converted into a [ROLLBACK].
** The rollback hook is invoked on a rollback that results from a commit
** hook returning non-zero, just as it would be with any other rollback.
**
** For the purposes of this API, a transaction is said to have been
** rolled back if an explicit "ROLLBACK" statement is executed, or
** an error or constraint causes an implicit rollback to occur.
** The rollback callback is not invoked if a transaction is
** automatically rolled back because the database connection is closed.
** The rollback callback is not invoked if a transaction is
** rolled back because a commit callback returned non-zero.
** <todo> Check on this </todo>
**
** See also the [sqlite3_update_hook()] interface.
**
** Requirements:
** [H12951] [H12952] [H12953] [H12954] [H12955]
** [H12961] [H12962] [H12963] [H12964]
*/
SQLITE_API void *sqlite3_commit_hook(sqlite3*, int(*)(void*), void*);
SQLITE_API void *sqlite3_rollback_hook(sqlite3*, void(*)(void *), void*);

/*
** CAPI3REF: Data Change Notification Callbacks {H12970} <S60400>
**
** The sqlite3_update_hook() interface registers a callback function
** with the [database connection] identified by the first argument
** to be invoked whenever a row is updated, inserted or deleted.
** Any callback set by a previous call to this function
** for the same database connection is overridden.
**
** The second argument is a pointer to the function to invoke when a
** row is updated, inserted or deleted.
** The first argument to the callback is a copy of the third argument
** to sqlite3_update_hook().
** The second callback argument is one of [SQLITE_INSERT], [SQLITE_DELETE],
** or [SQLITE_UPDATE], depending on the operation that caused the callback
** to be invoked.
** The third and fourth arguments to the callback contain pointers to the
** database and table name containing the affected row.
** The final callback parameter is the [rowid] of the row.
** In the case of an update, this is the [rowid] after the update takes place.
**
** The update hook is not invoked when internal system tables are
** modified (i.e. sqlite_master and sqlite_sequence).
**
** In the current implementation, the update hook
** is not invoked when duplication rows are deleted because of an
** [ON CONFLICT | ON CONFLICT REPLACE] clause.  Nor is the update hook
** invoked when rows are deleted using the [truncate optimization].
** The exceptions defined in this paragraph might change in a future
** release of SQLite.
**
** The update hook implementation must not do anything that will modify
** the database connection that invoked the update hook.  Any actions
** to modify the database connection must be deferred until after the
** completion of the [sqlite3_step()] call that triggered the update hook.
** Note that [sqlite3_prepare_v2()] and [sqlite3_step()] both modify their
** database connections for the meaning of "modify" in this paragraph.
**
** If another function was previously registered, its pArg value
** is returned.  Otherwise NULL is returned.
**
** See also the [sqlite3_commit_hook()] and [sqlite3_rollback_hook()]
** interfaces.
**
** Requirements:
** [H12971] [H12973] [H12975] [H12977] [H12979] [H12981] [H12983] [H12986]
*/
SQLITE_API void *sqlite3_update_hook(
  sqlite3*, 
  void(*)(void *,int ,char const *,char const *,sqlite3_int64),
  void*
);

/*
** CAPI3REF: Enable Or Disable Shared Pager Cache {H10330} <S30900>
** KEYWORDS: {shared cache}
**
** This routine enables or disables the sharing of the database cache
** and schema data structures between [database connection | connections]
** to the same database. Sharing is enabled if the argument is true
** and disabled if the argument is false.
**
** Cache sharing is enabled and disabled for an entire process.
** This is a change as of SQLite version 3.5.0. In prior versions of SQLite,
** sharing was enabled or disabled for each thread separately.
**
** The cache sharing mode set by this interface effects all subsequent
** calls to [sqlite3_open()], [sqlite3_open_v2()], and [sqlite3_open16()].
** Existing database connections continue use the sharing mode
** that was in effect at the time they were opened.
**
** Virtual tables cannot be used with a shared cache.  When shared
** cache is enabled, the [sqlite3_create_module()] API used to register
** virtual tables will always return an error.
**
** This routine returns [SQLITE_OK] if shared cache was enabled or disabled
** successfully.  An [error code] is returned otherwise.
**
** Shared cache is disabled by default. But this might change in
** future releases of SQLite.  Applications that care about shared
** cache setting should set it explicitly.
**
** See Also:  [SQLite Shared-Cache Mode]
**
** Requirements: [H10331] [H10336] [H10337] [H10339]
*/
SQLITE_API int sqlite3_enable_shared_cache(int);

/*
** CAPI3REF: Attempt To Free Heap Memory {H17340} <S30220>
**
** The sqlite3_release_memory() interface attempts to free N bytes
** of heap memory by deallocating non-essential memory allocations
** held by the database library. {END}  Memory used to cache database
** pages to improve performance is an example of non-essential memory.
** sqlite3_release_memory() returns the number of bytes actually freed,
** which might be more or less than the amount requested.
**
** Requirements: [H17341] [H17342]
*/
SQLITE_API int sqlite3_release_memory(int);

/*
** CAPI3REF: Impose A Limit On Heap Size {H17350} <S30220>
**
** The sqlite3_soft_heap_limit() interface places a "soft" limit
** on the amount of heap memory that may be allocated by SQLite.
** If an internal allocation is requested that would exceed the
** soft heap limit, [sqlite3_release_memory()] is invoked one or
** more times to free up some space before the allocation is performed.
**
** The limit is called "soft", because if [sqlite3_release_memory()]
** cannot free sufficient memory to prevent the limit from being exceeded,
** the memory is allocated anyway and the current operation proceeds.
**
** A negative or zero value for N means that there is no soft heap limit and
** [sqlite3_release_memory()] will only be called when memory is exhausted.
** The default value for the soft heap limit is zero.
**
** SQLite makes a best effort to honor the soft heap limit.
** But if the soft heap limit cannot be honored, execution will
** continue without error or notification.  This is why the limit is
** called a "soft" limit.  It is advisory only.
**
** Prior to SQLite version 3.5.0, this routine only constrained the memory
** allocated by a single thread - the same thread in which this routine
** runs.  Beginning with SQLite version 3.5.0, the soft heap limit is
** applied to all threads. The value specified for the soft heap limit
** is an upper bound on the total memory allocation for all threads. In
** version 3.5.0 there is no mechanism for limiting the heap usage for
** individual threads.
**
** Requirements:
** [H16351] [H16352] [H16353] [H16354] [H16355] [H16358]
*/
SQLITE_API void sqlite3_soft_heap_limit(int);

/*
** CAPI3REF: Extract Metadata About A Column Of A Table {H12850} <S60300>
**
** This routine returns metadata about a specific column of a specific
** database table accessible using the [database connection] handle
** passed as the first function argument.
**
** The column is identified by the second, third and fourth parameters to
** this function. The second parameter is either the name of the database
** (i.e. "main", "temp" or an attached database) containing the specified
** table or NULL. If it is NULL, then all attached databases are searched
** for the table using the same algorithm used by the database engine to
** resolve unqualified table references.
**
** The third and fourth parameters to this function are the table and column
** name of the desired column, respectively. Neither of these parameters
** may be NULL.
**
** Metadata is returned by writing to the memory locations passed as the 5th
** and subsequent parameters to this function. Any of these arguments may be
** NULL, in which case the corresponding element of metadata is omitted.
**
** <blockquote>
** <table border="1">
** <tr><th> Parameter <th> Output<br>Type <th>  Description
**
** <tr><td> 5th <td> const char* <td> Data type
** <tr><td> 6th <td> const char* <td> Name of default collation sequence
** <tr><td> 7th <td> int         <td> True if column has a NOT NULL constraint
** <tr><td> 8th <td> int         <td> True if column is part of the PRIMARY KEY
** <tr><td> 9th <td> int         <td> True if column is [AUTOINCREMENT]
** </table>
** </blockquote>
**
** The memory pointed to by the character pointers returned for the
** declaration type and collation sequence is valid only until the next
** call to any SQLite API function.
**
** If the specified table is actually a view, an [error code] is returned.
**
** If the specified column is "rowid", "oid" or "_rowid_" and an
** [INTEGER PRIMARY KEY] column has been explicitly declared, then the output
** parameters are set for the explicitly declared column. If there is no
** explicitly declared [INTEGER PRIMARY KEY] column, then the output
** parameters are set as follows:
**
** <pre>
**     data type: "INTEGER"
**     collation sequence: "BINARY"
**     not null: 0
**     primary key: 1
**     auto increment: 0
** </pre>
**
** This function may load one or more schemas from database files. If an
** error occurs during this process, or if the requested table or column
** cannot be found, an [error code] is returned and an error message left
** in the [database connection] (to be retrieved using sqlite3_errmsg()).
**
** This API is only available if the library was compiled with the
** [SQLITE_ENABLE_COLUMN_METADATA] C-preprocessor symbol defined.
*/
SQLITE_API int sqlite3_table_column_metadata(
  sqlite3 *db,                /* Connection handle */
  const char *zDbName,        /* Database name or NULL */
  const char *zTableName,     /* Table name */
  const char *zColumnName,    /* Column name */
  char const **pzDataType,    /* OUTPUT: Declared data type */
  char const **pzCollSeq,     /* OUTPUT: Collation sequence name */
  int *pNotNull,              /* OUTPUT: True if NOT NULL constraint exists */
  int *pPrimaryKey,           /* OUTPUT: True if column part of PK */
  int *pAutoinc               /* OUTPUT: True if column is auto-increment */
);

/*
** CAPI3REF: Load An Extension {H12600} <S20500>
**
** This interface loads an SQLite extension library from the named file.
**
** {H12601} The sqlite3_load_extension() interface attempts to load an
**          SQLite extension library contained in the file zFile.
**
** {H12602} The entry point is zProc.
**
** {H12603} zProc may be 0, in which case the name of the entry point
**          defaults to "sqlite3_extension_init".
**
** {H12604} The sqlite3_load_extension() interface shall return
**          [SQLITE_OK] on success and [SQLITE_ERROR] if something goes wrong.
**
** {H12605} If an error occurs and pzErrMsg is not 0, then the
**          [sqlite3_load_extension()] interface shall attempt to
**          fill *pzErrMsg with error message text stored in memory
**          obtained from [sqlite3_malloc()]. {END}  The calling function
**          should free this memory by calling [sqlite3_free()].
**
** {H12606} Extension loading must be enabled using
**          [sqlite3_enable_load_extension()] prior to calling this API,
**          otherwise an error will be returned.
*/
SQLITE_API int sqlite3_load_extension(
  sqlite3 *db,          /* Load the extension into this database connection */
  const char *zFile,    /* Name of the shared library containing extension */
  const char *zProc,    /* Entry point.  Derived from zFile if 0 */
  char **pzErrMsg       /* Put error message here if not 0 */
);

/*
** CAPI3REF: Enable Or Disable Extension Loading {H12620} <S20500>
**
** So as not to open security holes in older applications that are
** unprepared to deal with extension loading, and as a means of disabling
** extension loading while evaluating user-entered SQL, the following API
** is provided to turn the [sqlite3_load_extension()] mechanism on and off.
**
** Extension loading is off by default. See ticket #1863.
**
** {H12621} Call the sqlite3_enable_load_extension() routine with onoff==1
**          to turn extension loading on and call it with onoff==0 to turn
**          it back off again.
**
** {H12622} Extension loading is off by default.
*/
SQLITE_API int sqlite3_enable_load_extension(sqlite3 *db, int onoff);

/*
** CAPI3REF: Automatically Load An Extensions {H12640} <S20500>
**
** This API can be invoked at program startup in order to register
** one or more statically linked extensions that will be available
** to all new [database connections]. {END}
**
** This routine stores a pointer to the extension in an array that is
** obtained from [sqlite3_malloc()].  If you run a memory leak checker
** on your program and it reports a leak because of this array, invoke
** [sqlite3_reset_auto_extension()] prior to shutdown to free the memory.
**
** {H12641} This function registers an extension entry point that is
**          automatically invoked whenever a new [database connection]
**          is opened using [sqlite3_open()], [sqlite3_open16()],
**          or [sqlite3_open_v2()].
**
** {H12642} Duplicate extensions are detected so calling this routine
**          multiple times with the same extension is harmless.
**
** {H12643} This routine stores a pointer to the extension in an array
**          that is obtained from [sqlite3_malloc()].
**
** {H12644} Automatic extensions apply across all threads.
*/
SQLITE_API int sqlite3_auto_extension(void (*xEntryPoint)(void));

/*
** CAPI3REF: Reset Automatic Extension Loading {H12660} <S20500>
**
** This function disables all previously registered automatic
** extensions. {END}  It undoes the effect of all prior
** [sqlite3_auto_extension()] calls.
**
** {H12661} This function disables all previously registered
**          automatic extensions.
**
** {H12662} This function disables automatic extensions in all threads.
*/
SQLITE_API void sqlite3_reset_auto_extension(void);

/*
****** EXPERIMENTAL - subject to change without notice **************
**
** The interface to the virtual-table mechanism is currently considered
** to be experimental.  The interface might change in incompatible ways.
** If this is a problem for you, do not use the interface at this time.
**
** When the virtual-table mechanism stabilizes, we will declare the
** interface fixed, support it indefinitely, and remove this comment.
*/

/*
** Structures used by the virtual table interface
*/
typedef struct sqlite3_vtab sqlite3_vtab;
typedef struct sqlite3_index_info sqlite3_index_info;
typedef struct sqlite3_vtab_cursor sqlite3_vtab_cursor;
typedef struct sqlite3_module sqlite3_module;

/*
** CAPI3REF: Virtual Table Object {H18000} <S20400>
** KEYWORDS: sqlite3_module {virtual table module}
** EXPERIMENTAL
**
** This structure, sometimes called a a "virtual table module", 
** defines the implementation of a [virtual tables].  
** This structure consists mostly of methods for the module.
**
** A virtual table module is created by filling in a persistent
** instance of this structure and passing a pointer to that instance
** to [sqlite3_create_module()] or [sqlite3_create_module_v2()].
** The registration remains valid until it is replaced by a different
** module or until the [database connection] closes.  The content
** of this structure must not change while it is registered with
** any database connection.
*/
struct sqlite3_module {
  int iVersion;
  int (*xCreate)(sqlite3*, void *pAux,
               int argc, const char *const*argv,
               sqlite3_vtab **ppVTab, char**);
  int (*xConnect)(sqlite3*, void *pAux,
               int argc, const char *const*argv,
               sqlite3_vtab **ppVTab, char**);
  int (*xBestIndex)(sqlite3_vtab *pVTab, sqlite3_index_info*);
  int (*xDisconnect)(sqlite3_vtab *pVTab);
  int (*xDestroy)(sqlite3_vtab *pVTab);
  int (*xOpen)(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor);
  int (*xClose)(sqlite3_vtab_cursor*);
  int (*xFilter)(sqlite3_vtab_cursor*, int idxNum, const char *idxStr,
                int argc, sqlite3_value **argv);
  int (*xNext)(sqlite3_vtab_cursor*);
  int (*xEof)(sqlite3_vtab_cursor*);
  int (*xColumn)(sqlite3_vtab_cursor*, sqlite3_context*, int);
  int (*xRowid)(sqlite3_vtab_cursor*, sqlite3_int64 *pRowid);
  int (*xUpdate)(sqlite3_vtab *, int, sqlite3_value **, sqlite3_int64 *);
  int (*xBegin)(sqlite3_vtab *pVTab);
  int (*xSync)(sqlite3_vtab *pVTab);
  int (*xCommit)(sqlite3_vtab *pVTab);
  int (*xRollback)(sqlite3_vtab *pVTab);
  int (*xFindFunction)(sqlite3_vtab *pVtab, int nArg, const char *zName,
                       void (**pxFunc)(sqlite3_context*,int,sqlite3_value**),
                       void **ppArg);
  int (*xRename)(sqlite3_vtab *pVtab, const char *zNew);
};

/*
** CAPI3REF: Virtual Table Indexing Information {H18100} <S20400>
** KEYWORDS: sqlite3_index_info
** EXPERIMENTAL
**
** The sqlite3_index_info structure and its substructures is used to
** pass information into and receive the reply from the [xBestIndex]
** method of a [virtual table module].  The fields under **Inputs** are the
** inputs to xBestIndex and are read-only.  xBestIndex inserts its
** results into the **Outputs** fields.
**
** The aConstraint[] array records WHERE clause constraints of the form:
**
** <pre>column OP expr</pre>
**
** where OP is =, &lt;, &lt;=, &gt;, or &gt;=.  The particular operator is
** stored in aConstraint[].op.  The index of the column is stored in
** aConstraint[].iColumn.  aConstraint[].usable is TRUE if the
** expr on the right-hand side can be evaluated (and thus the constraint
** is usable) and false if it cannot.
**
** The optimizer automatically inverts terms of the form "expr OP column"
** and makes other simplifications to the WHERE clause in an attempt to
** get as many WHERE clause terms into the form shown above as possible.
** The aConstraint[] array only reports WHERE clause terms in the correct
** form that refer to the particular virtual table being queried.
**
** Information about the ORDER BY clause is stored in aOrderBy[].
** Each term of aOrderBy records a column of the ORDER BY clause.
**
** The [xBestIndex] method must fill aConstraintUsage[] with information
** about what parameters to pass to xFilter.  If argvIndex>0 then
** the right-hand side of the corresponding aConstraint[] is evaluated
** and becomes the argvIndex-th entry in argv.  If aConstraintUsage[].omit
** is true, then the constraint is assumed to be fully handled by the
** virtual table and is not checked again by SQLite.
**
** The idxNum and idxPtr values are recorded and passed into the
** [xFilter] method.
** [sqlite3_free()] is used to free idxPtr if and only iff
** needToFreeIdxPtr is true.
**
** The orderByConsumed means that output from [xFilter]/[xNext] will occur in
** the correct order to satisfy the ORDER BY clause so that no separate
** sorting step is required.
**
** The estimatedCost value is an estimate of the cost of doing the
** particular lookup.  A full scan of a table with N entries should have
** a cost of N.  A binary search of a table of N entries should have a
** cost of approximately log(N).
*/
struct sqlite3_index_info {
  /* Inputs */
  int nConstraint;           /* Number of entries in aConstraint */
  struct sqlite3_index_constraint {
     int iColumn;              /* Column on left-hand side of constraint */
     unsigned char op;         /* Constraint operator */
     unsigned char usable;     /* True if this constraint is usable */
     int iTermOffset;          /* Used internally - xBestIndex should ignore */
  } *aConstraint;            /* Table of WHERE clause constraints */
  int nOrderBy;              /* Number of terms in the ORDER BY clause */
  struct sqlite3_index_orderby {
     int iColumn;              /* Column number */
     unsigned char desc;       /* True for DESC.  False for ASC. */
  } *aOrderBy;               /* The ORDER BY clause */
  /* Outputs */
  struct sqlite3_index_constraint_usage {
    int argvIndex;           /* if >0, constraint is part of argv to xFilter */
    unsigned char omit;      /* Do not code a test for this constraint */
  } *aConstraintUsage;
  int idxNum;                /* Number used to identify the index */
  char *idxStr;              /* String, possibly obtained from sqlite3_malloc */
  int needToFreeIdxStr;      /* Free idxStr using sqlite3_free() if true */
  int orderByConsumed;       /* True if output is already ordered */
  double estimatedCost;      /* Estimated cost of using this index */
};
#define SQLITE_INDEX_CONSTRAINT_EQ    2
#define SQLITE_INDEX_CONSTRAINT_GT    4
#define SQLITE_INDEX_CONSTRAINT_LE    8
#define SQLITE_INDEX_CONSTRAINT_LT    16
#define SQLITE_INDEX_CONSTRAINT_GE    32
#define SQLITE_INDEX_CONSTRAINT_MATCH 64

/*
** CAPI3REF: Register A Virtual Table Implementation {H18200} <S20400>
** EXPERIMENTAL
**
** This routine is used to register a new [virtual table module] name.
** Module names must be registered before
** creating a new [virtual table] using the module, or before using a
** preexisting [virtual table] for the module.
**
** The module name is registered on the [database connection] specified
** by the first parameter.  The name of the module is given by the 
** second parameter.  The third parameter is a pointer to
** the implementation of the [virtual table module].   The fourth
** parameter is an arbitrary client data pointer that is passed through
** into the [xCreate] and [xConnect] methods of the virtual table module
** when a new virtual table is be being created or reinitialized.
**
** This interface has exactly the same effect as calling
** [sqlite3_create_module_v2()] with a NULL client data destructor.
*/
SQLITE_API SQLITE_EXPERIMENTAL int sqlite3_create_module(
  sqlite3 *db,               /* SQLite connection to register module with */
  const char *zName,         /* Name of the module */
  const sqlite3_module *p,   /* Methods for the module */
  void *pClientData          /* Client data for xCreate/xConnect */
);

/*
** CAPI3REF: Register A Virtual Table Implementation {H18210} <S20400>
** EXPERIMENTAL
**
** This routine is identical to the [sqlite3_create_module()] method,
** except that it has an extra parameter to specify 
** a destructor function for the client data pointer.  SQLite will
** invoke the destructor function (if it is not NULL) when SQLite
** no longer needs the pClientData pointer.  
*/
SQLITE_API SQLITE_EXPERIMENTAL int sqlite3_create_module_v2(
  sqlite3 *db,               /* SQLite connection to register module with */
  const char *zName,         /* Name of the module */
  const sqlite3_module *p,   /* Methods for the module */
  void *pClientData,         /* Client data for xCreate/xConnect */
  void(*xDestroy)(void*)     /* Module destructor function */
);

/*
** CAPI3REF: Virtual Table Instance Object {H18010} <S20400>
** KEYWORDS: sqlite3_vtab
** EXPERIMENTAL
**
** Every [virtual table module] implementation uses a subclass
** of the following structure to describe a particular instance
** of the [virtual table].  Each subclass will
** be tailored to the specific needs of the module implementation.
** The purpose of this superclass is to define certain fields that are
** common to all module implementations.
**
** Virtual tables methods can set an error message by assigning a
** string obtained from [sqlite3_mprintf()] to zErrMsg.  The method should
** take care that any prior string is freed by a call to [sqlite3_free()]
** prior to assigning a new string to zErrMsg.  After the error message
** is delivered up to the client application, the string will be automatically
** freed by sqlite3_free() and the zErrMsg field will be zeroed.
*/
struct sqlite3_vtab {
  const sqlite3_module *pModule;  /* The module for this virtual table */
  int nRef;                       /* NO LONGER USED */
  char *zErrMsg;                  /* Error message from sqlite3_mprintf() */
  /* Virtual table implementations will typically add additional fields */
};

/*
** CAPI3REF: Virtual Table Cursor Object  {H18020} <S20400>
** KEYWORDS: sqlite3_vtab_cursor {virtual table cursor}
** EXPERIMENTAL
**
** Every [virtual table module] implementation uses a subclass of the
** following structure to describe cursors that point into the
** [virtual table] and are used
** to loop through the virtual table.  Cursors are created using the
** [sqlite3_module.xOpen | xOpen] method of the module and are destroyed
** by the [sqlite3_module.xClose | xClose] method.  Cussors are used
** by the [xFilter], [xNext], [xEof], [xColumn], and [xRowid] methods
** of the module.  Each module implementation will define
** the content of a cursor structure to suit its own needs.
**
** This superclass exists in order to define fields of the cursor that
** are common to all implementations.
*/
struct sqlite3_vtab_cursor {
  sqlite3_vtab *pVtab;      /* Virtual table of this cursor */
  /* Virtual table implementations will typically add additional fields */
};

/*
** CAPI3REF: Declare The Schema Of A Virtual Table {H18280} <S20400>
** EXPERIMENTAL
**
** The [xCreate] and [xConnect] methods of a
** [virtual table module] call this interface
** to declare the format (the names and datatypes of the columns) of
** the virtual tables they implement.
*/
SQLITE_API SQLITE_EXPERIMENTAL int sqlite3_declare_vtab(sqlite3*, const char *zSQL);

/*
** CAPI3REF: Overload A Function For A Virtual Table {H18300} <S20400>
** EXPERIMENTAL
**
** Virtual tables can provide alternative implementations of functions
** using the [xFindFunction] method of the [virtual table module].  
** But global versions of those functions
** must exist in order to be overloaded.
**
** This API makes sure a global version of a function with a particular
** name and number of parameters exists.  If no such function exists
** before this API is called, a new function is created.  The implementation
** of the new function always causes an exception to be thrown.  So
** the new function is not good for anything by itself.  Its only
** purpose is to be a placeholder function that can be overloaded
** by a [virtual table].
*/
SQLITE_API SQLITE_EXPERIMENTAL int sqlite3_overload_function(sqlite3*, const char *zFuncName, int nArg);

/*
** The interface to the virtual-table mechanism defined above (back up
** to a comment remarkably similar to this one) is currently considered
** to be experimental.  The interface might change in incompatible ways.
** If this is a problem for you, do not use the interface at this time.
**
** When the virtual-table mechanism stabilizes, we will declare the
** interface fixed, support it indefinitely, and remove this comment.
**
****** EXPERIMENTAL - subject to change without notice **************
*/

/*
** CAPI3REF: A Handle To An Open BLOB {H17800} <S30230>
** KEYWORDS: {BLOB handle} {BLOB handles}
**
** An instance of this object represents an open BLOB on which
** [sqlite3_blob_open | incremental BLOB I/O] can be performed.
** Objects of this type are created by [sqlite3_blob_open()]
** and destroyed by [sqlite3_blob_close()].
** The [sqlite3_blob_read()] and [sqlite3_blob_write()] interfaces
** can be used to read or write small subsections of the BLOB.
** The [sqlite3_blob_bytes()] interface returns the size of the BLOB in bytes.
*/
typedef struct sqlite3_blob sqlite3_blob;

/*
** CAPI3REF: Open A BLOB For Incremental I/O {H17810} <S30230>
**
** This interfaces opens a [BLOB handle | handle] to the BLOB located
** in row iRow, column zColumn, table zTable in database zDb;
** in other words, the same BLOB that would be selected by:
**
** <pre>
**     SELECT zColumn FROM zDb.zTable WHERE [rowid] = iRow;
** </pre> {END}
**
** If the flags parameter is non-zero, then the BLOB is opened for read
** and write access. If it is zero, the BLOB is opened for read access.
** It is not possible to open a column that is part of an index or primary 
** key for writing. ^If [foreign key constraints] are enabled, it is 
** not possible to open a column that is part of a [child key] for writing.
**
** Note that the database name is not the filename that contains
** the database but rather the symbolic name of the database that
** is assigned when the database is connected using [ATTACH].
** For the main database file, the database name is "main".
** For TEMP tables, the database name is "temp".
**
** On success, [SQLITE_OK] is returned and the new [BLOB handle] is written
** to *ppBlob. Otherwise an [error code] is returned and *ppBlob is set
** to be a null pointer.
** This function sets the [database connection] error code and message
** accessible via [sqlite3_errcode()] and [sqlite3_errmsg()] and related
** functions.  Note that the *ppBlob variable is always initialized in a
** way that makes it safe to invoke [sqlite3_blob_close()] on *ppBlob
** regardless of the success or failure of this routine.
**
** If the row that a BLOB handle points to is modified by an
** [UPDATE], [DELETE], or by [ON CONFLICT] side-effects
** then the BLOB handle is marked as "expired".
** This is true if any column of the row is changed, even a column
** other than the one the BLOB handle is open on.
** Calls to [sqlite3_blob_read()] and [sqlite3_blob_write()] for
** a expired BLOB handle fail with an return code of [SQLITE_ABORT].
** Changes written into a BLOB prior to the BLOB expiring are not
** rollback by the expiration of the BLOB.  Such changes will eventually
** commit if the transaction continues to completion.
**
** Use the [sqlite3_blob_bytes()] interface to determine the size of
** the opened blob.  The size of a blob may not be changed by this
** interface.  Use the [UPDATE] SQL command to change the size of a
** blob.
**
** The [sqlite3_bind_zeroblob()] and [sqlite3_result_zeroblob()] interfaces
** and the built-in [zeroblob] SQL function can be used, if desired,
** to create an empty, zero-filled blob in which to read or write using
** this interface.
**
** To avoid a resource leak, every open [BLOB handle] should eventually
** be released by a call to [sqlite3_blob_close()].
**
** Requirements:
** [H17813] [H17814] [H17816] [H17819] [H17821] [H17824]
*/
SQLITE_API int sqlite3_blob_open(
  sqlite3*,
  const char *zDb,
  const char *zTable,
  const char *zColumn,
  sqlite3_int64 iRow,
  int flags,
  sqlite3_blob **ppBlob
);

/*
** CAPI3REF: Close A BLOB Handle {H17830} <S30230>
**
** Closes an open [BLOB handle].
**
** Closing a BLOB shall cause the current transaction to commit
** if there are no other BLOBs, no pending prepared statements, and the
** database connection is in [autocommit mode].
** If any writes were made to the BLOB, they might be held in cache
** until the close operation if they will fit.
**
** Closing the BLOB often forces the changes
** out to disk and so if any I/O errors occur, they will likely occur
** at the time when the BLOB is closed.  Any errors that occur during
** closing are reported as a non-zero return value.
**
** The BLOB is closed unconditionally.  Even if this routine returns
** an error code, the BLOB is still closed.
**
** Calling this routine with a null pointer (which as would be returned
** by failed call to [sqlite3_blob_open()]) is a harmless no-op.
**
** Requirements:
** [H17833] [H17836] [H17839]
*/
SQLITE_API int sqlite3_blob_close(sqlite3_blob *);

/*
** CAPI3REF: Return The Size Of An Open BLOB {H17840} <S30230>
**
** Returns the size in bytes of the BLOB accessible via the 
** successfully opened [BLOB handle] in its only argument.  The
** incremental blob I/O routines can only read or overwriting existing
** blob content; they cannot change the size of a blob.
**
** This routine only works on a [BLOB handle] which has been created
** by a prior successful call to [sqlite3_blob_open()] and which has not
** been closed by [sqlite3_blob_close()].  Passing any other pointer in
** to this routine results in undefined and probably undesirable behavior.
**
** Requirements:
** [H17843]
*/
SQLITE_API int sqlite3_blob_bytes(sqlite3_blob *);

/*
** CAPI3REF: Read Data From A BLOB Incrementally {H17850} <S30230>
**
** This function is used to read data from an open [BLOB handle] into a
** caller-supplied buffer. N bytes of data are copied into buffer Z
** from the open BLOB, starting at offset iOffset.
**
** If offset iOffset is less than N bytes from the end of the BLOB,
** [SQLITE_ERROR] is returned and no data is read.  If N or iOffset is
** less than zero, [SQLITE_ERROR] is returned and no data is read.
** The size of the blob (and hence the maximum value of N+iOffset)
** can be determined using the [sqlite3_blob_bytes()] interface.
**
** An attempt to read from an expired [BLOB handle] fails with an
** error code of [SQLITE_ABORT].
**
** On success, SQLITE_OK is returned.
** Otherwise, an [error code] or an [extended error code] is returned.
**
** This routine only works on a [BLOB handle] which has been created
** by a prior successful call to [sqlite3_blob_open()] and which has not
** been closed by [sqlite3_blob_close()].  Passing any other pointer in
** to this routine results in undefined and probably undesirable behavior.
**
** See also: [sqlite3_blob_write()].
**
** Requirements:
** [H17853] [H17856] [H17859] [H17862] [H17863] [H17865] [H17868]
*/
SQLITE_API int sqlite3_blob_read(sqlite3_blob *, void *Z, int N, int iOffset);

/*
** CAPI3REF: Write Data Into A BLOB Incrementally {H17870} <S30230>
**
** This function is used to write data into an open [BLOB handle] from a
** caller-supplied buffer. N bytes of data are copied from the buffer Z
** into the open BLOB, starting at offset iOffset.
**
** If the [BLOB handle] passed as the first argument was not opened for
** writing (the flags parameter to [sqlite3_blob_open()] was zero),
** this function returns [SQLITE_READONLY].
**
** This function may only modify the contents of the BLOB; it is
** not possible to increase the size of a BLOB using this API.
** If offset iOffset is less than N bytes from the end of the BLOB,
** [SQLITE_ERROR] is returned and no data is written.  If N is
** less than zero [SQLITE_ERROR] is returned and no data is written.
** The size of the BLOB (and hence the maximum value of N+iOffset)
** can be determined using the [sqlite3_blob_bytes()] interface.
**
** An attempt to write to an expired [BLOB handle] fails with an
** error code of [SQLITE_ABORT].  Writes to the BLOB that occurred
** before the [BLOB handle] expired are not rolled back by the
** expiration of the handle, though of course those changes might
** have been overwritten by the statement that expired the BLOB handle
** or by other independent statements.
**
** On success, SQLITE_OK is returned.
** Otherwise, an  [error code] or an [extended error code] is returned.
**
** This routine only works on a [BLOB handle] which has been created
** by a prior successful call to [sqlite3_blob_open()] and which has not
** been closed by [sqlite3_blob_close()].  Passing any other pointer in
** to this routine results in undefined and probably undesirable behavior.
**
** See also: [sqlite3_blob_read()].
**
** Requirements:
** [H17873] [H17874] [H17875] [H17876] [H17877] [H17879] [H17882] [H17885]
** [H17888]
*/
SQLITE_API int sqlite3_blob_write(sqlite3_blob *, const void *z, int n, int iOffset);

/*
** CAPI3REF: Virtual File System Objects {H11200} <S20100>
**
** A virtual filesystem (VFS) is an [sqlite3_vfs] object
** that SQLite uses to interact
** with the underlying operating system.  Most SQLite builds come with a
** single default VFS that is appropriate for the host computer.
** New VFSes can be registered and existing VFSes can be unregistered.
** The following interfaces are provided.
**
** The sqlite3_vfs_find() interface returns a pointer to a VFS given its name.
** Names are case sensitive.
** Names are zero-terminated UTF-8 strings.
** If there is no match, a NULL pointer is returned.
** If zVfsName is NULL then the default VFS is returned.
**
** New VFSes are registered with sqlite3_vfs_register().
** Each new VFS becomes the default VFS if the makeDflt flag is set.
** The same VFS can be registered multiple times without injury.
** To make an existing VFS into the default VFS, register it again
** with the makeDflt flag set.  If two different VFSes with the
** same name are registered, the behavior is undefined.  If a
** VFS is registered with a name that is NULL or an empty string,
** then the behavior is undefined.
**
** Unregister a VFS with the sqlite3_vfs_unregister() interface.
** If the default VFS is unregistered, another VFS is chosen as
** the default.  The choice for the new VFS is arbitrary.
**
** Requirements:
** [H11203] [H11206] [H11209] [H11212] [H11215] [H11218]
*/
SQLITE_API sqlite3_vfs *sqlite3_vfs_find(const char *zVfsName);
SQLITE_API int sqlite3_vfs_register(sqlite3_vfs*, int makeDflt);
SQLITE_API int sqlite3_vfs_unregister(sqlite3_vfs*);

/*
** CAPI3REF: Mutexes {H17000} <S20000>
**
** The SQLite core uses these routines for thread
** synchronization. Though they are intended for internal
** use by SQLite, code that links against SQLite is
** permitted to use any of these routines.
**
** The SQLite source code contains multiple implementations
** of these mutex routines.  An appropriate implementation
** is selected automatically at compile-time.  The following
** implementations are available in the SQLite core:
**
** <ul>
** <li>   SQLITE_MUTEX_OS2
** <li>   SQLITE_MUTEX_PTHREAD
** <li>   SQLITE_MUTEX_W32
** <li>   SQLITE_MUTEX_NOOP
** </ul>
**
** The SQLITE_MUTEX_NOOP implementation is a set of routines
** that does no real locking and is appropriate for use in
** a single-threaded application.  The SQLITE_MUTEX_OS2,
** SQLITE_MUTEX_PTHREAD, and SQLITE_MUTEX_W32 implementations
** are appropriate for use on OS/2, Unix, and Windows.
**
** If SQLite is compiled with the SQLITE_MUTEX_APPDEF preprocessor
** macro defined (with "-DSQLITE_MUTEX_APPDEF=1"), then no mutex
** implementation is included with the library. In this case the
** application must supply a custom mutex implementation using the
** [SQLITE_CONFIG_MUTEX] option of the sqlite3_config() function
** before calling sqlite3_initialize() or any other public sqlite3_
** function that calls sqlite3_initialize().
**
** {H17011} The sqlite3_mutex_alloc() routine allocates a new
** mutex and returns a pointer to it. {H17012} If it returns NULL
** that means that a mutex could not be allocated. {H17013} SQLite
** will unwind its stack and return an error. {H17014} The argument
** to sqlite3_mutex_alloc() is one of these integer constants:
**
** <ul>
** <li>  SQLITE_MUTEX_FAST
** <li>  SQLITE_MUTEX_RECURSIVE
** <li>  SQLITE_MUTEX_STATIC_MASTER
** <li>  SQLITE_MUTEX_STATIC_MEM
** <li>  SQLITE_MUTEX_STATIC_MEM2
** <li>  SQLITE_MUTEX_STATIC_PRNG
** <li>  SQLITE_MUTEX_STATIC_LRU
** <li>  SQLITE_MUTEX_STATIC_LRU2
** </ul>
**
** {H17015} The first two constants cause sqlite3_mutex_alloc() to create
** a new mutex.  The new mutex is recursive when SQLITE_MUTEX_RECURSIVE
** is used but not necessarily so when SQLITE_MUTEX_FAST is used. {END}
** The mutex implementation does not need to make a distinction
** between SQLITE_MUTEX_RECURSIVE and SQLITE_MUTEX_FAST if it does
** not want to.  {H17016} But SQLite will only request a recursive mutex in
** cases where it really needs one.  {END} If a faster non-recursive mutex
** implementation is available on the host platform, the mutex subsystem
** might return such a mutex in response to SQLITE_MUTEX_FAST.
**
** {H17017} The other allowed parameters to sqlite3_mutex_alloc() each return
** a pointer to a static preexisting mutex. {END}  Six static mutexes are
** used by the current version of SQLite.  Future versions of SQLite
** may add additional static mutexes.  Static mutexes are for internal
** use by SQLite only.  Applications that use SQLite mutexes should
** use only the dynamic mutexes returned by SQLITE_MUTEX_FAST or
** SQLITE_MUTEX_RECURSIVE.
**
** {H17018} Note that if one of the dynamic mutex parameters (SQLITE_MUTEX_FAST
** or SQLITE_MUTEX_RECURSIVE) is used then sqlite3_mutex_alloc()
** returns a different mutex on every call.  {H17034} But for the static
** mutex types, the same mutex is returned on every call that has
** the same type number.
**
** {H17019} The sqlite3_mutex_free() routine deallocates a previously
** allocated dynamic mutex. {H17020} SQLite is careful to deallocate every
** dynamic mutex that it allocates. {A17021} The dynamic mutexes must not be in
** use when they are deallocated. {A17022} Attempting to deallocate a static
** mutex results in undefined behavior. {H17023} SQLite never deallocates
** a static mutex. {END}
**
** The sqlite3_mutex_enter() and sqlite3_mutex_try() routines attempt
** to enter a mutex. {H17024} If another thread is already within the mutex,
** sqlite3_mutex_enter() will block and sqlite3_mutex_try() will return
** SQLITE_BUSY. {H17025}  The sqlite3_mutex_try() interface returns [SQLITE_OK]
** upon successful entry.  {H17026} Mutexes created using
** SQLITE_MUTEX_RECURSIVE can be entered multiple times by the same thread.
** {H17027} In such cases the,
** mutex must be exited an equal number of times before another thread
** can enter.  {A17028} If the same thread tries to enter any other
** kind of mutex more than once, the behavior is undefined.
** {H17029} SQLite will never exhibit
** such behavior in its own use of mutexes.
**
** Some systems (for example, Windows 95) do not support the operation
** implemented by sqlite3_mutex_try().  On those systems, sqlite3_mutex_try()
** will always return SQLITE_BUSY.  {H17030} The SQLite core only ever uses
** sqlite3_mutex_try() as an optimization so this is acceptable behavior.
**
** {H17031} The sqlite3_mutex_leave() routine exits a mutex that was
** previously entered by the same thread.  {A17032} The behavior
** is undefined if the mutex is not currently entered by the
** calling thread or is not currently allocated.  {H17033} SQLite will
** never do either. {END}
**
** If the argument to sqlite3_mutex_enter(), sqlite3_mutex_try(), or
** sqlite3_mutex_leave() is a NULL pointer, then all three routines
** behave as no-ops.
**
** See also: [sqlite3_mutex_held()] and [sqlite3_mutex_notheld()].
*/
SQLITE_API sqlite3_mutex *sqlite3_mutex_alloc(int);
SQLITE_API void sqlite3_mutex_free(sqlite3_mutex*);
SQLITE_API void sqlite3_mutex_enter(sqlite3_mutex*);
SQLITE_API int sqlite3_mutex_try(sqlite3_mutex*);
SQLITE_API void sqlite3_mutex_leave(sqlite3_mutex*);

/*
** CAPI3REF: Mutex Methods Object {H17120} <S20130>
** EXPERIMENTAL
**
** An instance of this structure defines the low-level routines
** used to allocate and use mutexes.
**
** Usually, the default mutex implementations provided by SQLite are
** sufficient, however the user has the option of substituting a custom
** implementation for specialized deployments or systems for which SQLite
** does not provide a suitable implementation. In this case, the user
** creates and populates an instance of this structure to pass
** to sqlite3_config() along with the [SQLITE_CONFIG_MUTEX] option.
** Additionally, an instance of this structure can be used as an
** output variable when querying the system for the current mutex
** implementation, using the [SQLITE_CONFIG_GETMUTEX] option.
**
** The xMutexInit method defined by this structure is invoked as
** part of system initialization by the sqlite3_initialize() function.
** {H17001} The xMutexInit routine shall be called by SQLite once for each
** effective call to [sqlite3_initialize()].
**
** The xMutexEnd method defined by this structure is invoked as
** part of system shutdown by the sqlite3_shutdown() function. The
** implementation of this method is expected to release all outstanding
** resources obtained by the mutex methods implementation, especially
** those obtained by the xMutexInit method. {H17003} The xMutexEnd()
** interface shall be invoked once for each call to [sqlite3_shutdown()].
**
** The remaining seven methods defined by this structure (xMutexAlloc,
** xMutexFree, xMutexEnter, xMutexTry, xMutexLeave, xMutexHeld and
** xMutexNotheld) implement the following interfaces (respectively):
**
** <ul>
**   <li>  [sqlite3_mutex_alloc()] </li>
**   <li>  [sqlite3_mutex_free()] </li>
**   <li>  [sqlite3_mutex_enter()] </li>
**   <li>  [sqlite3_mutex_try()] </li>
**   <li>  [sqlite3_mutex_leave()] </li>
**   <li>  [sqlite3_mutex_held()] </li>
**   <li>  [sqlite3_mutex_notheld()] </li>
** </ul>
**
** The only difference is that the public sqlite3_XXX functions enumerated
** above silently ignore any invocations that pass a NULL pointer instead
** of a valid mutex handle. The implementations of the methods defined
** by this structure are not required to handle this case, the results
** of passing a NULL pointer instead of a valid mutex handle are undefined
** (i.e. it is acceptable to provide an implementation that segfaults if
** it is passed a NULL pointer).
**
** The xMutexInit() method must be threadsafe.  It must be harmless to
** invoke xMutexInit() mutiple times within the same process and without
** intervening calls to xMutexEnd().  Second and subsequent calls to
** xMutexInit() must be no-ops.
**
** xMutexInit() must not use SQLite memory allocation ([sqlite3_malloc()]
** and its associates).  Similarly, xMutexAlloc() must not use SQLite memory
** allocation for a static mutex.  However xMutexAlloc() may use SQLite
** memory allocation for a fast or recursive mutex.
**
** SQLite will invoke the xMutexEnd() method when [sqlite3_shutdown()] is
** called, but only if the prior call to xMutexInit returned SQLITE_OK.
** If xMutexInit fails in any way, it is expected to clean up after itself
** prior to returning.
*/
typedef struct sqlite3_mutex_methods sqlite3_mutex_methods;
struct sqlite3_mutex_methods {
  int (*xMutexInit)(void);
  int (*xMutexEnd)(void);
  sqlite3_mutex *(*xMutexAlloc)(int);
  void (*xMutexFree)(sqlite3_mutex *);
  void (*xMutexEnter)(sqlite3_mutex *);
  int (*xMutexTry)(sqlite3_mutex *);
  void (*xMutexLeave)(sqlite3_mutex *);
  int (*xMutexHeld)(sqlite3_mutex *);
  int (*xMutexNotheld)(sqlite3_mutex *);
};

/*
** CAPI3REF: Mutex Verification Routines {H17080} <S20130> <S30800>
**
** The sqlite3_mutex_held() and sqlite3_mutex_notheld() routines
** are intended for use inside assert() statements. {H17081} The SQLite core
** never uses these routines except inside an assert() and applications
** are advised to follow the lead of the core.  {H17082} The core only
** provides implementations for these routines when it is compiled
** with the SQLITE_DEBUG flag.  {A17087} External mutex implementations
** are only required to provide these routines if SQLITE_DEBUG is
** defined and if NDEBUG is not defined.
**
** {H17083} These routines should return true if the mutex in their argument
** is held or not held, respectively, by the calling thread.
**
** {X17084} The implementation is not required to provided versions of these
** routines that actually work. If the implementation does not provide working
** versions of these routines, it should at least provide stubs that always
** return true so that one does not get spurious assertion failures.
**
** {H17085} If the argument to sqlite3_mutex_held() is a NULL pointer then
** the routine should return 1.  {END} This seems counter-intuitive since
** clearly the mutex cannot be held if it does not exist.  But the
** the reason the mutex does not exist is because the build is not
** using mutexes.  And we do not want the assert() containing the
** call to sqlite3_mutex_held() to fail, so a non-zero return is
** the appropriate thing to do.  {H17086} The sqlite3_mutex_notheld()
** interface should also return 1 when given a NULL pointer.
*/
SQLITE_API int sqlite3_mutex_held(sqlite3_mutex*);
SQLITE_API int sqlite3_mutex_notheld(sqlite3_mutex*);

/*
** CAPI3REF: Mutex Types {H17001} <H17000>
**
** The [sqlite3_mutex_alloc()] interface takes a single argument
** which is one of these integer constants.
**
** The set of static mutexes may change from one SQLite release to the
** next.  Applications that override the built-in mutex logic must be
** prepared to accommodate additional static mutexes.
*/
#define SQLITE_MUTEX_FAST             0
#define SQLITE_MUTEX_RECURSIVE        1
#define SQLITE_MUTEX_STATIC_MASTER    2
#define SQLITE_MUTEX_STATIC_MEM       3  /* sqlite3_malloc() */
#define SQLITE_MUTEX_STATIC_MEM2      4  /* NOT USED */
#define SQLITE_MUTEX_STATIC_OPEN      4  /* sqlite3BtreeOpen() */
#define SQLITE_MUTEX_STATIC_PRNG      5  /* sqlite3_random() */
#define SQLITE_MUTEX_STATIC_LRU       6  /* lru page list */
#define SQLITE_MUTEX_STATIC_LRU2      7  /* lru page list */

/*
** CAPI3REF: Retrieve the mutex for a database connection {H17002} <H17000>
**
** This interface returns a pointer the [sqlite3_mutex] object that 
** serializes access to the [database connection] given in the argument
** when the [threading mode] is Serialized.
** If the [threading mode] is Single-thread or Multi-thread then this
** routine returns a NULL pointer.
*/
SQLITE_API sqlite3_mutex *sqlite3_db_mutex(sqlite3*);

/*
** CAPI3REF: Low-Level Control Of Database Files {H11300} <S30800>
**
** {H11301} The [sqlite3_file_control()] interface makes a direct call to the
** xFileControl method for the [sqlite3_io_methods] object associated
** with a particular database identified by the second argument. {H11302} The
** name of the database is the name assigned to the database by the
** <a href="lang_attach.html">ATTACH</a> SQL command that opened the
** database. {H11303} To control the main database file, use the name "main"
** or a NULL pointer. {H11304} The third and fourth parameters to this routine
** are passed directly through to the second and third parameters of
** the xFileControl method.  {H11305} The return value of the xFileControl
** method becomes the return value of this routine.
**
** {H11306} If the second parameter (zDbName) does not match the name of any
** open database file, then SQLITE_ERROR is returned. {H11307} This error
** code is not remembered and will not be recalled by [sqlite3_errcode()]
** or [sqlite3_errmsg()]. {A11308} The underlying xFileControl method might
** also return SQLITE_ERROR.  {A11309} There is no way to distinguish between
** an incorrect zDbName and an SQLITE_ERROR return from the underlying
** xFileControl method. {END}
**
** See also: [SQLITE_FCNTL_LOCKSTATE]
*/
SQLITE_API int sqlite3_file_control(sqlite3*, const char *zDbName, int op, void*);

/*
** CAPI3REF: Testing Interface {H11400} <S30800>
**
** The sqlite3_test_control() interface is used to read out internal
** state of SQLite and to inject faults into SQLite for testing
** purposes.  The first parameter is an operation code that determines
** the number, meaning, and operation of all subsequent parameters.
**
** This interface is not for use by applications.  It exists solely
** for verifying the correct operation of the SQLite library.  Depending
** on how the SQLite library is compiled, this interface might not exist.
**
** The details of the operation codes, their meanings, the parameters
** they take, and what they do are all subject to change without notice.
** Unlike most of the SQLite API, this function is not guaranteed to
** operate consistently from one release to the next.
*/
SQLITE_API int sqlite3_test_control(int op, ...);

/*
** CAPI3REF: Testing Interface Operation Codes {H11410} <H11400>
**
** These constants are the valid operation code parameters used
** as the first argument to [sqlite3_test_control()].
**
** These parameters and their meanings are subject to change
** without notice.  These values are for testing purposes only.
** Applications should not use any of these parameters or the
** [sqlite3_test_control()] interface.
*/
#define SQLITE_TESTCTRL_PRNG_SAVE                5
#define SQLITE_TESTCTRL_PRNG_RESTORE             6
#define SQLITE_TESTCTRL_PRNG_RESET               7
#define SQLITE_TESTCTRL_BITVEC_TEST              8
#define SQLITE_TESTCTRL_FAULT_INSTALL            9
#define SQLITE_TESTCTRL_BENIGN_MALLOC_HOOKS     10
#define SQLITE_TESTCTRL_PENDING_BYTE            11
#define SQLITE_TESTCTRL_ASSERT                  12
#define SQLITE_TESTCTRL_ALWAYS                  13
#define SQLITE_TESTCTRL_RESERVE                 14

/*
** CAPI3REF: SQLite Runtime Status {H17200} <S60200>
** EXPERIMENTAL
**
** This interface is used to retrieve runtime status information
** about the preformance of SQLite, and optionally to reset various
** highwater marks.  The first argument is an integer code for
** the specific parameter to measure.  Recognized integer codes
** are of the form [SQLITE_STATUS_MEMORY_USED | SQLITE_STATUS_...].
** The current value of the parameter is returned into *pCurrent.
** The highest recorded value is returned in *pHighwater.  If the
** resetFlag is true, then the highest record value is reset after
** *pHighwater is written. Some parameters do not record the highest
** value.  For those parameters
** nothing is written into *pHighwater and the resetFlag is ignored.
** Other parameters record only the highwater mark and not the current
** value.  For these latter parameters nothing is written into *pCurrent.
**
** This routine returns SQLITE_OK on success and a non-zero
** [error code] on failure.
**
** This routine is threadsafe but is not atomic.  This routine can be
** called while other threads are running the same or different SQLite
** interfaces.  However the values returned in *pCurrent and
** *pHighwater reflect the status of SQLite at different points in time
** and it is possible that another thread might change the parameter
** in between the times when *pCurrent and *pHighwater are written.
**
** See also: [sqlite3_db_status()]
*/
SQLITE_API SQLITE_EXPERIMENTAL int sqlite3_status(int op, int *pCurrent, int *pHighwater, int resetFlag);


/*
** CAPI3REF: Status Parameters {H17250} <H17200>
** EXPERIMENTAL
**
** These integer constants designate various run-time status parameters
** that can be returned by [sqlite3_status()].
**
** <dl>
** <dt>SQLITE_STATUS_MEMORY_USED</dt>
** <dd>This parameter is the current amount of memory checked out
** using [sqlite3_malloc()], either directly or indirectly.  The
** figure includes calls made to [sqlite3_malloc()] by the application
** and internal memory usage by the SQLite library.  Scratch memory
** controlled by [SQLITE_CONFIG_SCRATCH] and auxiliary page-cache
** memory controlled by [SQLITE_CONFIG_PAGECACHE] is not included in
** this parameter.  The amount returned is the sum of the allocation
** sizes as reported by the xSize method in [sqlite3_mem_methods].</dd>
**
** <dt>SQLITE_STATUS_MALLOC_SIZE</dt>
** <dd>This parameter records the largest memory allocation request
** handed to [sqlite3_malloc()] or [sqlite3_realloc()] (or their
** internal equivalents).  Only the value returned in the
** *pHighwater parameter to [sqlite3_status()] is of interest.  
** The value written into the *pCurrent parameter is undefined.</dd>
**
** <dt>SQLITE_STATUS_PAGECACHE_USED</dt>
** <dd>This parameter returns the number of pages used out of the
** [pagecache memory allocator] that was configured using 
** [SQLITE_CONFIG_PAGECACHE].  The
** value returned is in pages, not in bytes.</dd>
**
** <dt>SQLITE_STATUS_PAGECACHE_OVERFLOW</dt>
** <dd>This parameter returns the number of bytes of page cache
** allocation which could not be statisfied by the [SQLITE_CONFIG_PAGECACHE]
** buffer and where forced to overflow to [sqlite3_malloc()].  The
** returned value includes allocations that overflowed because they
** where too large (they were larger than the "sz" parameter to
** [SQLITE_CONFIG_PAGECACHE]) and allocations that overflowed because
** no space was left in the page cache.</dd>
**
** <dt>SQLITE_STATUS_PAGECACHE_SIZE</dt>
** <dd>This parameter records the largest memory allocation request
** handed to [pagecache memory allocator].  Only the value returned in the
** *pHighwater parameter to [sqlite3_status()] is of interest.  
** The value written into the *pCurrent parameter is undefined.</dd>
**
** <dt>SQLITE_STATUS_SCRATCH_USED</dt>
** <dd>This parameter returns the number of allocations used out of the
** [scratch memory allocator] configured using
** [SQLITE_CONFIG_SCRATCH].  The value returned is in allocations, not
** in bytes.  Since a single thread may only have one scratch allocation
** outstanding at time, this parameter also reports the number of threads
** using scratch memory at the same time.</dd>
**
** <dt>SQLITE_STATUS_SCRATCH_OVERFLOW</dt>
** <dd>This parameter returns the number of bytes of scratch memory
** allocation which could not be statisfied by the [SQLITE_CONFIG_SCRATCH]
** buffer and where forced to overflow to [sqlite3_malloc()].  The values
** returned include overflows because the requested allocation was too
** larger (that is, because the requested allocation was larger than the
** "sz" parameter to [SQLITE_CONFIG_SCRATCH]) and because no scratch buffer
** slots were available.
** </dd>
**
** <dt>SQLITE_STATUS_SCRATCH_SIZE</dt>
** <dd>This parameter records the largest memory allocation request
** handed to [scratch memory allocator].  Only the value returned in the
** *pHighwater parameter to [sqlite3_status()] is of interest.  
** The value written into the *pCurrent parameter is undefined.</dd>
**
** <dt>SQLITE_STATUS_PARSER_STACK</dt>
** <dd>This parameter records the deepest parser stack.  It is only
** meaningful if SQLite is compiled with [YYTRACKMAXSTACKDEPTH].</dd>
** </dl>
**
** New status parameters may be added from time to time.
*/
#define SQLITE_STATUS_MEMORY_USED          0
#define SQLITE_STATUS_PAGECACHE_USED       1
#define SQLITE_STATUS_PAGECACHE_OVERFLOW   2
#define SQLITE_STATUS_SCRATCH_USED         3
#define SQLITE_STATUS_SCRATCH_OVERFLOW     4
#define SQLITE_STATUS_MALLOC_SIZE          5
#define SQLITE_STATUS_PARSER_STACK         6
#define SQLITE_STATUS_PAGECACHE_SIZE       7
#define SQLITE_STATUS_SCRATCH_SIZE         8

/*
** CAPI3REF: Database Connection Status {H17500} <S60200>
** EXPERIMENTAL
**
** This interface is used to retrieve runtime status information 
** about a single [database connection].  The first argument is the
** database connection object to be interrogated.  The second argument
** is the parameter to interrogate.  Currently, the only allowed value
** for the second parameter is [SQLITE_DBSTATUS_LOOKASIDE_USED].
** Additional options will likely appear in future releases of SQLite.
**
** The current value of the requested parameter is written into *pCur
** and the highest instantaneous value is written into *pHiwtr.  If
** the resetFlg is true, then the highest instantaneous value is
** reset back down to the current value.
**
** See also: [sqlite3_status()] and [sqlite3_stmt_status()].
*/
SQLITE_API SQLITE_EXPERIMENTAL int sqlite3_db_status(sqlite3*, int op, int *pCur, int *pHiwtr, int resetFlg);

/*
** CAPI3REF: Status Parameters for database connections {H17520} <H17500>
** EXPERIMENTAL
**
** These constants are the available integer "verbs" that can be passed as
** the second argument to the [sqlite3_db_status()] interface.
**
** New verbs may be added in future releases of SQLite. Existing verbs
** might be discontinued. Applications should check the return code from
** [sqlite3_db_status()] to make sure that the call worked.
** The [sqlite3_db_status()] interface will return a non-zero error code
** if a discontinued or unsupported verb is invoked.
**
** <dl>
** <dt>SQLITE_DBSTATUS_LOOKASIDE_USED</dt>
** <dd>This parameter returns the number of lookaside memory slots currently
** checked out.</dd>
** </dl>
*/
#define SQLITE_DBSTATUS_LOOKASIDE_USED     0


/*
** CAPI3REF: Prepared Statement Status {H17550} <S60200>
** EXPERIMENTAL
**
** Each prepared statement maintains various
** [SQLITE_STMTSTATUS_SORT | counters] that measure the number
** of times it has performed specific operations.  These counters can
** be used to monitor the performance characteristics of the prepared
** statements.  For example, if the number of table steps greatly exceeds
** the number of table searches or result rows, that would tend to indicate
** that the prepared statement is using a full table scan rather than
** an index.  
**
** This interface is used to retrieve and reset counter values from
** a [prepared statement].  The first argument is the prepared statement
** object to be interrogated.  The second argument
** is an integer code for a specific [SQLITE_STMTSTATUS_SORT | counter]
** to be interrogated. 
** The current value of the requested counter is returned.
** If the resetFlg is true, then the counter is reset to zero after this
** interface call returns.
**
** See also: [sqlite3_status()] and [sqlite3_db_status()].
*/
SQLITE_API SQLITE_EXPERIMENTAL int sqlite3_stmt_status(sqlite3_stmt*, int op,int resetFlg);

/*
** CAPI3REF: Status Parameters for prepared statements {H17570} <H17550>
** EXPERIMENTAL
**
** These preprocessor macros define integer codes that name counter
** values associated with the [sqlite3_stmt_status()] interface.
** The meanings of the various counters are as follows:
**
** <dl>
** <dt>SQLITE_STMTSTATUS_FULLSCAN_STEP</dt>
** <dd>This is the number of times that SQLite has stepped forward in
** a table as part of a full table scan.  Large numbers for this counter
** may indicate opportunities for performance improvement through 
** careful use of indices.</dd>
**
** <dt>SQLITE_STMTSTATUS_SORT</dt>
** <dd>This is the number of sort operations that have occurred.
** A non-zero value in this counter may indicate an opportunity to
** improvement performance through careful use of indices.</dd>
**
** </dl>
*/
#define SQLITE_STMTSTATUS_FULLSCAN_STEP     1
#define SQLITE_STMTSTATUS_SORT              2

/*
** CAPI3REF: Custom Page Cache Object
** EXPERIMENTAL
**
** The sqlite3_pcache type is opaque.  It is implemented by
** the pluggable module.  The SQLite core has no knowledge of
** its size or internal structure and never deals with the
** sqlite3_pcache object except by holding and passing pointers
** to the object.
**
** See [sqlite3_pcache_methods] for additional information.
*/
typedef struct sqlite3_pcache sqlite3_pcache;

/*
** CAPI3REF: Application Defined Page Cache.
** KEYWORDS: {page cache}
** EXPERIMENTAL
**
** The [sqlite3_config]([SQLITE_CONFIG_PCACHE], ...) interface can
** register an alternative page cache implementation by passing in an 
** instance of the sqlite3_pcache_methods structure. The majority of the 
** heap memory used by SQLite is used by the page cache to cache data read 
** from, or ready to be written to, the database file. By implementing a 
** custom page cache using this API, an application can control more 
** precisely the amount of memory consumed by SQLite, the way in which 
** that memory is allocated and released, and the policies used to 
** determine exactly which parts of a database file are cached and for 
** how long.
**
** The contents of the sqlite3_pcache_methods structure are copied to an
** internal buffer by SQLite within the call to [sqlite3_config].  Hence
** the application may discard the parameter after the call to
** [sqlite3_config()] returns.
**
** The xInit() method is called once for each call to [sqlite3_initialize()]
** (usually only once during the lifetime of the process). It is passed
** a copy of the sqlite3_pcache_methods.pArg value. It can be used to set
** up global structures and mutexes required by the custom page cache 
** implementation. 
**
** The xShutdown() method is called from within [sqlite3_shutdown()], 
** if the application invokes this API. It can be used to clean up 
** any outstanding resources before process shutdown, if required.
**
** SQLite holds a [SQLITE_MUTEX_RECURSIVE] mutex when it invokes
** the xInit method, so the xInit method need not be threadsafe.  The
** xShutdown method is only called from [sqlite3_shutdown()] so it does
** not need to be threadsafe either.  All other methods must be threadsafe
** in multithreaded applications.
**
** SQLite will never invoke xInit() more than once without an intervening
** call to xShutdown().
**
** The xCreate() method is used to construct a new cache instance.  SQLite
** will typically create one cache instance for each open database file,
** though this is not guaranteed. The
** first parameter, szPage, is the size in bytes of the pages that must
** be allocated by the cache.  szPage will not be a power of two.  szPage
** will the page size of the database file that is to be cached plus an
** increment (here called "R") of about 100 or 200.  SQLite will use the
** extra R bytes on each page to store metadata about the underlying
** database page on disk.  The value of R depends
** on the SQLite version, the target platform, and how SQLite was compiled.
** R is constant for a particular build of SQLite.  The second argument to
** xCreate(), bPurgeable, is true if the cache being created will
** be used to cache database pages of a file stored on disk, or
** false if it is used for an in-memory database. The cache implementation
** does not have to do anything special based with the value of bPurgeable;
** it is purely advisory.  On a cache where bPurgeable is false, SQLite will
** never invoke xUnpin() except to deliberately delete a page.
** In other words, a cache created with bPurgeable set to false will
** never contain any unpinned pages.
**
** The xCachesize() method may be called at any time by SQLite to set the
** suggested maximum cache-size (number of pages stored by) the cache
** instance passed as the first argument. This is the value configured using
** the SQLite "[PRAGMA cache_size]" command. As with the bPurgeable parameter,
** the implementation is not required to do anything with this
** value; it is advisory only.
**
** The xPagecount() method should return the number of pages currently
** stored in the cache.
** 
** The xFetch() method is used to fetch a page and return a pointer to it. 
** A 'page', in this context, is a buffer of szPage bytes aligned at an
** 8-byte boundary. The page to be fetched is determined by the key. The
** mimimum key value is 1. After it has been retrieved using xFetch, the page 
** is considered to be "pinned".
**
** If the requested page is already in the page cache, then the page cache
** implementation must return a pointer to the page buffer with its content
** intact.  If the requested page is not already in the cache, then the
** behavior of the cache implementation is determined by the value of the
** createFlag parameter passed to xFetch, according to the following table:
**
** <table border=1 width=85% align=center>
** <tr><th> createFlag <th> Behaviour when page is not already in cache
** <tr><td> 0 <td> Do not allocate a new page.  Return NULL.
** <tr><td> 1 <td> Allocate a new page if it easy and convenient to do so.
**                 Otherwise return NULL.
** <tr><td> 2 <td> Make every effort to allocate a new page.  Only return
**                 NULL if allocating a new page is effectively impossible.
** </table>
**
** SQLite will normally invoke xFetch() with a createFlag of 0 or 1.  If
** a call to xFetch() with createFlag==1 returns NULL, then SQLite will
** attempt to unpin one or more cache pages by spilling the content of
** pinned pages to disk and synching the operating system disk cache. After
** attempting to unpin pages, the xFetch() method will be invoked again with
** a createFlag of 2.
**
** xUnpin() is called by SQLite with a pointer to a currently pinned page
** as its second argument. If the third parameter, discard, is non-zero,
** then the page should be evicted from the cache. In this case SQLite 
** assumes that the next time the page is retrieved from the cache using
** the xFetch() method, it will be zeroed. If the discard parameter is
** zero, then the page is considered to be unpinned. The cache implementation
** may choose to evict unpinned pages at any time.
**
** The cache is not required to perform any reference counting. A single 
** call to xUnpin() unpins the page regardless of the number of prior calls 
** to xFetch().
**
** The xRekey() method is used to change the key value associated with the
** page passed as the second argument from oldKey to newKey. If the cache
** previously contains an entry associated with newKey, it should be
** discarded. Any prior cache entry associated with newKey is guaranteed not
** to be pinned.
**
** When SQLite calls the xTruncate() method, the cache must discard all
** existing cache entries with page numbers (keys) greater than or equal
** to the value of the iLimit parameter passed to xTruncate(). If any
** of these pages are pinned, they are implicitly unpinned, meaning that
** they can be safely discarded.
**
** The xDestroy() method is used to delete a cache allocated by xCreate().
** All resources associated with the specified cache should be freed. After
** calling the xDestroy() method, SQLite considers the [sqlite3_pcache*]
** handle invalid, and will not use it with any other sqlite3_pcache_methods
** functions.
*/
typedef struct sqlite3_pcache_methods sqlite3_pcache_methods;
struct sqlite3_pcache_methods {
  void *pArg;
  int (*xInit)(void*);
  void (*xShutdown)(void*);
  sqlite3_pcache *(*xCreate)(int szPage, int bPurgeable);
  void (*xCachesize)(sqlite3_pcache*, int nCachesize);
  int (*xPagecount)(sqlite3_pcache*);
  void *(*xFetch)(sqlite3_pcache*, unsigned key, int createFlag);
  void (*xUnpin)(sqlite3_pcache*, void*, int discard);
  void (*xRekey)(sqlite3_pcache*, void*, unsigned oldKey, unsigned newKey);
  void (*xTruncate)(sqlite3_pcache*, unsigned iLimit);
  void (*xDestroy)(sqlite3_pcache*);
};

/*
** CAPI3REF: Online Backup Object
** EXPERIMENTAL
**
** The sqlite3_backup object records state information about an ongoing
** online backup operation.  The sqlite3_backup object is created by
** a call to [sqlite3_backup_init()] and is destroyed by a call to
** [sqlite3_backup_finish()].
**
** See Also: [Using the SQLite Online Backup API]
*/
typedef struct sqlite3_backup sqlite3_backup;

/*
** CAPI3REF: Online Backup API.
** EXPERIMENTAL
**
** This API is used to overwrite the contents of one database with that
** of another. It is useful either for creating backups of databases or
** for copying in-memory databases to or from persistent files. 
**
** See Also: [Using the SQLite Online Backup API]
**
** Exclusive access is required to the destination database for the 
** duration of the operation. However the source database is only
** read-locked while it is actually being read, it is not locked
** continuously for the entire operation. Thus, the backup may be
** performed on a live database without preventing other users from
** writing to the database for an extended period of time.
** 
** To perform a backup operation: 
**   <ol>
**     <li><b>sqlite3_backup_init()</b> is called once to initialize the
**         backup, 
**     <li><b>sqlite3_backup_step()</b> is called one or more times to transfer 
**         the data between the two databases, and finally
**     <li><b>sqlite3_backup_finish()</b> is called to release all resources 
**         associated with the backup operation. 
**   </ol>
** There should be exactly one call to sqlite3_backup_finish() for each
** successful call to sqlite3_backup_init().
**
** <b>sqlite3_backup_init()</b>
**
** The first two arguments passed to [sqlite3_backup_init()] are the database
** handle associated with the destination database and the database name 
** used to attach the destination database to the handle. The database name
** is "main" for the main database, "temp" for the temporary database, or
** the name specified as part of the [ATTACH] statement if the destination is
** an attached database. The third and fourth arguments passed to 
** sqlite3_backup_init() identify the [database connection]
** and database name used
** to access the source database. The values passed for the source and 
** destination [database connection] parameters must not be the same.
**
** If an error occurs within sqlite3_backup_init(), then NULL is returned
** and an error code and error message written into the [database connection] 
** passed as the first argument. They may be retrieved using the
** [sqlite3_errcode()], [sqlite3_errmsg()], and [sqlite3_errmsg16()] functions.
** Otherwise, if successful, a pointer to an [sqlite3_backup] object is
** returned. This pointer may be used with the sqlite3_backup_step() and
** sqlite3_backup_finish() functions to perform the specified backup 
** operation.
**
** <b>sqlite3_backup_step()</b>
**
** Function [sqlite3_backup_step()] is used to copy up to nPage pages between 
** the source and destination databases, where nPage is the value of the 
** second parameter passed to sqlite3_backup_step(). If nPage is a negative
** value, all remaining source pages are copied. If the required pages are 
** succesfully copied, but there are still more pages to copy before the 
** backup is complete, it returns [SQLITE_OK]. If no error occured and there 
** are no more pages to copy, then [SQLITE_DONE] is returned. If an error 
** occurs, then an SQLite error code is returned. As well as [SQLITE_OK] and
** [SQLITE_DONE], a call to sqlite3_backup_step() may return [SQLITE_READONLY],
** [SQLITE_NOMEM], [SQLITE_BUSY], [SQLITE_LOCKED], or an
** [SQLITE_IOERR_ACCESS | SQLITE_IOERR_XXX] extended error code.
**
** As well as the case where the destination database file was opened for
** read-only access, sqlite3_backup_step() may return [SQLITE_READONLY] if
** the destination is an in-memory database with a different page size
** from the source database.
**
** If sqlite3_backup_step() cannot obtain a required file-system lock, then
** the [sqlite3_busy_handler | busy-handler function]
** is invoked (if one is specified). If the 
** busy-handler returns non-zero before the lock is available, then 
** [SQLITE_BUSY] is returned to the caller. In this case the call to
** sqlite3_backup_step() can be retried later. If the source
** [database connection]
** is being used to write to the source database when sqlite3_backup_step()
** is called, then [SQLITE_LOCKED] is returned immediately. Again, in this
** case the call to sqlite3_backup_step() can be retried later on. If
** [SQLITE_IOERR_ACCESS | SQLITE_IOERR_XXX], [SQLITE_NOMEM], or
** [SQLITE_READONLY] is returned, then 
** there is no point in retrying the call to sqlite3_backup_step(). These 
** errors are considered fatal. At this point the application must accept 
** that the backup operation has failed and pass the backup operation handle 
** to the sqlite3_backup_finish() to release associated resources.
**
** Following the first call to sqlite3_backup_step(), an exclusive lock is
** obtained on the destination file. It is not released until either 
** sqlite3_backup_finish() is called or the backup operation is complete 
** and sqlite3_backup_step() returns [SQLITE_DONE]. Additionally, each time 
** a call to sqlite3_backup_step() is made a [shared lock] is obtained on
** the source database file. This lock is released before the
** sqlite3_backup_step() call returns. Because the source database is not
** locked between calls to sqlite3_backup_step(), it may be modified mid-way
** through the backup procedure. If the source database is modified by an
** external process or via a database connection other than the one being
** used by the backup operation, then the backup will be transparently
** restarted by the next call to sqlite3_backup_step(). If the source 
** database is modified by the using the same database connection as is used
** by the backup operation, then the backup database is transparently 
** updated at the same time.
**
** <b>sqlite3_backup_finish()</b>
**
** Once sqlite3_backup_step() has returned [SQLITE_DONE], or when the 
** application wishes to abandon the backup operation, the [sqlite3_backup]
** object should be passed to sqlite3_backup_finish(). This releases all
** resources associated with the backup operation. If sqlite3_backup_step()
** has not yet returned [SQLITE_DONE], then any active write-transaction on the
** destination database is rolled back. The [sqlite3_backup] object is invalid
** and may not be used following a call to sqlite3_backup_finish().
**
** The value returned by sqlite3_backup_finish is [SQLITE_OK] if no error
** occurred, regardless or whether or not sqlite3_backup_step() was called
** a sufficient number of times to complete the backup operation. Or, if
** an out-of-memory condition or IO error occured during a call to
** sqlite3_backup_step() then [SQLITE_NOMEM] or an
** [SQLITE_IOERR_ACCESS | SQLITE_IOERR_XXX] error code
** is returned. In this case the error code and an error message are
** written to the destination [database connection].
**
** A return of [SQLITE_BUSY] or [SQLITE_LOCKED] from sqlite3_backup_step() is
** not a permanent error and does not affect the return value of
** sqlite3_backup_finish().
**
** <b>sqlite3_backup_remaining(), sqlite3_backup_pagecount()</b>
**
** Each call to sqlite3_backup_step() sets two values stored internally
** by an [sqlite3_backup] object. The number of pages still to be backed
** up, which may be queried by sqlite3_backup_remaining(), and the total
** number of pages in the source database file, which may be queried by
** sqlite3_backup_pagecount().
**
** The values returned by these functions are only updated by
** sqlite3_backup_step(). If the source database is modified during a backup
** operation, then the values are not updated to account for any extra
** pages that need to be updated or the size of the source database file
** changing.
**
** <b>Concurrent Usage of Database Handles</b>
**
** The source [database connection] may be used by the application for other
** purposes while a backup operation is underway or being initialized.
** If SQLite is compiled and configured to support threadsafe database
** connections, then the source database connection may be used concurrently
** from within other threads.
**
** However, the application must guarantee that the destination database
** connection handle is not passed to any other API (by any thread) after 
** sqlite3_backup_init() is called and before the corresponding call to
** sqlite3_backup_finish(). Unfortunately SQLite does not currently check
** for this, if the application does use the destination [database connection]
** for some other purpose during a backup operation, things may appear to
** work correctly but in fact be subtly malfunctioning.  Use of the
** destination database connection while a backup is in progress might
** also cause a mutex deadlock.
**
** Furthermore, if running in [shared cache mode], the application must
** guarantee that the shared cache used by the destination database
** is not accessed while the backup is running. In practice this means
** that the application must guarantee that the file-system file being 
** backed up to is not accessed by any connection within the process,
** not just the specific connection that was passed to sqlite3_backup_init().
**
** The [sqlite3_backup] object itself is partially threadsafe. Multiple 
** threads may safely make multiple concurrent calls to sqlite3_backup_step().
** However, the sqlite3_backup_remaining() and sqlite3_backup_pagecount()
** APIs are not strictly speaking threadsafe. If they are invoked at the
** same time as another thread is invoking sqlite3_backup_step() it is
** possible that they return invalid values.
*/
SQLITE_API sqlite3_backup *sqlite3_backup_init(
  sqlite3 *pDest,                        /* Destination database handle */
  const char *zDestName,                 /* Destination database name */
  sqlite3 *pSource,                      /* Source database handle */
  const char *zSourceName                /* Source database name */
);
SQLITE_API int sqlite3_backup_step(sqlite3_backup *p, int nPage);
SQLITE_API int sqlite3_backup_finish(sqlite3_backup *p);
SQLITE_API int sqlite3_backup_remaining(sqlite3_backup *p);
SQLITE_API int sqlite3_backup_pagecount(sqlite3_backup *p);

/*
** CAPI3REF: Unlock Notification
** EXPERIMENTAL
**
** When running in shared-cache mode, a database operation may fail with
** an [SQLITE_LOCKED] error if the required locks on the shared-cache or
** individual tables within the shared-cache cannot be obtained. See
** [SQLite Shared-Cache Mode] for a description of shared-cache locking. 
** This API may be used to register a callback that SQLite will invoke 
** when the connection currently holding the required lock relinquishes it.
** This API is only available if the library was compiled with the
** [SQLITE_ENABLE_UNLOCK_NOTIFY] C-preprocessor symbol defined.
**
** See Also: [Using the SQLite Unlock Notification Feature].
**
** Shared-cache locks are released when a database connection concludes
** its current transaction, either by committing it or rolling it back. 
**
** When a connection (known as the blocked connection) fails to obtain a
** shared-cache lock and SQLITE_LOCKED is returned to the caller, the
** identity of the database connection (the blocking connection) that
** has locked the required resource is stored internally. After an 
** application receives an SQLITE_LOCKED error, it may call the
** sqlite3_unlock_notify() method with the blocked connection handle as 
** the first argument to register for a callback that will be invoked
** when the blocking connections current transaction is concluded. The
** callback is invoked from within the [sqlite3_step] or [sqlite3_close]
** call that concludes the blocking connections transaction.
**
** If sqlite3_unlock_notify() is called in a multi-threaded application,
** there is a chance that the blocking connection will have already
** concluded its transaction by the time sqlite3_unlock_notify() is invoked.
** If this happens, then the specified callback is invoked immediately,
** from within the call to sqlite3_unlock_notify().
**
** If the blocked connection is attempting to obtain a write-lock on a
** shared-cache table, and more than one other connection currently holds
** a read-lock on the same table, then SQLite arbitrarily selects one of 
** the other connections to use as the blocking connection.
**
** There may be at most one unlock-notify callback registered by a 
** blocked connection. If sqlite3_unlock_notify() is called when the
** blocked connection already has a registered unlock-notify callback,
** then the new callback replaces the old. If sqlite3_unlock_notify() is
** called with a NULL pointer as its second argument, then any existing
** unlock-notify callback is cancelled. The blocked connections 
** unlock-notify callback may also be canceled by closing the blocked
** connection using [sqlite3_close()].
**
** The unlock-notify callback is not reentrant. If an application invokes
** any sqlite3_xxx API functions from within an unlock-notify callback, a
** crash or deadlock may be the result.
**
** Unless deadlock is detected (see below), sqlite3_unlock_notify() always
** returns SQLITE_OK.
**
** <b>Callback Invocation Details</b>
**
** When an unlock-notify callback is registered, the application provides a 
** single void* pointer that is passed to the callback when it is invoked.
** However, the signature of the callback function allows SQLite to pass
** it an array of void* context pointers. The first argument passed to
** an unlock-notify callback is a pointer to an array of void* pointers,
** and the second is the number of entries in the array.
**
** When a blocking connections transaction is concluded, there may be
** more than one blocked connection that has registered for an unlock-notify
** callback. If two or more such blocked connections have specified the
** same callback function, then instead of invoking the callback function
** multiple times, it is invoked once with the set of void* context pointers
** specified by the blocked connections bundled together into an array.
** This gives the application an opportunity to prioritize any actions 
** related to the set of unblocked database connections.
**
** <b>Deadlock Detection</b>
**
** Assuming that after registering for an unlock-notify callback a 
** database waits for the callback to be issued before taking any further
** action (a reasonable assumption), then using this API may cause the
** application to deadlock. For example, if connection X is waiting for
** connection Y's transaction to be concluded, and similarly connection
** Y is waiting on connection X's transaction, then neither connection
** will proceed and the system may remain deadlocked indefinitely.
**
** To avoid this scenario, the sqlite3_unlock_notify() performs deadlock
** detection. If a given call to sqlite3_unlock_notify() would put the
** system in a deadlocked state, then SQLITE_LOCKED is returned and no
** unlock-notify callback is registered. The system is said to be in
** a deadlocked state if connection A has registered for an unlock-notify
** callback on the conclusion of connection B's transaction, and connection
** B has itself registered for an unlock-notify callback when connection
** A's transaction is concluded. Indirect deadlock is also detected, so
** the system is also considered to be deadlocked if connection B has
** registered for an unlock-notify callback on the conclusion of connection
** C's transaction, where connection C is waiting on connection A. Any
** number of levels of indirection are allowed.
**
** <b>The "DROP TABLE" Exception</b>
**
** When a call to [sqlite3_step()] returns SQLITE_LOCKED, it is almost 
** always appropriate to call sqlite3_unlock_notify(). There is however,
** one exception. When executing a "DROP TABLE" or "DROP INDEX" statement,
** SQLite checks if there are any currently executing SELECT statements
** that belong to the same connection. If there are, SQLITE_LOCKED is
** returned. In this case there is no "blocking connection", so invoking
** sqlite3_unlock_notify() results in the unlock-notify callback being
** invoked immediately. If the application then re-attempts the "DROP TABLE"
** or "DROP INDEX" query, an infinite loop might be the result.
**
** One way around this problem is to check the extended error code returned
** by an sqlite3_step() call. If there is a blocking connection, then the
** extended error code is set to SQLITE_LOCKED_SHAREDCACHE. Otherwise, in
** the special "DROP TABLE/INDEX" case, the extended error code is just 
** SQLITE_LOCKED.
*/
SQLITE_API int sqlite3_unlock_notify(
  sqlite3 *pBlocked,                          /* Waiting connection */
  void (*xNotify)(void **apArg, int nArg),    /* Callback function to invoke */
  void *pNotifyArg                            /* Argument to pass to xNotify */
);


/*
** CAPI3REF: String Comparison
** EXPERIMENTAL
**
** The [sqlite3_strnicmp()] API allows applications and extensions to
** compare the contents of two buffers containing UTF-8 strings in a
** case-indendent fashion, using the same definition of case independence 
** that SQLite uses internally when comparing identifiers.
*/
SQLITE_API int sqlite3_strnicmp(const char *, const char *, int);

/*
** Undo the hack that converts floating point types to integer for
** builds on processors without floating point support.
*/
#ifdef SQLITE_OMIT_FLOATING_POINT
# undef double
#endif

#ifdef __cplusplus
}  /* End of the 'extern "C"' block */
#endif
#endif

