# -*- Makefile -*-
# Copyright Abandoned 2000 Monty Program KB & Detron HB
# This file is public domain and comes with NO WARRANTY of any kind

AUTOMAKE_OPTIONS=foreign
DEPTH     = ../../..
topsrcdir = @topsrcdir@
srcdir    = @srcdir@
VPATH     = @srcdir@

DEFS =			-DUNDEF_THREADS_HACK -U__USE_MISC
INCLUDES =		-I$(srcdir)
pkglib_LTLIBRARIES =	libmysql.la

libmysql_la_SOURCES = \
	my_init.c my_static.c my_malloc.c my_realloc.c my_create.c \
	my_delete.c my_tempnam.c my_open.c mf_casecnv.c my_read.c \
	my_write.c errors.c my_error.c my_getwd.c my_div.c mf_pack.c \
	my_messnc.c mf_dirname.c mf_fn_ext.c mf_wcomp.c typelib.c safemalloc.c \
	my_alloc.c mf_format.c mf_path.c mf_unixpath.c my_fopen.c mf_loadpath.c \
	my_pthread.c my_thr_init.c thr_mutex.c mulalloc.c string.c default.c \
	my_compress.c array.c my_once.c list.c my_net.c dbug.c \
	strmov.c strxmov.c strnmov.c strmake.c strend.c strfill.c \
	ct_init.c is_prefix.c int2str.c str2int.c strinstr.c \
	strcont.c strcend.c bchange.c bmove.c bmove_upp.c \
	longlong2str.c strtoull.c strtoll.c ctype-latin1.c
