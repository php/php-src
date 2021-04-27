/*
   +----------------------------------------------------------------------+
   | Zend JIT                                                             |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#define HAVE_PERFTOOLS 1

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>

#if defined(__linux__)
#include <sys/syscall.h>
#elif defined(__darwin__)
# include <pthread.h>
#elif defined(__FreeBSD__)
# include <sys/thr.h>
# include <sys/sysctl.h>
#elif defined(__NetBSD__)
# include <lwp.h>
#elif defined(__DragonFly__)
# include <sys/lwp.h>
# include <sys/sysctl.h>
#elif defined(__sun)
// avoiding thread.h inclusion as it conflicts with vtunes types.
extern unsigned int thr_self(void);
#elif defined(__HAIKU__)
#include <FindDirectory.h>
#endif

#include "zend_elf.h"

/*
 * 1) Profile using perf-<pid>.map
 *
 * perf record php -d opcache.huge_code_pages=0 -d opcache.jit_debug=0x10 bench.php
 * perf report
 *
 * 2) Profile using jit-<pid>.dump
 *
 * perf record -k 1 php -d opcache.huge_code_pages=0 -d opcache.jit_debug=0x20 bench.php
 * perf inject -j -i perf.data -o perf.data.jitted
 * perf report -i perf.data.jitted
 *
 */


#define ZEND_PERF_JITDUMP_HEADER_MAGIC   0x4A695444
#define ZEND_PERF_JITDUMP_HEADER_VERSION 1

#define ZEND_PERF_JITDUMP_RECORD_LOAD       0
#define ZEND_PERF_JITDUMP_RECORD_MOVE       1
#define ZEND_PERF_JITDUMP_RECORD_DEBUG_INFO 2
#define ZEND_PERF_JITDUMP_RECORD_CLOSE      3
#define ZEND_PERF_JITDUMP_UNWINDING_UNFO    4

#define ALIGN8(size)   (((size) + 7) & ~7)
#define PADDING8(size) (ALIGN8(size) - (size))

typedef struct zend_perf_jitdump_header {
	uint32_t magic;
	uint32_t version;
	uint32_t size;
	uint32_t elf_mach_target;
	uint32_t reserved;
	uint32_t process_id;
	uint64_t time_stamp;
	uint64_t flags;
} zend_perf_jitdump_header;

typedef struct _zend_perf_jitdump_record {
	uint32_t event;
	uint32_t size;
	uint64_t time_stamp;
} zend_perf_jitdump_record;

typedef struct _zend_perf_jitdump_load_record {
	zend_perf_jitdump_record hdr;
	uint32_t process_id;
	uint32_t thread_id;
	uint64_t vma;
	uint64_t code_address;
	uint64_t code_size;
	uint64_t code_id;
} zend_perf_jitdump_load_record;

static int   jitdump_fd  = -1;
static void *jitdump_mem = MAP_FAILED;

static uint64_t zend_perf_timestamp(void)
{
	struct timespec ts;

	if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
		return 0;
	}
	return ((uint64_t)ts.tv_sec * 1000000000) + ts.tv_nsec;
}

static void zend_jit_perf_jitdump_open(void)
{
	char filename[64];
	int fd, ret;
	zend_elf_header elf_hdr;
	zend_perf_jitdump_header jit_hdr;

	sprintf(filename, "/tmp/jit-%d.dump", getpid());
	if (!zend_perf_timestamp()) {
		return;
	}

#if defined(__linux__)
	fd = open("/proc/self/exe", O_RDONLY);
#elif defined(__NetBSD__)
	fd = open("/proc/curproc/exe", O_RDONLY);
#elif defined(__FreeBSD__) || defined(__DragonFly__)
	char path[PATH_MAX];
	size_t pathlen = sizeof(path);
	int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};
	if (sysctl(mib, 4, path, &pathlen, NULL, 0) == -1) {
		return;
	}
	fd = open(path, O_RDONLY);
#elif defined(__sun)
	fd = open("/proc/self/path/a.out", O_RDONLY);
#elif defined(__HAIKU__)
	char path[PATH_MAX];
	if (find_path(B_APP_IMAGE_SYMBOL, B_FIND_PATH_IMAGE_PATH,
		NULL, path, sizeof(path)) != B_OK) {
		return;
	}

	fd = open(path, O_RDONLY);
#else
	fd = -1;
