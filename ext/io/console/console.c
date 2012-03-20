/* -*- c-file-style: "ruby" -*- */
/*
 * console IO module
 */
#include "ruby.h"
#ifdef HAVE_RUBY_IO_H
#include "ruby/io.h"
#else
#include "rubyio.h"
#endif

#ifndef HAVE_RB_IO_T
typedef OpenFile rb_io_t;
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#if defined HAVE_TERMIOS_H
# include <termios.h>
typedef struct termios conmode;

static int
setattr(int fd, conmode *t)
{
    while (tcsetattr(fd, TCSAFLUSH, t)) {
	if (errno != EINTR) return 0;
    }
    return 1;
}
# define getattr(fd, t) (tcgetattr(fd, t) == 0)
#elif defined HAVE_TERMIO_H
# include <termio.h>
typedef struct termio conmode;
# define setattr(fd, t) (ioctl(fd, TCSETAF, t) == 0)
# define getattr(fd, t) (ioctl(fd, TCGETA, t) == 0)
#elif defined HAVE_SGTTY_H
# include <sgtty.h>
typedef struct sgttyb conmode;
# ifdef HAVE_STTY
# define setattr(fd, t)  (stty(fd, t) == 0)
# else
# define setattr(fd, t)  (ioctl((fd), TIOCSETP, (t)) == 0)
# endif
# ifdef HAVE_GTTY
# define getattr(fd, t)  (gtty(fd, t) == 0)
# else
# define getattr(fd, t)  (ioctl((fd), TIOCGETP, (t)) == 0)
# endif
#elif defined _WIN32
#include <winioctl.h>
typedef DWORD conmode;

#ifdef HAVE_RB_W32_MAP_ERRNO
#define LAST_ERROR rb_w32_map_errno(GetLastError())
#else
#define LAST_ERROR EBADF
#endif
#define SET_LAST_ERROR (errno = LAST_ERROR, 0)

static int
setattr(int fd, conmode *t)
{
    int x = SetConsoleMode((HANDLE)rb_w32_get_osfhandle(fd), *t);
    if (!x) errno = LAST_ERROR;
    return x;
}

static int
getattr(int fd, conmode *t)
{
    int x = GetConsoleMode((HANDLE)rb_w32_get_osfhandle(fd), t);
    if (!x) errno = LAST_ERROR;
    return x;
}
#endif
#ifndef SET_LAST_ERROR
#define SET_LAST_ERROR (0)
#endif

#ifndef InitVM
#define InitVM(ext) {void InitVM_##ext(void);InitVM_##ext();}
#endif

static ID id_getc, id_console;

typedef struct {
    int vmin;
    int vtime;
} rawmode_arg_t;

static rawmode_arg_t *
rawmode_opt(int argc, VALUE *argv, rawmode_arg_t *opts)
{
    rawmode_arg_t *optp = NULL;
    VALUE vopts;
    rb_scan_args(argc, argv, "0:", &vopts);
    if (!NIL_P(vopts)) {
	VALUE vmin = rb_hash_aref(vopts, ID2SYM(rb_intern("min")));
	VALUE vtime = rb_hash_aref(vopts, ID2SYM(rb_intern("time")));
	VALUE v10 = INT2FIX(10);
	if (!NIL_P(vmin)) {
	    vmin = rb_funcall3(vmin, '*', 1, &v10);
	    opts->vmin = NUM2INT(vmin);
	    optp = opts;
	}
	if (!NIL_P(vtime)) {
	    vtime = rb_funcall3(vtime, '*', 1, &v10);
	    opts->vtime = NUM2INT(vtime);
	    optp = opts;
	}
    }
    return optp;
}

