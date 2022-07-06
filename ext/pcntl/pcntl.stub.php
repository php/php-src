<?php

/** @generate-function-entries */

function pcntl_fork(): int {}

/**
 * @param int $status
 * @param array $resource_usage
 */
function pcntl_waitpid(int $process_id, &$status, int $flags = 0, &$resource_usage = []): int {}

/**
 * @param int $status
 * @param array $resource_usage
 */
function pcntl_wait(&$status, int $flags = 0, &$resource_usage = []): int {}

/** @param callable|int $handler */
function pcntl_signal(int $signal, $handler, bool $restart_syscalls = true): bool {}

/** @return callable|int */
function pcntl_signal_get_handler(int $signal) {}

function pcntl_signal_dispatch(): bool {}

#ifdef HAVE_SIGPROCMASK
/** @param array $old_signals */
function pcntl_sigprocmask(int $mode, array $signals, &$old_signals = null): bool {}
#endif

#ifdef HAVE_STRUCT_SIGINFO_T
#if defined(HAVE_SIGWAITINFO) && defined(HAVE_SIGTIMEDWAIT)
/** @param array $info */
function pcntl_sigwaitinfo(array $signals, &$info = []): int|false {}

/** @param array $info */
function pcntl_sigtimedwait(array $signals, &$info = [], int $seconds = 0, int $nanoseconds = 0): int|false {}
#endif
#endif

function pcntl_wifexited(int $status): bool {}

function pcntl_wifstopped(int $status): bool {}

#ifdef HAVE_WCONTINUED
function pcntl_wifcontinued(int $status): bool {}
#endif

function pcntl_wifsignaled(int $status): bool {}

function pcntl_wexitstatus(int $status): int|false {}

function pcntl_wtermsig(int $status): int|false {}

function pcntl_wstopsig(int $status): int|false {}

function pcntl_exec(string $path, array $args = [], array $env_vars = []): bool {}

function pcntl_alarm(int $seconds): int {}

function pcntl_get_last_error(): int {}

/** @alias pcntl_get_last_error */
function pcntl_errno(): int {}

#ifdef HAVE_GETPRIORITY
function pcntl_getpriority(?int $process_id = null, int $mode = PRIO_PROCESS): int|false {}
#endif

#ifdef HAVE_SETPRIORITY
function pcntl_setpriority(int $priority, ?int $process_id = null, int $mode = PRIO_PROCESS): bool{}
#endif

function pcntl_strerror(int $error_code): string {}

function pcntl_async_signals(?bool $enable = null): bool {}

#ifdef HAVE_UNSHARE
function pcntl_unshare(int $flags): bool {}
#endif
