/* -*- C -*-
 * $Id$
 */

#include <ruby/ruby.h>
#include <ruby/util.h>
#include <errno.h>
#include "dl.h"

VALUE rb_cDLCFunc;

static ID id_last_error;

static VALUE
rb_dl_get_last_error(VALUE self)
{
    return rb_thread_local_aref(rb_thread_current(), id_last_error);
}

static VALUE
rb_dl_set_last_error(VALUE self, VALUE val)
{
    rb_thread_local_aset(rb_thread_current(), id_last_error, val);
    return Qnil;
}

#if defined(_WIN32)
#include <windows.h>
static ID id_win32_last_error;

static VALUE
rb_dl_get_win32_last_error(VALUE self)
{
    return rb_thread_local_aref(rb_thread_current(), id_win32_last_error);
}

static VALUE
rb_dl_set_win32_last_error(VALUE self, VALUE val)
{
    rb_thread_local_aset(rb_thread_current(), id_win32_last_error, val);
    return Qnil;
}
#endif

static void
dlcfunc_mark(void *ptr)
{
    struct cfunc_data *data = ptr;
    if (data->wrap) {
	rb_gc_mark(data->wrap);
    }
}

static void
dlcfunc_free(void *ptr)
{
    struct cfunc_data *data = ptr;
    if( data->name ){
	xfree(data->name);
    }
    xfree(data);
}

static size_t
dlcfunc_memsize(const void *ptr)
{
    const struct cfunc_data *data = ptr;
    size_t size = 0;
    if( data ){
	size += sizeof(*data);
	if( data->name ){
	    size += strlen(data->name) + 1;
	}
    }
    return size;
}

const rb_data_type_t dlcfunc_data_type = {
    "dl/cfunc",
    {dlcfunc_mark, dlcfunc_free, dlcfunc_memsize,},
};

VALUE
rb_dlcfunc_new(void (*func)(), int type, const char *name, ID calltype)
{
    VALUE val;
    struct cfunc_data *data;

    rb_secure(4);
    if( func ){
	val = TypedData_Make_Struct(rb_cDLCFunc, struct cfunc_data, &dlcfunc_data_type, data);
	data->ptr  = (void *)(VALUE)func;
	data->name = name ? strdup(name) : NULL;
	data->type = type;
	data->calltype = calltype;
    }
    else{
	val = Qnil;
    }

    return val;
}

void *
rb_dlcfunc2ptr(VALUE val)
{
    struct cfunc_data *data;
    void * func;

    if( rb_typeddata_is_kind_of(val, &dlcfunc_data_type) ){
	data = DATA_PTR(val);
	func = data->ptr;
    }
    else if( val == Qnil ){
	func = NULL;
    }
    else{
	rb_raise(rb_eTypeError, "DL::CFunc was expected");
    }

    return func;
}

static VALUE
rb_dlcfunc_s_allocate(VALUE klass)
{
    VALUE obj;
    struct cfunc_data *data;

    obj = TypedData_Make_Struct(klass, struct cfunc_data, &dlcfunc_data_type, data);
    data->ptr  = 0;
    data->name = 0;
    data->type = 0;
    data->calltype = CFUNC_CDECL;

    return obj;
}

int
rb_dlcfunc_kind_p(VALUE func)
{
    return rb_typeddata_is_kind_of(func, &dlcfunc_data_type);
}

/*
 * call-seq:
 *    DL::CFunc.new(address, type=DL::TYPE_VOID, name=nil, calltype=:cdecl)
 *
 * Create a new function that points to +address+ with an optional return type
 * of +type+, a name of +name+ and a calltype of +calltype+.
 */
static VALUE
rb_dlcfunc_initialize(int argc, VALUE argv[], VALUE self)
{
    VALUE addr, name, type, calltype, addrnum;
    struct cfunc_data *data;
    void *saddr;
    const char *sname;

    rb_scan_args(argc, argv, "13", &addr, &type, &name, &calltype);

    addrnum = rb_Integer(addr);
    saddr = (void*)(NUM2PTR(addrnum));
    sname = NIL_P(name) ? NULL : StringValuePtr(name);

    TypedData_Get_Struct(self, struct cfunc_data, &dlcfunc_data_type, data);
    if( data->name ) xfree(data->name);
    data->ptr  = saddr;
    data->name = sname ? strdup(sname) : 0;
    data->type = NIL_P(type) ? DLTYPE_VOID : NUM2INT(type);
    data->calltype = NIL_P(calltype) ? CFUNC_CDECL : SYM2ID(calltype);
    data->wrap = (addrnum == addr) ? 0 : addr;

    return Qnil;
}

