dnl This file contains Zend specific autoconf macros.

dnl
dnl ZEND_CHECK_FLOAT_PRECISION
dnl
dnl x87 floating point internal precision control checks
dnl See: https://wiki.php.net/rfc/rounding
dnl
AC_DEFUN([ZEND_CHECK_FLOAT_PRECISION], [dnl
AC_CACHE_CHECK([for usable _FPU_SETCW],
  [php_cv_have__fpu_setcw],
  [AC_LINK_IFELSE([AC_LANG_PROGRAM([#include <fpu_control.h>], [dnl
    fpu_control_t fpu_oldcw, fpu_cw;
    volatile double result;
    double a = 2877.0;
    volatile double b = 1000000.0;

    _FPU_GETCW(fpu_oldcw);
    fpu_cw = (fpu_oldcw & ~_FPU_EXTENDED & ~_FPU_SINGLE) | _FPU_DOUBLE;
    _FPU_SETCW(fpu_cw);
    result = a / b;
    _FPU_SETCW(fpu_oldcw);
    (void)result;
  ])],
  [php_cv_have__fpu_setcw=yes],
  [php_cv_have__fpu_setcw=no])])
AS_VAR_IF([php_cv_have__fpu_setcw], [yes],
  [AC_DEFINE([HAVE__FPU_SETCW], [1],
    [Define to 1 if _FPU_SETCW is present and usable.])])

AC_CACHE_CHECK([for usable fpsetprec],
  [php_cv_have_fpsetprec],
  [AC_LINK_IFELSE([AC_LANG_PROGRAM([#include <machine/ieeefp.h>], [dnl
    fp_prec_t fpu_oldprec;
    volatile double result;
    double a = 2877.0;
    volatile double b = 1000000.0;

    fpu_oldprec = fpgetprec();
    fpsetprec(FP_PD);
    result = a / b;
    fpsetprec(fpu_oldprec);
    (void)result;
  ])],
  [php_cv_have_fpsetprec=yes],
  [php_cv_have_fpsetprec=no])])
AS_VAR_IF([php_cv_have_fpsetprec], [yes],
  [AC_DEFINE([HAVE_FPSETPREC], [1],
    [Define to 1 if fpsetprec is present and usable.])])

AC_CACHE_CHECK([for usable _controlfp],
  [php_cv_have__controlfp],
  [AC_LINK_IFELSE([AC_LANG_PROGRAM([#include <float.h>], [dnl
    unsigned int fpu_oldcw;
    volatile double result;
    double a = 2877.0;
    volatile double b = 1000000.0;

    fpu_oldcw = _controlfp(0, 0);
    _controlfp(_PC_53, _MCW_PC);
    result = a / b;
    _controlfp(fpu_oldcw, _MCW_PC);
    (void)result;
  ])],
  [php_cv_have__controlfp=yes],
  [php_cv_have__controlfp=no])])
AS_VAR_IF([php_cv_have__controlfp], [yes],
  [AC_DEFINE([HAVE__CONTROLFP], [1],
    [Define to 1 if _controlfp is present and usable.])])

AC_CACHE_CHECK([for usable _controlfp_s],
  [php_cv_have__controlfp_s],
  [AC_LINK_IFELSE([AC_LANG_PROGRAM([#include <float.h>], [dnl
    unsigned int fpu_oldcw, fpu_cw;
    volatile double result;
    double a = 2877.0;
    volatile double b = 1000000.0;

    _controlfp_s(&fpu_cw, 0, 0);
    fpu_oldcw = fpu_cw;
    _controlfp_s(&fpu_cw, _PC_53, _MCW_PC);
    result = a / b;
    _controlfp_s(&fpu_cw, fpu_oldcw, _MCW_PC);
    (void)result;
  ])],
  [php_cv_have__controlfp_s=yes],
  [php_cv_have__controlfp_s=no])])
AS_VAR_IF([php_cv_have__controlfp_s], [yes],
  [AC_DEFINE([HAVE__CONTROLFP_S], [1],
    [Define to 1 if _controlfp_s is present and usable.])])

AC_CACHE_CHECK([whether FPU control word can be manipulated by inline assembler],
  [php_cv_have_fpu_inline_asm_x86],
  [AC_LINK_IFELSE([AC_LANG_PROGRAM([], [dnl
    unsigned int oldcw, cw;
    volatile double result;
    double a = 2877.0;
    volatile double b = 1000000.0;

    __asm__ __volatile__ ("fnstcw %0" : "=m" (*&oldcw));
    cw = (oldcw & ~0x0 & ~0x300) | 0x200;
    __asm__ __volatile__ ("fldcw %0" : : "m" (*&cw));
    result = a / b;
    __asm__ __volatile__ ("fldcw %0" : : "m" (*&oldcw));
    (void)result;
  ])],
  [php_cv_have_fpu_inline_asm_x86=yes],
  [php_cv_have_fpu_inline_asm_x86=no])])
AS_VAR_IF([php_cv_have_fpu_inline_asm_x86], [yes],
  [AC_DEFINE([HAVE_FPU_INLINE_ASM_X86], [1],
    [Define to 1 if FPU control word can be manipulated by inline assembler.])])
])

