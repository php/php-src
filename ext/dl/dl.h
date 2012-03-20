#ifndef RUBY_DL_H
#define RUBY_DL_H

#include <ruby.h>

#if !defined(FUNC_CDECL)
#  define FUNC_CDECL(x) x
#endif

#if defined(HAVE_DLFCN_H)
# include <dlfcn.h>
# /* some stranger systems may not define all of these */
#ifndef RTLD_LAZY
#define RTLD_LAZY 0
#endif
#ifndef RTLD_GLOBAL
#define RTLD_GLOBAL 0
#endif
#ifndef RTLD_NOW
#define RTLD_NOW 0
#endif
#else
# if defined(_WIN32)
#   include <windows.h>
#   define dlopen(name,flag) ((void*)LoadLibrary(name))
#   define dlerror() strerror(rb_w32_map_errno(GetLastError()))
#   define dlsym(handle,name) ((void*)GetProcAddress((handle),(name)))
#   define RTLD_LAZY -1
#   define RTLD_NOW  -1
#   define RTLD_GLOBAL -1
# endif
#endif

#define MAX_CALLBACK 5
#define DLSTACK_TYPE SIGNED_VALUE
#define DLSTACK_SIZE (20)
#define DLSTACK_PROTO \
    DLSTACK_TYPE,DLSTACK_TYPE,DLSTACK_TYPE,DLSTACK_TYPE,DLSTACK_TYPE,\
    DLSTACK_TYPE,DLSTACK_TYPE,DLSTACK_TYPE,DLSTACK_TYPE,DLSTACK_TYPE,\
    DLSTACK_TYPE,DLSTACK_TYPE,DLSTACK_TYPE,DLSTACK_TYPE,DLSTACK_TYPE,\
    DLSTACK_TYPE,DLSTACK_TYPE,DLSTACK_TYPE,DLSTACK_TYPE,DLSTACK_TYPE
#define DLSTACK_ARGS(stack) \
    (stack)[0],(stack)[1],(stack)[2],(stack)[3],(stack)[4],\
    (stack)[5],(stack)[6],(stack)[7],(stack)[8],(stack)[9],\
    (stack)[10],(stack)[11],(stack)[12],(stack)[13],(stack)[14],\
    (stack)[15],(stack)[16],(stack)[17],(stack)[18],(stack)[19]

#define DLSTACK_PROTO0_ void
#define DLSTACK_PROTO1_ DLSTACK_TYPE
#define DLSTACK_PROTO2_ DLSTACK_PROTO1_, DLSTACK_TYPE
#define DLSTACK_PROTO3_ DLSTACK_PROTO2_, DLSTACK_TYPE
#define DLSTACK_PROTO4_ DLSTACK_PROTO3_, DLSTACK_TYPE
#define DLSTACK_PROTO4_ DLSTACK_PROTO3_, DLSTACK_TYPE
#define DLSTACK_PROTO5_ DLSTACK_PROTO4_, DLSTACK_TYPE
#define DLSTACK_PROTO6_ DLSTACK_PROTO5_, DLSTACK_TYPE
#define DLSTACK_PROTO7_ DLSTACK_PROTO6_, DLSTACK_TYPE
#define DLSTACK_PROTO8_ DLSTACK_PROTO7_, DLSTACK_TYPE
#define DLSTACK_PROTO9_ DLSTACK_PROTO8_, DLSTACK_TYPE
#define DLSTACK_PROTO10_ DLSTACK_PROTO9_, DLSTACK_TYPE
#define DLSTACK_PROTO11_ DLSTACK_PROTO10_, DLSTACK_TYPE
#define DLSTACK_PROTO12_ DLSTACK_PROTO11_, DLSTACK_TYPE
#define DLSTACK_PROTO13_ DLSTACK_PROTO12_, DLSTACK_TYPE
#define DLSTACK_PROTO14_ DLSTACK_PROTO13_, DLSTACK_TYPE
#define DLSTACK_PROTO14_ DLSTACK_PROTO13_, DLSTACK_TYPE
#define DLSTACK_PROTO15_ DLSTACK_PROTO14_, DLSTACK_TYPE
#define DLSTACK_PROTO16_ DLSTACK_PROTO15_, DLSTACK_TYPE
#define DLSTACK_PROTO17_ DLSTACK_PROTO16_, DLSTACK_TYPE
#define DLSTACK_PROTO18_ DLSTACK_PROTO17_, DLSTACK_TYPE
#define DLSTACK_PROTO19_ DLSTACK_PROTO18_, DLSTACK_TYPE
#define DLSTACK_PROTO20_ DLSTACK_PROTO19_, DLSTACK_TYPE

