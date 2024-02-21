dnl This file contains Zend specific autoconf macros.

dnl
dnl ZEND_CHECK_FLOAT_PRECISION
dnl
dnl x87 floating point internal precision control checks
dnl See: http://wiki.php.net/rfc/rounding
dnl
AC_DEFUN([ZEND_CHECK_FLOAT_PRECISION],[
  AC_MSG_CHECKING([for usable _FPU_SETCW])
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
    #include <fpu_control.h>
  ]],[[
    fpu_control_t fpu_oldcw, fpu_cw;
    volatile double result;
    double a = 2877.0;
    volatile double b = 1000000.0;

    _FPU_GETCW(fpu_oldcw);
    fpu_cw = (fpu_oldcw & ~_FPU_EXTENDED & ~_FPU_SINGLE) | _FPU_DOUBLE;
    _FPU_SETCW(fpu_cw);
    result = a / b;
    _FPU_SETCW(fpu_oldcw);
  ]])],[ac_cfp_have__fpu_setcw=yes],[ac_cfp_have__fpu_setcw=no])
  if test "$ac_cfp_have__fpu_setcw" = "yes" ; then
    AC_DEFINE(HAVE__FPU_SETCW, 1, [whether _FPU_SETCW is present and usable])
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi

  AC_MSG_CHECKING([for usable fpsetprec])
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
    #include <machine/ieeefp.h>
  ]],[[
    fp_prec_t fpu_oldprec;
    volatile double result;
    double a = 2877.0;
    volatile double b = 1000000.0;

    fpu_oldprec = fpgetprec();
    fpsetprec(FP_PD);
    result = a / b;
    fpsetprec(fpu_oldprec);
  ]])], [ac_cfp_have_fpsetprec=yes], [ac_cfp_have_fpsetprec=no])
  if test "$ac_cfp_have_fpsetprec" = "yes" ; then
    AC_DEFINE(HAVE_FPSETPREC, 1, [whether fpsetprec is present and usable])
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi

  AC_MSG_CHECKING([for usable _controlfp])
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
    #include <float.h>
  ]],[[
    unsigned int fpu_oldcw;
    volatile double result;
    double a = 2877.0;
    volatile double b = 1000000.0;

    fpu_oldcw = _controlfp(0, 0);
    _controlfp(_PC_53, _MCW_PC);
    result = a / b;
    _controlfp(fpu_oldcw, _MCW_PC);
  ]])], [ac_cfp_have__controlfp=yes], [ac_cfp_have__controlfp=no])
  if test "$ac_cfp_have__controlfp" = "yes" ; then
    AC_DEFINE(HAVE__CONTROLFP, 1, [whether _controlfp is present usable])
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi

  AC_MSG_CHECKING([for usable _controlfp_s])
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
   #include <float.h>
  ]],[[
    unsigned int fpu_oldcw, fpu_cw;
    volatile double result;
    double a = 2877.0;
    volatile double b = 1000000.0;

    _controlfp_s(&fpu_cw, 0, 0);
    fpu_oldcw = fpu_cw;
    _controlfp_s(&fpu_cw, _PC_53, _MCW_PC);
    result = a / b;
    _controlfp_s(&fpu_cw, fpu_oldcw, _MCW_PC);
  ]])], [ac_cfp_have__controlfp_s=yes], [ac_cfp_have__controlfp_s=no])
  if test "$ac_cfp_have__controlfp_s" = "yes" ; then
    AC_DEFINE(HAVE__CONTROLFP_S, 1, [whether _controlfp_s is present and usable])
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi

  AC_MSG_CHECKING([whether FPU control word can be manipulated by inline assembler])
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
    /* nothing */
  ]],[[
    unsigned int oldcw, cw;
    volatile double result;
    double a = 2877.0;
    volatile double b = 1000000.0;

    __asm__ __volatile__ ("fnstcw %0" : "=m" (*&oldcw));
    cw = (oldcw & ~0x0 & ~0x300) | 0x200;
    __asm__ __volatile__ ("fldcw %0" : : "m" (*&cw));

    result = a / b;

    __asm__ __volatile__ ("fldcw %0" : : "m" (*&oldcw));
  ]])], [ac_cfp_have_fpu_inline_asm_x86=yes], [ac_cfp_have_fpu_inline_asm_x86=no])
  if test "$ac_cfp_have_fpu_inline_asm_x86" = "yes" ; then
    AC_DEFINE(HAVE_FPU_INLINE_ASM_X86, 1, [whether FPU control word can be manipulated by inline assembler])
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
])

