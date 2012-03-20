/*
 * UNIX Syslog extension for Ruby
 * Amos Gouaux, University of Texas at Dallas
 * <amos+ruby@utdallas.edu>
 * Documented by mathew <meta@pobox.com>
 *
 * $RoughId: syslog.c,v 1.21 2002/02/25 12:21:17 knu Exp $
 * $Id$
 */

#include "ruby/ruby.h"
#include "ruby/util.h"
#include <syslog.h>

/* Syslog class */
static VALUE mSyslog, mSyslogConstants;
static const char *syslog_ident = NULL;
static int syslog_options = -1, syslog_facility = -1, syslog_mask = -1;
static int syslog_opened = 0;

/* Package helper routines */
static void syslog_write(int pri, int argc, VALUE *argv)
{
    VALUE str;

    rb_secure(4);
    if (argc < 1) {
        rb_raise(rb_eArgError, "no log message supplied");
    }

    if (!syslog_opened) {
        rb_raise(rb_eRuntimeError, "must open syslog before write");
    }

    str = rb_f_sprintf(argc, argv);

    syslog(pri, "%s", RSTRING_PTR(str));
}

/* Closes the syslog facility.
 * Raises a runtime exception if it is not open.
 */
static VALUE mSyslog_close(VALUE self)
{
    rb_secure(4);
    if (!syslog_opened) {
        rb_raise(rb_eRuntimeError, "syslog not opened");
    }

    closelog();

    xfree((void *)syslog_ident);
    syslog_ident = NULL;
    syslog_options = syslog_facility = syslog_mask = -1;
    syslog_opened = 0;

    return Qnil;
}

/* call-seq:
 *   open(ident, options, facility) => syslog
 *
 * :yields: syslog
 *
 * Open the syslog facility.
 * Raises a runtime exception if it is already open.
 *
 * Can be called with or without a code block. If called with a block, the
 * Syslog object created is passed to the block.
 *
 * If the syslog is already open, raises a RuntimeError.
 *
 * +ident+ is a String which identifies the calling program.
 *
 * +options+ is the logical OR of any of the following:
 *
 * LOG_CONS:: If there is an error while sending to the system logger,
 *            write directly to the console instead.
 *
 * LOG_NDELAY:: Open the connection now, rather than waiting for the first
 *              message to be written.
 *
 * LOG_NOWAIT:: Don't wait for any child processes created while logging
 *              messages. (Has no effect on Linux.)
 *
 * LOG_ODELAY:: Opposite of LOG_NDELAY; wait until a message is sent before
 *              opening the connection. (This is the default.)
 *
 * LOG_PERROR:: Print the message to stderr as well as sending it to syslog.
 *              (Not in POSIX.1-2001.)
 *
 * LOG_PID:: Include the current process ID with each message.
 *
 * +facility+ describes the type of program opening the syslog, and is
 * the logical OR of any of the following which are defined for the host OS:
 *
 * LOG_AUTH:: Security or authorization. Deprecated, use LOG_AUTHPRIV
 *            instead.
 *
 * LOG_AUTHPRIV:: Security or authorization messages which should be kept
 *                private.
 *
 * LOG_CONSOLE:: System console message.
 *
 * LOG_CRON:: System task scheduler (cron or at).
 *
 * LOG_DAEMON:: A system daemon which has no facility value of its own.
 *
 * LOG_FTP:: An FTP server.
 *
 * LOG_KERN:: A kernel message (not sendable by user processes, so not of
 *            much use to Ruby, but listed here for completeness).
 *
 * LOG_LPR:: Line printer subsystem.
 *
 * LOG_MAIL:: Mail delivery or transport subsystem.
 *
 * LOG_NEWS:: Usenet news system.
 *
 * LOG_NTP:: Network Time Protocol server.
 *
 * LOG_SECURITY:: General security message.
 *
 * LOG_SYSLOG:: Messages generated internally by syslog.
 *
 * LOG_USER:: Generic user-level message.
 *
 * LOG_UUCP:: UUCP subsystem.
 *
 * LOG_LOCAL0 to LOG_LOCAL7:: Locally-defined facilities.
 *
 * Example:
 *
 *  Syslog.open("webrick", Syslog::LOG_PID,
 *              Syslog::LOG_DAEMON | Syslog::LOG_LOCAL3)
 *
 */
