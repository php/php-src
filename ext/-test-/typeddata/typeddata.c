#include <ruby.h>

static const rb_data_type_t test_data = {
    "typed_data",
};

static VALUE
test_check(VALUE self, VALUE obj)
{
    rb_check_typeddata(obj, &test_data);
    return obj;
}

void
Init_typeddata(void)
{
    VALUE mBug = rb_define_module("Bug");
    VALUE klass = rb_define_class_under(mBug, "TypedData", rb_cData);
    rb_define_singleton_method(klass, "check", test_check, 1);
}
