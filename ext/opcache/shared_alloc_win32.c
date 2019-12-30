/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "ZendAccelerator.h"
#include "zend_shared_alloc.h"
#include "zend_accelerator_util_funcs.h"
#include "zend_execute.h"
#include "SAPI.h"
#include "tsrm_win32.h"
#include <winbase.h>
#include <process.h>
#include <LMCONS.H>

#define ACCEL_FILEMAP_NAME "ZendOPcache.SharedMemoryArea"
#define ACCEL_MUTEX_NAME "ZendOPcache.SharedMemoryMutex"
#define ACCEL_FILEMAP_BASE_DEFAULT 0x01000000
#define ACCEL_FILEMAP_BASE "ZendOPcache.MemoryBase"
#define ACCEL_EVENT_SOURCE "Zend OPcache"

static HANDLE memfile = NULL, memory_mutex = NULL;
static void *mapping_base;

#define MAX_MAP_RETRIES 25

static void zend_win_error_message(int type, char *msg, int err)
{
	LPVOID lpMsgBuf;
	HANDLE h;
	char *ev_msgs[2];

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL
	);

	h = RegisterEventSource(NULL, TEXT(ACCEL_EVENT_SOURCE));
	ev_msgs[0] = msg;
	ev_msgs[1] = lpMsgBuf;
	ReportEvent(h,				  // event log handle
            EVENTLOG_ERROR_TYPE,  // event type
            0,                    // category zero
            err,				  // event identifier
            NULL,                 // no user security identifier
            2,                    // one substitution string
            0,                    // no data
            ev_msgs,              // pointer to string array
            NULL);                // pointer to data
	DeregisterEventSource(h);

	LocalFree( lpMsgBuf );

	zend_accel_error(type, "%s", msg);
}

static char *create_name_with_username(char *name)
{
	static char newname[MAXPATHLEN + UNLEN + 4 + 1 + 32 + 21];
	char *uname;

	uname = php_win32_get_username();
	if (!uname) {
		return NULL;
	}
	snprintf(newname, sizeof(newname) - 1, "%s@%s@%.20s@%.32s", name, uname, sapi_module.name, ZCG(system_id));

	free(uname);

	return newname;
}

static char *get_mmap_base_file(void)
{
	static char windir[MAXPATHLEN+UNLEN + 3 + sizeof("\\\\@") + 1 + 32 + 21];
	char *uname;
	int l;

	uname = php_win32_get_username();
	if (!uname) {
		return NULL;
	}
	GetTempPath(MAXPATHLEN, windir);
	l = strlen(windir);
	if ('\\' == windir[l-1]) {
		l--;
	}
	snprintf(windir + l, sizeof(windir) - l - 1, "\\%s@%s@%.20s@%.32s", ACCEL_FILEMAP_BASE, uname, sapi_module.name, ZCG(system_id));

	free(uname);

	return windir;
}

void zend_shared_alloc_create_lock(void)
{
	memory_mutex = CreateMutex(NULL, FALSE, create_name_with_username(ACCEL_MUTEX_NAME));
	if (!memory_mutex) {
		zend_accel_error(ACCEL_LOG_FATAL, "Cannot create mutex");
		return;
	}
	ReleaseMutex(memory_mutex);
}

void zend_shared_alloc_lock_win32(void)
{
	DWORD waitRes = WaitForSingleObject(memory_mutex, INFINITE);

	if (waitRes == WAIT_FAILED) {
		zend_accel_error(ACCEL_LOG_ERROR, "Cannot lock mutex");
	}
}

void zend_shared_alloc_unlock_win32(void)
{
	ReleaseMutex(memory_mutex);
}