static void
set_rawmode(conmode *t, void *arg)
{
#ifdef HAVE_CFMAKERAW
    cfmakeraw(t);
    t->c_lflag &= ~(ECHOE|ECHOK);
#elif defined HAVE_TERMIOS_H || defined HAVE_TERMIO_H
    t->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
    t->c_oflag &= ~OPOST;
    t->c_lflag &= ~(ECHO|ECHOE|ECHOK|ECHONL|ICANON|ISIG|IEXTEN);
    t->c_cflag &= ~(CSIZE|PARENB);
    t->c_cflag |= CS8;
#elif defined HAVE_SGTTY_H
    t->sg_flags &= ~ECHO;
    t->sg_flags |= RAW;
#elif defined _WIN32
    *t = 0;
#endif
#if defined HAVE_TERMIOS_H || defined HAVE_TERMIO_H
    if (arg) {
	const rawmode_arg_t *r = arg;
	if (r->vmin >= 0) t->c_cc[VMIN] = r->vmin;
	if (r->vtime >= 0) t->c_cc[VTIME] = r->vtime;
    }
#endif
}

static void
set_cookedmode(conmode *t, void *arg)
{
#if defined HAVE_TERMIOS_H || defined HAVE_TERMIO_H
    t->c_iflag |= (BRKINT|ISTRIP|ICRNL|IXON);
    t->c_oflag |= OPOST;
    t->c_lflag |= (ECHO|ECHOE|ECHOK|ECHONL|ICANON|ISIG|IEXTEN);
#elif defined HAVE_SGTTY_H
    t->sg_flags |= ECHO;
    t->sg_flags &= ~RAW;
#elif defined _WIN32
    *t |= ENABLE_ECHO_INPUT|ENABLE_LINE_INPUT|ENABLE_PROCESSED_INPUT;
#endif
}

static void
set_noecho(conmode *t, void *arg)
{
#if defined HAVE_TERMIOS_H || defined HAVE_TERMIO_H
    t->c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
#elif defined HAVE_SGTTY_H
    t->sg_flags &= ~ECHO;
#elif defined _WIN32
    *t &= ~ENABLE_ECHO_INPUT;
#endif
}

static void
set_echo(conmode *t, void *arg)
{
#if defined HAVE_TERMIOS_H || defined HAVE_TERMIO_H
    t->c_lflag |= (ECHO | ECHOE | ECHOK | ECHONL);
#elif defined HAVE_SGTTY_H
    t->sg_flags |= ECHO;
#elif defined _WIN32
    *t |= ENABLE_ECHO_INPUT;
#endif
}

static int
echo_p(conmode *t)
{
#if defined HAVE_TERMIOS_H || defined HAVE_TERMIO_H
    return (t->c_lflag & (ECHO | ECHONL)) != 0;
#elif defined HAVE_SGTTY_H
    return (t->sg_flags & ECHO) != 0;
#elif defined _WIN32
    return (*t & ENABLE_ECHO_INPUT) != 0;
#endif
}

static int
set_ttymode(int fd, conmode *t, void (*setter)(conmode *, void *), void *arg)
{
    conmode r;
    if (!getattr(fd, t)) return 0;
    r = *t;
    setter(&r, arg);
    return setattr(fd, &r);
}

#ifdef GetReadFile
#define GetReadFD(fptr) fileno(GetReadFile(fptr))
#else
#define GetReadFD(fptr) ((fptr)->fd)
#endif

#ifdef GetWriteFile
#define GetWriteFD(fptr) fileno(GetWriteFile(fptr))
#else
static inline int
get_write_fd(const rb_io_t *fptr)
{
    VALUE wio = fptr->tied_io_for_writing;
    rb_io_t *ofptr;
    if (!wio) return fptr->fd;
    GetOpenFile(wio, ofptr);
    return ofptr->fd;
}
#define GetWriteFD(fptr) get_write_fd(fptr)
#endif

#define FD_PER_IO 2

