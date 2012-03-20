#include <fiddle.h>

VALUE mFiddle;

#ifndef TYPE_SSIZE_T
# if SIZEOF_SIZE_T == SIZEOF_INT
#   define TYPE_SSIZE_T TYPE_INT
# elif SIZEOF_SIZE_T == SIZEOF_LONG
#   define TYPE_SSIZE_T TYPE_LONG
# elif defined HAVE_LONG_LONG && SIZEOF_SIZE_T == SIZEOF_LONG_LONG
#   define TYPE_SSIZE_T TYPE_LONG_LONG
# endif
#endif
#define TYPE_SIZE_T (-1*SIGNEDNESS_OF_SIZE_T*TYPE_SSIZE_T)

#ifndef TYPE_PTRDIFF_T
# if SIZEOF_PTRDIFF_T == SIZEOF_INT
#   define TYPE_PTRDIFF_T TYPE_INT
# elif SIZEOF_PTRDIFF_T == SIZEOF_LONG
#   define TYPE_PTRDIFF_T TYPE_LONG
# elif defined HAVE_LONG_LONG && SIZEOF_PTRDIFF_T == SIZEOF_LONG_LONG
#   define TYPE_PTRDIFF_T TYPE_LONG_LONG
# endif
#endif

#ifndef TYPE_INTPTR_T
# if SIZEOF_INTPTR_T == SIZEOF_INT
#   define TYPE_INTPTR_T TYPE_INT
# elif SIZEOF_INTPTR_T == SIZEOF_LONG
#   define TYPE_INTPTR_T TYPE_LONG
# elif defined HAVE_LONG_LONG && SIZEOF_INTPTR_T == SIZEOF_LONG_LONG
#   define TYPE_INTPTR_T TYPE_LONG_LONG
# endif
#endif
#define TYPE_UINTPTR_T (-TYPE_INTPTR_T)

void
Init_fiddle(void)
{
    /*
     * Document-module: Fiddle
     *
     * == Description
     *
     * A libffi wrapper.
     *
     */
    mFiddle = rb_define_module("Fiddle");

    /* Document-const: TYPE_VOID
     *
     * C type - void
     */
    rb_define_const(mFiddle, "TYPE_VOID",      INT2NUM(TYPE_VOID));

    /* Document-const: TYPE_VOIDP
     *
     * C type - void*
     */
    rb_define_const(mFiddle, "TYPE_VOIDP",     INT2NUM(TYPE_VOIDP));

    /* Document-const: TYPE_CHAR
     *
     * C type - char
     */
    rb_define_const(mFiddle, "TYPE_CHAR",      INT2NUM(TYPE_CHAR));

    /* Document-const: TYPE_SHORT
     *
     * C type - short
     */
    rb_define_const(mFiddle, "TYPE_SHORT",     INT2NUM(TYPE_SHORT));

    /* Document-const: TYPE_INT
     *
     * C type - int
     */
    rb_define_const(mFiddle, "TYPE_INT",       INT2NUM(TYPE_INT));

    /* Document-const: TYPE_LONG
     *
     * C type - long
     */
    rb_define_const(mFiddle, "TYPE_LONG",      INT2NUM(TYPE_LONG));

#if HAVE_LONG_LONG
    /* Document-const: TYPE_LONG_LONG
     *
     * C type - long long
     */
    rb_define_const(mFiddle, "TYPE_LONG_LONG", INT2NUM(TYPE_LONG_LONG));
#endif

    /* Document-const: TYPE_FLOAT
     *
     * C type - float
     */
    rb_define_const(mFiddle, "TYPE_FLOAT",     INT2NUM(TYPE_FLOAT));

    /* Document-const: TYPE_DOUBLE
     *
     * C type - double
     */
    rb_define_const(mFiddle, "TYPE_DOUBLE",    INT2NUM(TYPE_DOUBLE));

    /* Document-const: TYPE_SIZE_T
     *
     * C type - size_t
     */
    rb_define_const(mFiddle, "TYPE_SIZE_T",   INT2NUM(TYPE_SIZE_T));

    /* Document-const: TYPE_SSIZE_T
     *
     * C type - ssize_t
     */
    rb_define_const(mFiddle, "TYPE_SSIZE_T",   INT2NUM(TYPE_SSIZE_T));

    /* Document-const: TYPE_PTRDIFF_T
     *
     * C type - ptrdiff_t
     */
    rb_define_const(mFiddle, "TYPE_PTRDIFF_T", INT2NUM(TYPE_PTRDIFF_T));

    /* Document-const: TYPE_INTPTR_T
     *
     * C type - intptr_t
     */
    rb_define_const(mFiddle, "TYPE_INTPTR_T",  INT2NUM(TYPE_INTPTR_T));

    /* Document-const: TYPE_UINTPTR_T
     *
     * C type - uintptr_t
     */
    rb_define_const(mFiddle, "TYPE_UINTPTR_T",  INT2NUM(TYPE_UINTPTR_T));

    /* Document-const: WINDOWS
     *
     * Returns a boolean regarding whether the host is WIN32
     */
#if defined(_WIN32)
    rb_define_const(mFiddle, "WINDOWS", Qtrue);
#else
    rb_define_const(mFiddle, "WINDOWS", Qfalse);
#endif

    Init_fiddle_function();
    Init_fiddle_closure();
}
/* vim: set noet sws=4 sw=4: */
