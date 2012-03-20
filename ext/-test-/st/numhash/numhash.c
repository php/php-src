#include <ruby.h>
#include <ruby/st.h>

static void
numhash_free(void *ptr)
{
    if (ptr) st_free_table(ptr);
}

static VALUE
numhash_alloc(VALUE klass)
{
    return Data_Wrap_Struct(klass, 0, numhash_free, 0);
}

static VALUE
numhash_init(VALUE self)
{
    st_table *tbl = (st_table *)DATA_PTR(self);
    if (tbl) st_free_table(tbl);
    DATA_PTR(self) = st_init_numtable();
    return self;
}

static VALUE
numhash_aref(VALUE self, VALUE key)
{
    st_data_t data;
    if (!SPECIAL_CONST_P(key)) rb_raise(rb_eArgError, "not a special const");
    if (st_lookup((st_table *)DATA_PTR(self), (st_data_t)key, &data))
	return (VALUE)data;
    return Qnil;
}

static VALUE
numhash_aset(VALUE self, VALUE key, VALUE data)
{
    if (!SPECIAL_CONST_P(key)) rb_raise(rb_eArgError, "not a special const");
    if (!SPECIAL_CONST_P(data)) rb_raise(rb_eArgError, "not a special const");
    st_insert((st_table *)DATA_PTR(self), (st_data_t)key, (st_data_t)data);
    return self;
}

static int
numhash_i(st_data_t key, st_data_t value, st_data_t arg, int error)
{
    VALUE ret;
    if (key == 0 && value == 0 && error == 1) rb_raise(rb_eRuntimeError, "numhash modified");
    ret = rb_yield_values(3, (VALUE)key, (VALUE)value, (VALUE)arg);
    if (ret == Qtrue) return ST_CHECK;
    return ST_CONTINUE;
}

static VALUE
numhash_each(VALUE self)
{
    st_table *table = DATA_PTR(self);
    st_data_t data = (st_data_t)self;
    return st_foreach_check(table, numhash_i, data, data) ? Qtrue : Qfalse;
}

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
numhash_update(VALUE self, VALUE key)
{
    if (st_update((st_table *)DATA_PTR(self), (st_data_t)key, update_func, 0))
	return Qtrue;
    else
	return Qfalse;
}

#if SIZEOF_LONG == SIZEOF_VOIDP
# define ST2NUM(x) ULONG2NUM(x)
#elif SIZEOF_LONG_LONG == SIZEOF_VOIDP
# define ST2NUM(x) ULL2NUM(x)
#endif

static VALUE
numhash_size(VALUE self)
{
    return ST2NUM(((st_table *)DATA_PTR(self))->num_entries);
}

static VALUE
numhash_delete_safe(VALUE self, VALUE key)
{
    st_data_t val, k = (st_data_t)key;
    if (st_delete_safe((st_table *)DATA_PTR(self), &k, &val, (st_data_t)self)) {
	return val;
    }
    return Qnil;
}

void
Init_numhash(void)
{
    VALUE st = rb_define_class_under(rb_define_module("Bug"), "StNumHash", rb_cData);
    rb_define_alloc_func(st, numhash_alloc);
    rb_define_method(st, "initialize", numhash_init, 0);
    rb_define_method(st, "[]", numhash_aref, 1);
    rb_define_method(st, "[]=", numhash_aset, 2);
    rb_define_method(st, "each", numhash_each, 0);
    rb_define_method(st, "update", numhash_update, 1);
    rb_define_method(st, "size", numhash_size, 0);
    rb_define_method(st, "delete_safe", numhash_delete_safe, 1);
}

