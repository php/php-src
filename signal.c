/**********************************************************************

  signal.c -

  $Author$
  created at: Tue Dec 20 10:13:44 JST 1994

  Copyright (C) 1993-2007 Yukihiro Matsumoto
  Copyright (C) 2000  Network Applied Communication Laboratory, Inc.
  Copyright (C) 2000  Information-technology Promotion Agency, Japan

**********************************************************************/

#include "ruby/ruby.h"
#include "vm_core.h"
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include "atomic.h"

#ifdef NEED_RUBY_ATOMIC_EXCHANGE
rb_atomic_t
ruby_atomic_exchange(rb_atomic_t *ptr, rb_atomic_t val)
{
    rb_atomic_t old = *ptr;
    *ptr = val;
    return old;
}
#endif

#if defined(__BEOS__) || defined(__HAIKU__)
#undef SIGBUS
#endif

#ifdef HAVE_PTHREAD_SIGMASK
#define USE_TRAP_MASK 1
#else
#define USE_TRAP_MASK 0
#endif

#ifndef NSIG
# define NSIG (_SIGMAX + 1)      /* For QNX */
#endif

static const struct signals {
    const char *signm;
    int  signo;
} siglist [] = {
    {"EXIT", 0},
#ifdef SIGHUP
    {"HUP", SIGHUP},
#endif
    {"INT", SIGINT},
#ifdef SIGQUIT
    {"QUIT", SIGQUIT},
#endif
#ifdef SIGILL
    {"ILL", SIGILL},
#endif
#ifdef SIGTRAP
    {"TRAP", SIGTRAP},
#endif
#ifdef SIGIOT
    {"IOT", SIGIOT},
#endif
#ifdef SIGABRT
    {"ABRT", SIGABRT},
#endif
#ifdef SIGEMT
    {"EMT", SIGEMT},
#endif
#ifdef SIGFPE
    {"FPE", SIGFPE},
#endif
#ifdef SIGKILL
    {"KILL", SIGKILL},
#endif
#ifdef SIGBUS
    {"BUS", SIGBUS},
#endif
#ifdef SIGSEGV
    {"SEGV", SIGSEGV},
#endif
#ifdef SIGSYS
    {"SYS", SIGSYS},
#endif
#ifdef SIGPIPE
    {"PIPE", SIGPIPE},
#endif
#ifdef SIGALRM
    {"ALRM", SIGALRM},
#endif
#ifdef SIGTERM
    {"TERM", SIGTERM},
#endif
#ifdef SIGURG
    {"URG", SIGURG},
#endif
#ifdef SIGSTOP
    {"STOP", SIGSTOP},
#endif
#ifdef SIGTSTP
    {"TSTP", SIGTSTP},
#endif
#ifdef SIGCONT
    {"CONT", SIGCONT},
#endif
#ifdef SIGCHLD
    {"CHLD", SIGCHLD},
#endif
#ifdef SIGCLD
    {"CLD", SIGCLD},
#else
# ifdef SIGCHLD
    {"CLD", SIGCHLD},
# endif
#endif
#ifdef SIGTTIN
    {"TTIN", SIGTTIN},
#endif
#ifdef SIGTTOU
    {"TTOU", SIGTTOU},
#endif
#ifdef SIGIO
    {"IO", SIGIO},
#endif
#ifdef SIGXCPU
    {"XCPU", SIGXCPU},
#endif
#ifdef SIGXFSZ
    {"XFSZ", SIGXFSZ},
#endif
#ifdef SIGVTALRM
    {"VTALRM", SIGVTALRM},
#endif
#ifdef SIGPROF
    {"PROF", SIGPROF},
#endif
#ifdef SIGWINCH
    {"WINCH", SIGWINCH},
#endif
#ifdef SIGUSR1
    {"USR1", SIGUSR1},
#endif
#ifdef SIGUSR2
    {"USR2", SIGUSR2},
#endif
#ifdef SIGLOST
    {"LOST", SIGLOST},
#endif
#ifdef SIGMSG
    {"MSG", SIGMSG},
#endif
#ifdef SIGPWR
    {"PWR", SIGPWR},
#endif
#ifdef SIGPOLL
    {"POLL", SIGPOLL},
#endif
#ifdef SIGDANGER
    {"DANGER", SIGDANGER},
#endif
#ifdef SIGMIGRATE
    {"MIGRATE", SIGMIGRATE},
#endif
#ifdef SIGPRE
    {"PRE", SIGPRE},
#endif
#ifdef SIGGRANT
    {"GRANT", SIGGRANT},
#endif
#ifdef SIGRETRACT
    {"RETRACT", SIGRETRACT},
#endif
#ifdef SIGSOUND
    {"SOUND", SIGSOUND},
#endif
#ifdef SIGINFO
    {"INFO", SIGINFO},
#endif
    {NULL, 0}
};

