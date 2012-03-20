#include <ruby.h>

static VALUE
bug_funcall(int argc, VALUE *argv, VALUE self)
{
    if (argc < 1) rb_raise(rb_eArgError, "not enough argument");
    return rb_funcall2(self, rb_to_id(*argv), argc-1, argv+1);
}

void
Init_bug(void)
{
    VALUE mBug = rb_define_module("Bug");
    rb_define_module_function(mBug, "funcall", bug_funcall, -1);
    rb_define_module_function(mBug, "notimplement", rb_f_notimplement, -1);
}
