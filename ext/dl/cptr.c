/* -*- C -*-
 * $Id$
 */

#include <ruby/ruby.h>
#include <ruby/io.h>
#include <ctype.h>
#include "dl.h"

VALUE rb_cDLCPtr;

static inline freefunc_t
get_freefunc(VALUE func, volatile VALUE *wrap)
{
    VALUE addrnum;
    if (NIL_P(func)) {
	*wrap = 0;
	return NULL;
    }
    if (rb_dlcfunc_kind_p(func)) {
	*wrap = func;
	return (freefunc_t)(VALUE)RCFUNC_DATA(func)->ptr;
    }
    addrnum = rb_Integer(func);
    *wrap = (addrnum != func) ? func : 0;
    return (freefunc_t)(VALUE)NUM2PTR(addrnum);
}

static ID id_to_ptr;

static void
dlptr_mark(void *ptr)
{
    struct ptr_data *data = ptr;
    if (data->wrap[0]) {
	rb_gc_mark(data->wrap[0]);
    }
    if (data->wrap[1]) {
	rb_gc_mark(data->wrap[1]);
    }
}

static void
dlptr_free(void *ptr)
{
    struct ptr_data *data = ptr;
    if (data->ptr) {
	if (data->free) {
	    (*(data->free))(data->ptr);
	}
    }
}

static size_t
dlptr_memsize(const void *ptr)
{
    const struct ptr_data *data = ptr;
    return data ? sizeof(*data) + data->size : 0;
}

static const rb_data_type_t dlptr_data_type = {
    "dl/ptr",
    {dlptr_mark, dlptr_free, dlptr_memsize,},
};

VALUE
rb_dlptr_new2(VALUE klass, void *ptr, long size, freefunc_t func)
{
    struct ptr_data *data;
    VALUE val;

    rb_secure(4);
    val = TypedData_Make_Struct(klass, struct ptr_data, &dlptr_data_type, data);
    data->ptr = ptr;
    data->free = func;
    data->size = size;
    OBJ_TAINT(val);

    return val;
}

VALUE
rb_dlptr_new(void *ptr, long size, freefunc_t func)
{
    return rb_dlptr_new2(rb_cDLCPtr, ptr, size, func);
}

VALUE
rb_dlptr_malloc(long size, freefunc_t func)
{
    void *ptr;

    rb_secure(4);
    ptr = ruby_xmalloc((size_t)size);
    memset(ptr,0,(size_t)size);
    return rb_dlptr_new(ptr, size, func);
}

void *
rb_dlptr2cptr(VALUE val)
{
    struct ptr_data *data;
    void *ptr;

    if (rb_obj_is_kind_of(val, rb_cDLCPtr)) {
	TypedData_Get_Struct(val, struct ptr_data, &dlptr_data_type, data);
	ptr = data->ptr;
    }
    else if (val == Qnil) {
	ptr = NULL;
    }
    else{
	rb_raise(rb_eTypeError, "DL::PtrData was expected");
    }

    return ptr;
}

static VALUE
rb_dlptr_s_allocate(VALUE klass)
{
    VALUE obj;
    struct ptr_data *data;

    rb_secure(4);
    obj = TypedData_Make_Struct(klass, struct ptr_data, &dlptr_data_type, data);
    data->ptr = 0;
    data->size = 0;
    data->free = 0;

    return obj;
}

/*
 * call-seq:
 *    DL::CPtr.new(address)                   => dl_cptr
 *    DL::CPtr.new(address, size)             => dl_cptr
 *    DL::CPtr.new(address, size, freefunc)   => dl_cptr
 *
 * Create a new pointer to +address+ with an optional +size+ and +freefunc+.
 * +freefunc+ will be called when the instance is garbage collected.
 */
static VALUE
rb_dlptr_initialize(int argc, VALUE argv[], VALUE self)
{
    VALUE ptr, sym, size, wrap = 0, funcwrap = 0;
    struct ptr_data *data;
    void *p = NULL;
    freefunc_t f = NULL;
    long s = 0;

    if (rb_scan_args(argc, argv, "12", &ptr, &size, &sym) >= 1) {
	VALUE addrnum = rb_Integer(ptr);
	if (addrnum != ptr) wrap = ptr;
	p = NUM2PTR(addrnum);
    }
    if (argc >= 2) {
	s = NUM2LONG(size);
    }
    if (argc >= 3) {
	f = get_freefunc(sym, &funcwrap);
    }

    if (p) {
	TypedData_Get_Struct(self, struct ptr_data, &dlptr_data_type, data);
	if (data->ptr && data->free) {
	    /* Free previous memory. Use of inappropriate initialize may cause SEGV. */
	    (*(data->free))(data->ptr);
	}
	data->wrap[0] = wrap;
	data->wrap[1] = funcwrap;
	data->ptr  = p;
	data->size = s;
	data->free = f;
    }

    return Qnil;
}

