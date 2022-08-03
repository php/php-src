<?php

/** @generate-class-entries */

/* Wait Constants */

#ifdef WNOHANG
/**
 * @var int
 * @cvalue LONG_CONST(WNOHANG)
 */
const WNOHANG = UNKNOWN;
#endif
#ifdef WUNTRACED
/**
 * @var int
 * @cvalue LONG_CONST(WUNTRACED)
 */
const WUNTRACED = UNKNOWN;
#endif
#ifdef HAVE_WCONTINUED
/**
 * @var int
 * @cvalue LONG_CONST(WCONTINUED)
 */
const WCONTINUED = UNKNOWN;
#endif

/* Signal Constants */

/**
 * @var int
 * @cvalue LONG_CONST(SIG_IGN)
 */
const SIG_IGN = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIG_DFL)
 */
const SIG_DFL = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIG_ERR)
 */
const SIG_ERR = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGHUP)
 */
const SIGHUP = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGINT)
 */
const SIGINT = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGQUIT)
 */
const SIGQUIT = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGILL)
 */
const SIGILL = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGTRAP)
 */
const SIGTRAP = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGABRT)
 */
const SIGABRT = UNKNOWN;
#ifdef SIGIOT
/**
 * @var int
 * @cvalue LONG_CONST(SIGIOT)
 */
const SIGIOT = UNKNOWN;
#endif

/**
 * @var int
 * @cvalue LONG_CONST(SIGBUS)
 */
const SIGBUS = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGFPE)
 */
const SIGFPE = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGKILL)
 */
const SIGKILL = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGUSR1)
 */
const SIGUSR1 = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGSEGV)
 */
const SIGSEGV = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGUSR2)
 */
const SIGUSR2 = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGPIPE)
 */
const SIGPIPE = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGALRM)
 */
const SIGALRM = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGTERM)
 */
const SIGTERM = UNKNOWN;
#ifdef SIGSTKFLT
/**
 * @var int
 * @cvalue LONG_CONST(SIGSTKFLT)
 */
const SIGSTKFLT = UNKNOWN;
#endif
#ifdef SIGCLD
/**
 * @var int
 * @cvalue LONG_CONST(SIGCLD)
 */
const SIGCLD = UNKNOWN;
#endif
#ifdef SIGCHLD
/**
 * @var int
 * @cvalue LONG_CONST(SIGCHLD)
 */
const SIGCHLD = UNKNOWN;
#endif

/**
 * @var int
 * @cvalue LONG_CONST(SIGCONT)
 */
const SIGCONT = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGSTOP)
 */
const SIGSTOP = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGTSTP)
 */
const SIGTSTP = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGTTIN)
 */
const SIGTTIN = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGTTOU)
 */
const SIGTTOU = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGURG)
 */
const SIGURG = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGXCPU)
 */
const SIGXCPU = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGXFSZ)
 */
const SIGXFSZ = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGVTALRM)
 */
const SIGVTALRM = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGPROF)
 */
const SIGPROF = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGWINCH)
 */
const SIGWINCH = UNKNOWN;
#ifdef SIGPOLL
/**
 * @var int
 * @cvalue LONG_CONST(SIGPOLL)
 */
const SIGPOLL = UNKNOWN;
#endif
#ifdef SIGIO
/**
 * @var int
 * @cvalue LONG_CONST(SIGIO)
 */
const SIGIO = UNKNOWN;
#endif
#ifdef SIGPWR
/**
 * @var int
 * @cvalue LONG_CONST(SIGPWR)
 */
const SIGPWR = UNKNOWN;
#endif
#ifdef SIGSYS
/**
 * @var int
 * @cvalue LONG_CONST(SIGSYS)
 */
const SIGSYS = UNKNOWN;
/**
 * @var int
 * @cvalue LONG_CONST(SIGSYS)
 */
const SIGBABY = UNKNOWN;
#endif
#ifdef SIGRTMIN
/**
 * @var int
 * @cvalue LONG_CONST(SIGRTMIN)
 */
