#include "ruby.h"

VALUE rb_funcall_passing_block(VALUE, ID, int, const VALUE*);

static VALUE
with_funcall2(int argc, VALUE *argv, VALUE self)
{
    return rb_funcall2(self, rb_intern("target"), argc, argv);
}

static VALUE
with_funcall_passing_block(int argc, VALUE *argv, VALUE self)
{
    return rb_funcall_passing_block(self, rb_intern("target"), argc, argv);
}

void
Init_funcall(void)
{
    VALUE cRelay = rb_path2class("TestFuncall::Relay");

    rb_define_singleton_method(cRelay,
			       "with_funcall2",
			       with_funcall2,
			       -1);
    rb_define_singleton_method(cRelay,
			       "with_funcall_passing_block",
			       with_funcall_passing_block,
			       -1);
}
