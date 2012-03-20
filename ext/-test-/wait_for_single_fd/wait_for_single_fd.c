#include "ruby/ruby.h"
#include "ruby/io.h"

static VALUE
wait_for_single_fd(VALUE ign, VALUE fd, VALUE events, VALUE timeout)
{
    struct timeval tv;
    struct timeval *tvp = NULL;
    int rc;

    if (!NIL_P(timeout)) {
	tv = rb_time_timeval(timeout);
	tvp = &tv;
    }

    rc = rb_wait_for_single_fd(NUM2INT(fd), NUM2INT(events), tvp);
    if (rc == -1)
	rb_sys_fail("rb_wait_for_single_fd");
    return INT2NUM(rc);
}

void
Init_wait_for_single_fd(void)
{
    rb_define_const(rb_cObject, "RB_WAITFD_IN", INT2NUM(RB_WAITFD_IN));
    rb_define_const(rb_cObject, "RB_WAITFD_OUT", INT2NUM(RB_WAITFD_OUT));
    rb_define_const(rb_cObject, "RB_WAITFD_PRI", INT2NUM(RB_WAITFD_PRI));
    rb_define_singleton_method(rb_cIO, "wait_for_single_fd",
                               wait_for_single_fd, 3);
}
