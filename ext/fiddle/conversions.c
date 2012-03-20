#include <fiddle.h>

ffi_type *
int_to_ffi_type(int type)
{
    int signed_p = 1;

    if (type < 0) {
	type = -1 * type;
	signed_p = 0;
    }

#define rb_ffi_type_of(t) (signed_p ? &ffi_type_s##t : &ffi_type_u##t)

    switch (type) {
      case TYPE_VOID:
	return &ffi_type_void;
      case TYPE_VOIDP:
	return &ffi_type_pointer;
      case TYPE_CHAR:
	return rb_ffi_type_of(char);
      case TYPE_SHORT:
	return rb_ffi_type_of(short);
      case TYPE_INT:
	return rb_ffi_type_of(int);
      case TYPE_LONG:
	return rb_ffi_type_of(long);
#if HAVE_LONG_LONG
      case TYPE_LONG_LONG:
	return rb_ffi_type_of(long_long);
#endif
      case TYPE_FLOAT:
	return &ffi_type_float;
      case TYPE_DOUBLE:
	return &ffi_type_double;
      default:
	rb_raise(rb_eRuntimeError, "unknown type %d", type);
    }
    return &ffi_type_pointer;
}

void
value_to_generic(int type, VALUE src, fiddle_generic * dst)
{
    switch (type) {
      case TYPE_VOID:
	break;
      case TYPE_VOIDP:
	dst->pointer = NUM2PTR(rb_Integer(src));
	break;
      case TYPE_CHAR:
	dst->schar = (signed char)NUM2INT(src);
	break;
      case -TYPE_CHAR:
	dst->uchar = (unsigned char)NUM2UINT(src);
	break;
      case TYPE_SHORT:
	dst->sshort = (unsigned short)NUM2INT(src);
	break;
      case -TYPE_SHORT:
	dst->sshort = (signed short)NUM2UINT(src);
	break;
      case TYPE_INT:
	dst->sint = NUM2INT(src);
	break;
      case -TYPE_INT:
	dst->uint = NUM2UINT(src);
	break;
      case TYPE_LONG:
	dst->slong = NUM2LONG(src);
	break;
      case -TYPE_LONG:
	dst->ulong = NUM2ULONG(src);
	break;
#if HAVE_LONG_LONG
      case TYPE_LONG_LONG:
	dst->slong_long = NUM2LL(src);
	break;
      case -TYPE_LONG_LONG:
	dst->ulong_long = NUM2ULL(src);
	break;
#endif
      case TYPE_FLOAT:
	dst->ffloat = (float)NUM2DBL(src);
	break;
      case TYPE_DOUBLE:
	dst->ddouble = NUM2DBL(src);
	break;
      default:
	rb_raise(rb_eRuntimeError, "unknown type %d", type);
    }
}

VALUE
generic_to_value(VALUE rettype, fiddle_generic retval)
{
    int type = NUM2INT(rettype);
    VALUE cPointer;

    cPointer = rb_const_get(mFiddle, rb_intern("Pointer"));

    switch (type) {
      case TYPE_VOID:
	return Qnil;
      case TYPE_VOIDP:
        return rb_funcall(cPointer, rb_intern("[]"), 1,
          PTR2NUM((void *)retval.pointer));
      case TYPE_CHAR:
	return INT2NUM((signed char)retval.fffi_sarg);
      case -TYPE_CHAR:
	return INT2NUM((unsigned char)retval.fffi_arg);
      case TYPE_SHORT:
	return INT2NUM((signed short)retval.fffi_sarg);
      case -TYPE_SHORT:
	return INT2NUM((unsigned short)retval.fffi_arg);
      case TYPE_INT:
	return INT2NUM((signed int)retval.fffi_sarg);
      case -TYPE_INT:
	return UINT2NUM((unsigned int)retval.fffi_arg);
      case TYPE_LONG:
	return LONG2NUM(retval.slong);
      case -TYPE_LONG:
	return ULONG2NUM(retval.ulong);
#if HAVE_LONG_LONG
      case TYPE_LONG_LONG:
	return LL2NUM(retval.slong_long);
      case -TYPE_LONG_LONG:
	return ULL2NUM(retval.ulong_long);
#endif
      case TYPE_FLOAT:
	return rb_float_new(retval.ffloat);
      case TYPE_DOUBLE:
	return rb_float_new(retval.ddouble);
      default:
	rb_raise(rb_eRuntimeError, "unknown type %d", type);
    }
}

/* vim: set noet sw=4 sts=4 */
