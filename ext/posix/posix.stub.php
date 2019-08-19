<?php

function posix_kill(int $pid, int $sig): bool {}

function posix_getpid(): int {}

function posix_getppid(): int {}

function posix_getuid(): int {}

function posix_setuid(int $uid): bool {}

function posix_geteuid(): int {}

#ifdef HAVE_SETEUID
function posix_seteuid(int $uid): bool {}
#endif

function posix_getgid(): int {}

function posix_setgid(int $gid): bool {}

function posix_getegid(): int {}

#ifdef HAVE_SETEGID
function posix_setegid(int $gid): bool {}
#endif

#ifdef HAVE_GETGROUPS
/** @return array|false */
function posix_getgroups() {}
#endif

#ifdef HAVE_GETLOGIN
/** @return string|false */
function posix_getlogin() {}
#endif

function posix_getpgrp(): int {}

#ifdef HAVE_SETSID
function posix_setsid(): int {}
#endif

function posix_setpgid(int $pid, int $pgid): bool {}

#ifdef HAVE_GETPGID
/** @return int|false */
function posix_getpgid(int $pid) {}
#endif

#ifdef HAVE_GETSID
/** @return int|false */
function posix_getsid(int $pid) {}
#endif

/** @return array|false */
function posix_uname() {}

/** @return array|false */
function posix_times() {}


#ifdef HAVE_CTERMID
/** @return string|false */
function posix_ctermid() {}
#endif

/**
 * @return string|false
 */
function posix_ttyname($fd) {}

function posix_isatty($fd): bool {}

/** @return string|false */
function posix_getcwd() {}

#ifdef HAVE_MKFIFO
function posix_mkfifo(string $pathname, int $mode): bool {}
#endif

#ifdef HAVE_MKNOD
function posix_mknod(string $pathname, int $mode, int $major = 0, int $minor = 0): bool {}
#endif

function posix_access(string $file, int $mode = 0): bool {}

/** @return array|false */
function posix_getgrnam(string $name) {}

/** @return array|false */
function posix_getgrgid(int $gid) {}

/** @return array|false */
function posix_getpwnam(string $username) {}

/** @return array|false */
function posix_getpwuid(int $uid) {}

#ifdef HAVE_GETRLIMIT
/** @return array|false */
function posix_getrlimit() {}
#endif

#ifdef HAVE_SETRLIMIT
function posix_setrlimit(int $resource, int $softlimit, int $hardlimit): bool {}
#endif

function posix_get_last_error(): int {}

function posix_errno(): int {}

function posix_strerror(int $errno): string {}

#ifdef HAVE_INITGROUPS
function posix_initgroups(string $name, int $base_group_id): bool {}
#endif
