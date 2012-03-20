/**********************************************************************

  io/wait.c -

  $Author$
  created at: Tue Jul 14 21:53:18 2009

  All the files in this distribution are covered under the Ruby's
  license (see the file COPYING).

**********************************************************************/

#include "ruby.h"
#include "ruby/io.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>

#ifdef F_GETFL
static int
io_nonblock_mode(int fd)
{
    int f = fcntl(fd, F_GETFL);
    if (f == -1) rb_sys_fail(0);
    return f;
}
#else
#define io_nonblock_mode(fd) ((void)(fd), 0)
#endif

#ifdef F_GETFL
static VALUE
rb_io_nonblock_p(VALUE io)
{
    rb_io_t *fptr;
    GetOpenFile(io, fptr);
    if (io_nonblock_mode(fptr->fd) & O_NONBLOCK)
	return Qtrue;
    return Qfalse;
}
#else
#define rb_io_nonblock_p rb_f_notimplement
#endif

#ifdef F_SETFL
static void
io_nonblock_set(int fd, int f, int nb)
{
    if (nb) {
	if ((f & O_NONBLOCK) != 0)
	    return;
	f |= O_NONBLOCK;
    }
    else {
	if ((f & O_NONBLOCK) == 0)
	    return;
	f &= ~O_NONBLOCK;
    }
    if (fcntl(fd, F_SETFL, f) == -1)
	rb_sys_fail(0);
}

static VALUE
rb_io_nonblock_set(VALUE io, VALUE nb)
{
    rb_io_t *fptr;
    GetOpenFile(io, fptr);
    io_nonblock_set(fptr->fd, io_nonblock_mode(fptr->fd), RTEST(nb));
    return io;
}

static VALUE
io_nonblock_restore(VALUE arg)
{
    int *restore = (int *)arg;
    if (fcntl(restore[0], F_SETFL, restore[1]) == -1)
	rb_sys_fail(0);
    return Qnil;
}

static VALUE
rb_io_nonblock_block(int argc, VALUE *argv, VALUE io)
{
    int nb = 1;
    rb_io_t *fptr;
    int f, restore[2];

    GetOpenFile(io, fptr);
    if (argc > 0) {
	VALUE v;
	rb_scan_args(argc, argv, "01", &v);
	nb = RTEST(v);
    }
    f = io_nonblock_mode(fptr->fd);
    restore[0] = fptr->fd;
    restore[1] = f;
    io_nonblock_set(fptr->fd, f, nb);
    return rb_ensure(rb_yield, io, io_nonblock_restore, (VALUE)restore);
}
#else
#define rb_io_nonblock_set rb_f_notimplement
#define rb_io_nonblock_block rb_f_notimplement
#endif

void
Init_nonblock(void)
{
    VALUE io = rb_cIO;

    rb_define_method(io, "nonblock?", rb_io_nonblock_p, 0);
    rb_define_method(io, "nonblock=", rb_io_nonblock_set, 1);
    rb_define_method(io, "nonblock", rb_io_nonblock_block, -1);
}