static VALUE mSyslog_open(int argc, VALUE *argv, VALUE self)
{
    VALUE ident, opt, fac;

    if (syslog_opened) {
        rb_raise(rb_eRuntimeError, "syslog already open");
    }

    rb_scan_args(argc, argv, "03", &ident, &opt, &fac);

    if (NIL_P(ident)) {
        ident = rb_gv_get("$0");
    }
    SafeStringValue(ident);
    syslog_ident = strdup(RSTRING_PTR(ident));

    if (NIL_P(opt)) {
	syslog_options = LOG_PID | LOG_CONS;
    } else {
	syslog_options = NUM2INT(opt);
    }

    if (NIL_P(fac)) {
	syslog_facility = LOG_USER;
    } else {
	syslog_facility = NUM2INT(fac);
    }

    openlog(syslog_ident, syslog_options, syslog_facility);

    syslog_opened = 1;

    setlogmask(syslog_mask = setlogmask(0));

    /* be like File.new.open {...} */
    if (rb_block_given_p()) {
        rb_ensure(rb_yield, self, mSyslog_close, self);
    }

    return self;
}

/* call-seq:
 *   reopen(ident, options, facility) => syslog
 *
 * :yields: syslog
 *
 * Closes and then reopens the syslog.
 *
 * Arguments are the same as for open().
 */
static VALUE mSyslog_reopen(int argc, VALUE *argv, VALUE self)
{
    mSyslog_close(self);

    return mSyslog_open(argc, argv, self);
}

/* call-seq:
 *   opened?
 *
 * Returns true if the syslog is open.
 */
static VALUE mSyslog_isopen(VALUE self)
{
    return syslog_opened ? Qtrue : Qfalse;
}

/* Returns the identity string used in the last call to open()
 */
static VALUE mSyslog_ident(VALUE self)
{
    return syslog_opened ? rb_str_new2(syslog_ident) : Qnil;
}

/* Returns the options bitmask used in the last call to open()
 */
static VALUE mSyslog_options(VALUE self)
{
    return syslog_opened ? INT2NUM(syslog_options) : Qnil;
}

/* Returns the facility number used in the last call to open()
 */
static VALUE mSyslog_facility(VALUE self)
{
    return syslog_opened ? INT2NUM(syslog_facility) : Qnil;
}

/* Returns the log priority mask in effect. The mask is not reset by opening
 * or closing syslog.
 */
static VALUE mSyslog_get_mask(VALUE self)
{
    return syslog_opened ? INT2NUM(syslog_mask) : Qnil;
}

/* call-seq:
 *   mask=(priority_mask)
 *
 * Sets the log priority mask. A method LOG_UPTO is defined to make it easier
 * to set mask values. Example:
 *
 *   Syslog.mask = Syslog::LOG_UPTO(Syslog::LOG_ERR)
 *
 * Alternatively, specific priorities can be selected and added together using
 * binary OR. Example:
 *
 *   Syslog.mask = Syslog::LOG_MASK(Syslog::LOG_ERR) | Syslog::LOG_MASK(Syslog::LOG_CRIT)
 *
 * The priority mask persists through calls to open() and close().
 */
static VALUE mSyslog_set_mask(VALUE self, VALUE mask)
{
    rb_secure(4);
    if (!syslog_opened) {
        rb_raise(rb_eRuntimeError, "must open syslog before setting log mask");
    }

    setlogmask(syslog_mask = NUM2INT(mask));

    return mask;
}

/* call-seq:
 *   log(priority, format_string, *format_args)
 *
 * Log a message with the specified priority. Example:
 *
 *   Syslog.log(Syslog::LOG_CRIT, "Out of disk space")
 *   Syslog.log(Syslog::LOG_CRIT, "User %s logged in", ENV['USER'])
 *
 * The priority levels, in descending order, are:
 *
 * LOG_EMERG::   System is unusable
 * LOG_ALERT::   Action needs to be taken immediately
 * LOG_CRIT::    A critical condition has occurred
 * LOG_ERR::     An error occurred
 * LOG_WARNING:: Warning of a possible problem
 * LOG_NOTICE::  A normal but significant condition occurred
 * LOG_INFO::    Informational message
 * LOG_DEBUG::   Debugging information
 *
 * Each priority level also has a shortcut method that logs with it's named priority.
 * As an example, the two following statements would produce the same result:
 *
 *   Syslog.log(Syslog::LOG_ALERT, "Out of memory")
 *   Syslog.alert("Out of memory")
 *
 * Format strings are as for printf/sprintf, except that in addition %m is
 * replaced with the error message string that would be returned by
 * strerror(errno).
 *
 */
static VALUE mSyslog_log(int argc, VALUE *argv, VALUE self)
{
    VALUE pri;

    if (argc < 2) {
        rb_raise(rb_eArgError, "wrong number of arguments (%d for 2+)", argc);
    }

    argc--;
    pri = *argv++;

    if (!FIXNUM_P(pri)) {
      rb_raise(rb_eTypeError, "type mismatch: %s given", rb_class2name(CLASS_OF(pri)));
    }

    syslog_write(FIX2INT(pri), argc, argv);

    return self;
}

/* Returns an inspect() string summarizing the object state.
 */