/*
 * call-seq:
 *
 *    DL::CPtr.malloc(size, freefunc = nil)  => dl cptr instance
 *
 * Allocate +size+ bytes of memory and associate it with an optional
 * +freefunc+ that will be called when the pointer is garbage collected.
 * +freefunc+ must be an address pointing to a function or an instance of
 * DL::CFunc
 */
static VALUE
rb_dlptr_s_malloc(int argc, VALUE argv[], VALUE klass)
{
    VALUE size, sym, obj, wrap = 0;
    long s;
    freefunc_t f;

    switch (rb_scan_args(argc, argv, "11", &size, &sym)) {
      case 1:
	s = NUM2LONG(size);
	f = NULL;
	break;
      case 2:
	s = NUM2LONG(size);
	f = get_freefunc(sym, &wrap);
	break;
      default:
	rb_bug("rb_dlptr_s_malloc");
    }

    obj = rb_dlptr_malloc(s,f);
    if (wrap) RPTR_DATA(obj)->wrap[1] = wrap;

    return obj;
}

/*
 * call-seq: to_i
 *
 * Returns the integer memory location of this DL::CPtr.
 */
static VALUE
rb_dlptr_to_i(VALUE self)
{
    struct ptr_data *data;

    TypedData_Get_Struct(self, struct ptr_data, &dlptr_data_type, data);
    return PTR2NUM(data->ptr);
}

/*
 * call-seq: to_value
 *
 * Cast this CPtr to a ruby object.
 */
static VALUE
rb_dlptr_to_value(VALUE self)
{
    struct ptr_data *data;
    TypedData_Get_Struct(self, struct ptr_data, &dlptr_data_type, data);
    return (VALUE)(data->ptr);
}

/*
 * call-seq: ptr
 *
 * Returns a DL::CPtr that is a dereferenced pointer for this DL::CPtr.
 * Analogous to the star operator in C.
 */
VALUE
rb_dlptr_ptr(VALUE self)
{
    struct ptr_data *data;

    TypedData_Get_Struct(self, struct ptr_data, &dlptr_data_type, data);
    return rb_dlptr_new(*((void**)(data->ptr)),0,0);
}

/*
 * call-seq: ref
 *
 * Returns a DL::CPtr that is a reference pointer for this DL::CPtr.
 * Analogous to the ampersand operator in C.
 */
VALUE
rb_dlptr_ref(VALUE self)
{
    struct ptr_data *data;

    TypedData_Get_Struct(self, struct ptr_data, &dlptr_data_type, data);
    return rb_dlptr_new(&(data->ptr),0,0);
}

/*
 * call-seq: null?
 *
 * Returns true if this is a null pointer.
 */
VALUE
rb_dlptr_null_p(VALUE self)
{
    struct ptr_data *data;

    TypedData_Get_Struct(self, struct ptr_data, &dlptr_data_type, data);
    return data->ptr ? Qfalse : Qtrue;
}

/*
 * call-seq: free=(function)
 *
 * Set the free function for this pointer to the DL::CFunc in +function+.
 */
static VALUE
rb_dlptr_free_set(VALUE self, VALUE val)
{
    struct ptr_data *data;

    TypedData_Get_Struct(self, struct ptr_data, &dlptr_data_type, data);
    data->free = get_freefunc(val, &data->wrap[1]);

    return Qnil;
}

/*
 * call-seq: free
 *
 * Get the free function for this pointer.  Returns  DL::CFunc or nil.
 */
static VALUE
rb_dlptr_free_get(VALUE self)
{
    struct ptr_data *pdata;

    TypedData_Get_Struct(self, struct ptr_data, &dlptr_data_type, pdata);

    return rb_dlcfunc_new(pdata->free, DLTYPE_VOID, "free<anonymous>", CFUNC_CDECL);
}

