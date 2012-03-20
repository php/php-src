/* -*- C -*-
 * $Id$
 */

#include <ruby.h>
#include "dl.h"

VALUE rb_cDLHandle;

#ifdef _WIN32
# ifndef _WIN32_WCE
static void *
w32_coredll(void)
{
    MEMORY_BASIC_INFORMATION m;
    memset(&m, 0, sizeof(m));
    if( !VirtualQuery(_errno, &m, sizeof(m)) ) return NULL;
    return m.AllocationBase;
}
# endif

static int
w32_dlclose(void *ptr)
{
# ifndef _WIN32_WCE
    if( ptr == w32_coredll() ) return 0;
# endif
    if( FreeLibrary((HMODULE)ptr) ) return 0;
    return errno = rb_w32_map_errno(GetLastError());
}
#define dlclose(ptr) w32_dlclose(ptr)
#endif

static void
dlhandle_free(void *ptr)
{
    struct dl_handle *dlhandle = ptr;
    if( dlhandle->ptr && dlhandle->open && dlhandle->enable_close ){
	dlclose(dlhandle->ptr);
    }
}

static size_t
dlhandle_memsize(const void *ptr)
{
    return ptr ? sizeof(struct dl_handle) : 0;
}

static const rb_data_type_t dlhandle_data_type = {
    "dl/handle",
    {0, dlhandle_free, dlhandle_memsize,},
};

/*
 * call-seq: close
 *
 * Close this DL::Handle.  Calling close more than once will raise a
 * DL::DLError exception.
 */
VALUE
rb_dlhandle_close(VALUE self)
{
    struct dl_handle *dlhandle;

    TypedData_Get_Struct(self, struct dl_handle, &dlhandle_data_type, dlhandle);
    if(dlhandle->open) {
	int ret = dlclose(dlhandle->ptr);
	dlhandle->open = 0;

	/* Check dlclose for successful return value */
	if(ret) {
#if defined(HAVE_DLERROR)
	    rb_raise(rb_eDLError, "%s", dlerror());
#else
	    rb_raise(rb_eDLError, "could not close handle");
#endif
	}
	return INT2NUM(ret);
    }
    rb_raise(rb_eDLError, "dlclose() called too many times");
}

VALUE
rb_dlhandle_s_allocate(VALUE klass)
{
    VALUE obj;
    struct dl_handle *dlhandle;

    obj = TypedData_Make_Struct(rb_cDLHandle, struct dl_handle, &dlhandle_data_type, dlhandle);
    dlhandle->ptr  = 0;
    dlhandle->open = 0;
    dlhandle->enable_close = 0;

    return obj;
}

static VALUE
predefined_dlhandle(void *handle)
{
    VALUE obj = rb_dlhandle_s_allocate(rb_cDLHandle);
    struct dl_handle *dlhandle = DATA_PTR(obj);

    dlhandle->ptr = handle;
    dlhandle->open = 1;
    OBJ_FREEZE(obj);
    return obj;
}

/*
 * call-seq:
 *    initialize(lib = nil, flags = DL::RTLD_LAZY | DL::RTLD_GLOBAL)
 *
 * Create a new handler that opens library named +lib+ with +flags+.  If no
 * library is specified, RTLD_DEFAULT is used.
 */
VALUE
rb_dlhandle_initialize(int argc, VALUE argv[], VALUE self)
{
    void *ptr;
    struct dl_handle *dlhandle;
    VALUE lib, flag;
    char  *clib;
    int   cflag;
    const char *err;

    switch( rb_scan_args(argc, argv, "02", &lib, &flag) ){
      case 0:
	clib = NULL;
	cflag = RTLD_LAZY | RTLD_GLOBAL;
	break;
      case 1:
	clib = NIL_P(lib) ? NULL : StringValuePtr(lib);
	cflag = RTLD_LAZY | RTLD_GLOBAL;
	break;
      case 2:
	clib = NIL_P(lib) ? NULL : StringValuePtr(lib);
	cflag = NUM2INT(flag);
	break;
      default:
	rb_bug("rb_dlhandle_new");
    }

    rb_secure(2);

#if defined(_WIN32)
    if( !clib ){
	HANDLE rb_libruby_handle(void);
	ptr = rb_libruby_handle();
    }
    else if( STRCASECMP(clib, "libc") == 0
# ifdef RUBY_COREDLL
	     || STRCASECMP(clib, RUBY_COREDLL) == 0
	     || STRCASECMP(clib, RUBY_COREDLL".dll") == 0
# endif
	){
# ifdef _WIN32_WCE
	ptr = dlopen("coredll.dll", cflag);
# else
	ptr = w32_coredll();
# endif
    }
    else
#endif
	ptr = dlopen(clib, cflag);
#if defined(HAVE_DLERROR)
    if( !ptr && (err = dlerror()) ){
	rb_raise(rb_eDLError, "%s", err);
    }
#else
    if( !ptr ){
	err = dlerror();
	rb_raise(rb_eDLError, "%s", err);
    }
#endif
    TypedData_Get_Struct(self, struct dl_handle, &dlhandle_data_type, dlhandle);
    if( dlhandle->ptr && dlhandle->open && dlhandle->enable_close ){
	dlclose(dlhandle->ptr);
    }
    dlhandle->ptr = ptr;
    dlhandle->open = 1;
    dlhandle->enable_close = 0;

    if( rb_block_given_p() ){
	rb_ensure(rb_yield, self, rb_dlhandle_close, self);
    }

    return Qnil;
}