static int
signm2signo(const char *nm)
{
    const struct signals *sigs;

    for (sigs = siglist; sigs->signm; sigs++)
	if (strcmp(sigs->signm, nm) == 0)
	    return sigs->signo;
    return 0;
}

static const char*
signo2signm(int no)
{
    const struct signals *sigs;

    for (sigs = siglist; sigs->signm; sigs++)
	if (sigs->signo == no)
	    return sigs->signm;
    return 0;
}

const char *
ruby_signal_name(int no)
{
    return signo2signm(no);
}

/*
 * call-seq:
 *    SignalException.new(sig_name)              ->  signal_exception
 *    SignalException.new(sig_number [, name])   ->  signal_exception
 *
 *  Construct a new SignalException object.  +sig_name+ should be a known
 *  signal name.
 */

static VALUE
esignal_init(int argc, VALUE *argv, VALUE self)
{
    int argnum = 1;
    VALUE sig = Qnil;
    int signo;
    const char *signm;

    if (argc > 0) {
	sig = rb_check_to_integer(argv[0], "to_int");
	if (!NIL_P(sig)) argnum = 2;
	else sig = argv[0];
    }
    rb_check_arity(argc, 1, argnum);
    if (argnum == 2) {
	signo = NUM2INT(sig);
	if (signo < 0 || signo > NSIG) {
	    rb_raise(rb_eArgError, "invalid signal number (%d)", signo);
	}
	if (argc > 1) {
	    sig = argv[1];
	}
	else {
	    signm = signo2signm(signo);
	    if (signm) {
		sig = rb_sprintf("SIG%s", signm);
	    }
	    else {
		sig = rb_sprintf("SIG%u", signo);
	    }
	}
    }
    else {
	signm = SYMBOL_P(sig) ? rb_id2name(SYM2ID(sig)) : StringValuePtr(sig);
	if (strncmp(signm, "SIG", 3) == 0) signm += 3;
	signo = signm2signo(signm);
	if (!signo) {
	    rb_raise(rb_eArgError, "unsupported name `SIG%s'", signm);
	}
	sig = rb_sprintf("SIG%s", signm);
    }
    rb_call_super(1, &sig);
    rb_iv_set(self, "signo", INT2NUM(signo));

    return self;
}

/*
 * call-seq:
 *    signal_exception.signo   ->  num
 *
 *  Returns a signal number.
 */

static VALUE
esignal_signo(VALUE self)
{
    return rb_iv_get(self, "signo");
}

/* :nodoc: */
static VALUE
interrupt_init(int argc, VALUE *argv, VALUE self)
{
    VALUE args[2];

    args[0] = INT2FIX(SIGINT);
    rb_scan_args(argc, argv, "01", &args[1]);
    return rb_call_super(2, args);
}

void
ruby_default_signal(int sig)
{
    signal(sig, SIG_DFL);
    raise(sig);
}