dnl
dnl ZEND_DLSYM_CHECK
dnl
dnl Ugly hack to check if dlsym() requires a leading underscore in symbol name.
dnl
AC_DEFUN([ZEND_DLSYM_CHECK], [dnl
AC_MSG_CHECKING([whether dlsym() requires a leading underscore in symbol names])
_LT_AC_TRY_DLOPEN_SELF([AC_MSG_RESULT([no])], [
  AC_MSG_RESULT([yes])
  AC_DEFINE([DLSYM_NEEDS_UNDERSCORE], [1],
    [Define to 1 if 'dlsym()' requires a leading underscore in symbol names.])
], [AC_MSG_RESULT([no])], [])
])

dnl
dnl ZEND_INIT
dnl
dnl Configure checks and initialization specific for the Zend engine library.
dnl
AC_DEFUN([ZEND_INIT], [dnl
AC_REQUIRE([AC_PROG_CC])

AC_CHECK_HEADERS(m4_normalize([
  cpuid.h
  libproc.h
]))

dnl Check for library functions.
AC_CHECK_FUNCS(m4_normalize([
  getpid
  gettid
  kill
  mremap
  pthread_attr_get_np
  pthread_attr_getstack
  pthread_get_stackaddr_np
  pthread_getattr_np
  pthread_stackseg_np
]))

dnl
dnl Check for sigsetjmp. If sigsetjmp is defined as a macro, use AC_CHECK_DECL
dnl as a fallback since AC_CHECK_FUNC cannot detect macros.
dnl
AC_CHECK_FUNC([sigsetjmp],,
  [AC_CHECK_DECL([sigsetjmp],,
    [AC_MSG_FAILURE([Required sigsetjmp not found.])],
    [#include <setjmp.h>])])

ZEND_CHECK_STACK_DIRECTION
ZEND_CHECK_FLOAT_PRECISION
ZEND_DLSYM_CHECK
ZEND_CHECK_GLOBAL_REGISTER_VARIABLES
ZEND_CHECK_CPUID_COUNT

AC_MSG_CHECKING([whether to enable thread safety])
AC_MSG_RESULT([$ZEND_ZTS])
AS_VAR_IF([ZEND_ZTS], [yes], [
  AC_DEFINE([ZTS], [1], [Define to 1 if thread safety (ZTS) is enabled.])
  AS_VAR_APPEND([CFLAGS], [" -DZTS"])
])

AC_MSG_CHECKING([whether to enable Zend debugging])
AC_MSG_RESULT([$ZEND_DEBUG])
AH_TEMPLATE([ZEND_DEBUG],
  [Define to 1 if debugging is enabled, and to 0 if not.])
AS_VAR_IF([ZEND_DEBUG], [yes], [
  AC_DEFINE([ZEND_DEBUG], [1])
  echo " $CFLAGS" | grep ' -g' >/dev/null || CFLAGS="$CFLAGS -g"
], [AC_DEFINE([ZEND_DEBUG], [0])])

AS_VAR_IF([GCC], [yes],
  [CFLAGS="-Wall -Wextra -Wno-unused-parameter -Wno-sign-compare $CFLAGS"])

dnl Check if compiler supports -Wno-clobbered (only GCC).
AX_CHECK_COMPILE_FLAG([-Wno-clobbered],
  [CFLAGS="-Wno-clobbered $CFLAGS"],,
  [-Werror])
dnl Check for support for implicit fallthrough level 1, also add after previous
dnl CFLAGS as level 3 is enabled in -Wextra.
AX_CHECK_COMPILE_FLAG([-Wimplicit-fallthrough=1],
  [CFLAGS="$CFLAGS -Wimplicit-fallthrough=1"],,
  [-Werror])
AX_CHECK_COMPILE_FLAG([-Wduplicated-cond],
  [CFLAGS="-Wduplicated-cond $CFLAGS"],,
  [-Werror])
AX_CHECK_COMPILE_FLAG([-Wlogical-op],
  [CFLAGS="-Wlogical-op $CFLAGS"],,
  [-Werror])
AX_CHECK_COMPILE_FLAG([-Wformat-truncation],
  [CFLAGS="-Wformat-truncation $CFLAGS"],,
  [-Werror])
AX_CHECK_COMPILE_FLAG([-Wstrict-prototypes],
  [CFLAGS="-Wstrict-prototypes $CFLAGS"],,
  [-Werror])
AX_CHECK_COMPILE_FLAG([-fno-common],
  [CFLAGS="-fno-common $CFLAGS"],,
  [-Werror])

ZEND_CHECK_ALIGNMENT
ZEND_CHECK_SIGNALS
ZEND_CHECK_MAX_EXECUTION_TIMERS
])

dnl
dnl ZEND_CHECK_STACK_DIRECTION
dnl
dnl Check whether the stack grows downwards, assumes contiguous stack.
dnl
AC_DEFUN([ZEND_CHECK_STACK_DIRECTION],
[AC_CACHE_CHECK([whether the stack grows downwards],
  [php_cv_have_stack_limit],
  [AC_RUN_IFELSE([AC_LANG_SOURCE([dnl
#include <stdint.h>

#ifdef __has_builtin
# if __has_builtin(__builtin_frame_address)
#  define builtin_frame_address __builtin_frame_address(0)
# endif
#endif

int (*volatile f)(uintptr_t);

int stack_grows_downwards(uintptr_t arg) {
#ifdef builtin_frame_address
  uintptr_t addr = (uintptr_t)builtin_frame_address;
#else
  int local;
  uintptr_t addr = (uintptr_t)&local;
#endif

  return addr < arg;
}

int main(void) {
#ifdef builtin_frame_address
  uintptr_t addr = (uintptr_t)builtin_frame_address;
#else
  int local;
  uintptr_t addr = (uintptr_t)&local;
#endif

  f = stack_grows_downwards;
  return f(addr) ? 0 : 1;
}])],
  [php_cv_have_stack_limit=yes],
  [php_cv_have_stack_limit=no],
  [php_cv_have_stack_limit=no])])
AS_VAR_IF([php_cv_have_stack_limit], [yes],
  [AC_DEFINE([ZEND_CHECK_STACK_LIMIT], [1],
    [Define to 1 if checking the stack limit is supported.])])
])

dnl
dnl ZEND_CHECK_GLOBAL_REGISTER_VARIABLES
dnl
dnl Check whether to enable global register variables if supported.
dnl
AC_DEFUN([ZEND_CHECK_GLOBAL_REGISTER_VARIABLES], [dnl
AC_ARG_ENABLE([gcc-global-regs],
  [AS_HELP_STRING([--disable-gcc-global-regs],
    [Disable GCC global register variables])],
  [ZEND_GCC_GLOBAL_REGS=$enableval],
  [ZEND_GCC_GLOBAL_REGS=yes])

AS_VAR_IF([ZEND_GCC_GLOBAL_REGS], [no],,
  [AC_CACHE_CHECK([whether system supports global register variables],
    [php_cv_have_global_register_vars],
    [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
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
}], [])],
  [php_cv_have_global_register_vars=yes],
  [php_cv_have_global_register_vars=no])
])
AS_VAR_IF([php_cv_have_global_register_vars], [yes],
  [AC_DEFINE([HAVE_GCC_GLOBAL_REGS], [1],
    [Define to 1 if the target system has support for global register
    variables.])],
  [ZEND_GCC_GLOBAL_REGS=no])
])
AC_MSG_CHECKING([whether to enable global register variables support])
AC_MSG_RESULT([$ZEND_GCC_GLOBAL_REGS])
])