/*
 * call-seq:
 *
 *    ptr.to_s        => string
 *    ptr.to_s(len)   => string
 *
 * Returns the pointer contents as a string.  When called with no arguments,
 * this method will return the contents until the first NULL byte.  When
 * called with +len+, a string of +len+ bytes will be returned.
 */
static VALUE
rb_dlptr_to_s(int argc, VALUE argv[], VALUE self)
{
    struct ptr_data *data;
    VALUE arg1, val;
    int len;

    TypedData_Get_Struct(self, struct ptr_data, &dlptr_data_type, data);
    switch (rb_scan_args(argc, argv, "01", &arg1)) {
      case 0:
	val = rb_tainted_str_new2((char*)(data->ptr));
	break;
      case 1:
	len = NUM2INT(arg1);
	val = rb_tainted_str_new((char*)(data->ptr), len);
	break;
      default:
	rb_bug("rb_dlptr_to_s");
    }

    return val;
}

/*
 * call-seq:
 *
 *    ptr.to_str        => string
 *    ptr.to_str(len)   => string
 *
 * Returns the pointer contents as a string.  When called with no arguments,
 * this method will return the contents with the length of this pointer's
 * +size+. When called with +len+, a string of +len+ bytes will be returned.
 */
static VALUE
rb_dlptr_to_str(int argc, VALUE argv[], VALUE self)
{
    struct ptr_data *data;
    VALUE arg1, val;
    int len;

    TypedData_Get_Struct(self, struct ptr_data, &dlptr_data_type, data);
    switch (rb_scan_args(argc, argv, "01", &arg1)) {
      case 0:
	val = rb_tainted_str_new((char*)(data->ptr),data->size);
	break;
      case 1:
	len = NUM2INT(arg1);
	val = rb_tainted_str_new((char*)(data->ptr), len);
	break;
      default:
	rb_bug("rb_dlptr_to_str");
    }

    return val;
}

/*
 * call-seq: inspect
 *
 * Returns a string formatted with an easily readable representation of the
 * internal state of the DL::CPtr
 */
static VALUE
rb_dlptr_inspect(VALUE self)
{
    struct ptr_data *data;
    char str[1024];

    TypedData_Get_Struct(self, struct ptr_data, &dlptr_data_type, data);
    snprintf(str, 1023, "#<%s:%p ptr=%p size=%ld free=%p>",
	     rb_class2name(CLASS_OF(self)), data, data->ptr, data->size, data->free);
    return rb_str_new2(str);
}

/*
 *  call-seq:
 *    ptr == other    => true or false
 *    ptr.eql?(other) => true or false
 *
 * Returns true if +other+ wraps the same pointer, otherwise returns
 * false.
 */
VALUE
rb_dlptr_eql(VALUE self, VALUE other)
{
    void *ptr1, *ptr2;

    if(!rb_obj_is_kind_of(other, rb_cDLCPtr)) return Qfalse;

    ptr1 = rb_dlptr2cptr(self);
    ptr2 = rb_dlptr2cptr(other);

    return ptr1 == ptr2 ? Qtrue : Qfalse;
}

/*
 *  call-seq:
 *    ptr <=> other   => -1, 0, 1, or nil
 *
 * Returns -1 if less than, 0 if equal to, 1 if greater than +other+.  Returns
 * nil if +ptr+ cannot be compared to +other+.
 */
static VALUE
rb_dlptr_cmp(VALUE self, VALUE other)
{
    void *ptr1, *ptr2;
    SIGNED_VALUE diff;

    if(!rb_obj_is_kind_of(other, rb_cDLCPtr)) return Qnil;

    ptr1 = rb_dlptr2cptr(self);
    ptr2 = rb_dlptr2cptr(other);
    diff = (SIGNED_VALUE)ptr1 - (SIGNED_VALUE)ptr2;
    if (!diff) return INT2FIX(0);
    return diff > 0 ? INT2NUM(1) : INT2NUM(-1);
}

/*
 * call-seq:
 *    ptr + n   => new cptr
 *
 * Returns a new DL::CPtr that has been advanced +n+ bytes.
 */
static VALUE
rb_dlptr_plus(VALUE self, VALUE other)
{
    void *ptr;
    long num, size;

    ptr = rb_dlptr2cptr(self);
    size = RPTR_DATA(self)->size;
    num = NUM2LONG(other);
    return rb_dlptr_new((char *)ptr + num, size - num, 0);
}