static VALUE mSyslog_inspect(VALUE self)
{
    char buf[1024];

    if (syslog_opened) {
	snprintf(buf, sizeof(buf),
	  "<#%s: opened=true, ident=\"%s\", options=%d, facility=%d, mask=%d>",
	  rb_class2name(self),
	  syslog_ident,
	  syslog_options,
	  syslog_facility,
	  syslog_mask);
    } else {
	snprintf(buf, sizeof(buf),
	  "<#%s: opened=false>", rb_class2name(self));
    }

    return rb_str_new2(buf);
}

/* Returns self, for backward compatibility.
 */
static VALUE mSyslog_instance(VALUE self)
{
    return self;
}

#define define_syslog_shortcut_method(pri, name) \
static VALUE mSyslog_##name(int argc, VALUE *argv, VALUE self) \
{ \
    syslog_write((pri), argc, argv); \
\
    return self; \
}

#ifdef LOG_EMERG
define_syslog_shortcut_method(LOG_EMERG, emerg)
#endif
#ifdef LOG_ALERT
define_syslog_shortcut_method(LOG_ALERT, alert)
#endif
#ifdef LOG_CRIT
define_syslog_shortcut_method(LOG_CRIT, crit)
#endif
#ifdef LOG_ERR
define_syslog_shortcut_method(LOG_ERR, err)
#endif
#ifdef LOG_WARNING
define_syslog_shortcut_method(LOG_WARNING, warning)
#endif
#ifdef LOG_NOTICE
define_syslog_shortcut_method(LOG_NOTICE, notice)
#endif
#ifdef LOG_INFO
define_syslog_shortcut_method(LOG_INFO, info)
#endif
#ifdef LOG_DEBUG
define_syslog_shortcut_method(LOG_DEBUG, debug)
#endif

/* call-seq:
 *   LOG_MASK(priority_level) => priority_mask
 *
 * Generates a mask bit for a priority level. See #mask=
 */
static VALUE mSyslogConstants_LOG_MASK(VALUE klass, VALUE pri)
{
    return INT2FIX(LOG_MASK(NUM2INT(pri)));
}

/* call-seq:
 *   LOG_UPTO(priority_level) => priority_mask
 *
 * Generates a mask value for priority levels at or below the level specified.
 * See #mask=
 */
static VALUE mSyslogConstants_LOG_UPTO(VALUE klass, VALUE pri)
{
    return INT2FIX(LOG_UPTO(NUM2INT(pri)));
}

/* The syslog package provides a Ruby interface to the POSIX system logging
 * facility.
 *
 * Syslog messages are typically passed to a central logging daemon.
 * The daemon may filter them; route them into different files (usually
 * found under /var/log); place them in SQL databases; forward
 * them to centralized logging servers via TCP or UDP; or even alert the
 * system administrator via email, pager or text message.
 *
 * Unlike application-level logging via Logger or Log4r, syslog is designed
 * to allow secure tamper-proof logging.
 *
 * The syslog protocol is standardized in RFC 5424.
 */
