#include "ruby.h"

static VALUE
bug_str_set_len(VALUE str, VALUE len)
{
    rb_str_set_len(str, NUM2LONG(len));
    return str;
}

void
Init_set_len(VALUE klass)
{
    rb_define_method(klass, "set_len", bug_str_set_len, 1);
}
