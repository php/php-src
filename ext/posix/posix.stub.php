<?php

/** @generate-class-entries */

/**
 * @var int
 * @cvalue F_OK
 */
const POSIX_F_OK = UNKNOWN;
/**
 * @var int
 * @cvalue X_OK
 */
const POSIX_X_OK = UNKNOWN;
/**
 * @var int
 * @cvalue W_OK
 */
const POSIX_W_OK = UNKNOWN;
/**
 * @var int
 * @cvalue R_OK
 */
const POSIX_R_OK = UNKNOWN;

#ifdef S_IFREG
/**
 * @var int
 * @cvalue S_IFREG
 */
const POSIX_S_IFREG = UNKNOWN;
#endif
#ifdef S_IFCHR
/**
 * @var int
 * @cvalue S_IFCHR
 */
const POSIX_S_IFCHR = UNKNOWN;
#endif
#ifdef S_IFBLK
/**
 * @var int
 * @cvalue S_IFBLK
 */
const POSIX_S_IFBLK = UNKNOWN;
#endif
#ifdef S_IFIFO
/**
 * @var int
 * @cvalue S_IFIFO
 */
const POSIX_S_IFIFO = UNKNOWN;
#endif
#ifdef S_IFSOCK
/**
 * @var int
 * @cvalue S_IFSOCK
 */
const POSIX_S_IFSOCK = UNKNOWN;
#endif
#ifdef RLIMIT_AS
/**
 * @var int
 * @cvalue RLIMIT_AS
 */
const POSIX_RLIMIT_AS = UNKNOWN;
#endif
#ifdef RLIMIT_CORE
/**
 * @var int
 * @cvalue RLIMIT_CORE
 */
const POSIX_RLIMIT_CORE = UNKNOWN;
#endif
#ifdef RLIMIT_CPU
/**
 * @var int
 * @cvalue RLIMIT_CPU
 */
const POSIX_RLIMIT_CPU = UNKNOWN;
#endif
#ifdef RLIMIT_DATA
/**
 * @var int
 * @cvalue RLIMIT_DATA
 */
const POSIX_RLIMIT_DATA = UNKNOWN;
#endif
#ifdef RLIMIT_FSIZE
/**
 * @var int
 * @cvalue RLIMIT_FSIZE
 */
const POSIX_RLIMIT_FSIZE = UNKNOWN;
#endif
#ifdef RLIMIT_LOCKS
/**
 * @var int
 * @cvalue RLIMIT_LOCKS
 */
const POSIX_RLIMIT_LOCKS = UNKNOWN;
#endif
#ifdef RLIMIT_MEMLOCK
/**
 * @var int
 * @cvalue RLIMIT_MEMLOCK
 */
const POSIX_RLIMIT_MEMLOCK = UNKNOWN;
#endif
#ifdef RLIMIT_MSGQUEUE
/**
 * @var int
 * @cvalue RLIMIT_MSGQUEUE
 */
const POSIX_RLIMIT_MSGQUEUE = UNKNOWN;
#endif
#ifdef RLIMIT_NICE
/**
 * @var int
 * @cvalue RLIMIT_NICE
 */
const POSIX_RLIMIT_NICE = UNKNOWN;
#endif
#ifdef RLIMIT_NOFILE
/**
 * @var int
 * @cvalue RLIMIT_NOFILE
 */
const POSIX_RLIMIT_NOFILE = UNKNOWN;
#endif
#ifdef RLIMIT_NPROC
/**
 * @var int
 * @cvalue RLIMIT_NPROC
 */
const POSIX_RLIMIT_NPROC = UNKNOWN;
#endif
#ifdef RLIMIT_RSS
/**
 * @var int
 * @cvalue RLIMIT_RSS
 */
const POSIX_RLIMIT_RSS = UNKNOWN;
#endif
#ifdef RLIMIT_RTPRIO
/**
 * @var int
 * @cvalue RLIMIT_RTPRIO
 */
const POSIX_RLIMIT_RTPRIO = UNKNOWN;
#endif
#ifdef RLIMIT_RTTIME
/**
 * @var int
 * @cvalue RLIMIT_RTTIME
 */