/*
 *  call-seq:
 *     Process.kill(signal, pid, ...)    -> fixnum
 *
 *  Sends the given signal to the specified process id(s), or to the
 *  current process if _pid_ is zero. _signal_ may be an
 *  integer signal number or a POSIX signal name (either with or without
 *  a +SIG+ prefix). If _signal_ is negative (or starts
 *  with a minus sign), kills process groups instead of
 *  processes. Not all signals are available on all platforms.
 *
 *     pid = fork do
 *        Signal.trap("HUP") { puts "Ouch!"; exit }
 *        # ... do some work ...
 *     end
 *     # ...
 *     Process.kill("HUP", pid)
 *     Process.wait
 *
 *  <em>produces:</em>
 *
 *     Ouch!
 *
 *  If _signal_ is an integer but wrong for signal,
 *  <code>Errno::EINVAL</code> or +RangeError+ will be raised.
 *  Otherwise unless _signal_ is a +String+ or a +Symbol+, and a known
 *  signal name, +ArgumentError+ will be raised.
 *
 *  Also, <code>Errno::ESRCH</code> or +RangeError+ for invalid _pid_,
 *  <code>Errno::EPERM</code> when failed because of no privilege,
 *  will be raised.  In these cases, signals may have been sent to
 *  preceding processes.
 */

VALUE
rb_f_kill(int argc, VALUE *argv)
{
#ifndef HAS_KILLPG
#define killpg(pg, sig) kill(-(pg), (sig))
#endif
    int negative = 0;
    int sig;
    int i;
    volatile VALUE str;
    const char *s;

    rb_secure(2);
    rb_check_arity(argc, 2, UNLIMITED_ARGUMENTS);

    switch (TYPE(argv[0])) {
      case T_FIXNUM:
	sig = FIX2INT(argv[0]);
	break;

      case T_SYMBOL:
	s = rb_id2name(SYM2ID(argv[0]));
	if (!s) rb_raise(rb_eArgError, "bad signal");
	goto str_signal;

      case T_STRING:
	s = RSTRING_PTR(argv[0]);
      str_signal:
	if (s[0] == '-') {
	    negative++;
	    s++;
	}
	if (strncmp("SIG", s, 3) == 0)
	    s += 3;
	if((sig = signm2signo(s)) == 0)
	    rb_raise(rb_eArgError, "unsupported name `SIG%s'", s);

	if (negative)
	    sig = -sig;
	break;

      default:
	str = rb_check_string_type(argv[0]);
	if (!NIL_P(str)) {
	    s = RSTRING_PTR(str);
	    goto str_signal;
	}
	rb_raise(rb_eArgError, "bad signal type %s",
		 rb_obj_classname(argv[0]));
	break;
    }

    if (sig < 0) {
	sig = -sig;
	for (i=1; i<argc; i++) {
	    if (killpg(NUM2PIDT(argv[i]), sig) < 0)
		rb_sys_fail(0);
	}
    }
    else {
	for (i=1; i<argc; i++) {
	    if (kill(NUM2PIDT(argv[i]), sig) < 0)
		rb_sys_fail(0);
	}
    }
    rb_thread_polling();
    return INT2FIX(i-1);
}

static struct {
    rb_atomic_t cnt[RUBY_NSIG];
    rb_atomic_t size;
} signal_buff;

#ifdef __dietlibc__
#define sighandler_t sh_t
#endif

typedef RETSIGTYPE (*sighandler_t)(int);
#ifdef USE_SIGALTSTACK
typedef void ruby_sigaction_t(int, siginfo_t*, void*);
#define SIGINFO_ARG , siginfo_t *info, void *ctx
#else
typedef RETSIGTYPE ruby_sigaction_t(int);
#define SIGINFO_ARG
#endif

#ifdef POSIX_SIGNAL

#ifdef USE_SIGALTSTACK
/* alternate stack for SIGSEGV */
void
rb_register_sigaltstack(rb_thread_t *th)
{
    stack_t newSS, oldSS;

    if (!th->altstack)
	rb_bug("rb_register_sigaltstack: th->altstack not initialized\n");

    newSS.ss_sp = th->altstack;
    newSS.ss_size = ALT_STACK_SIZE;
    newSS.ss_flags = 0;

    sigaltstack(&newSS, &oldSS); /* ignore error. */
}
#endif /* USE_SIGALTSTACK */

