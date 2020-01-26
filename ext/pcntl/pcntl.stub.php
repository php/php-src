<?php

function pcntl_fork(): int {}

function pcntl_waitpid(int $pid, &$status, int $options = 0, &$rusage = []): int {}

function pcntl_wait(&$status, int $options = 0, &$rusage = []): int {}

/**
 * @param callable|int $handler
 */
function pcntl_signal(int $signo, $handler, bool $restart_syscalls = true): bool {}

/** @return mixed */
function pcntl_signal_get_handler(int $signo) {}

function pcntl_signal_dispatch(): bool {}

#ifdef HAVE_SIGPROCMASK
function pcntl_sigprocmask(int $how, array $set, &$oldset = null): bool {}
#endif

#ifdef HAVE_STRUCT_SIGINFO_T
#if HAVE_SIGWAITINFO && HAVE_SIGTIMEDWAIT
function pcntl_sigwaitinfo(array $set, &$info = []): int|false {}

function pcntl_sigtimedwait(array $set, &$info = [], int $seconds = 0, int $nanoseconds = 0): int|false {}
#endif
#endif

function pcntl_wifexited(int $status): bool {}

function pcntl_wifstopped(int $status): bool {}

#ifdef HAVE_WCONTINUED
function pcntl_wifcontinued(int $status): bool {}
#endif

function pcntl_wifsignaled(int $status): bool {}

function pcntl_wifexitstatus(int $status): int|false {}

function pcntl_wtermsig(int $status): int|false {}

function pcntl_wstopsig(int $status): int|false {}

function pcntl_exec(string $path, array $args = [], array $envs = []): bool {}

function pcntl_alarm(int $seconds): int {}

function pcntl_get_last_error(): int {}

#ifdef HAVE_GETPRIORITY
function pcntl_getpriority(int $pid = UNKNOWN, int $process_identifier = PRIO_PROCESS): int|false {}
#endif

#ifdef HAVE_SETPRIORITY
function pcntl_setpriority(int $priority, int $pid = UNKNOWN, int $process_identifier = PRIO_PROCESS): bool{}
#endif

function pcntl_strerror(int $errno): string {}

function pcntl_async_signals(bool $on = UNKNOWN): bool {}

#ifdef HAVE_UNSHARE
function pcntl_unshare(int $flags): bool {}
#endif
