#include <ruby.h>

static VALUE
iter_break_value(VALUE self, VALUE val)
{
    rb_iter_break_value(val);
    return self;		/* not reached */
}

void
Init_break(void)
{
    VALUE breakable = rb_define_module_under(rb_define_module("Bug"), "Breakable");
    rb_define_module_function(breakable, "iter_break", iter_break_value, 1);
}