static sighandler_t
ruby_signal(int signum, sighandler_t handler)
{
    struct sigaction sigact, old;

#if 0
    rb_trap_accept_nativethreads[signum] = 0;
#endif

    sigemptyset(&sigact.sa_mask);
#ifdef USE_SIGALTSTACK
    sigact.sa_sigaction = (ruby_sigaction_t*)handler;
    sigact.sa_flags = SA_SIGINFO;
#else
    sigact.sa_handler = handler;
    sigact.sa_flags = 0;
#endif

#ifdef SA_NOCLDWAIT
    if (signum == SIGCHLD && handler == SIG_IGN)
	sigact.sa_flags |= SA_NOCLDWAIT;
#endif
#if defined(SA_ONSTACK) && defined(USE_SIGALTSTACK)
    if (signum == SIGSEGV || signum == SIGBUS)
	sigact.sa_flags |= SA_ONSTACK;
#endif
    if (sigaction(signum, &sigact, &old) < 0) {
	if (errno != 0 && errno != EINVAL) {
	    rb_bug_errno("sigaction", errno);
	}
    }
    return old.sa_handler;
}

sighandler_t
posix_signal(int signum, sighandler_t handler)
{
    return ruby_signal(signum, handler);
}

#else /* !POSIX_SIGNAL */
#define ruby_signal(sig,handler) (/* rb_trap_accept_nativethreads[(sig)] = 0,*/ signal((sig),(handler)))
#if 0 /* def HAVE_NATIVETHREAD */
static sighandler_t
ruby_nativethread_signal(int signum, sighandler_t handler)
{
    sighandler_t old;

    old = signal(signum, handler);
    rb_trap_accept_nativethreads[signum] = 1;
    return old;
}
#endif
#endif

static RETSIGTYPE
sighandler(int sig)
{
    ATOMIC_INC(signal_buff.cnt[sig]);
    ATOMIC_INC(signal_buff.size);
    rb_thread_wakeup_timer_thread();
#if !defined(BSD_SIGNAL) && !defined(POSIX_SIGNAL)
    ruby_signal(sig, sighandler);
#endif
}

int
rb_signal_buff_size(void)
{
    return signal_buff.size;
}

#if HAVE_PTHREAD_H
#include <pthread.h>
#endif

static void
rb_disable_interrupt(void)
{
#if USE_TRAP_MASK
    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_SETMASK, &mask, NULL);
#endif
}

static void
rb_enable_interrupt(void)
{
#if USE_TRAP_MASK
    sigset_t mask;
    sigemptyset(&mask);
    pthread_sigmask(SIG_SETMASK, &mask, NULL);
#endif
}

int
rb_get_next_signal(void)
{
    int i, sig = 0;

    if (signal_buff.size != 0) {
	for (i=1; i<RUBY_NSIG; i++) {
	    if (signal_buff.cnt[i] > 0) {
		rb_disable_interrupt();
		{
		    ATOMIC_DEC(signal_buff.cnt[i]);
		    ATOMIC_DEC(signal_buff.size);
		}
		rb_enable_interrupt();
		sig = i;
		break;
	    }
	}
    }
    return sig;
}

#ifdef SIGBUS
static RETSIGTYPE
sigbus(int sig SIGINFO_ARG)
{
/*
 * Mac OS X makes KERN_PROTECTION_FAILURE when thread touch guard page.
 * and it's delivered as SIGBUS instaed of SIGSEGV to userland. It's crazy
 * wrong IMHO. but anyway we have to care it. Sigh.
 */
#if defined __MACH__ && defined __APPLE__ && defined USE_SIGALTSTACK
    int ruby_stack_overflowed_p(const rb_thread_t *, const void *);
    NORETURN(void ruby_thread_stack_overflow(rb_thread_t *th));
    rb_thread_t *th = GET_THREAD();
    if (ruby_stack_overflowed_p(th, info->si_addr)) {
	ruby_thread_stack_overflow(th);
    }
#endif
    rb_bug("Bus Error");
}
#endif