/*
 * Add ",..." as the last argument.
 * This is required for variable argument functions such
 * as fprintf() on x86_64-linux.
 *
 * http://refspecs.linuxfoundation.org/elf/x86_64-abi-0.95.pdf
 * page 19:
 *
 *   For calls that may call functions that use varargs or stdargs
 *   (prototype-less calls or calls to functions containing ellipsis
 *   (...) in the declaration) %al is used as hidden argument to
 *   specify the number of SSE registers used.
 */
#define DLSTACK_PROTO0 void
#define DLSTACK_PROTO1 DLSTACK_PROTO1_, ...
#define DLSTACK_PROTO2 DLSTACK_PROTO2_, ...
#define DLSTACK_PROTO3 DLSTACK_PROTO3_, ...
#define DLSTACK_PROTO4 DLSTACK_PROTO4_, ...
#define DLSTACK_PROTO4 DLSTACK_PROTO4_, ...
#define DLSTACK_PROTO5 DLSTACK_PROTO5_, ...
#define DLSTACK_PROTO6 DLSTACK_PROTO6_, ...
#define DLSTACK_PROTO7 DLSTACK_PROTO7_, ...
#define DLSTACK_PROTO8 DLSTACK_PROTO8_, ...
#define DLSTACK_PROTO9 DLSTACK_PROTO9_, ...
#define DLSTACK_PROTO10 DLSTACK_PROTO10_, ...
#define DLSTACK_PROTO11 DLSTACK_PROTO11_, ...
#define DLSTACK_PROTO12 DLSTACK_PROTO12_, ...
#define DLSTACK_PROTO13 DLSTACK_PROTO13_, ...
#define DLSTACK_PROTO14 DLSTACK_PROTO14_, ...
#define DLSTACK_PROTO14 DLSTACK_PROTO14_, ...
#define DLSTACK_PROTO15 DLSTACK_PROTO15_, ...
#define DLSTACK_PROTO16 DLSTACK_PROTO16_, ...
#define DLSTACK_PROTO17 DLSTACK_PROTO17_, ...
#define DLSTACK_PROTO18 DLSTACK_PROTO18_, ...
#define DLSTACK_PROTO19 DLSTACK_PROTO19_, ...
#define DLSTACK_PROTO20 DLSTACK_PROTO20_, ...

#define DLSTACK_ARGS0(stack)
#define DLSTACK_ARGS1(stack) (stack)[0]
#define DLSTACK_ARGS2(stack) DLSTACK_ARGS1(stack), (stack)[1]
#define DLSTACK_ARGS3(stack) DLSTACK_ARGS2(stack), (stack)[2]
#define DLSTACK_ARGS4(stack) DLSTACK_ARGS3(stack), (stack)[3]
#define DLSTACK_ARGS5(stack) DLSTACK_ARGS4(stack), (stack)[4]
#define DLSTACK_ARGS6(stack) DLSTACK_ARGS5(stack), (stack)[5]
#define DLSTACK_ARGS7(stack) DLSTACK_ARGS6(stack), (stack)[6]
#define DLSTACK_ARGS8(stack) DLSTACK_ARGS7(stack), (stack)[7]
#define DLSTACK_ARGS9(stack) DLSTACK_ARGS8(stack), (stack)[8]
#define DLSTACK_ARGS10(stack) DLSTACK_ARGS9(stack), (stack)[9]
#define DLSTACK_ARGS11(stack) DLSTACK_ARGS10(stack), (stack)[10]
#define DLSTACK_ARGS12(stack) DLSTACK_ARGS11(stack), (stack)[11]
#define DLSTACK_ARGS13(stack) DLSTACK_ARGS12(stack), (stack)[12]
#define DLSTACK_ARGS14(stack) DLSTACK_ARGS13(stack), (stack)[13]
#define DLSTACK_ARGS15(stack) DLSTACK_ARGS14(stack), (stack)[14]
#define DLSTACK_ARGS16(stack) DLSTACK_ARGS15(stack), (stack)[15]
#define DLSTACK_ARGS17(stack) DLSTACK_ARGS16(stack), (stack)[16]
#define DLSTACK_ARGS18(stack) DLSTACK_ARGS17(stack), (stack)[17]
#define DLSTACK_ARGS19(stack) DLSTACK_ARGS18(stack), (stack)[18]
#define DLSTACK_ARGS20(stack) DLSTACK_ARGS19(stack), (stack)[19]

