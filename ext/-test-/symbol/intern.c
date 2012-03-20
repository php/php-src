#include "ruby.h"

static VALUE
bug_sym_interned_p(VALUE self, VALUE name)
{
    ID id = rb_check_id(&name);
    return id ? Qtrue : Qfalse;
}

void
Init_intern(VALUE klass)
{
    rb_define_singleton_method(klass, "interned?", bug_sym_interned_p, 1);
}
