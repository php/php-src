dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(opcache, whether to enable Zend OPcache support,
[  --enable-opcache Enable Zend OPcache support], yes)

if test "$PHP_OPCACHE" != "no"; then

  AC_CHECK_FUNC(mprotect,[
    AC_DEFINE(HAVE_MPROTECT, 1, [Define if you have mprotect() function])
  ])

  AC_MSG_CHECKING(for sysvipc shared memory support)
  AC_TRY_RUN([
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
],dnl
    AC_DEFINE(HAVE_SHM_IPC, 1, [Define if you have SysV IPC SHM support])
    msg=yes,msg=no,msg=no)
  AC_MSG_RESULT([$msg])

  AC_MSG_CHECKING(for mmap() using MAP_ANON shared memory support)
  AC_TRY_RUN([
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
],dnl
    AC_DEFINE(HAVE_SHM_MMAP_ANON, 1, [Define if you have mmap(MAP_ANON) SHM support])
    msg=yes,msg=no,msg=no)
  AC_MSG_RESULT([$msg])

  AC_MSG_CHECKING(for mmap() using /dev/zero shared memory support)
  AC_TRY_RUN([
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#ifndef MAP_FAILED
# define MAP_FAILED ((void*)-1)
#endif

int main() {
  pid_t pid;
  int status;
  int fd;
  char *shm;

  fd = open("/dev/zero", O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    return 1;
  }

  shm = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shm == MAP_FAILED) {
    return 2;
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
],dnl
    AC_DEFINE(HAVE_SHM_MMAP_ZERO, 1, [Define if you have mmap("/dev/zero") SHM support])
    msg=yes,msg=no,msg=no)
  AC_MSG_RESULT([$msg])

  AC_MSG_CHECKING(for mmap() using shm_open() shared memory support)
  AC_TRY_RUN([
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

  sprintf(tmpname,"test.shm.%dXXXXXX", getpid());
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
],dnl
    AC_DEFINE(HAVE_SHM_MMAP_POSIX, 1, [Define if you have POSIX mmap() SHM support])
    msg=yes,msg=no,msg=no)
  AC_MSG_RESULT([$msg])

  AC_MSG_CHECKING(for mmap() using regular file shared memory support)
  AC_TRY_RUN([
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

  sprintf(tmpname,"test.shm.%dXXXXXX", getpid());
  if (mktemp(tmpname) == NULL) {
    return 1;
  }
  fd = open(tmpname, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    return 2;
  }
  if (ftruncate(fd, 4096) < 0) {
    close(fd);
    unlink(tmpname);
    return 3;
  }

  shm = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shm == MAP_FAILED) {
    return 4;
  }
  unlink(tmpname);
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
],dnl
    AC_DEFINE(HAVE_SHM_MMAP_FILE, 1, [Define if you have mmap() SHM support])
    msg=yes,msg=no,msg=no)
  AC_MSG_RESULT([$msg])

  AC_MSG_CHECKING(for known struct flock definition)
  dnl Copied from ZendAccelerator.h
  AC_TRY_RUN([
#include <fcntl.h>
#include <stdlib.h>

#ifndef ZEND_WIN32
extern int lock_file;

# if defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || (defined(__APPLE__) && defined(__MACH__)/* Darwin */) || defined(__OpenBSD__) || defined(__NetBSD__)
#  define FLOCK_STRUCTURE(name, type, whence, start, len) \
                struct flock name = {start, len, -1, type, whence}
# elif defined(__svr4__)
#  define FLOCK_STRUCTURE(name, type, whence, start, len) \
                struct flock name = {type, whence, start, len}
# elif defined(__linux__) || defined(__hpux)
#  define FLOCK_STRUCTURE(name, type, whence, start, len) \
                struct flock name = {type, whence, start, len, 0}
# elif defined(_AIX)
#  if defined(_LARGE_FILES) || defined(__64BIT__)
#   define FLOCK_STRUCTURE(name, type, whence, start, len) \
                struct flock name = {type, whence, 0, 0, 0, start, len }
#  else
#   define FLOCK_STRUCTURE(name, type, whence, start, len) \
                struct flock name = {type, whence, start, len}
#  endif
# else
#  error "Don't know how to define struct flock"
# endif
#endif
int main() { return 0; }
], [], [AC_MSG_ERROR([Don't know how to define struct flock on this system[,] set --enable-opcache=no])], [])
  
  PHP_NEW_EXTENSION(opcache,
	ZendAccelerator.c \
	zend_accelerator_blacklist.c \
	zend_accelerator_debug.c \
	zend_accelerator_hash.c \
	zend_accelerator_module.c \
	zend_persist.c \
	zend_persist_calc.c \
	zend_shared_alloc.c \
	zend_accelerator_util_funcs.c \
	shared_alloc_shm.c \
	shared_alloc_mmap.c \
	shared_alloc_posix.c \
	Optimizer/zend_optimizer.c,
	shared,,,,yes)

  PHP_ADD_BUILD_DIR([$ext_builddir/Optimizer], 1)
fi