dnl
dnl ZEND_CHECK_CPUID_COUNT
dnl
dnl Check whether __cpuid_count is available.
dnl
AC_DEFUN([ZEND_CHECK_CPUID_COUNT],
[AC_CACHE_CHECK([whether __cpuid_count is available],
  [php_cv_have___cpuid_count],
  [AC_LINK_IFELSE([AC_LANG_PROGRAM([#include <cpuid.h>], [dnl
    unsigned eax, ebx, ecx, edx;
    __cpuid_count(0, 0, eax, ebx, ecx, edx);
  ])],
  [php_cv_have___cpuid_count=yes],
  [php_cv_have___cpuid_count=no])])
AS_VAR_IF([php_cv_have___cpuid_count], [yes],
  [AC_DEFINE([HAVE_CPUID_COUNT], [1],
    [Define to 1 if '__cpuid_count' is available.])])
])

dnl
dnl ZEND_CHECK_ALIGNMENT
dnl
dnl Test and set the alignment defines for the Zend memory manager (ZEND_MM).
dnl This also does the logarithmic test.
dnl
AC_DEFUN([ZEND_CHECK_ALIGNMENT],
[AC_CACHE_CHECK([for Zend memory manager alignment and log values],
[php_cv_align_mm],
[AC_RUN_IFELSE([AC_LANG_SOURCE([
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
  fprintf(fp, "(size_t)%zu (size_t)%d %d\n",
    ZEND_MM_ALIGNMENT, zeros, ZEND_MM_ALIGNMENT < 4);
  fclose(fp);

  return 0;
}
])],
  [php_cv_align_mm=$(cat conftest.zend)],
  [php_cv_align_mm=failed],
  [php_cv_align_mm="(size_t)8 (size_t)3 0"])])
AS_VAR_IF([php_cv_align_mm], [failed],
  [AC_MSG_FAILURE([ZEND_MM alignment defines failed.])],
  [zend_mm_alignment=$(echo $php_cv_align_mm | cut -d ' ' -f 1)
  zend_mm_alignment_log2=$(echo $php_cv_align_mm | cut -d ' ' -f 2)
  zend_mm_8byte_realign=$(echo $php_cv_align_mm | cut -d ' ' -f 3)
  AC_DEFINE_UNQUOTED([ZEND_MM_ALIGNMENT],
    [$zend_mm_alignment],
    [Number of bytes for the ZEND_MM alignment.])
  AC_DEFINE_UNQUOTED([ZEND_MM_ALIGNMENT_LOG2],
    [$zend_mm_alignment_log2],
    [Number of bytes for the logarithmic ZEND_MM alignment.])
  AC_DEFINE_UNQUOTED([ZEND_MM_NEED_EIGHT_BYTE_REALIGNMENT],
    [$zend_mm_8byte_realign],
    [Define to 1 if ZEND_MM needs 8-byte realignment, and to 0 if not.])
])
])