dnl
dnl LIBZEND_BASIC_CHECKS
dnl
dnl Basic checks specific for the Zend engine library.
dnl
AC_DEFUN([LIBZEND_BASIC_CHECKS],[
AC_REQUIRE([AC_PROG_CC])

AC_CHECK_HEADERS([cpuid.h])

dnl
dnl LIBZEND_DLSYM_CHECK
dnl
dnl Ugly hack to check if dlsym() requires a leading underscore in symbol name.
dnl
AC_DEFUN([LIBZEND_DLSYM_CHECK],[
AC_MSG_CHECKING([whether dlsym() requires a leading underscore in symbol names])
_LT_AC_TRY_DLOPEN_SELF([
  AC_MSG_RESULT(no)
], [
  AC_MSG_RESULT(yes)
  AC_DEFINE(DLSYM_NEEDS_UNDERSCORE, 1, [Define if dlsym() requires a leading underscore in symbol names. ])
], [
  AC_MSG_RESULT(no)
], [])
])

dnl Checks for library functions.
AC_CHECK_FUNCS(getpid kill pthread_getattr_np pthread_attr_get_np pthread_get_stackaddr_np pthread_attr_getstack pthread_stackseg_np gettid)

dnl Check for sigsetjmp. If it's defined as a macro, AC_CHECK_FUNCS won't work.
AC_CHECK_FUNCS([sigsetjmp],,
  [AC_CHECK_DECL([sigsetjmp],
    [AC_DEFINE([HAVE_SIGSETJMP], [1])],,
    [#include <setjmp.h>])])

dnl Test whether the stack grows downwards
dnl Assumes contiguous stack
AC_MSG_CHECKING(whether the stack grows downwards)

AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <stdint.h>

int (*volatile f)(uintptr_t);

int stack_grows_downwards(uintptr_t arg) {
    int local;
    return (uintptr_t)&local < arg;
}

int main(void) {
    int local;

    f = stack_grows_downwards;
    return f((uintptr_t)&local) ? 0 : 1;
}
]])], [
  AC_DEFINE([ZEND_CHECK_STACK_LIMIT], 1, [Define if checking the stack limit is supported])
  AC_MSG_RESULT(yes)
], [
  AC_MSG_RESULT(no)
], [
  AC_MSG_RESULT(no)
])

ZEND_CHECK_FLOAT_PRECISION
])

