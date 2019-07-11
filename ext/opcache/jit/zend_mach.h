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
*/

#ifndef ZEND_MACHO
#define ZEND_MACHO

typedef struct _zend_mach_header {
	uint32_t magic;
	int cpu_type;
	int cpusubtype;
	uint32_t filetype;
	uint32_t ncmds;
	uint32_t sncmds;
	uint32_t flags;
} zend_mach_header;

#define MACHO_MAGIC 0xfeedface

typedef struct _zend_mach_header_64 {
	uint32_t magic;
	int cpu_type;
	int cpusubtype;
	uint32_t filetype;
	uint32_t ncmds;
	uint32_t sncmds;
	uint32_t flags;
	uint32_t padding;
} zend_mach_header_64;

#define MACHO_MAGIC64 0xfeedfacf

#endif