static VALUE
ttymode(VALUE io, VALUE (*func)(VALUE), void (*setter)(conmode *, void *), void *arg)
{
    rb_io_t *fptr;
    int status = -1;
    int error = 0;
    int fd[FD_PER_IO];
    conmode t[FD_PER_IO];
    VALUE result = Qnil;

    GetOpenFile(io, fptr);
    fd[0] = GetReadFD(fptr);
    if (fd[0] != -1) {
	if (set_ttymode(fd[0], t+0, setter, arg)) {
	    status = 0;
	}
	else {
	    error = errno;
	    fd[0] = -1;
	}
    }
    fd[1] = GetWriteFD(fptr);
    if (fd[1] != -1 && fd[1] != fd[0]) {
	if (set_ttymode(fd[1], t+1, setter, arg)) {
	    status = 0;
	}
	else {
	    error = errno;
	    fd[1] = -1;
	}
    }
    if (status == 0) {
	result = rb_protect(func, io, &status);
    }
    GetOpenFile(io, fptr);
    if (fd[0] != -1 && fd[0] == GetReadFD(fptr)) {
	if (!setattr(fd[0], t+0)) {
	    error = errno;
	    status = -1;
	}
    }
    if (fd[1] != -1 && fd[1] != fd[0] && fd[1] == GetWriteFD(fptr)) {
	if (!setattr(fd[1], t+1)) {
	    error = errno;
	    status = -1;
	}
    }
    if (status) {
	if (status == -1) {
	    errno = error;
	    rb_sys_fail(0);
	}
	rb_jump_tag(status);
    }
    return result;
}

/*
 * call-seq:
 *   io.raw(min: nil, time: nil) {|io| }
 *
 * Yields +self+ within raw mode.
 *
 *   STDIN.raw(&:gets)
 *
 * will read and return a line without echo back and line editing.
 *
 * You must require 'io/console' to use this method.
 */
static VALUE
console_raw(int argc, VALUE *argv, VALUE io)
{
    rawmode_arg_t opts, *optp = rawmode_opt(argc, argv, &opts);
    return ttymode(io, rb_yield, set_rawmode, optp);
}

/*
 * call-seq:
 *   io.raw!(min: nil, time: nil)
 *
 * Enables raw mode.
 *
 * If the terminal mode needs to be back, use io.raw { ... }.
 *
 * You must require 'io/console' to use this method.
 */
static VALUE
console_set_raw(int argc, VALUE *argv, VALUE io)
{
    conmode t;
    rb_io_t *fptr;
    int fd;
    rawmode_arg_t opts, *optp = rawmode_opt(argc, argv, &opts);

    GetOpenFile(io, fptr);
    fd = GetReadFD(fptr);
    if (!getattr(fd, &t)) rb_sys_fail(0);
    set_rawmode(&t, optp);
    if (!setattr(fd, &t)) rb_sys_fail(0);
    return io;
}

/*
 * call-seq:
 *   io.cooked {|io| }
 *
 * Yields +self+ within cooked mode.
 *
 *   STDIN.cooked(&:gets)
 *
 * will read and return a line with echo back and line editing.
 *
 * You must require 'io/console' to use this method.
 */
static VALUE
console_cooked(VALUE io)
{
    return ttymode(io, rb_yield, set_cookedmode, NULL);
}

/*
 * call-seq:
 *   io.cooked!
 *
 * Enables cooked mode.
 *
 * If the terminal mode needs to be back, use io.cooked { ... }.
 *
 * You must require 'io/console' to use this method.
 */
static VALUE
console_set_cooked(VALUE io)
{
    conmode t;
    rb_io_t *fptr;
    int fd;

    GetOpenFile(io, fptr);
    fd = GetReadFD(fptr);
    if (!getattr(fd, &t)) rb_sys_fail(0);
    set_cookedmode(&t, NULL);
    if (!setattr(fd, &t)) rb_sys_fail(0);
    return io;
}

static VALUE
getc_call(VALUE io)
{
    return rb_funcall2(io, id_getc, 0, 0);
}

/*
 * call-seq:
 *   io.getch(min: nil, time: nil)       -> char
 *
 * Reads and returns a character in raw mode.
 *
 * You must require 'io/console' to use this method.
 */
static VALUE
console_getch(int argc, VALUE *argv, VALUE io)
{
    rawmode_arg_t opts, *optp = rawmode_opt(argc, argv, &opts);
    return ttymode(io, getc_call, set_rawmode, optp);
}

/*
 * call-seq:
 *   io.noecho {|io| }
 *
 * Yields +self+ with disabling echo back.
 *
 *   STDIN.noecho(&:gets)
 *
 * will read and return a line without echo back.
 *
 * You must require 'io/console' to use this method.
 */