/*
 * call-seq:
 *    name  => str
 *
 * Get the name of this function
 */
static VALUE
rb_dlcfunc_name(VALUE self)
{
    struct cfunc_data *cfunc;

    TypedData_Get_Struct(self, struct cfunc_data, &dlcfunc_data_type, cfunc);
    return cfunc->name ? rb_tainted_str_new2(cfunc->name) : Qnil;
}

/*
 * call-seq:
 *    cfunc.ctype   => num
 *
 * Get the C function return value type.  See DL for a list of constants
 * corresponding to this method's return value.
 */
static VALUE
rb_dlcfunc_ctype(VALUE self)
{
    struct cfunc_data *cfunc;

    TypedData_Get_Struct(self, struct cfunc_data, &dlcfunc_data_type, cfunc);
    return INT2NUM(cfunc->type);
}

/*
 * call-seq:
 *    cfunc.ctype = type
 *
 * Set the C function return value type to +type+.
 */
static VALUE
rb_dlcfunc_set_ctype(VALUE self, VALUE ctype)
{
    struct cfunc_data *cfunc;

    TypedData_Get_Struct(self, struct cfunc_data, &dlcfunc_data_type, cfunc);
    cfunc->type = NUM2INT(ctype);
    return ctype;
}

/*
 * call-seq:
 *    cfunc.calltype    => symbol
 *
 * Get the call type of this function.
 */
static VALUE
rb_dlcfunc_calltype(VALUE self)
{
    struct cfunc_data *cfunc;

    TypedData_Get_Struct(self, struct cfunc_data, &dlcfunc_data_type, cfunc);
    return ID2SYM(cfunc->calltype);
}

/*
 * call-seq:
 *    cfunc.calltype = symbol
 *
 * Set the call type for this function.
 */
static VALUE
rb_dlcfunc_set_calltype(VALUE self, VALUE sym)
{
    struct cfunc_data *cfunc;

    TypedData_Get_Struct(self, struct cfunc_data, &dlcfunc_data_type, cfunc);
    cfunc->calltype = SYM2ID(sym);
    return sym;
}

/*
 * call-seq:
 *    cfunc.ptr
 *
 * Get the underlying function pointer as a DL::CPtr object.
 */
static VALUE
rb_dlcfunc_ptr(VALUE self)
{
    struct cfunc_data *cfunc;

    TypedData_Get_Struct(self, struct cfunc_data, &dlcfunc_data_type, cfunc);
    return PTR2NUM(cfunc->ptr);
}

/*
 * call-seq:
 *    cfunc.ptr = pointer
 *
 * Set the underlying function pointer to a DL::CPtr named +pointer+.
 */
static VALUE
rb_dlcfunc_set_ptr(VALUE self, VALUE addr)
{
    struct cfunc_data *cfunc;

    TypedData_Get_Struct(self, struct cfunc_data, &dlcfunc_data_type, cfunc);
    cfunc->ptr = NUM2PTR(addr);

    return Qnil;
}

/*
 * call-seq: inspect
 *
 * Returns a string formatted with an easily readable representation of the
 * internal state of the DL::CFunc
 */
static VALUE
rb_dlcfunc_inspect(VALUE self)
{
    VALUE val;
    struct cfunc_data *cfunc;

    TypedData_Get_Struct(self, struct cfunc_data, &dlcfunc_data_type, cfunc);

    val = rb_sprintf("#<DL::CFunc:%p ptr=%p type=%d name='%s'>",
	     cfunc,
	     cfunc->ptr,
	     cfunc->type,
	     cfunc->name ? cfunc->name : "");
    OBJ_TAINT(val);
    return val;
}


# define DECL_FUNC_CDECL(f,ret,args,val) \
    ret (FUNC_CDECL(*(f)))(args) = (ret (FUNC_CDECL(*))(args))(VALUE)(val)
#ifdef FUNC_STDCALL
# define DECL_FUNC_STDCALL(f,ret,args,val) \
    ret (FUNC_STDCALL(*(f)))(args) = (ret (FUNC_STDCALL(*))(args))(VALUE)(val)
#endif

#define CALL_CASE switch( RARRAY_LEN(ary) ){ \
  CASE(0); break; \
  CASE(1); break; CASE(2); break; CASE(3); break; CASE(4); break; CASE(5); break; \
  CASE(6); break; CASE(7); break; CASE(8); break; CASE(9); break; CASE(10);break; \
  CASE(11);break; CASE(12);break; CASE(13);break; CASE(14);break; CASE(15);break; \
  CASE(16);break; CASE(17);break; CASE(18);break; CASE(19);break; CASE(20);break; \
  default: rb_raise(rb_eArgError, "too many arguments"); \
}