void Init_syslog()
{
    mSyslog = rb_define_module("Syslog");

    /* Document-module: Syslog::Constants
     *
     * Module holding Syslog constants.  See Syslog::log and Syslog::open for
     * constant descriptions.
     */
    mSyslogConstants = rb_define_module_under(mSyslog, "Constants");

    rb_include_module(mSyslog, mSyslogConstants);

    rb_define_module_function(mSyslog, "open", mSyslog_open, -1);
    rb_define_module_function(mSyslog, "reopen", mSyslog_reopen, -1);
    rb_define_module_function(mSyslog, "open!", mSyslog_reopen, -1);
    rb_define_module_function(mSyslog, "opened?", mSyslog_isopen, 0);

    rb_define_module_function(mSyslog, "ident", mSyslog_ident, 0);
    rb_define_module_function(mSyslog, "options", mSyslog_options, 0);
    rb_define_module_function(mSyslog, "facility", mSyslog_facility, 0);

    rb_define_module_function(mSyslog, "log", mSyslog_log, -1);
    rb_define_module_function(mSyslog, "close", mSyslog_close, 0);
    rb_define_module_function(mSyslog, "mask", mSyslog_get_mask, 0);
    rb_define_module_function(mSyslog, "mask=", mSyslog_set_mask, 1);

    rb_define_module_function(mSyslog, "LOG_MASK", mSyslogConstants_LOG_MASK, 1);
    rb_define_module_function(mSyslog, "LOG_UPTO", mSyslogConstants_LOG_UPTO, 1);

    rb_define_module_function(mSyslog, "inspect", mSyslog_inspect, 0);
    rb_define_module_function(mSyslog, "instance", mSyslog_instance, 0);

    rb_define_module_function(mSyslogConstants, "LOG_MASK", mSyslogConstants_LOG_MASK, 1);
    rb_define_module_function(mSyslogConstants, "LOG_UPTO", mSyslogConstants_LOG_UPTO, 1);

#define rb_define_syslog_const(id) \
    rb_define_const(mSyslogConstants, #id, INT2NUM(id))

    /* Various options when opening log */
#ifdef LOG_PID
    rb_define_syslog_const(LOG_PID);
#endif
#ifdef LOG_CONS
    rb_define_syslog_const(LOG_CONS);
#endif
#ifdef LOG_ODELAY
    rb_define_syslog_const(LOG_ODELAY); /* deprecated */
#endif
#ifdef LOG_NDELAY
    rb_define_syslog_const(LOG_NDELAY);
#endif
#ifdef LOG_NOWAIT
    rb_define_syslog_const(LOG_NOWAIT); /* deprecated */
#endif
#ifdef LOG_PERROR
    rb_define_syslog_const(LOG_PERROR);
#endif

    /* Various syslog facilities */
#ifdef LOG_AUTH
    rb_define_syslog_const(LOG_AUTH);
#endif
#ifdef LOG_AUTHPRIV
    rb_define_syslog_const(LOG_AUTHPRIV);
#endif
#ifdef LOG_CONSOLE
    rb_define_syslog_const(LOG_CONSOLE);
#endif
#ifdef LOG_CRON
    rb_define_syslog_const(LOG_CRON);
#endif
#ifdef LOG_DAEMON
    rb_define_syslog_const(LOG_DAEMON);
#endif
#ifdef LOG_FTP
    rb_define_syslog_const(LOG_FTP);
#endif
#ifdef LOG_KERN
    rb_define_syslog_const(LOG_KERN);
#endif
#ifdef LOG_LPR
    rb_define_syslog_const(LOG_LPR);
#endif
#ifdef LOG_MAIL
    rb_define_syslog_const(LOG_MAIL);
#endif
#ifdef LOG_NEWS
    rb_define_syslog_const(LOG_NEWS);
#endif
#ifdef LOG_NTP
   rb_define_syslog_const(LOG_NTP);
#endif
#ifdef LOG_SECURITY
    rb_define_syslog_const(LOG_SECURITY);
#endif
#ifdef LOG_SYSLOG
    rb_define_syslog_const(LOG_SYSLOG);
#endif
#ifdef LOG_USER
    rb_define_syslog_const(LOG_USER);
#endif
#ifdef LOG_UUCP
    rb_define_syslog_const(LOG_UUCP);
#endif
#ifdef LOG_LOCAL0
    rb_define_syslog_const(LOG_LOCAL0);
#endif
#ifdef LOG_LOCAL1
    rb_define_syslog_const(LOG_LOCAL1);
#endif
#ifdef LOG_LOCAL2
    rb_define_syslog_const(LOG_LOCAL2);
#endif
#ifdef LOG_LOCAL3
    rb_define_syslog_const(LOG_LOCAL3);
#endif
#ifdef LOG_LOCAL4
    rb_define_syslog_const(LOG_LOCAL4);
#endif
#ifdef LOG_LOCAL5
    rb_define_syslog_const(LOG_LOCAL5);
#endif
#ifdef LOG_LOCAL6
    rb_define_syslog_const(LOG_LOCAL6);
#endif
#ifdef LOG_LOCAL7
    rb_define_syslog_const(LOG_LOCAL7);
#endif

#define rb_define_syslog_shortcut(name) \
    rb_define_module_function(mSyslog, #name, mSyslog_##name, -1)

    /* Various syslog priorities and the shortcut methods */
#ifdef LOG_EMERG
    rb_define_syslog_const(LOG_EMERG);
    rb_define_syslog_shortcut(emerg);
#endif
#ifdef LOG_ALERT
    rb_define_syslog_const(LOG_ALERT);
    rb_define_syslog_shortcut(alert);
#endif
#ifdef LOG_CRIT
    rb_define_syslog_const(LOG_CRIT);
    rb_define_syslog_shortcut(crit);
#endif
#ifdef LOG_ERR
    rb_define_syslog_const(LOG_ERR);
    rb_define_syslog_shortcut(err);
#endif
#ifdef LOG_WARNING
    rb_define_syslog_const(LOG_WARNING);
    rb_define_syslog_shortcut(warning);
#endif
#ifdef LOG_NOTICE
    rb_define_syslog_const(LOG_NOTICE);
    rb_define_syslog_shortcut(notice);
#endif
#ifdef LOG_INFO
    rb_define_syslog_const(LOG_INFO);
    rb_define_syslog_shortcut(info);
#endif
#ifdef LOG_DEBUG
    rb_define_syslog_const(LOG_DEBUG);
    rb_define_syslog_shortcut(debug);
#endif
}