dnl
dnl LIBZEND_OTHER_CHECKS
dnl
AC_DEFUN([LIBZEND_OTHER_CHECKS],[

AC_MSG_CHECKING(whether to enable thread-safety)
AC_MSG_RESULT($ZEND_ZTS)

AC_MSG_CHECKING(whether to enable Zend debugging)
AC_MSG_RESULT($ZEND_DEBUG)

if test "$ZEND_DEBUG" = "yes"; then
  AC_DEFINE(ZEND_DEBUG,1,[ ])
  echo " $CFLAGS" | grep ' -g' >/dev/null || DEBUG_CFLAGS="-g"
  if test "$CFLAGS" = "-g -O2"; then
  	CFLAGS=-g
  fi
else
  AC_DEFINE(ZEND_DEBUG,0,[ ])
fi

test -n "$GCC" && CFLAGS="-Wall -Wextra -Wno-unused-parameter -Wno-sign-compare $CFLAGS"
dnl Check if compiler supports -Wno-clobbered (only GCC)
AX_CHECK_COMPILE_FLAG([-Wno-clobbered], CFLAGS="-Wno-clobbered $CFLAGS", , [-Werror])
dnl Check for support for implicit fallthrough level 1, also add after previous CFLAGS as level 3 is enabled in -Wextra
AX_CHECK_COMPILE_FLAG([-Wimplicit-fallthrough=1], CFLAGS="$CFLAGS -Wimplicit-fallthrough=1", , [-Werror])
AX_CHECK_COMPILE_FLAG([-Wduplicated-cond], CFLAGS="-Wduplicated-cond $CFLAGS", , [-Werror])
AX_CHECK_COMPILE_FLAG([-Wlogical-op], CFLAGS="-Wlogical-op $CFLAGS", , [-Werror])
AX_CHECK_COMPILE_FLAG([-Wformat-truncation], CFLAGS="-Wformat-truncation $CFLAGS", , [-Werror])
AX_CHECK_COMPILE_FLAG([-Wstrict-prototypes], CFLAGS="-Wstrict-prototypes $CFLAGS", , [-Werror])
AX_CHECK_COMPILE_FLAG([-fno-common], CFLAGS="-fno-common $CFLAGS", , [-Werror])

test -n "$DEBUG_CFLAGS" && CFLAGS="$CFLAGS $DEBUG_CFLAGS"

if test "$ZEND_ZTS" = "yes"; then
  AC_DEFINE(ZTS,1,[ ])
  CFLAGS="$CFLAGS -DZTS"
fi

dnl Test and set the alignment define for ZEND_MM. This also does the
dnl logarithmic test for ZEND_MM.
AC_MSG_CHECKING(for MM alignment and log values)

AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <stdio.h>
#include <stdlib.h>

typedef union _mm_align_test {
  void *ptr;
  double dbl;
  long lng;
} mm_align_test;

#if (defined (__GNUC__) && __GNUC__ >= 2)
#define ZEND_MM_ALIGNMENT (__alignof__ (mm_align_test))
#else
#define ZEND_MM_ALIGNMENT (sizeof(mm_align_test))
#endif

int main(void)
{
  size_t i = ZEND_MM_ALIGNMENT;
  int zeros = 0;
  FILE *fp;

  while (i & ~0x1) {
    zeros++;
    i = i >> 1;
  }

  fp = fopen("conftest.zend", "w");
  fprintf(fp, "(size_t)%zu (size_t)%d %d\n", ZEND_MM_ALIGNMENT, zeros, ZEND_MM_ALIGNMENT < 4);
  fclose(fp);

  return 0;
}
]])], [
  LIBZEND_MM_ALIGN=`cat conftest.zend | cut -d ' ' -f 1`
  LIBZEND_MM_ALIGN_LOG2=`cat conftest.zend | cut -d ' ' -f 2`
  LIBZEND_MM_NEED_EIGHT_BYTE_REALIGNMENT=`cat conftest.zend | cut -d ' ' -f 3`
  AC_DEFINE_UNQUOTED(ZEND_MM_ALIGNMENT, $LIBZEND_MM_ALIGN, [ ])
  AC_DEFINE_UNQUOTED(ZEND_MM_ALIGNMENT_LOG2, $LIBZEND_MM_ALIGN_LOG2, [ ])
  AC_DEFINE_UNQUOTED(ZEND_MM_NEED_EIGHT_BYTE_REALIGNMENT, $LIBZEND_MM_NEED_EIGHT_BYTE_REALIGNMENT, [ ])
], [], [
  dnl Cross compilation needs something here.
  AC_DEFINE(ZEND_MM_ALIGNMENT, 8, [ ])
  AC_DEFINE(ZEND_MM_ALIGNMENT_LOG2, 3, [ ])
  AC_DEFINE(ZEND_MM_NEED_EIGHT_BYTE_REALIGNMENT, 0, [ ])
])

AC_MSG_RESULT(done)

AC_CHECK_FUNCS(mremap)

AC_ARG_ENABLE([zend-signals],
  [AS_HELP_STRING([--disable-zend-signals],
    [whether to enable zend signal handling])],
  [ZEND_SIGNALS=$enableval],
  [ZEND_SIGNALS=yes])

AC_CHECK_FUNCS([sigaction], [], [
  ZEND_SIGNALS=no
])
if test "$ZEND_SIGNALS" = "yes"; then
	AC_DEFINE(ZEND_SIGNALS, 1, [Use zend signal handling])
	CFLAGS="$CFLAGS -DZEND_SIGNALS"
fi

AC_MSG_CHECKING(whether to enable zend signal handling)
AC_MSG_RESULT($ZEND_SIGNALS)

dnl Don't enable Zend Max Execution Timers by default until PHP 8.3 to not break the ABI
AC_ARG_ENABLE([zend-max-execution-timers],
  [AS_HELP_STRING([--enable-zend-max-execution-timers],
    [whether to enable zend max execution timers])],
    [ZEND_MAX_EXECUTION_TIMERS=$enableval],
    [ZEND_MAX_EXECUTION_TIMERS=$ZEND_ZTS])

AS_CASE(
  ["$host_alias"],
  [*linux*|*freebsd*], [
    PHP_CHECK_FUNC(timer_create, rt)
    if test "$ac_cv_func_timer_create" != "yes"; then
      ZEND_MAX_EXECUTION_TIMERS='no'
    fi
  ],
  [*darwin*], [],
  [ZEND_MAX_EXECUTION_TIMERS='no']
)