const POSIX_RLIMIT_RTTIME = UNKNOWN;
#endif
#ifdef RLIMIT_SIGPENDING
/**
 * @var int
 * @cvalue RLIMIT_SIGPENDING
 */
const POSIX_RLIMIT_SIGPENDING = UNKNOWN;
#endif
#ifdef RLIMIT_STACK
/**
 * @var int
 * @cvalue RLIMIT_STACK
 */
const POSIX_RLIMIT_STACK = UNKNOWN;
#endif
#ifdef RLIMIT_KQUEUES
/**
 * @var int
 * @cvalue RLIMIT_KQUEUES
 */
const POSIX_RLIMIT_KQUEUES = UNKNOWN;
#endif
#ifdef RLIMIT_NPTS
/**
 * @var int
 * @cvalue RLIMIT_NPTS
 */
const POSIX_RLIMIT_NPTS = UNKNOWN;
#endif
#ifdef HAVE_SETRLIMIT
/**
 * @var int
 * @cvalue RLIM_INFINITY
 */
const POSIX_RLIMIT_INFINITY = UNKNOWN;
#endif

function posix_kill(int $process_id, int $signal): bool {}

function posix_getpid(): int {}

function posix_getppid(): int {}

function posix_getuid(): int {}

function posix_setuid(int $user_id): bool {}

function posix_geteuid(): int {}

#ifdef HAVE_SETEUID
function posix_seteuid(int $user_id): bool {}
#endif

function posix_getgid(): int {}

function posix_setgid(int $group_id): bool {}

function posix_getegid(): int {}

#ifdef HAVE_SETEGID
function posix_setegid(int $group_id): bool {}
#endif

#ifdef HAVE_GETGROUPS
/**
 * @return array<int, int>|false
 * @refcount 1
 */
function posix_getgroups(): array|false {}
#endif

#ifdef HAVE_GETLOGIN
function posix_getlogin(): string|false {}
#endif

function posix_getpgrp(): int {}

#ifdef HAVE_SETSID
function posix_setsid(): int {}
#endif

function posix_setpgid(int $process_id, int $process_group_id): bool {}

#ifdef HAVE_GETPGID
function posix_getpgid(int $process_id): int|false {}
#endif

#ifdef HAVE_GETSID
function posix_getsid(int $process_id): int|false {}
#endif

/**
 * @return array<string, string>|false
 * @refcount 1
 */
function posix_uname(): array|false {}

/**
 * @return array<string, int>|false
 * @refcount 1
 */
function posix_times(): array|false {}


#ifdef HAVE_CTERMID
function posix_ctermid(): string|false {}
#endif

/** @param resource|int $file_descriptor */
function posix_ttyname($file_descriptor): string|false {}

/** @param resource|int $file_descriptor */
function posix_isatty($file_descriptor): bool {}

function posix_getcwd(): string|false {}

#ifdef HAVE_MKFIFO
function posix_mkfifo(string $filename, int $permissions): bool {}
#endif

#ifdef HAVE_MKNOD
function posix_mknod(string $filename, int $flags, int $major = 0, int $minor = 0): bool {}
#endif

function posix_access(string $filename, int $flags = 0): bool {}

/**
 * @return array<string, int|string|array|null>|false
 * @refcount 1
 */
function posix_getgrnam(string $name): array|false {}

/**
 * @return array<string, int|string|array|null>|false
 * @refcount 1
 */
function posix_getgrgid(int $group_id): array|false {}

/**
 * @return array<string, int|string>|false
 * @refcount 1
 */
function posix_getpwnam(string $username): array|false {}

/**
 * @return array<string, int|string>|false
 * @refcount 1
 */
function posix_getpwuid(int $user_id): array|false {}

#ifdef HAVE_GETRLIMIT
/**
 * @return array<string, int|string>|false
 * @refcount 1
 */
function posix_getrlimit(): array|false {}
#endif

#ifdef HAVE_SETRLIMIT
function posix_setrlimit(int $resource, int $soft_limit, int $hard_limit): bool {}
#endif

function posix_get_last_error(): int {}

/** @alias posix_get_last_error */
function posix_errno(): int {}

function posix_strerror(int $error_code): string {}

#ifdef HAVE_INITGROUPS
function posix_initgroups(string $username, int $group_id): bool {}
#endif
