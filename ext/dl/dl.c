/*
 * ext/dl/dl.c
 *
 * doumentation:
 * - Vincent Batts (vbatts@hashbangbash.com)
 *
 */
#include <ruby/ruby.h>
#include <ruby/io.h>
#include <ctype.h>
#include "dl.h"

VALUE rb_mDL;
VALUE rb_eDLError;
VALUE rb_eDLTypeError;

ID rbdl_id_cdecl;
ID rbdl_id_stdcall;

#ifndef DLTYPE_SSIZE_T
# if SIZEOF_SIZE_T == SIZEOF_INT
#   define DLTYPE_SSIZE_T DLTYPE_INT
# elif SIZEOF_SIZE_T == SIZEOF_LONG
#   define DLTYPE_SSIZE_T DLTYPE_LONG
# elif defined HAVE_LONG_LONG && SIZEOF_SIZE_T == SIZEOF_LONG_LONG
#   define DLTYPE_SSIZE_T DLTYPE_LONG_LONG
# endif
#endif
#define DLTYPE_SIZE_T (-1*SIGNEDNESS_OF_SIZE_T*DLTYPE_SSIZE_T)

#ifndef DLTYPE_PTRDIFF_T
# if SIZEOF_PTRDIFF_T == SIZEOF_INT
#   define DLTYPE_PTRDIFF_T DLTYPE_INT
# elif SIZEOF_PTRDIFF_T == SIZEOF_LONG
#   define DLTYPE_PTRDIFF_T DLTYPE_LONG
# elif defined HAVE_LONG_LONG && SIZEOF_PTRDIFF_T == SIZEOF_LONG_LONG
#   define DLTYPE_PTRDIFF_T DLTYPE_LONG_LONG
# endif
#endif

#ifndef DLTYPE_INTPTR_T
# if SIZEOF_INTPTR_T == SIZEOF_INT
#   define DLTYPE_INTPTR_T DLTYPE_INT
# elif SIZEOF_INTPTR_T == SIZEOF_LONG
#   define DLTYPE_INTPTR_T DLTYPE_LONG
# elif defined HAVE_LONG_LONG && SIZEOF_INTPTR_T == SIZEOF_LONG_LONG
#   define DLTYPE_INTPTR_T DLTYPE_LONG_LONG
# endif
#endif
#define DLTYPE_UINTPTR_T (-DLTYPE_INTPTR_T)

VALUE
rb_dl_dlopen(int argc, VALUE argv[], VALUE self)
{
    return rb_class_new_instance(argc, argv, rb_cDLHandle);
}

/*
 * call-seq: DL.malloc
 *
 * Allocate +size+ bytes of memory and return the integer memory address
 * for the allocated memory.
 */
VALUE
rb_dl_malloc(VALUE self, VALUE size)
{
    void *ptr;

    rb_secure(4);
    ptr = (void*)ruby_xmalloc(NUM2INT(size));
    return PTR2NUM(ptr);
}

/*
 * call-seq: DL.realloc(addr, size)
 *
 * Change the size of the memory allocated at the memory location +addr+ to
 * +size+ bytes.  Returns the memory address of the reallocated memory, which
 * may be different than the address passed in.
 */
VALUE
rb_dl_realloc(VALUE self, VALUE addr, VALUE size)
{
    void *ptr = NUM2PTR(addr);

    rb_secure(4);
    ptr = (void*)ruby_xrealloc(ptr, NUM2INT(size));
    return PTR2NUM(ptr);
}

/*
 * call-seq: DL.free(addr)
 *
 * Free the memory at address +addr+
 */
VALUE
rb_dl_free(VALUE self, VALUE addr)
{
    void *ptr = NUM2PTR(addr);

    rb_secure(4);
    ruby_xfree(ptr);
    return Qnil;
}

VALUE
rb_dl_ptr2value(VALUE self, VALUE addr)
{
    rb_secure(4);
    return (VALUE)NUM2PTR(addr);
}

VALUE
rb_dl_value2ptr(VALUE self, VALUE val)
{
    return PTR2NUM((void*)val);
}

static void
rb_dl_init_callbacks(VALUE dl)
{
    static const char cb[] = "dl/callback.so";

    rb_autoload(dl, rb_intern_const("CdeclCallbackAddrs"), cb);
    rb_autoload(dl, rb_intern_const("CdeclCallbackProcs"), cb);
#ifdef FUNC_STDCALL
    rb_autoload(dl, rb_intern_const("StdcallCallbackAddrs"), cb);
    rb_autoload(dl, rb_intern_const("StdcallCallbackProcs"), cb);
#endif
}

