#include <ruby.h>
#include <ruby/st.h>

static int
update_func(st_data_t key, st_data_t *value, st_data_t arg)
{
    VALUE ret = rb_yield_values(2, (VALUE)key, (VALUE)*value);
    switch (ret) {
      case Qfalse:
	return ST_STOP;
      case Qnil:
	return ST_DELETE;
      default:
	*value = ret;
	return ST_CONTINUE;
    }
}

static VALUE
test_st_update(VALUE self, VALUE key)
{
    if (st_update(RHASH_TBL(self), (st_data_t)key, update_func, 0))
	return Qtrue;
    else
	return Qfalse;
}

void
Init_update(void)
{
    VALUE st = rb_define_class_under(rb_define_module("Bug"), "StTable", rb_cHash);
    rb_define_method(st, "st_update", test_st_update, 1);
}