static int zend_shared_alloc_reattach(size_t requested_size, char **error_in)
{
	int err;
	void *wanted_mapping_base;
	char *mmap_base_file = get_mmap_base_file();
	FILE *fp = fopen(mmap_base_file, "r");
	MEMORY_BASIC_INFORMATION info;
	void *execute_ex_base;
	int execute_ex_moved;

	if (!fp) {
		err = GetLastError();
		zend_win_error_message(ACCEL_LOG_WARNING, mmap_base_file, err);
		zend_win_error_message(ACCEL_LOG_FATAL, "Unable to open base address file", err);
		*error_in="fopen";
		return ALLOC_FAILURE;
	}
	if (!fscanf(fp, "%p", &wanted_mapping_base)) {
		err = GetLastError();
		zend_win_error_message(ACCEL_LOG_FATAL, "Unable to read base address", err);
		*error_in="read mapping base";
		fclose(fp);
		return ALLOC_FAILURE;
	}
	if (!fscanf(fp, "%p", &execute_ex_base)) {
		err = GetLastError();
		zend_win_error_message(ACCEL_LOG_FATAL, "Unable to read execute_ex base address", err);
		*error_in="read execute_ex base";
		fclose(fp);
		return ALLOC_FAILURE;
	}
	fclose(fp);

	if (0 > win32_utime(mmap_base_file, NULL)) {
		err = GetLastError();
		zend_win_error_message(ACCEL_LOG_WARNING, mmap_base_file, err);
	}

	execute_ex_moved = (void *)execute_ex != execute_ex_base;

	/* Check if execute_ex is at the same address and if the requested address space is free */
	if (execute_ex_moved ||
	    VirtualQuery(wanted_mapping_base, &info, sizeof(info)) == 0 ||
	    info.State != MEM_FREE ||
	    info.RegionSize < requested_size) {
#if ENABLE_FILE_CACHE_FALLBACK
		if (ZCG(accel_directives).file_cache && ZCG(accel_directives).file_cache_fallback) {
			size_t pre_size, wanted_mb_save;

			wanted_mb_save = (size_t)wanted_mapping_base;

			if (execute_ex_moved) {
				err = ERROR_INVALID_ADDRESS;
				zend_win_error_message(ACCEL_LOG_WARNING, "Opcode handlers are unusable due to ASLR (fall-back to file cache)", err);
			} else {
				err = ERROR_INVALID_ADDRESS;
				zend_win_error_message(ACCEL_LOG_WARNING, "Base address marks unusable memory region (fall-back to file cache)", err);
			}

			pre_size = ZEND_ALIGNED_SIZE(sizeof(zend_smm_shared_globals)) + ZEND_ALIGNED_SIZE(sizeof(zend_shared_segment)) + ZEND_ALIGNED_SIZE(sizeof(void *)) + ZEND_ALIGNED_SIZE(sizeof(int));
			/* Map only part of SHM to have access opcache shared globals */
			mapping_base = MapViewOfFileEx(memfile, FILE_MAP_ALL_ACCESS, 0, 0, pre_size + ZEND_ALIGNED_SIZE(sizeof(zend_accel_shared_globals)), NULL);
			if (mapping_base == NULL) {
				err = GetLastError();
				zend_win_error_message(ACCEL_LOG_FATAL, "Unable to reattach to opcache shared globals", err);
				return ALLOC_FAILURE;
			}
			accel_shared_globals = (zend_accel_shared_globals *)((char *)((zend_smm_shared_globals *)mapping_base)->app_shared_globals + ((char *)mapping_base - (char *)wanted_mb_save));

			return ALLOC_FALLBACK;
		}
#endif
		if (execute_ex_moved) {
			err = ERROR_INVALID_ADDRESS;
			zend_win_error_message(ACCEL_LOG_FATAL, "Opcode handlers are unusable due to ASLR. Please setup opcache.file_cache and opcache.file_cache_fallback directives for more convenient Opcache usage", err);
		} else {
			err = ERROR_INVALID_ADDRESS;
			zend_win_error_message(ACCEL_LOG_FATAL, "Base address marks unusable memory region. Please setup opcache.file_cache and opcache.file_cache_fallback directives for more convenient Opcache usage", err);
		}
		return ALLOC_FAILURE;
	}

	mapping_base = MapViewOfFileEx(memfile, FILE_MAP_ALL_ACCESS, 0, 0, 0, wanted_mapping_base);

	if (mapping_base == NULL) {
		err = GetLastError();
		if (err == ERROR_INVALID_ADDRESS) {
			zend_win_error_message(ACCEL_LOG_FATAL, "Unable to reattach to base address", err);
			return ALLOC_FAILURE;
		}
		return ALLOC_FAIL_MAPPING;
	}
	smm_shared_globals = (zend_smm_shared_globals *) mapping_base;

	return SUCCESSFULLY_REATTACHED;
}

