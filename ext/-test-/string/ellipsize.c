#include "ruby.h"

static VALUE
bug_str_ellipsize(VALUE str, VALUE len)
{
    return rb_str_ellipsize(str, NUM2LONG(len));
}

void
Init_ellipsize(VALUE klass)
{
    rb_define_method(klass, "ellipsize", bug_str_ellipsize, 1);
}
