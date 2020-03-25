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
function posix_getgroups(): array|false {}
#endif

#ifdef HAVE_GETLOGIN
function posix_getlogin(): string|false {}
#endif

function posix_getpgrp(): int {}

#ifdef HAVE_SETSID
function posix_setsid(): int {}
#endif

function posix_setpgid(int $pid, int $pgid): bool {}

#ifdef HAVE_GETPGID
function posix_getpgid(int $pid): int|false {}
#endif

#ifdef HAVE_GETSID
function posix_getsid(int $pid): int|false {}
#endif

function posix_uname(): array|false {}

function posix_times(): array|false {}


#ifdef HAVE_CTERMID
function posix_ctermid(): string|false {}
#endif

function posix_ttyname($fd): string|false {}

function posix_isatty($fd): bool {}

function posix_getcwd(): string|false {}

#ifdef HAVE_MKFIFO
function posix_mkfifo(string $pathname, int $mode): bool {}
#endif

#ifdef HAVE_MKNOD
function posix_mknod(string $pathname, int $mode, int $major = 0, int $minor = 0): bool {}
#endif

function posix_access(string $file, int $mode = 0): bool {}

function posix_getgrnam(string $name): array|false {}

function posix_getgrgid(int $gid): array|false {}

function posix_getpwnam(string $username): array|false {}

function posix_getpwuid(int $uid): array|false {}

#ifdef HAVE_GETRLIMIT
function posix_getrlimit(): array|false {}
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