#ifdef SIGSEGV
static int segv_received = 0;
static RETSIGTYPE
sigsegv(int sig SIGINFO_ARG)
{
#ifdef USE_SIGALTSTACK
    int ruby_stack_overflowed_p(const rb_thread_t *, const void *);
    NORETURN(void ruby_thread_stack_overflow(rb_thread_t *th));
    rb_thread_t *th = GET_THREAD();
    if (ruby_stack_overflowed_p(th, info->si_addr)) {
	ruby_thread_stack_overflow(th);
    }
#endif
    if (segv_received) {
	fprintf(stderr, "SEGV received in SEGV handler\n");
	abort();
    }
    else {
	extern int ruby_disable_gc_stress;
	segv_received = 1;
	ruby_disable_gc_stress = 1;
	rb_bug("Segmentation fault");
    }
}
#endif

static void
signal_exec(VALUE cmd, int safe, int sig)
{
    VALUE signum = INT2NUM(sig);
    rb_eval_cmd(cmd, rb_ary_new3(1, signum), safe);
}

void
rb_trap_exit(void)
{
    rb_vm_t *vm = GET_VM();
    VALUE trap_exit = vm->trap_list[0].cmd;

    if (trap_exit) {
	vm->trap_list[0].cmd = 0;
	signal_exec(trap_exit, vm->trap_list[0].safe, 0);
    }
}

void
rb_signal_exec(rb_thread_t *th, int sig)
{
    rb_vm_t *vm = GET_VM();
    VALUE cmd = vm->trap_list[sig].cmd;
    int safe = vm->trap_list[sig].safe;

    if (cmd == 0) {
	switch (sig) {
	  case SIGINT:
	    rb_interrupt();
	    break;
#ifdef SIGHUP
	  case SIGHUP:
#endif
#ifdef SIGQUIT
	  case SIGQUIT:
#endif
#ifdef SIGTERM
	  case SIGTERM:
#endif
#ifdef SIGALRM
	  case SIGALRM:
#endif
#ifdef SIGUSR1
	  case SIGUSR1:
#endif
#ifdef SIGUSR2
	  case SIGUSR2:
#endif
	    rb_threadptr_signal_raise(th, sig);
	    break;
	}
    }
    else if (cmd == Qundef) {
	rb_threadptr_signal_exit(th);
    }
    else {
	signal_exec(cmd, safe, sig);
    }
}

struct trap_arg {
#if USE_TRAP_MASK
    sigset_t mask;
#endif
    int sig;
    sighandler_t func;
    VALUE cmd;
};

static sighandler_t
default_handler(int sig)
{
    sighandler_t func;
    switch (sig) {
      case SIGINT:
#ifdef SIGHUP
      case SIGHUP:
#endif
#ifdef SIGQUIT
      case SIGQUIT:
#endif
#ifdef SIGTERM
      case SIGTERM:
#endif
#ifdef SIGALRM
      case SIGALRM:
#endif
#ifdef SIGUSR1
      case SIGUSR1:
#endif
#ifdef SIGUSR2
      case SIGUSR2:
#endif
        func = sighandler;
        break;
#ifdef SIGBUS
      case SIGBUS:
        func = (sighandler_t)sigbus;
        break;
#endif
#ifdef SIGSEGV
      case SIGSEGV:
        func = (sighandler_t)sigsegv;
# ifdef USE_SIGALTSTACK
        rb_register_sigaltstack(GET_THREAD());
# endif
        break;
#endif
#ifdef SIGPIPE
      case SIGPIPE:
        func = SIG_IGN;
        break;
#endif
      default:
        func = SIG_DFL;
        break;
    }

    return func;
}