static VALUE
console_noecho(VALUE io)
{
    return ttymode(io, rb_yield, set_noecho, NULL);
}

/*
 * call-seq:
 *   io.echo = flag
 *
 * Enables/disables echo back.
 * On some platforms, all combinations of this flags and raw/cooked
 * mode may not be valid.
 *
 * You must require 'io/console' to use this method.
 */
static VALUE
console_set_echo(VALUE io, VALUE f)
{
    conmode t;
    rb_io_t *fptr;
    int fd;

    GetOpenFile(io, fptr);
    fd = GetReadFD(fptr);
    if (!getattr(fd, &t)) rb_sys_fail(0);
    if (RTEST(f))
	set_echo(&t, NULL);
    else
	set_noecho(&t, NULL);
    if (!setattr(fd, &t)) rb_sys_fail(0);
    return io;
}

/*
 * call-seq:
 *   io.echo?       -> true or false
 *
 * Returns +true+ if echo back is enabled.
 *
 * You must require 'io/console' to use this method.
 */
static VALUE
console_echo_p(VALUE io)
{
    conmode t;
    rb_io_t *fptr;
    int fd;

    GetOpenFile(io, fptr);
    fd = GetReadFD(fptr);
    if (!getattr(fd, &t)) rb_sys_fail(0);
    return echo_p(&t) ? Qtrue : Qfalse;
}

#if defined TIOCGWINSZ
typedef struct winsize rb_console_size_t;
#define getwinsize(fd, buf) (ioctl((fd), TIOCGWINSZ, (buf)) == 0)
#define setwinsize(fd, buf) (ioctl((fd), TIOCSWINSZ, (buf)) == 0)
#define winsize_row(buf) (buf)->ws_row
#define winsize_col(buf) (buf)->ws_col
#elif defined _WIN32
typedef CONSOLE_SCREEN_BUFFER_INFO rb_console_size_t;
#define getwinsize(fd, buf) ( \
    GetConsoleScreenBufferInfo((HANDLE)rb_w32_get_osfhandle(fd), (buf)) || \
    SET_LAST_ERROR)
#define winsize_row(buf) ((buf)->srWindow.Bottom - (buf)->srWindow.Top + 1)
#define winsize_col(buf) (buf)->dwSize.X
#endif

#if defined TIOCGWINSZ || defined _WIN32
#define USE_CONSOLE_GETSIZE 1
#endif

#ifdef USE_CONSOLE_GETSIZE
/*
 * call-seq:
 *   io.winsize     -> [rows, columns]
 *
 * Returns console size.
 *
 * You must require 'io/console' to use this method.
 */
static VALUE
console_winsize(VALUE io)
{
    rb_io_t *fptr;
    int fd;
    rb_console_size_t ws;

    GetOpenFile(io, fptr);
    fd = GetWriteFD(fptr);
    if (!getwinsize(fd, &ws)) rb_sys_fail(0);
    return rb_assoc_new(INT2NUM(winsize_row(&ws)), INT2NUM(winsize_col(&ws)));
}

/*
 * call-seq:
 *   io.winsize = [rows, columns]
 *
 * Tries to set console size.  The effect depends on the platform and
 * the running environment.
 *
 * You must require 'io/console' to use this method.
 */