extern VALUE rb_mDL;
extern VALUE rb_cDLHandle;
extern VALUE rb_cDLSymbol;
extern VALUE rb_eDLError;
extern VALUE rb_eDLTypeError;

#define ALIGN_OF(type) offsetof(struct {char align_c; type align_x;}, align_x)

#define ALIGN_VOIDP  ALIGN_OF(void*)
#define ALIGN_SHORT  ALIGN_OF(short)
#define ALIGN_CHAR   ALIGN_OF(char)
#define ALIGN_INT    ALIGN_OF(int)
#define ALIGN_LONG   ALIGN_OF(long)
#if HAVE_LONG_LONG
#define ALIGN_LONG_LONG ALIGN_OF(LONG_LONG)
#endif
#define ALIGN_FLOAT  ALIGN_OF(float)
#define ALIGN_DOUBLE ALIGN_OF(double)

#define DLALIGN(ptr,offset,align) \
    ((offset) += ((align) - ((uintptr_t)((char *)(ptr) + (offset))) % (align)) % (align))


#define DLTYPE_VOID  0
#define DLTYPE_VOIDP 1
#define DLTYPE_CHAR  2
#define DLTYPE_SHORT 3
#define DLTYPE_INT   4
#define DLTYPE_LONG  5
#if HAVE_LONG_LONG
#define DLTYPE_LONG_LONG 6
#endif
#define DLTYPE_FLOAT 7
#define DLTYPE_DOUBLE 8
#define MAX_DLTYPE 9

#if SIZEOF_VOIDP == SIZEOF_LONG
# define PTR2NUM(x)   (ULONG2NUM((unsigned long)(x)))
# define NUM2PTR(x)   ((void*)(NUM2ULONG(x)))
#else
/* # error --->> Ruby/DL2 requires sizeof(void*) == sizeof(long) to be compiled. <<--- */
# define PTR2NUM(x)   (ULL2NUM((unsigned long long)(x)))
# define NUM2PTR(x)   ((void*)(NUM2ULL(x)))
#endif

#define BOOL2INT(x)  (((x) == Qtrue)?1:0)
#define INT2BOOL(x)  ((x)?Qtrue:Qfalse)

typedef void (*freefunc_t)(void*);

struct dl_handle {
    void *ptr;
    int  open;
    int  enable_close;
};


struct cfunc_data {
    void *ptr;
    char *name;
    int  type;
    ID   calltype;
    VALUE wrap;
};
extern ID rbdl_id_cdecl;
extern ID rbdl_id_stdcall;
#define CFUNC_CDECL   (rbdl_id_cdecl)
#define CFUNC_STDCALL (rbdl_id_stdcall)

struct ptr_data {
    void *ptr;
    long size;
    freefunc_t free;
    VALUE wrap[2];
};

#define RDL_HANDLE(obj) ((struct dl_handle *)(DATA_PTR(obj)))
#define RCFUNC_DATA(obj) ((struct cfunc_data *)(DATA_PTR(obj)))
#define RPTR_DATA(obj) ((struct ptr_data *)(DATA_PTR(obj)))

VALUE rb_dlcfunc_new(void (*func)(), int dltype, const char * name, ID calltype);
int rb_dlcfunc_kind_p(VALUE func);
VALUE rb_dlptr_new(void *ptr, long size, freefunc_t func);
VALUE rb_dlptr_new2(VALUE klass, void *ptr, long size, freefunc_t func);
VALUE rb_dlptr_malloc(long size, freefunc_t func);

#endif
