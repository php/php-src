PHP_ARG_ENABLE([opcache],
  [whether to enable Zend OPcache support],
  [AS_HELP_STRING([--disable-opcache],
    [Disable Zend OPcache support])],
  [yes])

PHP_ARG_ENABLE([huge-code-pages],
  [whether to enable copying PHP CODE pages into HUGE PAGES],
  [AS_HELP_STRING([--disable-huge-code-pages],
    [Disable copying PHP CODE pages into HUGE PAGES])],
  [yes],
  [no])

PHP_ARG_ENABLE([opcache-jit],
  [whether to enable JIT],
  [AS_HELP_STRING([--disable-opcache-jit],
    [Disable JIT])],
  [yes],
  [no])

PHP_ARG_WITH([capstone],
  [whether to support opcache JIT disassembly through Capstone],
  [AS_HELP_STRING([--with-capstone],
    [Support opcache JIT disassembly through Capstone])],
  [no],
  [no])

if test "$PHP_OPCACHE" != "no"; then
  dnl Always build as shared extension.
  ext_shared=yes

  AS_VAR_IF([PHP_HUGE_CODE_PAGES], [yes],
    [AC_DEFINE([HAVE_HUGE_CODE_PAGES], [1],
      [Define to 1 to enable copying PHP CODE pages into HUGE PAGES.])])

  AS_VAR_IF([PHP_OPCACHE_JIT], [yes], [
    AS_CASE([$host_cpu],
      [[i[34567]86*|x86*|aarch64|amd64]], [],
      [
        AC_MSG_WARN([JIT not supported by host architecture])
        PHP_OPCACHE_JIT=no
      ])

    if test "$host_vendor" = "apple" && test "$host_cpu" = "aarch64" && test "$PHP_THREAD_SAFETY" = "yes"; then
      AC_MSG_WARN([JIT not supported on Apple Silicon with ZTS])
      PHP_OPCACHE_JIT=no
    fi
  ])

  AS_VAR_IF([PHP_OPCACHE_JIT], [yes], [
    AC_DEFINE([HAVE_JIT], [1], [Define to 1 to enable JIT.])
    ZEND_JIT_SRC=m4_normalize(["
      jit/ir/ir_cfg.c
      jit/ir/ir_check.c
      jit/ir/ir_dump.c
      jit/ir/ir_emit.c
      jit/ir/ir_gcm.c
      jit/ir/ir_gdb.c
      jit/ir/ir_patch.c
      jit/ir/ir_perf.c
      jit/ir/ir_ra.c
      jit/ir/ir_save.c
      jit/ir/ir_sccp.c
      jit/ir/ir_strtab.c
      jit/ir/ir.c
      jit/zend_jit_vm_helpers.c
      jit/zend_jit.c
    "])

    dnl Find out which ABI we are using.
    AS_CASE([$host_alias],
      [x86_64-*-darwin*], [
        IR_TARGET=IR_TARGET_X64
        DASM_FLAGS="-D X64APPLE=1 -D X64=1"
        DASM_ARCH="x86"
      ],
      [*x86_64*|amd64-*-freebsd*], [
        IR_TARGET=IR_TARGET_X64
        DASM_FLAGS="-D X64=1"
        DASM_ARCH="x86"
      ],
      [[i[34567]86*|x86*]], [
        IR_TARGET=IR_TARGET_X86
        DASM_ARCH="x86"
      ],
      [aarch64*], [
        IR_TARGET=IR_TARGET_AARCH64
        DASM_ARCH="aarch64"
      ])

    AS_VAR_IF([PHP_CAPSTONE], [yes],
      [PKG_CHECK_MODULES([CAPSTONE], [capstone >= 3.0.0], [
        AC_DEFINE([HAVE_CAPSTONE], [1], [Define to 1 if Capstone is available.])
        PHP_EVAL_LIBLINE([$CAPSTONE_LIBS], [OPCACHE_SHARED_LIBADD])
        PHP_EVAL_INCLINE([$CAPSTONE_CFLAGS])
        ZEND_JIT_SRC="$ZEND_JIT_SRC jit/ir/ir_disasm.c"
      ])])

    PHP_SUBST([IR_TARGET])
    PHP_SUBST([DASM_FLAGS])
    PHP_SUBST([DASM_ARCH])

    JIT_CFLAGS="-I@ext_builddir@/jit/ir -D$IR_TARGET -DIR_PHP"
    AS_VAR_IF([ZEND_DEBUG], [yes], [JIT_CFLAGS="$JIT_CFLAGS -DIR_DEBUG"])
  ])

  AC_CHECK_FUNCS([mprotect shm_create_largepage])

  AC_CACHE_CHECK([for sysvipc shared memory support], [php_cv_shm_ipc],
    [AC_RUN_IFELSE([AC_LANG_SOURCE([
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>

int main(void) {
  pid_t pid;
  int status;
  int ipc_id;
  char *shm;
  struct shmid_ds shmbuf;

  ipc_id = shmget(IPC_PRIVATE, 4096, (IPC_CREAT | SHM_R | SHM_W));
  if (ipc_id == -1) {
    return 1;
  }

  shm = shmat(ipc_id, NULL, 0);
  if (shm == (void *)-1) {
    shmctl(ipc_id, IPC_RMID, NULL);
    return 2;
  }

  if (shmctl(ipc_id, IPC_STAT, &shmbuf) != 0) {
    shmdt(shm);
    shmctl(ipc_id, IPC_RMID, NULL);
    return 3;
  }

  shmbuf.shm_perm.uid = getuid();
  shmbuf.shm_perm.gid = getgid();
  shmbuf.shm_perm.mode = 0600;

  if (shmctl(ipc_id, IPC_SET, &shmbuf) != 0) {
    shmdt(shm);
    shmctl(ipc_id, IPC_RMID, NULL);
    return 4;
  }

  shmctl(ipc_id, IPC_RMID, NULL);

  strcpy(shm, "hello");

  pid = fork();
  if (pid < 0) {
    return 5;
  } else if (pid == 0) {
    strcpy(shm, "bye");
    return 6;
  }
  if (wait(&status) != pid) {
    return 7;
  }
  if (!WIFEXITED(status) || WEXITSTATUS(status) != 6) {
    return 8;
  }
  if (strcmp(shm, "bye") != 0) {
    return 9;
  }
  return 0;
}
])],
[php_cv_shm_ipc=yes],
[php_cv_shm_ipc=no],
[php_cv_shm_ipc=no])])
AS_VAR_IF([php_cv_shm_ipc], [yes],
  [AC_DEFINE([HAVE_SHM_IPC], [1],
    [Define to 1 if you have the SysV IPC SHM support.])])

  AC_CACHE_CHECK([for mmap() using MAP_ANON shared memory support],
  [php_cv_shm_mmap_anon],
  [AC_RUN_IFELSE([AC_LANG_SOURCE([
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

#ifndef MAP_ANON
# ifdef MAP_ANONYMOUS
#  define MAP_ANON MAP_ANONYMOUS
# endif
#endif
#ifndef MAP_FAILED
# define MAP_FAILED ((void*)-1)
#endif

int main(void) {
  pid_t pid;
  int status;
  char *shm;

  shm = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
  if (shm == MAP_FAILED) {
    return 1;
  }

  strcpy(shm, "hello");

  pid = fork();
  if (pid < 0) {
    return 5;
  } else if (pid == 0) {
    strcpy(shm, "bye");
    return 6;
  }
  if (wait(&status) != pid) {
    return 7;
  }
  if (!WIFEXITED(status) || WEXITSTATUS(status) != 6) {
    return 8;
  }
  if (strcmp(shm, "bye") != 0) {
    return 9;
  }
  return 0;
}
])],
[php_cv_shm_mmap_anon=yes],
[php_cv_shm_mmap_anon=no],
[AS_CASE([$host_alias],
  [*linux*|*midipix], [php_cv_shm_mmap_anon=yes],
  [php_cv_shm_mmap_anon=no])])])
AS_VAR_IF([php_cv_shm_mmap_anon], [yes],
  [AC_DEFINE([HAVE_SHM_MMAP_ANON], [1],
    [Define to 1 if you have the mmap(MAP_ANON) SHM support.])])

  dnl Check POSIX shared memory object operations and link required library as
  dnl needed: rt (older Linux and Solaris <= 10). Most systems have it in the C
  dnl standard library (newer Linux, illumos, Solaris 11.4, macOS, BSD-based
  dnl systems...). Haiku has it in the root library, which is linked by default.
  LIBS_save=$LIBS
  LIBS=
  AC_SEARCH_LIBS([shm_open], [rt],
    [AC_CACHE_CHECK([for mmap() using shm_open() shared memory support],
      [php_cv_shm_mmap_posix],
      [AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef MAP_FAILED
# define MAP_FAILED ((void*)-1)
#endif

int main(void) {
  pid_t pid;
  int status;
  int fd;
  char *shm;
  char tmpname[4096];

  sprintf(tmpname,"/opcache.test.shm.%dXXXXXX", getpid());
  if (mktemp(tmpname) == NULL) {
    return 1;
  }
  fd = shm_open(tmpname, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    return 2;
  }
  if (ftruncate(fd, 4096) < 0) {
    close(fd);
    shm_unlink(tmpname);
    return 3;
  }

  shm = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shm == MAP_FAILED) {
    return 4;
  }
  shm_unlink(tmpname);
  close(fd);

  strcpy(shm, "hello");

  pid = fork();
  if (pid < 0) {
    return 5;
  } else if (pid == 0) {
    strcpy(shm, "bye");
    return 6;
  }
  if (wait(&status) != pid) {
    return 7;
  }
  if (!WIFEXITED(status) || WEXITSTATUS(status) != 6) {
    return 8;
  }
  if (strcmp(shm, "bye") != 0) {
    return 9;
  }
  return 0;
}]])],
      [php_cv_shm_mmap_posix=yes],
      [php_cv_shm_mmap_posix=no],
      [php_cv_shm_mmap_posix=no])
    ])
  ])
  LIBS=$LIBS_save

  AS_VAR_IF([php_cv_shm_mmap_posix], [yes], [
    AC_DEFINE([HAVE_SHM_MMAP_POSIX], [1],
      [Define to 1 if you have the POSIX mmap() SHM support.])
    AS_CASE([$ac_cv_search_shm_open], ["none required"|no], [],
      [PHP_EVAL_LIBLINE([$ac_cv_search_shm_open], [OPCACHE_SHARED_LIBADD])])
  ])

  PHP_NEW_EXTENSION([opcache], m4_normalize([
      shared_alloc_mmap.c
      shared_alloc_posix.c
      shared_alloc_shm.c
      zend_accelerator_blacklist.c
      zend_accelerator_debug.c
      zend_accelerator_hash.c
      zend_accelerator_module.c
      zend_accelerator_util_funcs.c
      zend_file_cache.c
      zend_persist_calc.c
      zend_persist.c
      zend_shared_alloc.c
      ZendAccelerator.c
      $ZEND_JIT_SRC
    ]),
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1 $JIT_CFLAGS],,
    [yes])

  PHP_ADD_EXTENSION_DEP(opcache, date)
  PHP_ADD_EXTENSION_DEP(opcache, pcre)

  if test "$php_cv_shm_ipc" != "yes" && test "$php_cv_shm_mmap_posix" != "yes" && test "$php_cv_shm_mmap_anon" != "yes"; then
    AC_MSG_FAILURE(m4_text_wrap([
      No supported shared memory caching support was found when configuring
      opcache.
    ]))
  fi

  AS_VAR_IF([PHP_OPCACHE_JIT], [yes], [
    PHP_ADD_BUILD_DIR([
      $ext_builddir/jit
      $ext_builddir/jit/ir
    ])
    PHP_ADD_MAKEFILE_FRAGMENT([$ext_srcdir/jit/Makefile.frag])
  ])
  PHP_SUBST([OPCACHE_SHARED_LIBADD])
fi