#endif
	if (fd < 0) {
		return;
	}

	ret = read(fd, &elf_hdr, sizeof(elf_hdr));
	close(fd);

	if (ret != sizeof(elf_hdr) ||
	    elf_hdr.emagic[0] != 0x7f ||
	    elf_hdr.emagic[1] != 'E' ||
	    elf_hdr.emagic[2] != 'L' ||
	    elf_hdr.emagic[3] != 'F') {
		return;
	}

	jitdump_fd = open(filename, O_CREAT | O_TRUNC | O_RDWR, 0666);
	if (jitdump_fd < 0) {
		return;
	}

	jitdump_mem = mmap(NULL,
			sysconf(_SC_PAGESIZE),
			PROT_READ|PROT_EXEC,
			MAP_PRIVATE, jitdump_fd, 0);

	if (jitdump_mem == MAP_FAILED) {
		close(jitdump_fd);
		jitdump_fd = -1;
		return;
	}

	memset(&jit_hdr, 0, sizeof(jit_hdr));
	jit_hdr.magic           = ZEND_PERF_JITDUMP_HEADER_MAGIC;
	jit_hdr.version         = ZEND_PERF_JITDUMP_HEADER_VERSION;
	jit_hdr.size            = sizeof(jit_hdr);
	jit_hdr.elf_mach_target = elf_hdr.machine;
	jit_hdr.process_id      = getpid();
	jit_hdr.time_stamp      = zend_perf_timestamp();
	jit_hdr.flags           = 0;
	zend_quiet_write(jitdump_fd, &jit_hdr, sizeof(jit_hdr));
}

static void zend_jit_perf_jitdump_close(void)
{
	if (jitdump_fd >= 0) {
		zend_perf_jitdump_record rec;

		rec.event      = ZEND_PERF_JITDUMP_RECORD_CLOSE;
		rec.size       = sizeof(rec);
		rec.time_stamp = zend_perf_timestamp();
		zend_quiet_write(jitdump_fd, &rec, sizeof(rec));
		close(jitdump_fd);

		if (jitdump_mem != MAP_FAILED) {
			munmap(jitdump_mem, sysconf(_SC_PAGESIZE));
		}
	}
}

static void zend_jit_perf_jitdump_register(const char *name, void *start, size_t size)
{
	if (jitdump_fd >= 0) {
		static uint64_t id = 1;
		zend_perf_jitdump_load_record rec;
		size_t len = strlen(name);
		uint32_t thread_id = 0;
#if defined(__linux__)
		thread_id = syscall(SYS_gettid);
#elif defined(__darwin__)
		uint64_t thread_id_u64;
		pthread_threadid_np(NULL, &thread_id_u64);
		thread_id = (uint32_t) thread_id_u64;
#elif defined(__FreeBSD__)
		long tid;
		thr_self(&tid);
		thread_id = (uint32_t)tid;
#elif defined(__OpenBSD__)
		thread_id = getthrid();
#elif defined(__NetBSD__)
		thread_id = _lwp_self();
#elif defined(__DragonFly__)
		thread_id = lwp_gettid();
#elif defined(__sun)
		thread_id = thr_self();
#endif

		memset(&rec, 0, sizeof(rec));
		rec.hdr.event      = ZEND_PERF_JITDUMP_RECORD_LOAD;
		rec.hdr.size       = sizeof(rec) + len + 1 + size;
		rec.hdr.time_stamp = zend_perf_timestamp();
		rec.process_id     = getpid();
		rec.thread_id      = thread_id;
		rec.vma            = (uint64_t)(uintptr_t)start;
		rec.code_address   = (uint64_t)(uintptr_t)start;
		rec.code_size      = (uint64_t)size;
		rec.code_id        = id++;

		zend_quiet_write(jitdump_fd, &rec, sizeof(rec));
		zend_quiet_write(jitdump_fd, name, len + 1);
		zend_quiet_write(jitdump_fd, start, size);
	}
}

static void zend_jit_perf_map_register(const char *name, void *start, size_t size)
{
	static FILE *fp = NULL;

	if (!fp) {
		char filename[64];

		sprintf(filename, "/tmp/perf-%d.map", getpid());
		fp = fopen(filename, "w");
		if (!fp) {
			return;
		}
	    setlinebuf(fp);
	}
	fprintf(fp, "%zx %zx %s\n", (size_t)(uintptr_t)start, size, name);
}