AS_CASE(["$host_alias"], [*darwin*], [], [
  PHP_CHECK_FUNC(timer_create, rt)
  if test "$ac_cv_func_timer_create" != "yes"; then
    ZEND_MAX_EXECUTION_TIMERS='no'
  fi
])

if test "$ZEND_MAX_EXECUTION_TIMERS" = "yes"; then
  AC_DEFINE(ZEND_MAX_EXECUTION_TIMERS, 1, [Use zend max execution timers])
  CFLAGS="$CFLAGS -DZEND_MAX_EXECUTION_TIMERS"
fi

AC_MSG_CHECKING(whether to enable zend max execution timers)
AC_MSG_RESULT($ZEND_MAX_EXECUTION_TIMERS)

])

AC_ARG_ENABLE([gcc-global-regs],
  [AS_HELP_STRING([--disable-gcc-global-regs],
    [whether to enable GCC global register variables])],
  [ZEND_GCC_GLOBAL_REGS=$enableval],
  [ZEND_GCC_GLOBAL_REGS=yes])

AC_MSG_CHECKING(for global register variables support)
if test "$ZEND_GCC_GLOBAL_REGS" != "no"; then
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#if defined(__GNUC__)
# define ZEND_GCC_VERSION (__GNUC__ * 1000 + __GNUC_MINOR__)
#else
# define ZEND_GCC_VERSION 0
#endif
#if defined(__GNUC__) && ZEND_GCC_VERSION >= 4008 && defined(i386)
# define ZEND_VM_FP_GLOBAL_REG "%esi"
# define ZEND_VM_IP_GLOBAL_REG "%edi"
#elif defined(__GNUC__) && ZEND_GCC_VERSION >= 4008 && defined(__x86_64__)
# define ZEND_VM_FP_GLOBAL_REG "%r14"
# define ZEND_VM_IP_GLOBAL_REG "%r15"
#elif defined(__GNUC__) && ZEND_GCC_VERSION >= 4008 && defined(__powerpc64__)
# define ZEND_VM_FP_GLOBAL_REG "r28"
# define ZEND_VM_IP_GLOBAL_REG "r29"
#elif defined(__IBMC__) && ZEND_GCC_VERSION >= 4002 && defined(__powerpc64__)
# define ZEND_VM_FP_GLOBAL_REG "r28"
# define ZEND_VM_IP_GLOBAL_REG "r29"
#elif defined(__GNUC__) && ZEND_GCC_VERSION >= 4008 && defined(__aarch64__)
# define ZEND_VM_FP_GLOBAL_REG "x27"
# define ZEND_VM_IP_GLOBAL_REG "x28"
#elif defined(__GNUC__) && ZEND_GCC_VERSION >= 4008 && defined(__riscv) && __riscv_xlen == 64
# define ZEND_VM_FP_GLOBAL_REG "x18"
# define ZEND_VM_IP_GLOBAL_REG "x19"
#else
# error "global register variables are not supported"
#endif
typedef int (*opcode_handler_t)(void);
register void *FP  __asm__(ZEND_VM_FP_GLOBAL_REG);
register const opcode_handler_t *IP __asm__(ZEND_VM_IP_GLOBAL_REG);
int emu(const opcode_handler_t *ip, void *fp) {
	const opcode_handler_t *orig_ip = IP;
	void *orig_fp = FP;
	IP = ip;
	FP = fp;
	while ((*ip)());
	FP = orig_fp;
	IP = orig_ip;
}
  ]], [[
  ]])], [
    ZEND_GCC_GLOBAL_REGS=yes
  ], [
    ZEND_GCC_GLOBAL_REGS=no
  ])
fi
if test "$ZEND_GCC_GLOBAL_REGS" = "yes"; then
  AC_DEFINE([HAVE_GCC_GLOBAL_REGS], 1, [Define if the target system has support for global register variables])
fi
AC_MSG_RESULT($ZEND_GCC_GLOBAL_REGS)

dnl Check whether __cpuid_count is available.
AC_CACHE_CHECK(whether __cpuid_count is available, ac_cv_cpuid_count_available, [
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
  #include <cpuid.h>
]], [[
  unsigned eax, ebx, ecx, edx;
  __cpuid_count(0, 0, eax, ebx, ecx, edx);
]])], [
  ac_cv_cpuid_count_available=yes
], [
  ac_cv_cpuid_count_available=no
])])
if test "$ac_cv_cpuid_count_available" = "yes"; then
  AC_DEFINE([HAVE_CPUID_COUNT], 1, [whether __cpuid_count is available])
fi
