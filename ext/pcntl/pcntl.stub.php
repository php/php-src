<?php

/** @generate-function-entries */

function pcntl_fork(): int {}

/** @param int $status */
function pcntl_waitpid(int $pid, &$status, int $options = 0, &$rusage = []): int {}

/** @param int $status */
function pcntl_wait(&$status, int $options = 0, &$rusage = []): int {}

/** @param callable|int $handler */
function pcntl_signal(int $signo, $handler, bool $restart_syscalls = true): bool {}

/** @return mixed */
function pcntl_signal_get_handler(int $signo) {}

function pcntl_signal_dispatch(): bool {}

#ifdef HAVE_SIGPROCMASK
/** @param array $oldset */
function pcntl_sigprocmask(int $how, array $set, &$oldset = null): bool {}
#endif

#ifdef HAVE_STRUCT_SIGINFO_T
#if defined(HAVE_SIGWAITINFO) && defined(HAVE_SIGTIMEDWAIT)
/** @param array $info */
function pcntl_sigwaitinfo(array $set, &$info = []): int|false {}

/** @param array $info */
function pcntl_sigtimedwait(array $set, &$info = [], int $seconds = 0, int $nanoseconds = 0): int|false {}
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

function pcntl_exec(string $path, array $args = [], array $envs = []): bool {}

function pcntl_alarm(int $seconds): int {}

function pcntl_get_last_error(): int {}

/** @alias pcntl_get_last_error */
function pcntl_errno(): int {}

#ifdef HAVE_GETPRIORITY
function pcntl_getpriority(?int $pid = null, int $process_identifier = PRIO_PROCESS): int|false {}
#endif

#ifdef HAVE_SETPRIORITY
function pcntl_setpriority(int $priority, ?int $pid = null, int $process_identifier = PRIO_PROCESS): bool{}
#endif

function pcntl_strerror(int $errno): string {}

function pcntl_async_signals(?bool $on = null): bool {}

#ifdef HAVE_UNSHARE
function pcntl_unshare(int $flags): bool {}
#endif