/*
 * call-seq: enable_close
 *
 * Enable a call to dlclose() when this DL::Handle is garbage collected.
 */
VALUE
rb_dlhandle_enable_close(VALUE self)
{
    struct dl_handle *dlhandle;

    TypedData_Get_Struct(self, struct dl_handle, &dlhandle_data_type, dlhandle);
    dlhandle->enable_close = 1;
    return Qnil;
}

/*
 * call-seq: disable_close
 *
 * Disable a call to dlclose() when this DL::Handle is garbage collected.
 */
VALUE
rb_dlhandle_disable_close(VALUE self)
{
    struct dl_handle *dlhandle;

    TypedData_Get_Struct(self, struct dl_handle, &dlhandle_data_type, dlhandle);
    dlhandle->enable_close = 0;
    return Qnil;
}

/*
 * call-seq: close_enabled?
 *
 * Returns +true+ if dlclose() will be called when this DL::Handle is
 * garbage collected.
 */
static VALUE
rb_dlhandle_close_enabled_p(VALUE self)
{
    struct dl_handle *dlhandle;

    TypedData_Get_Struct(self, struct dl_handle, &dlhandle_data_type, dlhandle);

    if(dlhandle->enable_close) return Qtrue;
    return Qfalse;
}

/*
 * call-seq: to_i
 *
 * Returns the memory address for this handle.
 */
VALUE
rb_dlhandle_to_i(VALUE self)
{
    struct dl_handle *dlhandle;

    TypedData_Get_Struct(self, struct dl_handle, &dlhandle_data_type, dlhandle);
    return PTR2NUM(dlhandle);
}

static VALUE dlhandle_sym(void *handle, const char *symbol);

/*
 * Document-method: sym
 * Document-method: []
 *
 * call-seq: sym(name)
 *
 * Get the address as an Integer for the function named +name+.
 */
VALUE
rb_dlhandle_sym(VALUE self, VALUE sym)
{
    struct dl_handle *dlhandle;

    TypedData_Get_Struct(self, struct dl_handle, &dlhandle_data_type, dlhandle);
    if( ! dlhandle->open ){
	rb_raise(rb_eDLError, "closed handle");
    }

    return dlhandle_sym(dlhandle->ptr, StringValueCStr(sym));
}

#ifndef RTLD_NEXT
#define RTLD_NEXT NULL
#endif
#ifndef RTLD_DEFAULT
#define RTLD_DEFAULT NULL
#endif

/*
 * Document-method: sym
 * Document-method: []
 *
 * call-seq: sym(name)
 *
 * Get the address as an Integer for the function named +name+.  The function
 * is searched via dlsym on RTLD_NEXT.  See man(3) dlsym() for more info.
 */
VALUE
rb_dlhandle_s_sym(VALUE self, VALUE sym)
{
    return dlhandle_sym(RTLD_NEXT, StringValueCStr(sym));
}