static int create_segments(size_t requested_size, zend_shared_segment ***shared_segments_p, int *shared_segments_count, char **error_in)
{
	int err = 0, ret;
	zend_shared_segment *shared_segment;
	int map_retries = 0;
	void *default_mapping_base_set[] = { 0, 0 };
	/* TODO:
	  improve fixed addresses on x64. It still makes no sense to do it as Windows addresses are virtual per se and can or should be randomized anyway
	  through Address Space Layout Radomization (ASLR). We can still let the OS do its job and be sure that each process gets the same address if
	  desired. Not done yet, @zend refused but did not remember the exact reason, pls add info here if one of you know why :)
	*/
#if defined(_WIN64)
	void *vista_mapping_base_set[] = { (void *) 0x0000100000000000, (void *) 0x0000200000000000, (void *) 0x0000300000000000, (void *) 0x0000700000000000, 0 };
	DWORD size_high = (requested_size >> 32), size_low = (requested_size & 0xffffffff);
#else
	void *vista_mapping_base_set[] = { (void *) 0x20000000, (void *) 0x21000000, (void *) 0x30000000, (void *) 0x31000000, (void *) 0x50000000, 0 };
	DWORD size_high = 0, size_low = requested_size;
#endif
	void **wanted_mapping_base = default_mapping_base_set;

	zend_shared_alloc_lock_win32();
	/* Mapping retries: When Apache2 restarts, the parent process startup routine
	   can be called before the child process is killed. In this case, the map will fail
	   and we have to sleep some time (until the child releases the mapping object) and retry.*/
	do {
		memfile = OpenFileMapping(FILE_MAP_WRITE, 0, create_name_with_username(ACCEL_FILEMAP_NAME));
		if (memfile == NULL) {
			err = GetLastError();
			break;
		}

		ret =  zend_shared_alloc_reattach(requested_size, error_in);
		if (ret == ALLOC_FAIL_MAPPING) {
			err = GetLastError();
			/* Mapping failed, wait for mapping object to get freed and retry */
			CloseHandle(memfile);
			memfile = NULL;
			if (++map_retries >= MAX_MAP_RETRIES) {
				break;
			}
			zend_shared_alloc_unlock_win32();
			Sleep(1000 * (map_retries + 1));
			zend_shared_alloc_lock_win32();
		} else {
			zend_shared_alloc_unlock_win32();
			return ret;
		}
	} while (1);

	if (map_retries == MAX_MAP_RETRIES) {
		zend_shared_alloc_unlock_win32();
		zend_win_error_message(ACCEL_LOG_FATAL, "Unable to open file mapping", err);
		*error_in = "OpenFileMapping";
		return ALLOC_FAILURE;
	}

	/* creating segment here */
	*shared_segments_count = 1;
	*shared_segments_p = (zend_shared_segment **) calloc(1, sizeof(zend_shared_segment)+sizeof(void *));
	if (!*shared_segments_p) {
		err = GetLastError();
		zend_shared_alloc_unlock_win32();
		zend_win_error_message(ACCEL_LOG_FATAL, "calloc() failed", err);
		*error_in = "calloc";
		return ALLOC_FAILURE;
	}
	shared_segment = (zend_shared_segment *)((char *)(*shared_segments_p) + sizeof(void *));
	(*shared_segments_p)[0] = shared_segment;

	memfile	= CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, size_high, size_low,
								create_name_with_username(ACCEL_FILEMAP_NAME));
	if (memfile == NULL) {
		err = GetLastError();
		zend_shared_alloc_unlock_win32();
		zend_win_error_message(ACCEL_LOG_FATAL, "Unable to create file mapping", err);
		*error_in = "CreateFileMapping";
		return ALLOC_FAILURE;
	}

	/* Starting from windows Vista, heap randomization occurs which might cause our mapping base to
	   be taken (fail to map). So under Vista, we try to map into a hard coded predefined addresses
	   in high memory. */
	if (!ZCG(accel_directives).mmap_base || !*ZCG(accel_directives).mmap_base) {
		wanted_mapping_base = vista_mapping_base_set;
	} else {
		char *s = ZCG(accel_directives).mmap_base;

		/* skip leading 0x, %p assumes hexdeciaml format anyway */
		if (*s == '0' && *(s + 1) == 'x') {
			s += 2;
		}
		if (sscanf(s, "%p", &default_mapping_base_set[0]) != 1) {
			zend_shared_alloc_unlock_win32();
			zend_win_error_message(ACCEL_LOG_FATAL, "Bad mapping address specified in opcache.mmap_base", err);
			return ALLOC_FAILURE;
		}
	}

	do {
		shared_segment->p = mapping_base = MapViewOfFileEx(memfile, FILE_MAP_ALL_ACCESS, 0, 0, 0, *wanted_mapping_base);
		if (*wanted_mapping_base == NULL) { /* Auto address (NULL) is the last option on the array */
			break;
		}
		wanted_mapping_base++;
	} while (!mapping_base);

	if (mapping_base == NULL) {
		err = GetLastError();
		zend_shared_alloc_unlock_win32();
		zend_win_error_message(ACCEL_LOG_FATAL, "Unable to create view for file mapping", err);
		*error_in = "MapViewOfFile";
		return ALLOC_FAILURE;
	} else {
		char *mmap_base_file = get_mmap_base_file();
		void *execute_ex_base = (void *)execute_ex;
		FILE *fp = fopen(mmap_base_file, "w");
		if (!fp) {
			err = GetLastError();
			zend_shared_alloc_unlock_win32();
			zend_win_error_message(ACCEL_LOG_WARNING, mmap_base_file, err);
			zend_win_error_message(ACCEL_LOG_FATAL, "Unable to write base address", err);
			return ALLOC_FAILURE;
		}
		fprintf(fp, "%p\n", mapping_base);
		fprintf(fp, "%p\n", execute_ex_base);
		fclose(fp);
	}

	shared_segment->pos = 0;
	shared_segment->size = requested_size;

	zend_shared_alloc_unlock_win32();

	return ALLOC_SUCCESS;
}

static int detach_segment(zend_shared_segment *shared_segment)
{
	zend_shared_alloc_lock_win32();
	if (mapping_base) {
		UnmapViewOfFile(mapping_base);
		mapping_base = NULL;
	}
	CloseHandle(memfile);
	memfile = NULL;
	zend_shared_alloc_unlock_win32();
	CloseHandle(memory_mutex);
	memory_mutex = NULL;
	return 0;
}

static size_t segment_type_size(void)
{
	return sizeof(zend_shared_segment);
}

zend_shared_memory_handlers zend_alloc_win32_handlers = {
	create_segments,
	detach_segment,
	segment_type_size
};