/*
 * call-seq:
 *    ptr - n   => new cptr
 *
 * Returns a new DL::CPtr that has been moved back +n+ bytes.
 */
static VALUE
rb_dlptr_minus(VALUE self, VALUE other)
{
    void *ptr;
    long num, size;

    ptr = rb_dlptr2cptr(self);
    size = RPTR_DATA(self)->size;
    num = NUM2LONG(other);
    return rb_dlptr_new((char *)ptr - num, size + num, 0);
}

/*
 *  call-seq:
 *     ptr[index]                -> an_integer
 *     ptr[start, length]        -> a_string
 *
 * Returns integer stored at _index_.  If _start_ and _length_ are given,
 * a string containing the bytes from _start_ of length _length_ will be
 * returned.
 */
VALUE
rb_dlptr_aref(int argc, VALUE argv[], VALUE self)
{
    VALUE arg0, arg1;
    VALUE retval = Qnil;
    size_t offset, len;
    struct ptr_data *data;

    TypedData_Get_Struct(self, struct ptr_data, &dlptr_data_type, data);
    if (!data->ptr) rb_raise(rb_eDLError, "NULL pointer dereference");
    switch( rb_scan_args(argc, argv, "11", &arg0, &arg1) ){
      case 1:
	offset = NUM2ULONG(arg0);
	retval = INT2NUM(*((char *)data->ptr + offset));
	break;
      case 2:
	offset = NUM2ULONG(arg0);
	len    = NUM2ULONG(arg1);
	retval = rb_tainted_str_new((char *)data->ptr + offset, len);
	break;
      default:
	rb_bug("rb_dlptr_aref()");
    }
    return retval;
}

/*
 *  call-seq:
 *     ptr[index]         = int                    ->  int
 *     ptr[start, length] = string or cptr or addr ->  string or dl_cptr or addr
 *
 * Set the value at +index+ to +int+.  Or, set the memory at +start+ until
 * +length+ with the contents of +string+, the memory from +dl_cptr+, or the
 * memory pointed at by the memory address +addr+.
 */
VALUE
rb_dlptr_aset(int argc, VALUE argv[], VALUE self)
{
    VALUE arg0, arg1, arg2;
    VALUE retval = Qnil;
    size_t offset, len;
    void *mem;
    struct ptr_data *data;

    TypedData_Get_Struct(self, struct ptr_data, &dlptr_data_type, data);
    if (!data->ptr) rb_raise(rb_eDLError, "NULL pointer dereference");
    switch( rb_scan_args(argc, argv, "21", &arg0, &arg1, &arg2) ){
      case 2:
	offset = NUM2ULONG(arg0);
	((char*)data->ptr)[offset] = NUM2UINT(arg1);
	retval = arg1;
	break;
      case 3:
	offset = NUM2ULONG(arg0);
	len    = NUM2ULONG(arg1);
	if (RB_TYPE_P(arg2, T_STRING)) {
	    mem = StringValuePtr(arg2);
	}
	else if( rb_obj_is_kind_of(arg2, rb_cDLCPtr) ){
	    mem = rb_dlptr2cptr(arg2);
	}
	else{
	    mem    = NUM2PTR(arg2);
	}
	memcpy((char *)data->ptr + offset, mem, len);
	retval = arg2;
	break;
      default:
	rb_bug("rb_dlptr_aset()");
    }
    return retval;
}

/*
 * call-seq: size=(size)
 *
 * Set the size of this pointer to +size+
 */
static VALUE
rb_dlptr_size_set(VALUE self, VALUE size)
{
    RPTR_DATA(self)->size = NUM2LONG(size);
    return size;
}

/*
 * call-seq: size
 *
 * Get the size of this pointer.
 */
static VALUE
rb_dlptr_size_get(VALUE self)
{
    return LONG2NUM(RPTR_DATA(self)->size);
}

/*
 * call-seq:
 *    DL::CPtr.to_ptr(val)  => cptr
 *    DL::CPtr[val]         => cptr
 *
 * Get the underlying pointer for ruby object +val+ and return it as a
 * DL::CPtr object.
 */