static VALUE
console_set_winsize(VALUE io, VALUE size)
{
    rb_io_t *fptr;
    rb_console_size_t ws;
#if defined _WIN32
    HANDLE wh;
    int newrow, newcol;
#endif
    VALUE row, col, xpixel, ypixel;
#if defined TIOCSWINSZ
    int fd;
#endif

    GetOpenFile(io, fptr);
    size = rb_Array(size);
    rb_scan_args((int)RARRAY_LEN(size), RARRAY_PTR(size), "22",
                &row, &col, &xpixel, &ypixel);
#if defined TIOCSWINSZ
    fd = GetWriteFD(fptr);
    ws.ws_row = ws.ws_col = ws.ws_xpixel = ws.ws_ypixel = 0;
#define SET(m) ws.ws_##m = NIL_P(m) ? 0 : (unsigned short)NUM2UINT(m)
    SET(row);
    SET(col);
    SET(xpixel);
    SET(ypixel);
#undef SET
    if (!setwinsize(fd, &ws)) rb_sys_fail(0);
#elif defined _WIN32
    wh = (HANDLE)rb_w32_get_osfhandle(GetReadFD(fptr));
    newrow = (SHORT)NUM2UINT(row);
    newcol = (SHORT)NUM2UINT(col);
    if (!getwinsize(GetReadFD(fptr), &ws)) {
	rb_sys_fail("GetConsoleScreenBufferInfo");
    }
    if ((ws.dwSize.X < newcol && (ws.dwSize.X = newcol, 1)) ||
	(ws.dwSize.Y < newrow && (ws.dwSize.Y = newrow, 1))) {
	if (!(SetConsoleScreenBufferSize(wh, ws.dwSize) || SET_LAST_ERROR)) {
	    rb_sys_fail("SetConsoleScreenBufferInfo");
	}
    }
    ws.srWindow.Left = 0;
    ws.srWindow.Top = 0;
    ws.srWindow.Right = newcol;
    ws.srWindow.Bottom = newrow;
    if (!(SetConsoleWindowInfo(wh, FALSE, &ws.srWindow) || SET_LAST_ERROR)) {
	rb_sys_fail("SetConsoleWindowInfo");
    }
#endif
    return io;
}
#endif

/*
 * call-seq:
 *   io.iflush
 *
 * Flushes input buffer in kernel.
 *
 * You must require 'io/console' to use this method.
 */
static VALUE
console_iflush(VALUE io)
{
    rb_io_t *fptr;
    int fd;

    GetOpenFile(io, fptr);
    fd = GetReadFD(fptr);
#if defined HAVE_TERMIOS_H || defined HAVE_TERMIO_H
    if (tcflush(fd, TCIFLUSH)) rb_sys_fail(0);
#endif
    return io;
}

/*
 * call-seq:
 *   io.oflush
 *
 * Flushes output buffer in kernel.
 *
 * You must require 'io/console' to use this method.
 */
static VALUE
console_oflush(VALUE io)
{
    rb_io_t *fptr;
    int fd;

    GetOpenFile(io, fptr);
    fd = GetWriteFD(fptr);
#if defined HAVE_TERMIOS_H || defined HAVE_TERMIO_H
    if (tcflush(fd, TCOFLUSH)) rb_sys_fail(0);
#endif
    return io;
}

/*
 * call-seq:
 *   io.ioflush
 *
 * Flushes input and output buffers in kernel.
 *
 * You must require 'io/console' to use this method.
 */
static VALUE
console_ioflush(VALUE io)
{
    rb_io_t *fptr;
#if defined HAVE_TERMIOS_H || defined HAVE_TERMIO_H
    int fd1, fd2;
#endif

    GetOpenFile(io, fptr);
#if defined HAVE_TERMIOS_H || defined HAVE_TERMIO_H
    fd1 = GetReadFD(fptr);
    fd2 = GetWriteFD(fptr);
    if (fd2 != -1 && fd1 != fd2) {
	if (tcflush(fd1, TCIFLUSH)) rb_sys_fail(0);
	if (tcflush(fd2, TCOFLUSH)) rb_sys_fail(0);
    }
    else {
	if (tcflush(fd1, TCIOFLUSH)) rb_sys_fail(0);
    }
#endif
    return io;
}

/*
 * call-seq:
 *   IO.console      -> #<File:/dev/tty>
 *
 * Returns an File instance opened console.
 *
 * You must require 'io/console' to use this method.
 */