const SIGRTMIN = UNKNOWN;
#endif
#ifdef SIGRTMAX
/**
 * @var int
 * @cvalue LONG_CONST(SIGRTMAX)
 */
const SIGRTMAX = UNKNOWN;
#endif

#if (defined(HAVE_GETPRIORITY) || defined(HAVE_SETPRIORITY))
/**
 * @var int
 * @cvalue PRIO_PGRP
 */
const PRIO_PGRP = UNKNOWN;
/**
 * @var int
 * @cvalue PRIO_USER
 */
const PRIO_USER = UNKNOWN;
/**
 * @var int
 * @cvalue PRIO_PROCESS
 */
const PRIO_PROCESS = UNKNOWN;
#if defined(PRIO_DARWIN_BG)
/**
 * @var int
 * @cvalue PRIO_DARWIN_BG
 */
const PRIO_DARWIN_BG = UNKNOWN;
/**
 * @var int
 * @cvalue PRIO_DARWIN_THREAD
 */
const PRIO_DARWIN_THREAD = UNKNOWN;
#endif
#endif

/* "mode" argument for sigprocmask */

#ifdef HAVE_SIGPROCMASK
/**
 * @var int
 * @cvalue SIG_BLOCK
 */
const SIG_BLOCK = UNKNOWN;
/**
 * @var int
 * @cvalue SIG_UNBLOCK
 */
const SIG_UNBLOCK = UNKNOWN;
/**
 * @var int
 * @cvalue SIG_SETMASK
 */
const SIG_SETMASK = UNKNOWN;
#endif

#if (defined(HAVE_SIGWAITINFO) && defined(HAVE_SIGTIMEDWAIT))
/**
 * @var int
 * @cvalue SI_USER
 */
const SI_USER = UNKNOWN;
#ifdef SI_NOINFO
/**
 * @var int
 * @cvalue SI_NOINFO
 */
const SI_NOINFO = UNKNOWN;
#endif
#ifdef SI_KERNEL
/**
 * @var int
 * @cvalue SI_KERNEL
 */
const SI_KERNEL = UNKNOWN;
#endif
/**
 * @var int
 * @cvalue SI_QUEUE
 */
const SI_QUEUE = UNKNOWN;
/**
 * @var int
 * @cvalue SI_TIMER
 */
const SI_TIMER = UNKNOWN;
/**
 * @var int
 * @cvalue SI_MESGQ
 */
const SI_MESGQ = UNKNOWN;
/**
 * @var int
 * @cvalue SI_ASYNCIO
 */
const SI_ASYNCIO = UNKNOWN;
#ifdef SI_SIGIO
/**
 * @var int
 * @cvalue SI_SIGIO
 */
const SI_SIGIO = UNKNOWN;
#endif
#ifdef SI_TKILL
/**
 * @var int
 * @cvalue SI_TKILL
 */
const SI_TKILL = UNKNOWN;
#endif

/* si_code for SIGCHILD */

#ifdef CLD_EXITED
/**
 * @var int
 * @cvalue CLD_EXITED
 */
const CLD_EXITED = UNKNOWN;
#endif
#ifdef CLD_KILLED
/**
 * @var int
 * @cvalue CLD_KILLED
 */
const CLD_KILLED = UNKNOWN;
#endif
#ifdef CLD_DUMPED
/**
 * @var int
 * @cvalue CLD_DUMPED
 */
const CLD_DUMPED = UNKNOWN;
#endif
#ifdef CLD_TRAPPED
/**
 * @var int
 * @cvalue CLD_TRAPPED
 */
const CLD_TRAPPED = UNKNOWN;
#endif
#ifdef CLD_STOPPED
/**
 * @var int
 * @cvalue CLD_STOPPED
 */
const CLD_STOPPED = UNKNOWN;
#endif
#ifdef CLD_CONTINUED
/**
 * @var int
 * @cvalue CLD_CONTINUED
 */
const CLD_CONTINUED = UNKNOWN;
#endif

/* si_code for SIGTRAP */