static VALUE
dlhandle_sym(void *handle, const char *name)
{
#if defined(HAVE_DLERROR)
    const char *err;
# define CHECK_DLERROR if( err = dlerror() ){ func = 0; }
#else
# define CHECK_DLERROR
#endif
    void (*func)();

    rb_secure(2);
#ifdef HAVE_DLERROR
    dlerror();
#endif
    func = (void (*)())(VALUE)dlsym(handle, name);
    CHECK_DLERROR;
#if defined(FUNC_STDCALL)
    if( !func ){
	int  i;
	int  len = (int)strlen(name);
	char *name_n;
#if defined(__CYGWIN__) || defined(_WIN32) || defined(__MINGW32__)
	{
	    char *name_a = (char*)xmalloc(len+2);
	    strcpy(name_a, name);
	    name_n = name_a;
	    name_a[len]   = 'A';
	    name_a[len+1] = '\0';
	    func = dlsym(handle, name_a);
	    CHECK_DLERROR;
	    if( func ) goto found;
	    name_n = xrealloc(name_a, len+6);
	}
#else
	name_n = (char*)xmalloc(len+6);
#endif
	memcpy(name_n, name, len);
	name_n[len++] = '@';
	for( i = 0; i < 256; i += 4 ){
	    sprintf(name_n + len, "%d", i);
	    func = dlsym(handle, name_n);
	    CHECK_DLERROR;
	    if( func ) break;
	}
	if( func ) goto found;
	name_n[len-1] = 'A';
	name_n[len++] = '@';
	for( i = 0; i < 256; i += 4 ){
	    sprintf(name_n + len, "%d", i);
	    func = dlsym(handle, name_n);
	    CHECK_DLERROR;
	    if( func ) break;
	}
      found:
	xfree(name_n);
    }
#endif
    if( !func ){
	rb_raise(rb_eDLError, "unknown symbol \"%s\"", name);
    }

    return PTR2NUM(func);
}

void
Init_dlhandle(void)
{
    /*
     * Document-class: DL::Handle
     *
     * The DL::Handle is the manner to access the dynamic library
     *
     * == Example
     *
     * === Setup
     *
     *   libc_so = "/lib64/libc.so.6"
     *   => "/lib64/libc.so.6"
     *   @handle = DL::Handle.new(libc_so)
     *   => #<DL::Handle:0x00000000d69ef8>
     *
     * === Setup, with flags
     *
     *   libc_so = "/lib64/libc.so.6"
     *   => "/lib64/libc.so.6"
     *   @handle = DL::Handle.new(libc_so, DL::RTLD_LAZY | DL::RTLD_GLOBAL)
     *   => #<DL::Handle:0x00000000d69ef8>
     *
     * === Addresses to symbols
     *
     *   strcpy_addr = @handle['strcpy']
     *   => 140062278451968
     *
     * or
     *
     *   strcpy_addr = @handle.sym('strcpy')
     *   => 140062278451968
     *
     */
    rb_cDLHandle = rb_define_class_under(rb_mDL, "Handle", rb_cObject);
    rb_define_alloc_func(rb_cDLHandle, rb_dlhandle_s_allocate);
    rb_define_singleton_method(rb_cDLHandle, "sym", rb_dlhandle_s_sym, 1);
    rb_define_singleton_method(rb_cDLHandle, "[]", rb_dlhandle_s_sym,  1);

    /* Document-const: NEXT
     *
     * A predefined pseudo-handle of RTLD_NEXT
     *
     * Which will find the next occurrence of a function in the search order
     * after the current library.
     */
    rb_define_const(rb_cDLHandle, "NEXT", predefined_dlhandle(RTLD_NEXT));

    /* Document-const: DEFAULT
     *
     * A predefined pseudo-handle of RTLD_DEFAULT
     *
     * Which will find the first occurrence of the desired symbol using the
     * default library search order
     */
    rb_define_const(rb_cDLHandle, "DEFAULT", predefined_dlhandle(RTLD_DEFAULT));
    rb_define_method(rb_cDLHandle, "initialize", rb_dlhandle_initialize, -1);
    rb_define_method(rb_cDLHandle, "to_i", rb_dlhandle_to_i, 0);
    rb_define_method(rb_cDLHandle, "close", rb_dlhandle_close, 0);
    rb_define_method(rb_cDLHandle, "sym",  rb_dlhandle_sym, 1);
    rb_define_method(rb_cDLHandle, "[]",  rb_dlhandle_sym,  1);
    rb_define_method(rb_cDLHandle, "disable_close", rb_dlhandle_disable_close, 0);
    rb_define_method(rb_cDLHandle, "enable_close", rb_dlhandle_enable_close, 0);
    rb_define_method(rb_cDLHandle, "close_enabled?", rb_dlhandle_close_enabled_p, 0);
}

/* mode: c; tab-with=8; sw=4; ts=8; noexpandtab: */