static VALUE
rb_dlptr_s_to_ptr(VALUE self, VALUE val)
{
    VALUE ptr, wrap = val, vptr;

    if (RTEST(rb_obj_is_kind_of(val, rb_cIO))){
	rb_io_t *fptr;
	FILE *fp;
	GetOpenFile(val, fptr);
	fp = rb_io_stdio_file(fptr);
	ptr = rb_dlptr_new(fp, 0, NULL);
    }
    else if (RTEST(rb_obj_is_kind_of(val, rb_cString))){
	char *str = StringValuePtr(val);
	ptr = rb_dlptr_new(str, RSTRING_LEN(val), NULL);
    }
    else if ((vptr = rb_check_funcall(val, id_to_ptr, 0, 0)) != Qundef){
	if (rb_obj_is_kind_of(vptr, rb_cDLCPtr)){
	    ptr = vptr;
	    wrap = 0;
	}
	else{
	    rb_raise(rb_eDLError, "to_ptr should return a CPtr object");
	}
    }
    else{
	VALUE num = rb_Integer(val);
	if (num == val) wrap = 0;
	ptr = rb_dlptr_new(NUM2PTR(num), 0, NULL);
    }
    OBJ_INFECT(ptr, val);
    if (wrap) RPTR_DATA(ptr)->wrap[0] = wrap;
    return ptr;
}

void
Init_dlptr(void)
{
    id_to_ptr = rb_intern("to_ptr");

    /* Document-class: DL::CPtr
     *
     * CPtr is a class to handle C pointers
     *
     */
    rb_cDLCPtr = rb_define_class_under(rb_mDL, "CPtr", rb_cObject);
    rb_define_alloc_func(rb_cDLCPtr, rb_dlptr_s_allocate);
    rb_define_singleton_method(rb_cDLCPtr, "malloc", rb_dlptr_s_malloc, -1);
    rb_define_singleton_method(rb_cDLCPtr, "to_ptr", rb_dlptr_s_to_ptr, 1);
    rb_define_singleton_method(rb_cDLCPtr, "[]", rb_dlptr_s_to_ptr, 1);
    rb_define_method(rb_cDLCPtr, "initialize", rb_dlptr_initialize, -1);
    rb_define_method(rb_cDLCPtr, "free=", rb_dlptr_free_set, 1);
    rb_define_method(rb_cDLCPtr, "free",  rb_dlptr_free_get, 0);
    rb_define_method(rb_cDLCPtr, "to_i",  rb_dlptr_to_i, 0);
    rb_define_method(rb_cDLCPtr, "to_int",  rb_dlptr_to_i, 0);
    rb_define_method(rb_cDLCPtr, "to_value",  rb_dlptr_to_value, 0);
    rb_define_method(rb_cDLCPtr, "ptr",   rb_dlptr_ptr, 0);
    rb_define_method(rb_cDLCPtr, "+@", rb_dlptr_ptr, 0);
    rb_define_method(rb_cDLCPtr, "ref",   rb_dlptr_ref, 0);
    rb_define_method(rb_cDLCPtr, "-@", rb_dlptr_ref, 0);
    rb_define_method(rb_cDLCPtr, "null?", rb_dlptr_null_p, 0);
    rb_define_method(rb_cDLCPtr, "to_s", rb_dlptr_to_s, -1);
    rb_define_method(rb_cDLCPtr, "to_str", rb_dlptr_to_str, -1);
    rb_define_method(rb_cDLCPtr, "inspect", rb_dlptr_inspect, 0);
    rb_define_method(rb_cDLCPtr, "<=>", rb_dlptr_cmp, 1);
    rb_define_method(rb_cDLCPtr, "==", rb_dlptr_eql, 1);
    rb_define_method(rb_cDLCPtr, "eql?", rb_dlptr_eql, 1);
    rb_define_method(rb_cDLCPtr, "+", rb_dlptr_plus, 1);
    rb_define_method(rb_cDLCPtr, "-", rb_dlptr_minus, 1);
    rb_define_method(rb_cDLCPtr, "[]", rb_dlptr_aref, -1);
    rb_define_method(rb_cDLCPtr, "[]=", rb_dlptr_aset, -1);
    rb_define_method(rb_cDLCPtr, "size", rb_dlptr_size_get, 0);
    rb_define_method(rb_cDLCPtr, "size=", rb_dlptr_size_set, 1);

    /*  Document-const: NULL
     *
     * A NULL pointer
     */
    rb_define_const(rb_mDL, "NULL", rb_dlptr_new(0, 0, 0));
}