static VALUE
console_dev(VALUE klass)
{
    VALUE con = 0;
    rb_io_t *fptr;

    if (klass == rb_cIO) klass = rb_cFile;
    if (rb_const_defined(klass, id_console)) {
	con = rb_const_get(klass, id_console);
	if (TYPE(con) == T_FILE) {
	    if ((fptr = RFILE(con)->fptr) && GetReadFD(fptr) != -1)
		return con;
	}
	rb_mod_remove_const(klass, ID2SYM(id_console));
    }
    {
	VALUE args[2];
#if defined HAVE_TERMIOS_H || defined HAVE_TERMIO_H || defined HAVE_SGTTY_H
# define CONSOLE_DEVICE "/dev/tty"
#elif defined _WIN32
# define CONSOLE_DEVICE "con$"
# define CONSOLE_DEVICE_FOR_READING "conin$"
# define CONSOLE_DEVICE_FOR_WRITING "conout$"
#endif
#ifndef CONSOLE_DEVICE_FOR_READING
# define CONSOLE_DEVICE_FOR_READING CONSOLE_DEVICE
#endif
#ifdef CONSOLE_DEVICE_FOR_WRITING
	VALUE out;
	rb_io_t *ofptr;
#endif
	int fd;

#ifdef CONSOLE_DEVICE_FOR_WRITING
	fd = rb_cloexec_open(CONSOLE_DEVICE_FOR_WRITING, O_WRONLY, 0);
	if (fd < 0) return Qnil;
        rb_update_max_fd(fd);
	args[1] = INT2FIX(O_WRONLY);
	args[0] = INT2NUM(fd);
	out = rb_class_new_instance(2, args, klass);
#endif
	fd = rb_cloexec_open(CONSOLE_DEVICE_FOR_READING, O_RDWR, 0);
	if (fd < 0) {
#ifdef CONSOLE_DEVICE_FOR_WRITING
	    rb_io_close(out);
#endif
	    return Qnil;
	}
        rb_update_max_fd(fd);
	args[1] = INT2FIX(O_RDWR);
	args[0] = INT2NUM(fd);
	con = rb_class_new_instance(2, args, klass);
	GetOpenFile(con, fptr);
	fptr->pathv = rb_obj_freeze(rb_str_new2(CONSOLE_DEVICE));
#ifdef CONSOLE_DEVICE_FOR_WRITING
	GetOpenFile(out, ofptr);
# ifdef HAVE_RB_IO_GET_WRITE_IO
	ofptr->pathv = fptr->pathv;
	fptr->tied_io_for_writing = out;
# else
	fptr->f2 = ofptr->f;
	ofptr->f = 0;
# endif
	ofptr->mode |= FMODE_SYNC;
#endif
	fptr->mode |= FMODE_SYNC;
	rb_const_set(klass, id_console, con);
    }
    return con;
}

static VALUE
io_getch(int argc, VALUE *argv, VALUE io)
{
    return rb_funcall2(io, rb_intern("getc"), argc, argv);
}

/*
 * IO console methods
 */
void
Init_console(void)
{
    id_getc = rb_intern("getc");
    id_console = rb_intern("console");
    InitVM(console);
}

void
InitVM_console(void)
{
    rb_define_method(rb_cIO, "raw", console_raw, -1);
    rb_define_method(rb_cIO, "raw!", console_set_raw, -1);
    rb_define_method(rb_cIO, "cooked", console_cooked, 0);
    rb_define_method(rb_cIO, "cooked!", console_set_cooked, 0);
    rb_define_method(rb_cIO, "getch", console_getch, -1);
    rb_define_method(rb_cIO, "echo=", console_set_echo, 1);
    rb_define_method(rb_cIO, "echo?", console_echo_p, 0);
    rb_define_method(rb_cIO, "noecho", console_noecho, 0);
    rb_define_method(rb_cIO, "winsize", console_winsize, 0);
    rb_define_method(rb_cIO, "winsize=", console_set_winsize, 1);
    rb_define_method(rb_cIO, "iflush", console_iflush, 0);
    rb_define_method(rb_cIO, "oflush", console_oflush, 0);
    rb_define_method(rb_cIO, "ioflush", console_ioflush, 0);
    rb_define_singleton_method(rb_cIO, "console", console_dev, 0);
    {
	VALUE mReadable = rb_define_module_under(rb_cIO, "readable");
	rb_define_method(mReadable, "getch", io_getch, -1);
    }
}
