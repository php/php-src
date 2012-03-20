#include <fiddle.h>

VALUE cFiddleFunction;

static void
deallocate(void *p)
{
    ffi_cif *ptr = p;
    if (ptr->arg_types) xfree(ptr->arg_types);
    xfree(ptr);
}

static size_t
function_memsize(const void *p)
{
    /* const */ffi_cif *ptr = (ffi_cif *)p;
    size_t size = 0;

    if (ptr) {
	size += sizeof(*ptr);
#if !defined(FFI_NO_RAW_API) || !FFI_NO_RAW_API
	size += ffi_raw_size(ptr);
#endif
    }
    return size;
}

const rb_data_type_t function_data_type = {
    "fiddle/function",
    {0, deallocate, function_memsize,},
};

static VALUE
allocate(VALUE klass)
{
    ffi_cif * cif;

    return TypedData_Make_Struct(klass, ffi_cif, &function_data_type, cif);
}

static VALUE
initialize(int argc, VALUE argv[], VALUE self)
{
    ffi_cif * cif;
    ffi_type **arg_types;
    ffi_status result;
    VALUE ptr, args, ret_type, abi;
    int i;

    rb_scan_args(argc, argv, "31", &ptr, &args, &ret_type, &abi);
    if(NIL_P(abi)) abi = INT2NUM(FFI_DEFAULT_ABI);

    Check_Type(args, T_ARRAY);

    rb_iv_set(self, "@ptr", ptr);
    rb_iv_set(self, "@args", args);
    rb_iv_set(self, "@return_type", ret_type);
    rb_iv_set(self, "@abi", abi);

    TypedData_Get_Struct(self, ffi_cif, &function_data_type, cif);

    arg_types = xcalloc(RARRAY_LEN(args) + 1, sizeof(ffi_type *));

    for (i = 0; i < RARRAY_LEN(args); i++) {
	int type = NUM2INT(RARRAY_PTR(args)[i]);
	arg_types[i] = INT2FFI_TYPE(type);
    }
    arg_types[RARRAY_LEN(args)] = NULL;

    result = ffi_prep_cif (
	    cif,
	    NUM2INT(abi),
	    RARRAY_LENINT(args),
	    INT2FFI_TYPE(NUM2INT(ret_type)),
	    arg_types);

    if (result)
	rb_raise(rb_eRuntimeError, "error creating CIF %d", result);

    return self;
}

static VALUE
function_call(int argc, VALUE argv[], VALUE self)
{
    ffi_cif * cif;
    fiddle_generic retval;
    fiddle_generic *generic_args;
    void **values;
    VALUE cfunc, types, cPointer;
    int i;

    cfunc    = rb_iv_get(self, "@ptr");
    types    = rb_iv_get(self, "@args");
    cPointer = rb_const_get(mFiddle, rb_intern("Pointer"));

    if(argc != RARRAY_LENINT(types)) {
	rb_raise(rb_eArgError, "wrong number of arguments (%d for %d)",
		argc, RARRAY_LENINT(types));
    }

    TypedData_Get_Struct(self, ffi_cif, &function_data_type, cif);

    values = xcalloc((size_t)argc + 1, (size_t)sizeof(void *));
    generic_args = xcalloc((size_t)argc, (size_t)sizeof(fiddle_generic));

    for (i = 0; i < argc; i++) {
	VALUE type = RARRAY_PTR(types)[i];
	VALUE src = argv[i];

	if(NUM2INT(type) == TYPE_VOIDP) {
	    if(NIL_P(src)) {
		src = INT2NUM(0);
	    } else if(cPointer != CLASS_OF(src)) {
	        src = rb_funcall(cPointer, rb_intern("[]"), 1, src);
	    }
	    src = rb_Integer(src);
	}

	VALUE2GENERIC(NUM2INT(type), src, &generic_args[i]);
	values[i] = (void *)&generic_args[i];
    }
    values[argc] = NULL;

    ffi_call(cif, NUM2PTR(rb_Integer(cfunc)), &retval, values);

    rb_funcall(mFiddle, rb_intern("last_error="), 1, INT2NUM(errno));
#if defined(_WIN32)
    rb_funcall(mFiddle, rb_intern("win32_last_error="), 1, INT2NUM(errno));
#endif

    xfree(values);
    xfree(generic_args);

    return GENERIC2VALUE(rb_iv_get(self, "@return_type"), retval);
}

void
Init_fiddle_function(void)
{
    /*
     * Document-class: Fiddle::Function
     *
     * == Description
     *
     * A representation of a C function
     *
     * == Examples
     *
     * === 'strcpy'
     *
     *   @libc = DL.dlopen "/lib/libc.so.6"
     *   => #<DL::Handle:0x00000001d7a8d8>
     *   f = Fiddle::Function.new(@libc['strcpy'], [TYPE_VOIDP, TYPE_VOIDP], TYPE_VOIDP)
     *   => #<Fiddle::Function:0x00000001d8ee00>
     *   buff = "000"
     *   => "000"
     *   str = f.call(buff, "123")
     *   => #<DL::CPtr:0x00000001d0c380 ptr=0x000000018a21b8 size=0 free=0x00000000000000>
     *   str.to_s
     *   => "123"
     *
     * === ABI check
     *
     *   @libc = DL.dlopen "/lib/libc.so.6"
     *   => #<DL::Handle:0x00000001d7a8d8>
     *   f = Fiddle::Function.new(@libc['strcpy'], [TYPE_VOIDP, TYPE_VOIDP], TYPE_VOIDP)
     *   => #<Fiddle::Function:0x00000001d8ee00>
     *   f.abi == Fiddle::Function::DEFAULT
     *   => true
     */
    cFiddleFunction = rb_define_class_under(mFiddle, "Function", rb_cObject);

    /*
     * Document-const: DEFAULT
     *
     * Default ABI
     *
     */
    rb_define_const(cFiddleFunction, "DEFAULT", INT2NUM(FFI_DEFAULT_ABI));

#ifdef FFI_STDCALL
    /*
     * Document-const: STDCALL
     *
     * FFI implementation of WIN32 stdcall convention
     *
     */
    rb_define_const(cFiddleFunction, "STDCALL", INT2NUM(FFI_STDCALL));
#endif

    rb_define_alloc_func(cFiddleFunction, allocate);

    /*
     * Document-method: call
     *
     * Calls the constructed Function, with +args+
     *
     * For an example see Fiddle::Function
     *
     */
    rb_define_method(cFiddleFunction, "call", function_call, -1);

    /*
     * Document-method: new
     * call-seq: new(ptr, *args, ret_type, abi = DEFAULT)
     *
     * Constructs a Function object.
     * * +ptr+ is a referenced function, of a DL::Handle
     * * +args+ is an Array of arguments, passed to the +ptr+ function
     * * +ret_type+ is the return type of the function
     * * +abi+ is the ABI of the function
     *
     */
    rb_define_method(cFiddleFunction, "initialize", initialize, -1);
}
/* vim: set noet sws=4 sw=4: */