static sighandler_t
trap_handler(VALUE *cmd, int sig)
{
    sighandler_t func = sighandler;
    VALUE command;

    if (NIL_P(*cmd)) {
	func = SIG_IGN;
    }
    else {
	command = rb_check_string_type(*cmd);
	if (NIL_P(command) && SYMBOL_P(*cmd)) {
	    command = rb_id2str(SYM2ID(*cmd));
	    if (!command) rb_raise(rb_eArgError, "bad handler");
	}
	if (!NIL_P(command)) {
	    SafeStringValue(command);	/* taint check */
	    *cmd = command;
	    switch (RSTRING_LEN(command)) {
	      case 0:
                goto sig_ign;
		break;
              case 14:
		if (strncmp(RSTRING_PTR(command), "SYSTEM_DEFAULT", 14) == 0) {
                    func = SIG_DFL;
                    *cmd = 0;
		}
                break;
	      case 7:
		if (strncmp(RSTRING_PTR(command), "SIG_IGN", 7) == 0) {
sig_ign:
                    func = SIG_IGN;
                    *cmd = 0;
		}
		else if (strncmp(RSTRING_PTR(command), "SIG_DFL", 7) == 0) {
sig_dfl:
                    func = default_handler(sig);
                    *cmd = 0;
		}
		else if (strncmp(RSTRING_PTR(command), "DEFAULT", 7) == 0) {
                    goto sig_dfl;
		}
		break;
	      case 6:
		if (strncmp(RSTRING_PTR(command), "IGNORE", 6) == 0) {
                    goto sig_ign;
		}
		break;
	      case 4:
		if (strncmp(RSTRING_PTR(command), "EXIT", 4) == 0) {
		    *cmd = Qundef;
		}
		break;
	    }
	}
	else {
	    rb_proc_t *proc;
	    GetProcPtr(*cmd, proc);
	    (void)proc;
	}
    }

    return func;
}

static int
trap_signm(VALUE vsig)
{
    int sig = -1;
    const char *s;

    switch (TYPE(vsig)) {
      case T_FIXNUM:
	sig = FIX2INT(vsig);
	if (sig < 0 || sig >= NSIG) {
	    rb_raise(rb_eArgError, "invalid signal number (%d)", sig);
	}
	break;

      case T_SYMBOL:
	s = rb_id2name(SYM2ID(vsig));
	if (!s) rb_raise(rb_eArgError, "bad signal");
	goto str_signal;

      default:
	s = StringValuePtr(vsig);

      str_signal:
	if (strncmp("SIG", s, 3) == 0)
	    s += 3;
	sig = signm2signo(s);
	if (sig == 0 && strcmp(s, "EXIT") != 0)
	    rb_raise(rb_eArgError, "unsupported signal SIG%s", s);
    }
    return sig;
}

static VALUE
trap(struct trap_arg *arg)
{
    sighandler_t oldfunc, func = arg->func;
    VALUE oldcmd, command = arg->cmd;
    int sig = arg->sig;
    rb_vm_t *vm = GET_VM();

    oldfunc = ruby_signal(sig, func);
    oldcmd = vm->trap_list[sig].cmd;
    switch (oldcmd) {
      case 0:
	if (oldfunc == SIG_IGN) oldcmd = rb_str_new2("IGNORE");
	else if (oldfunc == sighandler) oldcmd = rb_str_new2("DEFAULT");
	else oldcmd = Qnil;
	break;
      case Qundef:
	oldcmd = rb_str_new2("EXIT");
	break;
    }

    vm->trap_list[sig].cmd = command;
    vm->trap_list[sig].safe = rb_safe_level();
    /* enable at least specified signal. */
#if USE_TRAP_MASK
    sigdelset(&arg->mask, sig);
#endif
    return oldcmd;
}

#if USE_TRAP_MASK
static VALUE
trap_ensure(struct trap_arg *arg)
{
    /* enable interrupt */
    pthread_sigmask(SIG_SETMASK, &arg->mask, NULL);
    return 0;
}
#endif

static int
reserved_signal_p(int signo)
{
/* Synchronous signal can't deliver to main thread */
#ifdef SIGSEGV
    if (signo == SIGSEGV)
	return 1;
#endif
#ifdef SIGBUS
    if (signo == SIGBUS)
	return 1;
#endif
#ifdef SIGILL
    if (signo == SIGILL)
	return 1;
#endif
#ifdef SIGFPE
    if (signo == SIGFPE)
	return 1;
#endif

/* used ubf internal see thread_pthread.c. */
#ifdef SIGVTALRM
    if (signo == SIGVTALRM)
	return 1;
#endif

    return 0;
}