#ifdef TRAP_BRKPT
/**
 * @var int
 * @cvalue TRAP_BRKPT
 */
const TRAP_BRKPT = UNKNOWN;
#endif
#ifdef TRAP_TRACE
/**
 * @var int
 * @cvalue TRAP_TRACE
 */
const TRAP_TRACE = UNKNOWN;
#endif

/* si_code for SIGPOLL */

#ifdef POLL_IN
/**
 * @var int
 * @cvalue POLL_IN
 */
const POLL_IN = UNKNOWN;
#endif
#ifdef POLL_OUT
/**
 * @var int
 * @cvalue POLL_OUT
 */
const POLL_OUT = UNKNOWN;
#endif
#ifdef POLL_MSG
/**
 * @var int
 * @cvalue POLL_MSG
 */
const POLL_MSG = UNKNOWN;
#endif
#ifdef POLL_ERR
/**
 * @var int
 * @cvalue POLL_ERR
 */
const POLL_ERR = UNKNOWN;
#endif
#ifdef POLL_PRI
/**
 * @var int
 * @cvalue POLL_PRI
 */
const POLL_PRI = UNKNOWN;
#endif
#ifdef POLL_HUP
/**
 * @var int
 * @cvalue POLL_HUP
 */
const POLL_HUP = UNKNOWN;
#endif

#ifdef ILL_ILLOPC
/**
 * @var int
 * @cvalue ILL_ILLOPC
 */
const ILL_ILLOPC = UNKNOWN;
#endif
#ifdef ILL_ILLOPN
/**
 * @var int
 * @cvalue ILL_ILLOPN
 */
const ILL_ILLOPN = UNKNOWN;
#endif
#ifdef ILL_ILLADR
/**
 * @var int
 * @cvalue ILL_ILLADR
 */
const ILL_ILLADR = UNKNOWN;
#endif
#ifdef ILL_ILLTRP
/**
 * @var int
 * @cvalue ILL_ILLTRP
 */
const ILL_ILLTRP = UNKNOWN;
#endif
#ifdef ILL_PRVOPC
/**
 * @var int
 * @cvalue ILL_PRVOPC
 */
const ILL_PRVOPC = UNKNOWN;
#endif
#ifdef ILL_PRVREG
/**
 * @var int
 * @cvalue ILL_PRVREG
 */
const ILL_PRVREG = UNKNOWN;
#endif
#ifdef ILL_COPROC
/**
 * @var int
 * @cvalue ILL_COPROC
 */
const ILL_COPROC = UNKNOWN;
#endif
#ifdef ILL_BADSTK
/**
 * @var int
 * @cvalue ILL_BADSTK
 */
const ILL_BADSTK = UNKNOWN;
#endif
#ifdef FPE_INTDIV
/**
 * @var int
 * @cvalue FPE_INTDIV
 */
const FPE_INTDIV = UNKNOWN;
#endif
#ifdef FPE_INTOVF
/**
 * @var int
 * @cvalue FPE_INTOVF
 */
const FPE_INTOVF = UNKNOWN;
#endif
#ifdef FPE_FLTDIV
/**
 * @var int
 * @cvalue FPE_FLTDIV
 */
const FPE_FLTDIV = UNKNOWN;
#endif
#ifdef FPE_FLTOVF
/**
 * @var int
 * @cvalue FPE_FLTOVF
 */
const FPE_FLTOVF = UNKNOWN;
#endif
#ifdef FPE_FLTUND
/**
 * @var int
 * @cvalue FPE_FLTUND
 */
const FPE_FLTUND = UNKNOWN;
#endif
#ifdef FPE_FLTRES
/**
 * @var int
 * @cvalue FPE_FLTRES
 */
const FPE_FLTRES = UNKNOWN;
#endif
#ifdef FPE_FLTINV
/**
 * @var int
 * @cvalue FPE_FLTINV
 */
const FPE_FLTINV = UNKNOWN;
#endif
#ifdef FPE_FLTSUB
/**
 * @var int
 * @cvalue FPE_FLTSUB
 */
