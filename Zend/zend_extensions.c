/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#include "zend_extensions.h"

ZEND_API zend_llist zend_extensions;
ZEND_API uint32_t zend_extension_flags = 0;
ZEND_API int zend_op_array_extension_handles = 0;
static int last_resource_number;

int zend_load_extension(const char *path)
{
#if ZEND_EXTENSIONS_SUPPORT
	DL_HANDLE handle;

	handle = DL_LOAD(path);
	if (!handle) {
#ifndef ZEND_WIN32
		fprintf(stderr, "Failed loading %s:  %s\n", path, DL_ERROR());
#else
		fprintf(stderr, "Failed loading %s\n", path);
		/* See http://support.microsoft.com/kb/190351 */
		fflush(stderr);
#endif
		return FAILURE;
	}
	return zend_load_extension_handle(handle, path);
#else
	fprintf(stderr, "Extensions are not supported on this platform.\n");
/* See http://support.microsoft.com/kb/190351 */
#ifdef ZEND_WIN32
	fflush(stderr);
#endif
	return FAILURE;
#endif
}

int zend_load_extension_handle(DL_HANDLE handle, const char *path)
{
#if ZEND_EXTENSIONS_SUPPORT
	zend_extension *new_extension;
	zend_extension_version_info *extension_version_info;

	extension_version_info = (zend_extension_version_info *) DL_FETCH_SYMBOL(handle, "extension_version_info");
	if (!extension_version_info) {
		extension_version_info = (zend_extension_version_info *) DL_FETCH_SYMBOL(handle, "_extension_version_info");
	}
	new_extension = (zend_extension *) DL_FETCH_SYMBOL(handle, "zend_extension_entry");
	if (!new_extension) {
		new_extension = (zend_extension *) DL_FETCH_SYMBOL(handle, "_zend_extension_entry");
	}
	if (!extension_version_info || !new_extension) {
		fprintf(stderr, "%s doesn't appear to be a valid Zend extension\n", path);
/* See http://support.microsoft.com/kb/190351 */
#ifdef ZEND_WIN32
		fflush(stderr);
#endif
		DL_UNLOAD(handle);
		return FAILURE;
	}

	/* allow extension to proclaim compatibility with any Zend version */
	if (extension_version_info->zend_extension_api_no != ZEND_EXTENSION_API_NO &&(!new_extension->api_no_check || new_extension->api_no_check(ZEND_EXTENSION_API_NO) != SUCCESS)) {
		if (extension_version_info->zend_extension_api_no > ZEND_EXTENSION_API_NO) {
			fprintf(stderr, "%s requires Zend Engine API version %d.\n"
					"The Zend Engine API version %d which is installed, is outdated.\n\n",
					new_extension->name,
					extension_version_info->zend_extension_api_no,
					ZEND_EXTENSION_API_NO);
/* See http://support.microsoft.com/kb/190351 */
#ifdef ZEND_WIN32
			fflush(stderr);
#endif
			DL_UNLOAD(handle);
			return FAILURE;
		} else if (extension_version_info->zend_extension_api_no < ZEND_EXTENSION_API_NO) {
			fprintf(stderr, "%s requires Zend Engine API version %d.\n"
					"The Zend Engine API version %d which is installed, is newer.\n"
					"Contact %s at %s for a later version of %s.\n\n",
					new_extension->name,
					extension_version_info->zend_extension_api_no,
					ZEND_EXTENSION_API_NO,
					new_extension->author,
					new_extension->URL,
					new_extension->name);
/* See http://support.microsoft.com/kb/190351 */
#ifdef ZEND_WIN32
			fflush(stderr);
#endif
			DL_UNLOAD(handle);
			return FAILURE;
		}
	} else if (strcmp(ZEND_EXTENSION_BUILD_ID, extension_version_info->build_id) &&
	           (!new_extension->build_id_check || new_extension->build_id_check(ZEND_EXTENSION_BUILD_ID) != SUCCESS)) {
		fprintf(stderr, "Cannot load %s - it was built with configuration %s, whereas running engine is %s\n",
					new_extension->name, extension_version_info->build_id, ZEND_EXTENSION_BUILD_ID);
/* See http://support.microsoft.com/kb/190351 */
#ifdef ZEND_WIN32
		fflush(stderr);
#endif
		DL_UNLOAD(handle);
		return FAILURE;
	} else if (zend_get_extension(new_extension->name)) {
		fprintf(stderr, "Cannot load %s - it was already loaded\n", new_extension->name);
/* See http://support.microsoft.com/kb/190351 */
#ifdef ZEND_WIN32
		fflush(stderr);
#endif
		DL_UNLOAD(handle);
		return FAILURE;
	}

	return zend_register_extension(new_extension, handle);
#else
	fprintf(stderr, "Extensions are not supported on this platform.\n");
/* See http://support.microsoft.com/kb/190351 */
#ifdef ZEND_WIN32
	fflush(stderr);
#endif
	return FAILURE;
#endif
}


