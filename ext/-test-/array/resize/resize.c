#include "ruby/ruby.h"

static VALUE
ary_resize(VALUE ary, VALUE len)
{
    rb_ary_resize(ary, NUM2LONG(len));
    return ary;
}

void
Init_resize(void)
{
    rb_define_method(rb_cArray, "__resize__", ary_resize, 1);
}