const FPE_FLTSUB = UNKNOWN;
#endif
#ifdef SEGV_MAPERR
/**
 * @var int
 * @cvalue SEGV_MAPERR
 */
const SEGV_MAPERR = UNKNOWN;
#endif
#ifdef SEGV_ACCERR
/**
 * @var int
 * @cvalue SEGV_ACCERR
 */
const SEGV_ACCERR = UNKNOWN;
#endif
#ifdef BUS_ADRALN
/**
 * @var int
 * @cvalue BUS_ADRALN
 */
const BUS_ADRALN = UNKNOWN;
#endif
#ifdef BUS_ADRERR
/**
 * @var int
 * @cvalue BUS_ADRERR
 */
const BUS_ADRERR = UNKNOWN;
#endif
#ifdef BUS_OBJERR
/**
 * @var int
 * @cvalue BUS_OBJERR
 */
const BUS_OBJERR = UNKNOWN;
#endif
#endif

/* unshare(/clone) constants */

#ifdef HAVE_UNSHARE
/**
 * @var int
 * @cvalue CLONE_NEWNS
 */
const CLONE_NEWNS = UNKNOWN;
#ifdef CLONE_NEWIPC
/**
 * @var int
 * @cvalue CLONE_NEWIPC
 */
const CLONE_NEWIPC = UNKNOWN;
#endif
#ifdef CLONE_NEWUTS
/**
 * @var int
 * @cvalue CLONE_NEWUTS
 */
const CLONE_NEWUTS = UNKNOWN;
#endif
#ifdef CLONE_NEWNET
/**
 * @var int
 * @cvalue CLONE_NEWNET
 */
const CLONE_NEWNET = UNKNOWN;
#endif
#ifdef CLONE_NEWPID
/**
 * @var int
 * @cvalue CLONE_NEWPID
 */
const CLONE_NEWPID = UNKNOWN;
#endif
#ifdef CLONE_NEWUSER
/**
 * @var int
 * @cvalue CLONE_NEWUSER
 */
const CLONE_NEWUSER = UNKNOWN;
#endif
#ifdef CLONE_NEWCGROUP
/**
 * @var int
 * @cvalue CLONE_NEWCGROUP
 */
const CLONE_NEWCGROUP = UNKNOWN;
#endif
#endif

#ifdef HAVE_RFORK
#ifdef RFPROC
/**
 * @var int
 * @cvalue RFPROC
 */
const RFPROC = UNKNOWN;
#endif
#ifdef RFNOWAIT
/**
 * @var int
 * @cvalue RFNOWAIT
 */
const RFNOWAIT = UNKNOWN;
#endif
#ifdef RFCFDG
/**
 * @var int
 * @cvalue RFCFDG
 */
const RFCFDG = UNKNOWN;
#endif
#ifdef RFFDG
/**
 * @var int
 * @cvalue RFFDG
 */
const RFFDG = UNKNOWN;
#endif
#ifdef RFLINUXTHPN
/**
 * @var int
 * @cvalue RFLINUXTHPN
 */
const RFLINUXTHPN = UNKNOWN;
#endif
#ifdef RFTSIGZMB
/**
 * @var int
 * @cvalue RFTSIGZMB
 */
const RFTSIGZMB = UNKNOWN;
#endif
#ifdef RFTHREAD
/**
 * @var int
 * @cvalue RFTHREAD
 */
const RFTHREAD = UNKNOWN;
#endif
#endif

#ifdef HAVE_FORKX
/**
 * @var int
 * @cvalue FORK_NOSIGCHLD
 */
const FORK_NOSIGCHLD = UNKNOWN;
/**
 * @var int
 * @cvalue FORK_WAITPID
 */
const FORK_WAITPID = UNKNOWN;
#endif

#ifdef EINTR
/**
 * @var int
 * @cvalue EINTR
 */
const PCNTL_EINTR = UNKNOWN;
#endif
#ifdef ECHILD
/**
 * @var int
 * @cvalue ECHILD
 */
