--TEST--
FFI 002: Check C declaration parser
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
echo "Empty declaration\n";
$ffi = FFI::cdef(<<<EOF
EOF
);
echo "  ok\n";

echo "Various declarations\n";
$ffi = FFI::cdef(<<<EOF
    /* allowed storage classes */
    typedef  int type1;
//	extern   int var2;
    static   int var3;
    auto     int var4;
    register int var5;

    /* allowed types */
    typedef void type6;
    typedef char type7;                     /* sint8_t or uint8_t */
    typedef signed char type8;              /* sint8_t  */
    typedef unsigned char type9;            /* uint8_t  */
    typedef short type10;                   /* sint16_t */
    typedef signed short type11;            /* sint16_t */
    typedef short int type12;               /* sint16_t */
    typedef signed short int type13;        /* sint16_t */
    typedef unsigned short type14;          /* uint16_t */
    typedef unsigned short int type15;      /* uint16_t */
    typedef int type16;                     /* sint32_t */
    typedef signed type17;                  /* sint32_t */
    typedef signed int type18;              /* sint32_t */
    typedef unsigned type19;                /* uint32_t */
    typedef unsigned int type20;            /* uint32_t */
    typedef long type21;                    /* sint32_t or sint64_t */
    typedef signed long type22;             /* sint32_t or sint64_t */
    typedef long int type23;                /* sint32_t or sint64_t */
    typedef signed long int type24;         /* sint32_t or sint64_t */
    typedef unsigned long type25;           /* uint32_t or uint64_t */
    typedef unsigned long int type25_2;     /* uint32_t or uint64_t */
    typedef long long type26;               /* sint64_t */
    typedef signed long long type27;        /* sint64_t */
    typedef long long int type28;           /* sint64_t */
    typedef signed long long int type29;    /* sint64_t */
    typedef unsigned long long type30;      /* uint64_t */
    typedef unsigned long long int type31;  /* uint64_t */
    typedef float type32;
    typedef double type33;
    typedef long double type34;
    typedef _Bool type35;
//	typedef float _Complex type36;
//	typedef double _Complex type36_2;
//	typedef long double _Complex type36_3;

    /* struct and union */
    struct tag1;
    union tag2;
    typedef struct tag1 {int x; int y;} type37;
    typedef union tag2 {int x; int y;} type38;
    typedef struct {int x, y; int z;} type39;
    typedef struct {unsigned int x:8, y:8;} type40;
    typedef struct {unsigned int x:8, :8, y:8;} type41;

    /* enum */
    enum tag3;
    typedef enum tag3 {A,B,C} type42;
    typedef enum {D,E=10,F,} type43;

    /* type qualifiers */
    typedef void* type46;
    typedef const void* type47;
    typedef restrict void* type48;
    typedef volatile void* type49;
    typedef _Atomic void* type50;
    typedef const volatile void* type51;

    /* function specifiers */
    static void f1();
    static inline void f2();
    static _Noreturn void f3();

    /* align specifier */
    typedef double _Alignas(char) type52;
    typedef double _Alignas(1) type53;

    /* pointers */
    typedef void * type54;
    typedef void ** type55;
    typedef const void * const volatile * const type56;

    /* arrays */
    typedef char type57[];
    typedef char type58[const];
    typedef char type59[const volatile];
    typedef char type60[10];
    typedef char type61[const 10];
    typedef char type62[static 10];
    typedef char type63[static const volatile 10];
    typedef char type64[const volatile static 10];
    typedef char type65[];
    typedef char type66[const volatile];
    typedef char type67[10][10];

    /* functions */
    static void f4();
    static void f5(void);
    static void f6(int x);
    static void f7(int x, int y);
    static void f8(int x, int y, ...);
    static void f9(int, int);
    static void f9(int, int, ...);
    static void f10(...);
    static void f11(const char *name);
    static void f12(const char *);
    static void f13(const int a[5]);
    static void f14(const int[5]);

    /* nested */
    typedef int *type69[4];
    typedef int (*type70)[4];
    typedef int (*type71[3])(int *x, int *y);
    typedef int (*type72(int (*)(long), int))(int, ...);
EOF
);
echo "  ok\n";
?>
--EXPECT--
Empty declaration
  ok
Various declarations
  ok