#if defined(_MSC_VER) && defined(_M_AMD64) && _MSC_VER == 1500
# pragma optimize("", off)
#endif
/*
 * call-seq:
 *    dlcfunc.call(ary)   => some_value
 *    dlcfunc[ary]        => some_value
 *
 * Calls the function pointer passing in +ary+ as values to the underlying
 * C function.  The return value depends on the ctype.
 */
static VALUE
rb_dlcfunc_call(VALUE self, VALUE ary)
{
    struct cfunc_data *cfunc;
    int i;
    DLSTACK_TYPE stack[DLSTACK_SIZE];
    VALUE result = Qnil;

    rb_secure_update(self);

    memset(stack, 0, sizeof(DLSTACK_TYPE) * DLSTACK_SIZE);
    Check_Type(ary, T_ARRAY);

    TypedData_Get_Struct(self, struct cfunc_data, &dlcfunc_data_type, cfunc);

    if( cfunc->ptr == 0 ){
	rb_raise(rb_eDLError, "can't call null-function");
	return Qnil;
    }

    for( i = 0; i < RARRAY_LEN(ary); i++ ){
	VALUE arg;
	if( i >= DLSTACK_SIZE ){
	    rb_raise(rb_eDLError, "too many arguments (stack overflow)");
	}
	arg = rb_to_int(RARRAY_PTR(ary)[i]);
	rb_check_safe_obj(arg);
	if (FIXNUM_P(arg)) {
	    stack[i] = (DLSTACK_TYPE)FIX2LONG(arg);
	}
	else if (RB_TYPE_P(arg, T_BIGNUM)) {
	    stack[i] = (DLSTACK_TYPE)rb_big2ulong_pack(arg);
	}
	else {
	    Check_Type(arg, T_FIXNUM);
	}
    }

    /* calltype == CFUNC_CDECL */
    if( cfunc->calltype == CFUNC_CDECL
#ifndef FUNC_STDCALL
	|| cfunc->calltype == CFUNC_STDCALL
#endif
	){
	switch( cfunc->type ){
	case DLTYPE_VOID:
#define CASE(n) case n: { \
	    DECL_FUNC_CDECL(f,void,DLSTACK_PROTO##n,cfunc->ptr); \
	    f(DLSTACK_ARGS##n(stack)); \
	    result = Qnil; \
}
	    CALL_CASE;
#undef CASE
	    break;
	case DLTYPE_VOIDP:
#define CASE(n) case n: { \
	    DECL_FUNC_CDECL(f,void*,DLSTACK_PROTO##n,cfunc->ptr); \
	    void * ret; \
	    ret = f(DLSTACK_ARGS##n(stack)); \
	    result = PTR2NUM(ret); \
}
	    CALL_CASE;
#undef CASE
	    break;
	case DLTYPE_CHAR:
#define CASE(n) case n: { \
	    DECL_FUNC_CDECL(f,char,DLSTACK_PROTO##n,cfunc->ptr); \
	    char ret; \
	    ret = f(DLSTACK_ARGS##n(stack)); \
	    result = CHR2FIX(ret); \
}
	    CALL_CASE;
#undef CASE
	    break;
	case DLTYPE_SHORT:
#define CASE(n) case n: { \
	    DECL_FUNC_CDECL(f,short,DLSTACK_PROTO##n,cfunc->ptr); \
	    short ret; \
	    ret = f(DLSTACK_ARGS##n(stack)); \
	    result = INT2NUM((int)ret); \
}
	    CALL_CASE;
#undef CASE
	    break;
	case DLTYPE_INT:
#define CASE(n) case n: { \
	    DECL_FUNC_CDECL(f,int,DLSTACK_PROTO##n,cfunc->ptr); \
	    int ret; \
	    ret = f(DLSTACK_ARGS##n(stack)); \
	    result = INT2NUM(ret); \
}
	    CALL_CASE;
#undef CASE
	    break;
	case DLTYPE_LONG:
#define CASE(n) case n: { \
	    DECL_FUNC_CDECL(f,long,DLSTACK_PROTO##n,cfunc->ptr); \
	    long ret; \
	    ret = f(DLSTACK_ARGS##n(stack)); \
	    result = LONG2NUM(ret); \
}
	    CALL_CASE;
#undef CASE
	    break;
#if HAVE_LONG_LONG  /* used in ruby.h */
	case DLTYPE_LONG_LONG:
#define CASE(n) case n: { \
	    DECL_FUNC_CDECL(f,LONG_LONG,DLSTACK_PROTO##n,cfunc->ptr); \
	    LONG_LONG ret; \
	    ret = f(DLSTACK_ARGS##n(stack)); \
	    result = LL2NUM(ret); \
}
	    CALL_CASE;
#undef CASE
	    break;
#endif
	case DLTYPE_FLOAT:
#define CASE(n) case n: { \
	    DECL_FUNC_CDECL(f,float,DLSTACK_PROTO##n,cfunc->ptr); \
	    float ret; \
	    ret = f(DLSTACK_ARGS##n(stack)); \
	    result = rb_float_new(ret); \
}
	    CALL_CASE;
#undef CASE
	    break;
	case DLTYPE_DOUBLE:
#define CASE(n) case n: { \
	    DECL_FUNC_CDECL(f,double,DLSTACK_PROTO##n,cfunc->ptr); \
	    double ret; \
	    ret = f(DLSTACK_ARGS##n(stack)); \
	    result = rb_float_new(ret); \
}
	    CALL_CASE;
#undef CASE
	    break;
	default:
	    rb_raise(rb_eDLTypeError, "unknown type %d", cfunc->type);
	}
    }
#ifdef FUNC_STDCALL
    else if( cfunc->calltype == CFUNC_STDCALL ){
	/* calltype == CFUNC_STDCALL */
	switch( cfunc->type ){
	case DLTYPE_VOID:
#define CASE(n) case n: { \
	    DECL_FUNC_STDCALL(f,void,DLSTACK_PROTO##n##_,cfunc->ptr); \
	    f(DLSTACK_ARGS##n(stack)); \
	    result = Qnil; \
}
	    CALL_CASE;
#undef CASE
	    break;
	case DLTYPE_VOIDP:
#define CASE(n) case n: { \
	    DECL_FUNC_STDCALL(f,void*,DLSTACK_PROTO##n##_,cfunc->ptr); \
	    void * ret; \
	    ret = f(DLSTACK_ARGS##n(stack)); \
	    result = PTR2NUM(ret); \
}
	    CALL_CASE;
#undef CASE
	    break;
	case DLTYPE_CHAR:
#define CASE(n) case n: { \
	    DECL_FUNC_STDCALL(f,char,DLSTACK_PROTO##n##_,cfunc->ptr); \
	    char ret; \
	    ret = f(DLSTACK_ARGS##n(stack)); \
	    result = CHR2FIX(ret); \
}
	    CALL_CASE;
#undef CASE
	    break;
	case DLTYPE_SHORT:
#define CASE(n) case n: { \
	    DECL_FUNC_STDCALL(f,short,DLSTACK_PROTO##n##_,cfunc->ptr); \
	    short ret; \
	    ret = f(DLSTACK_ARGS##n(stack)); \
	    result = INT2NUM((int)ret); \
}
	    CALL_CASE;
#undef CASE
	    break;
	case DLTYPE_INT:
#define CASE(n) case n: { \
	    DECL_FUNC_STDCALL(f,int,DLSTACK_PROTO##n##_,cfunc->ptr); \
	    int ret; \
	    ret = f(DLSTACK_ARGS##n(stack)); \
	    result = INT2NUM(ret); \
}
	    CALL_CASE;
#undef CASE
	    break;
	case DLTYPE_LONG:
#define CASE(n) case n: { \
	    DECL_FUNC_STDCALL(f,long,DLSTACK_PROTO##n##_,cfunc->ptr); \
	    long ret; \
	    ret = f(DLSTACK_ARGS##n(stack)); \
	    result = LONG2NUM(ret); \
}
	    CALL_CASE;
#undef CASE
	    break;
#if HAVE_LONG_LONG  /* used in ruby.h */
	case DLTYPE_LONG_LONG:
#define CASE(n) case n: { \
	    DECL_FUNC_STDCALL(f,LONG_LONG,DLSTACK_PROTO##n##_,cfunc->ptr); \
	    LONG_LONG ret; \
	    ret = f(DLSTACK_ARGS##n(stack)); \
	    result = LL2NUM(ret); \
}
	    CALL_CASE;
#undef CASE
	    break;
#endif
	case DLTYPE_FLOAT:
#define CASE(n) case n: { \
	    DECL_FUNC_STDCALL(f,float,DLSTACK_PROTO##n##_,cfunc->ptr); \
	    float ret; \
	    ret = f(DLSTACK_ARGS##n(stack)); \
	    result = rb_float_new(ret); \
}
	    CALL_CASE;
#undef CASE
	    break;
	case DLTYPE_DOUBLE:
#define CASE(n) case n: { \
	    DECL_FUNC_STDCALL(f,double,DLSTACK_PROTO##n##_,cfunc->ptr); \
	    double ret; \
	    ret = f(DLSTACK_ARGS##n(stack)); \
	    result = rb_float_new(ret); \
}
	    CALL_CASE;
#undef CASE
	    break;
	default:
	    rb_raise(rb_eDLTypeError, "unknown type %d", cfunc->type);
	}
    }
#endif
    else{
	const char *name = rb_id2name(cfunc->calltype);
	if( name ){
	    rb_raise(rb_eDLError, "unsupported call type: %s",
		     name);
	}
	else{
	    rb_raise(rb_eDLError, "unsupported call type: %"PRIxVALUE,
		     cfunc->calltype);
	}
    }

    rb_dl_set_last_error(self, INT2NUM(errno));
#if defined(_WIN32)
    rb_dl_set_win32_last_error(self, INT2NUM(GetLastError()));
#endif

    return result;
}
#if defined(_MSC_VER) && defined(_M_AMD64) && _MSC_VER == 1500
# pragma optimize("", on)
#endif

/*
 * call-seq:
 *    dlfunc.to_i   => integer
 *
 * Returns the memory location of this function pointer as an integer.
 */
static VALUE
rb_dlcfunc_to_i(VALUE self)
{
  struct cfunc_data *cfunc;

  TypedData_Get_Struct(self, struct cfunc_data, &dlcfunc_data_type, cfunc);
  return PTR2NUM(cfunc->ptr);
}

void
Init_dlcfunc(void)
{
    id_last_error = rb_intern("__DL2_LAST_ERROR__");
#if defined(_WIN32)
    id_win32_last_error = rb_intern("__DL2_WIN32_LAST_ERROR__");
#endif

    /*
     * Document-class: DL::CFunc
     *
     * A direct accessor to a function in a C library
     *
     * == Example
     *
     *   libc_so = "/lib64/libc.so.6"
     *   => "/lib64/libc.so.6"
     *   libc = DL::dlopen(libc_so)
     *   => #<DL::Handle:0x00000000e05b00>
     *   @cfunc = DL::CFunc.new(libc,['strcpy'], DL::TYPE_VOIDP, 'strcpy')
     *   => #<DL::CFunc:0x000000012daec0 ptr=0x007f62ca5a8300 type=1 name='strcpy'>
     *
     */
    rb_cDLCFunc = rb_define_class_under(rb_mDL, "CFunc", rb_cObject);
    rb_define_alloc_func(rb_cDLCFunc, rb_dlcfunc_s_allocate);

    /*
     * Document-method: last_error
     *
     * Returns the last error for the current executing thread
     */
    rb_define_module_function(rb_cDLCFunc, "last_error", rb_dl_get_last_error, 0);
#if defined(_WIN32)

    /*
     * Document-method: win32_last_error
     *
     * Returns the last win32 error for the current executing thread
     */
    rb_define_module_function(rb_cDLCFunc, "win32_last_error", rb_dl_get_win32_last_error, 0);
#endif
    rb_define_method(rb_cDLCFunc, "initialize", rb_dlcfunc_initialize, -1);
    rb_define_method(rb_cDLCFunc, "call", rb_dlcfunc_call, 1);
    rb_define_method(rb_cDLCFunc, "[]",   rb_dlcfunc_call, 1);
    rb_define_method(rb_cDLCFunc, "name", rb_dlcfunc_name, 0);
    rb_define_method(rb_cDLCFunc, "ctype", rb_dlcfunc_ctype, 0);
    rb_define_method(rb_cDLCFunc, "ctype=", rb_dlcfunc_set_ctype, 1);
    rb_define_method(rb_cDLCFunc, "calltype", rb_dlcfunc_calltype, 0);
    rb_define_method(rb_cDLCFunc, "calltype=", rb_dlcfunc_set_calltype, 1);
    rb_define_method(rb_cDLCFunc, "ptr",  rb_dlcfunc_ptr, 0);
    rb_define_method(rb_cDLCFunc, "ptr=", rb_dlcfunc_set_ptr, 1);
    rb_define_method(rb_cDLCFunc, "inspect", rb_dlcfunc_inspect, 0);
    rb_define_method(rb_cDLCFunc, "to_s", rb_dlcfunc_inspect, 0);
    rb_define_method(rb_cDLCFunc, "to_i", rb_dlcfunc_to_i, 0);
}