/*
 * call-seq:
 *   Signal.trap( signal, command ) -> obj
 *   Signal.trap( signal ) {| | block } -> obj
 *
 * Specifies the handling of signals. The first parameter is a signal
 * name (a string such as ``SIGALRM'', ``SIGUSR1'', and so on) or a
 * signal number. The characters ``SIG'' may be omitted from the
 * signal name. The command or block specifies code to be run when the
 * signal is raised.
 * If the command is the string ``IGNORE'' or ``SIG_IGN'', the signal
 * will be ignored.
 * If the command is ``DEFAULT'' or ``SIG_DFL'', the Ruby's default handler
 * will be invoked.
 * If the command is ``EXIT'', the script will be terminated by the signal.
 * If the command is ``SYSTEM_DEFAULT'', the operating system's default
 * handler will be invoked.
 * Otherwise, the given command or block will be run.
 * The special signal name ``EXIT'' or signal number zero will be
 * invoked just prior to program termination.
 * trap returns the previous handler for the given signal.
 *
 *     Signal.trap(0, proc { puts "Terminating: #{$$}" })
 *     Signal.trap("CLD")  { puts "Child died" }
 *     fork && Process.wait
 *
 * produces:
 *     Terminating: 27461
 *     Child died
 *     Terminating: 27460
 */
static VALUE
sig_trap(int argc, VALUE *argv)
{
    struct trap_arg arg;

    rb_secure(2);
    rb_check_arity(argc, 1, 2);

    arg.sig = trap_signm(argv[0]);
    if (reserved_signal_p(arg.sig)) {
        const char *name = signo2signm(arg.sig);
        if (name)
            rb_raise(rb_eArgError, "can't trap reserved signal: SIG%s", name);
        else
            rb_raise(rb_eArgError, "can't trap reserved signal: %d", (int)arg.sig);
    }

    if (argc == 1) {
	arg.cmd = rb_block_proc();
	arg.func = sighandler;
    }
    else {
	arg.cmd = argv[1];
	arg.func = trap_handler(&arg.cmd, arg.sig);
    }

    if (OBJ_TAINTED(arg.cmd)) {
	rb_raise(rb_eSecurityError, "Insecure: tainted signal trap");
    }
#if USE_TRAP_MASK
    {
      sigset_t fullmask;

      /* disable interrupt */
      sigfillset(&fullmask);
      pthread_sigmask(SIG_BLOCK, &fullmask, &arg.mask);

      return rb_ensure(trap, (VALUE)&arg, trap_ensure, (VALUE)&arg);
    }
#else
    return trap(&arg);
#endif
}

/*
 * call-seq:
 *   Signal.list -> a_hash
 *
 * Returns a list of signal names mapped to the corresponding
 * underlying signal numbers.
 *
 *   Signal.list   #=> {"EXIT"=>0, "HUP"=>1, "INT"=>2, "QUIT"=>3, "ILL"=>4, "TRAP"=>5, "IOT"=>6, "ABRT"=>6, "FPE"=>8, "KILL"=>9, "BUS"=>7, "SEGV"=>11, "SYS"=>31, "PIPE"=>13, "ALRM"=>14, "TERM"=>15, "URG"=>23, "STOP"=>19, "TSTP"=>20, "CONT"=>18, "CHLD"=>17, "CLD"=>17, "TTIN"=>21, "TTOU"=>22, "IO"=>29, "XCPU"=>24, "XFSZ"=>25, "VTALRM"=>26, "PROF"=>27, "WINCH"=>28, "USR1"=>10, "USR2"=>12, "PWR"=>30, "POLL"=>29}
 */
static VALUE
sig_list(void)
{
    VALUE h = rb_hash_new();
    const struct signals *sigs;

    for (sigs = siglist; sigs->signm; sigs++) {
	rb_hash_aset(h, rb_str_new2(sigs->signm), INT2FIX(sigs->signo));
    }
    return h;
}