dnl
dnl ZEND_CHECK_SIGNALS
dnl
dnl Check whether to enable Zend signal handling if supported by the system.
dnl
AC_DEFUN([ZEND_CHECK_SIGNALS], [dnl
AC_ARG_ENABLE([zend-signals],
  [AS_HELP_STRING([--disable-zend-signals],
    [Disable Zend signal handling])],
  [ZEND_SIGNALS=$enableval],
  [ZEND_SIGNALS=yes])

AC_CHECK_FUNCS([sigaction],, [ZEND_SIGNALS=no])
AS_VAR_IF([ZEND_SIGNALS], [yes],
  [AC_DEFINE([ZEND_SIGNALS], [1],
    [Define to 1 if Zend signal handling is supported and enabled.])
  AS_VAR_APPEND([CFLAGS], [" -DZEND_SIGNALS"])])

AC_MSG_CHECKING([whether to enable Zend signal handling])
AC_MSG_RESULT([$ZEND_SIGNALS])
])

dnl
dnl ZEND_CHECK_MAX_EXECUTION_TIMERS
dnl
dnl Check whether to enable Zend max execution timers.
dnl
AC_DEFUN([ZEND_CHECK_MAX_EXECUTION_TIMERS], [dnl
AC_ARG_ENABLE([zend-max-execution-timers],
  [AS_HELP_STRING([--enable-zend-max-execution-timers],
    [Enable Zend max execution timers; when building with thread safety
    (--enable-zts), they are automatically enabled by default based on the
    system support])],
    [ZEND_MAX_EXECUTION_TIMERS=$enableval],
    [ZEND_MAX_EXECUTION_TIMERS=$ZEND_ZTS])

AS_CASE([$host_alias], [*linux*|*freebsd*],,
  [ZEND_MAX_EXECUTION_TIMERS=no])

AS_VAR_IF([ZEND_MAX_EXECUTION_TIMERS], [yes],
  [AC_SEARCH_LIBS([timer_create], [rt],,
    [ZEND_MAX_EXECUTION_TIMERS=no])])

AS_VAR_IF([ZEND_MAX_EXECUTION_TIMERS], [yes],
  [AC_DEFINE([ZEND_MAX_EXECUTION_TIMERS], [1],
    [Define to 1 if Zend max execution timers are supported and enabled.])
    AS_VAR_APPEND([CFLAGS], [" -DZEND_MAX_EXECUTION_TIMERS"])])

AC_MSG_CHECKING([whether to enable Zend max execution timers])
AC_MSG_RESULT([$ZEND_MAX_EXECUTION_TIMERS])
])