void
Init_dl(void)
{
    void Init_dlhandle(void);
    void Init_dlcfunc(void);
    void Init_dlptr(void);

    rbdl_id_cdecl = rb_intern_const("cdecl");
    rbdl_id_stdcall = rb_intern_const("stdcall");

    /* Document-module: DL
     *
     * A bridge to the dlopen() or dynamic library linker function.
     *
     * == Example
     *
     *   bash $> cat > sum.c <<EOF
     *   double sum(double *arry, int len)
     *   {
     *           double ret = 0;
     *           int i;
     *           for(i = 0; i < len; i++){
     *                   ret = ret + arry[i];
     *           }
     *           return ret;
     *   }
     *
     *   double split(double num)
     *   {
     *           double ret = 0;
     *           ret = num / 2;
     *           return ret;
     *   }
     *   EOF
     *   bash $> gcc -o libsum.so -shared sum.c
     *   bash $> cat > sum.rb <<EOF
     *   require 'dl'
     *   require 'dl/import'
     *
     *   module LibSum
     *           extend DL::Importer
     *           dlload './libsum.so'
     *           extern 'double sum(double*, int)'
     *           extern 'double split(double)'
     *   end
     *
     *   a = [2.0, 3.0, 4.0]
     *
     *   sum = LibSum.sum(a.pack("d*"), a.count)
     *   p LibSum.split(sum)
     *   EOF
     *   bash $> ruby sum.rb
     *   4.5
     *
     * WIN! :-)
     */
    rb_mDL = rb_define_module("DL");

    /*
     * Document-class: DL::DLError
     *
     * standard dynamic load exception
     */
    rb_eDLError = rb_define_class_under(rb_mDL, "DLError", rb_eStandardError);

    /*
     * Document-class: DL::DLTypeError
     *
     * dynamic load incorrect type exception
     */
    rb_eDLTypeError = rb_define_class_under(rb_mDL, "DLTypeError", rb_eDLError);

    /* Document-const: MAX_CALLBACK
     *
     * Maximum number of callbacks
     */
    rb_define_const(rb_mDL, "MAX_CALLBACK", INT2NUM(MAX_CALLBACK));

    /* Document-const: DLSTACK_SIZE
     *
     * Dynamic linker stack size
     */
    rb_define_const(rb_mDL, "DLSTACK_SIZE", INT2NUM(DLSTACK_SIZE));

    rb_dl_init_callbacks(rb_mDL);

    /* Document-const: RTLD_GLOBAL
     *
     * rtld DL::Handle flag.
     *
     * The symbols defined by this library will be made available for symbol
     * resolution of subsequently loaded libraries.
     */
    rb_define_const(rb_mDL, "RTLD_GLOBAL", INT2NUM(RTLD_GLOBAL));

    /* Document-const: RTLD_LAZY
     *
     * rtld DL::Handle flag.
     *
     * Perform lazy binding.  Only resolve symbols as the code that references
     * them is executed.  If the  symbol is never referenced, then it is never
     * resolved.  (Lazy binding is only performed for function references;
     * references to variables are always immediately bound when the library
     * is loaded.)
     */
    rb_define_const(rb_mDL, "RTLD_LAZY",   INT2NUM(RTLD_LAZY));

    /* Document-const: RTLD_NOW
     *
     * rtld DL::Handle flag.
     *
     * If this value is specified or the environment variable LD_BIND_NOW is
     * set to a nonempty string, all undefined symbols in the library are
     * resolved before dlopen() returns.  If this cannot be done an error is
     * returned.
     */
    rb_define_const(rb_mDL, "RTLD_NOW",    INT2NUM(RTLD_NOW));

    /* Document-const: TYPE_VOID
     *
     * DL::CFunc type - void
     */
    rb_define_const(rb_mDL, "TYPE_VOID",  INT2NUM(DLTYPE_VOID));

    /* Document-const: TYPE_VOIDP
     *
     * DL::CFunc type - void*
     */
    rb_define_const(rb_mDL, "TYPE_VOIDP",  INT2NUM(DLTYPE_VOIDP));

    /* Document-const: TYPE_CHAR
     *
     * DL::CFunc type - char
     */
    rb_define_const(rb_mDL, "TYPE_CHAR",  INT2NUM(DLTYPE_CHAR));

    /* Document-const: TYPE_SHORT
     *
     * DL::CFunc type - short
     */
    rb_define_const(rb_mDL, "TYPE_SHORT",  INT2NUM(DLTYPE_SHORT));

    /* Document-const: TYPE_INT
     *
     * DL::CFunc type - int
     */
    rb_define_const(rb_mDL, "TYPE_INT",  INT2NUM(DLTYPE_INT));

    /* Document-const: TYPE_LONG
     *
     * DL::CFunc type - long
     */
    rb_define_const(rb_mDL, "TYPE_LONG",  INT2NUM(DLTYPE_LONG));

#if HAVE_LONG_LONG
    /* Document-const: TYPE_LONG_LONG
     *
     * DL::CFunc type - long long
     */
    rb_define_const(rb_mDL, "TYPE_LONG_LONG",  INT2NUM(DLTYPE_LONG_LONG));
#endif

    /* Document-const: TYPE_FLOAT
     *
     * DL::CFunc type - float
     */
    rb_define_const(rb_mDL, "TYPE_FLOAT",  INT2NUM(DLTYPE_FLOAT));

    /* Document-const: TYPE_DOUBLE
     *
     * DL::CFunc type - double
     */
    rb_define_const(rb_mDL, "TYPE_DOUBLE",  INT2NUM(DLTYPE_DOUBLE));

    /* Document-const: TYPE_SIZE_T
     *
     * DL::CFunc type - size_t
     */
    rb_define_const(rb_mDL, "TYPE_SIZE_T",  INT2NUM(DLTYPE_SIZE_T));

    /* Document-const: TYPE_SSIZE_T
     *
     * DL::CFunc type - ssize_t
     */
    rb_define_const(rb_mDL, "TYPE_SSIZE_T", INT2NUM(DLTYPE_SSIZE_T));

    /* Document-const: TYPE_PTRDIFF_T
     *
     * DL::CFunc type - ptrdiff_t
     */
    rb_define_const(rb_mDL, "TYPE_PTRDIFF_T", INT2NUM(DLTYPE_PTRDIFF_T));

    /* Document-const: TYPE_INTPTR_T
     *
     * DL::CFunc type - intptr_t
     */
    rb_define_const(rb_mDL, "TYPE_INTPTR_T", INT2NUM(DLTYPE_INTPTR_T));

    /* Document-const: TYPE_UINTPTR_T
     *
     * DL::CFunc type - uintptr_t
     */
    rb_define_const(rb_mDL, "TYPE_UINTPTR_T", INT2NUM(DLTYPE_UINTPTR_T));

    /* Document-const: ALIGN_VOIDP
     *
     * The alignment size of a void*
     */
    rb_define_const(rb_mDL, "ALIGN_VOIDP", INT2NUM(ALIGN_VOIDP));

    /* Document-const: ALIGN_CHAR
     *
     * The alignment size of a char
     */
    rb_define_const(rb_mDL, "ALIGN_CHAR",  INT2NUM(ALIGN_CHAR));

    /* Document-const: ALIGN_SHORT
     *
     * The alignment size of a short
     */
    rb_define_const(rb_mDL, "ALIGN_SHORT", INT2NUM(ALIGN_SHORT));

    /* Document-const: ALIGN_INT
     *
     * The alignment size of an int
     */
    rb_define_const(rb_mDL, "ALIGN_INT",   INT2NUM(ALIGN_INT));

    /* Document-const: ALIGN_LONG
     *
     * The alignment size of a long
     */
    rb_define_const(rb_mDL, "ALIGN_LONG",  INT2NUM(ALIGN_LONG));

#if HAVE_LONG_LONG
    /* Document-const: ALIGN_LONG_LONG
     *
     * The alignment size of a long long
     */
    rb_define_const(rb_mDL, "ALIGN_LONG_LONG",  INT2NUM(ALIGN_LONG_LONG));
#endif

    /* Document-const: ALIGN_FLOAT
     *
     * The alignment size of a float
     */
    rb_define_const(rb_mDL, "ALIGN_FLOAT", INT2NUM(ALIGN_FLOAT));

    /* Document-const: ALIGN_DOUBLE
     *
     * The alignment size of a double
     */
    rb_define_const(rb_mDL, "ALIGN_DOUBLE",INT2NUM(ALIGN_DOUBLE));

    /* Document-const: ALIGN_SIZE_T
     *
     * The alignment size of a size_t
     */
    rb_define_const(rb_mDL, "ALIGN_SIZE_T", INT2NUM(ALIGN_OF(size_t)));

    /* Document-const: ALIGN_SSIZE_T
     *
     * The alignment size of a ssize_t
     */
    rb_define_const(rb_mDL, "ALIGN_SSIZE_T", INT2NUM(ALIGN_OF(size_t))); /* same as size_t */

    /* Document-const: ALIGN_PTRDIFF_T
     *
     * The alignment size of a ptrdiff_t
     */
    rb_define_const(rb_mDL, "ALIGN_PTRDIFF_T", INT2NUM(ALIGN_OF(ptrdiff_t)));

    /* Document-const: ALIGN_INTPTR_T
     *
     * The alignment size of a intptr_t
     */
    rb_define_const(rb_mDL, "ALIGN_INTPTR_T", INT2NUM(ALIGN_OF(intptr_t)));

    /* Document-const: ALIGN_UINTPTR_T
     *
     * The alignment size of a uintptr_t
     */
    rb_define_const(rb_mDL, "ALIGN_UINTPTR_T", INT2NUM(ALIGN_OF(uintptr_t)));

    /* Document-const: SIZEOF_VOIDP
     *
     * size of a void*
     */
    rb_define_const(rb_mDL, "SIZEOF_VOIDP", INT2NUM(sizeof(void*)));

    /* Document-const: SIZEOF_CHAR
     *
     * size of a char
     */
    rb_define_const(rb_mDL, "SIZEOF_CHAR",  INT2NUM(sizeof(char)));

    /* Document-const: SIZEOF_SHORT
     *
     * size of a short
     */
    rb_define_const(rb_mDL, "SIZEOF_SHORT", INT2NUM(sizeof(short)));

    /* Document-const: SIZEOF_INT
     *
     * size of an int
     */
    rb_define_const(rb_mDL, "SIZEOF_INT",   INT2NUM(sizeof(int)));

    /* Document-const: SIZEOF_LONG
     *
     * size of a long
     */
    rb_define_const(rb_mDL, "SIZEOF_LONG",  INT2NUM(sizeof(long)));

#if HAVE_LONG_LONG
    /* Document-const: SIZEOF_LONG_LONG
     *
     * size of a long long
     */
    rb_define_const(rb_mDL, "SIZEOF_LONG_LONG",  INT2NUM(sizeof(LONG_LONG)));
#endif

    /* Document-const: SIZEOF_FLOAT
     *
     * size of a float
     */
    rb_define_const(rb_mDL, "SIZEOF_FLOAT", INT2NUM(sizeof(float)));

    /* Document-const: SIZEOF_DOUBLE
     *
     * size of a double
     */
    rb_define_const(rb_mDL, "SIZEOF_DOUBLE",INT2NUM(sizeof(double)));

    /* Document-const: SIZEOF_SIZE_T
     *
     * size of a size_t
     */
    rb_define_const(rb_mDL, "SIZEOF_SIZE_T",  INT2NUM(sizeof(size_t)));

    /* Document-const: SIZEOF_SSIZE_T
     *
     * size of a ssize_t
     */
    rb_define_const(rb_mDL, "SIZEOF_SSIZE_T",  INT2NUM(sizeof(size_t))); /* same as size_t */

    /* Document-const: SIZEOF_PTRDIFF_T
     *
     * size of a ptrdiff_t
     */
    rb_define_const(rb_mDL, "SIZEOF_PTRDIFF_T",  INT2NUM(sizeof(ptrdiff_t)));

    /* Document-const: SIZEOF_INTPTR_T
     *
     * size of a intptr_t
     */
    rb_define_const(rb_mDL, "SIZEOF_INTPTR_T",  INT2NUM(sizeof(intptr_t)));

    /* Document-const: SIZEOF_UINTPTR_T
     *
     * size of a intptr_t
     */
    rb_define_const(rb_mDL, "SIZEOF_UINTPTR_T",  INT2NUM(sizeof(uintptr_t)));

    rb_define_module_function(rb_mDL, "dlwrap", rb_dl_value2ptr, 1);
    rb_define_module_function(rb_mDL, "dlunwrap", rb_dl_ptr2value, 1);

    rb_define_module_function(rb_mDL, "dlopen", rb_dl_dlopen, -1);
    rb_define_module_function(rb_mDL, "malloc", rb_dl_malloc, 1);
    rb_define_module_function(rb_mDL, "realloc", rb_dl_realloc, 2);
    rb_define_module_function(rb_mDL, "free", rb_dl_free, 1);

    /* Document-const: RUBY_FREE
     *
     * Address of the ruby_xfree() function
     */
    rb_define_const(rb_mDL, "RUBY_FREE", PTR2NUM(ruby_xfree));

    /* Document-const: BUILD_RUBY_PLATFORM
     *
     * Platform built against (i.e. "x86_64-linux", etc.)
     *
     * See also RUBY_PLATFORM
     */
    rb_define_const(rb_mDL, "BUILD_RUBY_PLATFORM", rb_str_new2(RUBY_PLATFORM));

    /* Document-const: BUILD_RUBY_VERSION
     *
     * Ruby Version built. (i.e. "1.9.3")
     *
     * See also RUBY_VERSION
     */
    rb_define_const(rb_mDL, "BUILD_RUBY_VERSION",  rb_str_new2(RUBY_VERSION));

    Init_dlhandle();
    Init_dlcfunc();
    Init_dlptr();
}