const PCNTL_ECHILD = UNKNOWN;
#endif
#ifdef EINVAL
/**
 * @var int
 * @cvalue EINVAL
 */
const PCNTL_EINVAL = UNKNOWN;
#endif
#ifdef EAGAIN
/**
 * @var int
 * @cvalue EAGAIN
 */
const PCNTL_EAGAIN = UNKNOWN;
#endif
#ifdef ESRCH
/**
 * @var int
 * @cvalue ESRCH
 */
const PCNTL_ESRCH = UNKNOWN;
#endif
#ifdef EACCES
/**
 * @var int
 * @cvalue EACCES
 */
const PCNTL_EACCES = UNKNOWN;
#endif
#ifdef EPERM
/**
 * @var int
 * @cvalue EPERM
 */
const PCNTL_EPERM = UNKNOWN;
#endif
#ifdef ENOMEM
/**
 * @var int
 * @cvalue ENOMEM
 */
const PCNTL_ENOMEM = UNKNOWN;
#endif
#ifdef E2BIG
/**
 * @var int
 * @cvalue E2BIG
 */
const PCNTL_E2BIG = UNKNOWN;
#endif
#ifdef EFAULT
/**
 * @var int
 * @cvalue EFAULT
 */
const PCNTL_EFAULT = UNKNOWN;
#endif
#ifdef EIO
/**
 * @var int
 * @cvalue EIO
 */
const PCNTL_EIO = UNKNOWN;
#endif
#ifdef EISDIR
/**
 * @var int
 * @cvalue EISDIR
 */
const PCNTL_EISDIR = UNKNOWN;
#endif
#ifdef ELIBBAD
/**
 * @var int
 * @cvalue ELIBBAD
 */
const PCNTL_ELIBBAD = UNKNOWN;
#endif
#ifdef ELOOP
/**
 * @var int
 * @cvalue ELOOP
 */
const PCNTL_ELOOP = UNKNOWN;
#endif
#ifdef EMFILE
/**
 * @var int
 * @cvalue EMFILE
 */
const PCNTL_EMFILE = UNKNOWN;
#endif
#ifdef ENAMETOOLONG
/**
 * @var int
 * @cvalue ENAMETOOLONG
 */
const PCNTL_ENAMETOOLONG = UNKNOWN;
#endif
#ifdef ENFILE
/**
 * @var int
 * @cvalue ENFILE
 */
const PCNTL_ENFILE = UNKNOWN;
#endif
#ifdef ENOENT
/**
 * @var int
 * @cvalue ENOENT
 */
const PCNTL_ENOENT = UNKNOWN;
#endif
#ifdef ENOEXEC
/**
 * @var int
 * @cvalue ENOEXEC
 */
const PCNTL_ENOEXEC = UNKNOWN;
#endif
#ifdef ENOTDIR
/**
 * @var int
 * @cvalue ENOTDIR
 */
const PCNTL_ENOTDIR = UNKNOWN;
#endif
#ifdef ETXTBSY
/**
 * @var int
 * @cvalue ETXTBSY
 */
const PCNTL_ETXTBSY = UNKNOWN;
#endif
#ifdef ENOSPC
/**
 * @var int
 * @cvalue ENOSPC
 */
const PCNTL_ENOSPC = UNKNOWN;
#endif
#ifdef EUSERS
/**
 * @var int
 * @cvalue EUSERS
 */
const PCNTL_EUSERS = UNKNOWN;
#endif
#ifdef ECAPMODE
/**
 * @var int
 * @cvalue ECAPMODE
 */
const PCNTL_ECAPMODE = UNKNOWN;
#endif

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
#if (defined(HAVE_SIGWAITINFO) && defined(HAVE_SIGTIMEDWAIT))
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

#ifdef HAVE_RFORK
function pcntl_rfork(int $flags, int $signal = 0): int{}
#endif

#ifdef HAVE_FORKX
function pcntl_forkx(int $flags): int{}
#endif
