#include "ruby.h"

VALUE
bug_str_modify(VALUE str)
{
    rb_str_modify(str);
    return str;
}

VALUE
bug_str_modify_expand(VALUE str, VALUE expand)
{
    rb_str_modify_expand(str, NUM2LONG(expand));
    return str;
}

void
Init_modify(VALUE klass)
{
    rb_define_method(klass, "modify!", bug_str_modify, 0);
    rb_define_method(klass, "modify_expand!", bug_str_modify_expand, 1);
}
