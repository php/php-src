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

if test "$PHP_OPCACHE" != "no"; then

  dnl Always build as shared extension
  ext_shared=yes

  if test "$PHP_HUGE_CODE_PAGES" = "yes"; then
    AC_DEFINE(HAVE_HUGE_CODE_PAGES, 1, [Define to enable copying PHP CODE pages into HUGE PAGES (experimental)])
  fi

  if test "$PHP_OPCACHE_JIT" = "yes"; then
    case $host_cpu in
      x86*)
        ;;
      *)
        AC_MSG_WARN([JIT not supported by host architecture])
        PHP_OPCACHE_JIT=no
        ;;
    esac
  fi

  if test "$PHP_OPCACHE_JIT" = "yes"; then
    AC_DEFINE(HAVE_JIT, 1, [Define to enable JIT])
    ZEND_JIT_SRC="jit/zend_jit.c jit/zend_jit_vm_helpers.c"

    dnl Find out which ABI we are using.
    AC_RUN_IFELSE([AC_LANG_SOURCE([[
      int main(void) {
        return sizeof(void*) == 4;
      }
    ]])],[
      ac_cv_32bit_build=no
    ],[
      ac_cv_32bit_build=yes
    ],[
      ac_cv_32bit_build=no
    ])

    if test "$ac_cv_32bit_build" = "no"; then
      case $host_alias in
        *x86_64-*-darwin*)
          DASM_FLAGS="-D X64APPLE=1 -D X64=1"
        ;;
        *x86_64*)
          DASM_FLAGS="-D X64=1"
        ;;
      esac
    fi

    if test "$enable_zts" = "yes"; then
      DASM_FLAGS="$DASM_FLAGS -D ZTS=1"
    fi

    PHP_SUBST(DASM_FLAGS)

    AC_MSG_CHECKING(for opagent in default path)
    for i in /usr/local /usr; do
      if test -r $i/include/opagent.h; then
        OPAGENT_DIR=$i
        AC_MSG_RESULT(found in $i)
        break
      fi
    done
    if test -z "$OPAGENT_DIR"; then
      AC_MSG_RESULT(not found)
    else
      PHP_CHECK_LIBRARY(opagent, op_write_native_code,
      [
        AC_DEFINE(HAVE_OPROFILE,1,[ ])
        PHP_ADD_INCLUDE($OPAGENT_DIR/include)
        PHP_ADD_LIBRARY_WITH_PATH(opagent, $OPAGENT_DIR/$PHP_LIBDIR/oprofile, OPCACHE_SHARED_LIBADD)
        PHP_SUBST(OPCACHE_SHARED_LIBADD)
      ],[
        AC_MSG_RESULT(not found)
      ],[
        -L$OPAGENT_DIR/$PHP_LIBDIR/oprofile
      ])
    fi

  fi

  AC_CHECK_FUNCS([mprotect])

  AC_MSG_CHECKING(for sysvipc shared memory support)
  AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>

int main() {
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
]])],[dnl
    AC_DEFINE(HAVE_SHM_IPC, 1, [Define if you have SysV IPC SHM support])
    have_shm_ipc=yes],[have_shm_ipc=no],[have_shm_ipc=no])
  AC_MSG_RESULT([$have_shm_ipc])

  AC_MSG_CHECKING(for mmap() using MAP_ANON shared memory support)
  AC_RUN_IFELSE([AC_LANG_SOURCE([[
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

int main() {
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
]])],[dnl
    AC_DEFINE(HAVE_SHM_MMAP_ANON, 1, [Define if you have mmap(MAP_ANON) SHM support])
    have_shm_mmap_anon=yes],[have_shm_mmap_anon=no],[have_shm_mmap_anon=no])
  AC_MSG_RESULT([$have_shm_mmap_anon=yes])

  PHP_CHECK_FUNC_LIB(shm_open, rt)
  AC_MSG_CHECKING(for mmap() using shm_open() shared memory support)
  AC_RUN_IFELSE([AC_LANG_SOURCE([[
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

int main() {
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
}
]])],[dnl
    AC_DEFINE(HAVE_SHM_MMAP_POSIX, 1, [Define if you have POSIX mmap() SHM support])
    AC_MSG_RESULT([yes])
    have_shm_mmap_posix=yes
    PHP_CHECK_LIBRARY(rt, shm_unlink, [PHP_ADD_LIBRARY(rt,1,OPCACHE_SHARED_LIBADD)])
  ],[
    AC_MSG_RESULT([no])
  ],[
    AC_MSG_RESULT([no])
  ])

  PHP_NEW_EXTENSION(opcache,
	ZendAccelerator.c \
	zend_accelerator_blacklist.c \
	zend_accelerator_debug.c \
	zend_accelerator_hash.c \
	zend_accelerator_module.c \
	zend_persist.c \
	zend_persist_calc.c \
	zend_file_cache.c \
	zend_shared_alloc.c \
	zend_accelerator_util_funcs.c \
	shared_alloc_shm.c \
	shared_alloc_mmap.c \
	shared_alloc_posix.c \
	Optimizer/zend_optimizer.c \
	Optimizer/pass1.c \
	Optimizer/pass3.c \
	Optimizer/optimize_func_calls.c \
	Optimizer/block_pass.c \
	Optimizer/optimize_temp_vars_5.c \
	Optimizer/nop_removal.c \
	Optimizer/compact_literals.c \
	Optimizer/zend_cfg.c \
	Optimizer/zend_dfg.c \
	Optimizer/dfa_pass.c \
	Optimizer/zend_ssa.c \
	Optimizer/zend_inference.c \
	Optimizer/zend_func_info.c \
	Optimizer/zend_call_graph.c \
	Optimizer/sccp.c \
	Optimizer/scdf.c \
	Optimizer/dce.c \
	Optimizer/escape_analysis.c \
	Optimizer/compact_vars.c \
	Optimizer/zend_dump.c \
	$ZEND_JIT_SRC,
	shared,,-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1,,yes)

  PHP_ADD_BUILD_DIR([$ext_builddir/Optimizer], 1)
  PHP_ADD_EXTENSION_DEP(opcache, pcre)

  if test "$have_shm_ipc" != "yes" && test "$have_shm_mmap_posix" != "yes" && test "$have_shm_mmap_anon" != "yes"; then
    AC_MSG_ERROR([No supported shared memory caching support was found when configuring opcache. Check config.log for any errors or missing dependencies.])
  fi

  if test "$PHP_OPCACHE_JIT" = "yes"; then
    PHP_ADD_BUILD_DIR([$ext_builddir/jit], 1)
    PHP_ADD_MAKEFILE_FRAGMENT($ext_srcdir/jit/Makefile.frag)
  fi
  PHP_SUBST(OPCACHE_SHARED_LIBADD)
fi
