<?php

/** @generate-function-entries */

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

function posix_uname(): array|false {}

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

function posix_getgrnam(string $name): array|false {}

function posix_getgrgid(int $group_id): array|false {}

function posix_getpwnam(string $username): array|false {}

function posix_getpwuid(int $user_id): array|false {}

#ifdef HAVE_GETRLIMIT
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