static void
install_sighandler(int signum, sighandler_t handler)
{
    sighandler_t old;

    rb_disable_interrupt();
    old = ruby_signal(signum, handler);
    if (old != SIG_DFL) {
	ruby_signal(signum, old);
    }
    rb_enable_interrupt();
}

#if defined(SIGCLD) || defined(SIGCHLD)
static void
init_sigchld(int sig)
{
    sighandler_t oldfunc;

    rb_disable_interrupt();
    oldfunc = ruby_signal(sig, SIG_DFL);
    if (oldfunc != SIG_DFL && oldfunc != SIG_IGN) {
	ruby_signal(sig, oldfunc);
    } else {
	GET_VM()->trap_list[sig].cmd = 0;
    }
    rb_enable_interrupt();
}
#endif

void
ruby_sig_finalize(void)
{
    sighandler_t oldfunc;

    oldfunc = ruby_signal(SIGINT, SIG_IGN);
    if (oldfunc == sighandler) {
	ruby_signal(SIGINT, SIG_DFL);
    }
}


int ruby_enable_coredump = 0;
#ifndef RUBY_DEBUG_ENV
#define ruby_enable_coredump 0
#endif

/*
 * Many operating systems allow signals to be sent to running
 * processes. Some signals have a defined effect on the process, while
 * others may be trapped at the code level and acted upon. For
 * example, your process may trap the USR1 signal and use it to toggle
 * debugging, and may use TERM to initiate a controlled shutdown.
 *
 *     pid = fork do
 *       Signal.trap("USR1") do
 *         $debug = !$debug
 *         puts "Debug now: #$debug"
 *       end
 *       Signal.trap("TERM") do
 *         puts "Terminating..."
 *         shutdown()
 *       end
 *       # . . . do some work . . .
 *     end
 *
 *     Process.detach(pid)
 *
 *     # Controlling program:
 *     Process.kill("USR1", pid)
 *     # ...
 *     Process.kill("USR1", pid)
 *     # ...
 *     Process.kill("TERM", pid)
 *
 * produces:
 *     Debug now: true
 *     Debug now: false
 *    Terminating...
 *
 * The list of available signal names and their interpretation is
 * system dependent. Signal delivery semantics may also vary between
 * systems; in particular signal delivery may not always be reliable.
 */
void
Init_signal(void)
{
    VALUE mSignal = rb_define_module("Signal");

    rb_define_global_function("trap", sig_trap, -1);
    rb_define_module_function(mSignal, "trap", sig_trap, -1);
    rb_define_module_function(mSignal, "list", sig_list, 0);

    rb_define_method(rb_eSignal, "initialize", esignal_init, -1);
    rb_define_method(rb_eSignal, "signo", esignal_signo, 0);
    rb_alias(rb_eSignal, rb_intern("signm"), rb_intern("message"));
    rb_define_method(rb_eInterrupt, "initialize", interrupt_init, -1);

    install_sighandler(SIGINT, sighandler);
#ifdef SIGHUP
    install_sighandler(SIGHUP, sighandler);
#endif
#ifdef SIGQUIT
    install_sighandler(SIGQUIT, sighandler);
#endif
#ifdef SIGTERM
    install_sighandler(SIGTERM, sighandler);
#endif
#ifdef SIGALRM
    install_sighandler(SIGALRM, sighandler);
#endif
#ifdef SIGUSR1
    install_sighandler(SIGUSR1, sighandler);
#endif
#ifdef SIGUSR2
    install_sighandler(SIGUSR2, sighandler);
#endif

    if (!ruby_enable_coredump) {
#ifdef SIGBUS
	install_sighandler(SIGBUS, (sighandler_t)sigbus);
#endif
#ifdef SIGSEGV
# ifdef USE_SIGALTSTACK
	rb_register_sigaltstack(GET_THREAD());
# endif
	install_sighandler(SIGSEGV, (sighandler_t)sigsegv);
#endif
    }
#ifdef SIGPIPE
    install_sighandler(SIGPIPE, SIG_IGN);
#endif

#if defined(SIGCLD)
    init_sigchld(SIGCLD);
#elif defined(SIGCHLD)
    init_sigchld(SIGCHLD);
#endif
}
