#include "ruby.h"

static VALUE
bug_str_cstr_term(VALUE str)
{
    long len;
    char *s;
    rb_str_modify(str);
    len = RSTRING_LEN(str);
    RSTRING_PTR(str)[len] = 'x';
    s = StringValueCStr(str);
    rb_gc();
    return INT2NUM(s[len]);
}

void
Init_cstr(VALUE klass)
{
    rb_define_method(klass, "cstr_term", bug_str_cstr_term, 0);
}