int zend_register_extension(zend_extension *new_extension, DL_HANDLE handle)
{
#if ZEND_EXTENSIONS_SUPPORT
	zend_extension extension;

	extension = *new_extension;
	extension.handle = handle;

	zend_extension_dispatch_message(ZEND_EXTMSG_NEW_EXTENSION, &extension);

	zend_llist_add_element(&zend_extensions, &extension);

	if (extension.op_array_ctor) {
		zend_extension_flags |= ZEND_EXTENSIONS_HAVE_OP_ARRAY_CTOR;
	}
	if (extension.op_array_dtor) {
		zend_extension_flags |= ZEND_EXTENSIONS_HAVE_OP_ARRAY_DTOR;
	}
	if (extension.op_array_handler) {
		zend_extension_flags |= ZEND_EXTENSIONS_HAVE_OP_ARRAY_HANDLER;
	}
	if (extension.op_array_persist_calc) {
		zend_extension_flags |= ZEND_EXTENSIONS_HAVE_OP_ARRAY_PERSIST_CALC;
	}
	if (extension.op_array_persist) {
		zend_extension_flags |= ZEND_EXTENSIONS_HAVE_OP_ARRAY_PERSIST;
	}
	/*fprintf(stderr, "Loaded %s, version %s\n", extension.name, extension.version);*/
#endif

	return SUCCESS;
}


static void zend_extension_shutdown(zend_extension *extension)
{
#if ZEND_EXTENSIONS_SUPPORT
	if (extension->shutdown) {
		extension->shutdown(extension);
	}
#endif
}

static int zend_extension_startup(zend_extension *extension)
{
#if ZEND_EXTENSIONS_SUPPORT
	if (extension->startup) {
		if (extension->startup(extension)!=SUCCESS) {
			return 1;
		}
		zend_append_version_info(extension);
	}
#endif
	return 0;
}


int zend_startup_extensions_mechanism()
{
	/* Startup extensions mechanism */
	zend_llist_init(&zend_extensions, sizeof(zend_extension), (void (*)(void *)) zend_extension_dtor, 1);
	zend_op_array_extension_handles = 0;
	last_resource_number = 0;
	return SUCCESS;
}


int zend_startup_extensions()
{
	zend_llist_apply_with_del(&zend_extensions, (int (*)(void *)) zend_extension_startup);
	return SUCCESS;
}


void zend_shutdown_extensions(void)
{
	zend_llist_apply(&zend_extensions, (llist_apply_func_t) zend_extension_shutdown);
	zend_llist_destroy(&zend_extensions);
}


void zend_extension_dtor(zend_extension *extension)
{
#if ZEND_EXTENSIONS_SUPPORT && !ZEND_DEBUG
	if (extension->handle && !getenv("ZEND_DONT_UNLOAD_MODULES")) {
		DL_UNLOAD(extension->handle);
	}
#endif
}


static void zend_extension_message_dispatcher(const zend_extension *extension, int num_args, va_list args)
{
	int message;
	void *arg;

	if (!extension->message_handler || num_args!=2) {
		return;
	}
	message = va_arg(args, int);
	arg = va_arg(args, void *);
	extension->message_handler(message, arg);
}


ZEND_API void zend_extension_dispatch_message(int message, void *arg)
{
	zend_llist_apply_with_arguments(&zend_extensions, (llist_apply_with_args_func_t) zend_extension_message_dispatcher, 2, message, arg);
}


ZEND_API int zend_get_resource_handle(zend_extension *extension)
{
	if (last_resource_number<ZEND_MAX_RESERVED_RESOURCES) {
		extension->resource_number = last_resource_number;
		return last_resource_number++;
	} else {
		return -1;
	}
}

ZEND_API int zend_get_op_array_extension_handle(void)
{
	return zend_op_array_extension_handles++;
}

ZEND_API zend_extension *zend_get_extension(const char *extension_name)
{
	zend_llist_element *element;

	for (element = zend_extensions.head; element; element = element->next) {
		zend_extension *extension = (zend_extension *) element->data;

		if (!strcmp(extension->name, extension_name)) {
			return extension;
		}
	}
	return NULL;
}

typedef struct _zend_extension_persist_data {
	zend_op_array *op_array;
	size_t         size;
	char          *mem;
} zend_extension_persist_data;

static void zend_extension_op_array_persist_calc_handler(zend_extension *extension, zend_extension_persist_data *data)
{
	if (extension->op_array_persist_calc) {
		data->size += extension->op_array_persist_calc(data->op_array);
	}
}

static void zend_extension_op_array_persist_handler(zend_extension *extension, zend_extension_persist_data *data)
{
	if (extension->op_array_persist) {
		size_t size = extension->op_array_persist(data->op_array, data->mem);
		if (size) {
			data->mem = (void*)((char*)data->mem + size);
			data->size += size;
		}
	}
}

ZEND_API size_t zend_extensions_op_array_persist_calc(zend_op_array *op_array)
{
	if (zend_extension_flags & ZEND_EXTENSIONS_HAVE_OP_ARRAY_PERSIST_CALC) {
		zend_extension_persist_data data;

		data.op_array = op_array;
		data.size = 0;
		data.mem  = NULL;
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_op_array_persist_calc_handler, &data);
		return data.size;
	}
	return 0;
}

ZEND_API size_t zend_extensions_op_array_persist(zend_op_array *op_array, void *mem)
{
	if (zend_extension_flags & ZEND_EXTENSIONS_HAVE_OP_ARRAY_PERSIST) {
		zend_extension_persist_data data;

		data.op_array = op_array;
		data.size = 0;
		data.mem  = mem;
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) zend_extension_op_array_persist_handler, &data);
		return data.size;
	}
	return 0;
}
