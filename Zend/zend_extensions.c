/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2000 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.92 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_92.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#include "zend_extensions.h"

ZEND_API zend_llist zend_extensions;
static int last_resource_number;

int zend_load_extension(char *path)
{
#if ZEND_EXTENSIONS_SUPPORT
	DL_HANDLE handle;
	zend_extension *new_extension;
	zend_extension_version_info *extension_version_info;

	handle = DL_LOAD(path);
	if (!handle) {
#ifndef ZEND_WIN32
		fprintf(stderr, "Failed loading %s:  %s\n", path, dlerror());
#else
		fprintf(stderr, "Failed loading %s\n", path);
#endif
		return FAILURE;
	}

	extension_version_info = (zend_extension_version_info *) DL_FETCH_SYMBOL(handle, "extension_version_info");
	new_extension = (zend_extension *) DL_FETCH_SYMBOL(handle, "zend_extension_entry");
	if (!extension_version_info || !new_extension) {
		fprintf(stderr, "%s doesn't appear to be a valid Zend extension\n", path);
		return FAILURE;
	}


	/* allow extension to proclaim compatibility with any Zend version */
	if(extension_version_info->zend_extension_api_no != ZEND_EXTENSION_API_NO &&(!new_extension->api_no_check || new_extension->api_no_check(ZEND_EXTENSION_API_NO) != SUCCESS)) {
		if (extension_version_info->zend_extension_api_no > ZEND_EXTENSION_API_NO) {
			fprintf(stderr, "%s requires Zend Engine API version %d\n"
					"The installed Zend Engine API version is %d\n\n",
					new_extension->name,
					extension_version_info->zend_extension_api_no,
					ZEND_EXTENSION_API_NO);
			DL_UNLOAD(handle);
			return FAILURE;
		} else if (extension_version_info->zend_extension_api_no < ZEND_EXTENSION_API_NO) {
			fprintf(stderr, "%s designed to be used with the Zend Engine API %d is outdated\n"
					"It requires a more recent version of the Zend Engine\n"
					"The installed Zend Engine API version is %d\n"
					"Contact %s at %s for a later version of this module.\n\n",
					new_extension->name,
					extension_version_info->zend_extension_api_no,
					ZEND_EXTENSION_API_NO,
					new_extension->author,
					new_extension->URL);
			DL_UNLOAD(handle);
			return FAILURE;
		} 
	} else if (ZTS_V!=extension_version_info->thread_safe) {
		fprintf(stderr, "Cannot load %s - it %s thread safe, whereas Zend %s\n",
					new_extension->name,
					(extension_version_info->thread_safe?"is":"isn't"),
					(ZTS_V?"is":"isn't"));
		DL_UNLOAD(handle);
		return FAILURE;
	} else if (ZEND_DEBUG!=extension_version_info->debug) {
		fprintf(stderr, "Cannot load %s - it %s debug information, whereas Zend %s\n",
					new_extension->name,
					(extension_version_info->debug?"contains":"does not contain"),
					(ZEND_DEBUG?"does":"does not"));
		DL_UNLOAD(handle);
		return FAILURE;
	}

	return zend_register_extension(new_extension, handle);
#else
	fprintf(stderr, "Extensions are not supported on this platform.\n");
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

	zend_append_version_info(&extension);
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
	}
#endif
	return 0;
}


int zend_startup_extensions_mechanism()
{
	/* Startup extensions mechanism */
	zend_llist_init(&zend_extensions, sizeof(zend_extension), (void (*)(void *)) zend_extension_dtor, 1);
	last_resource_number = 0;
	return SUCCESS;
}


int zend_startup_extensions()
{
	zend_llist_apply_with_del(&zend_extensions, (int (*)(void *)) zend_extension_startup);
	return SUCCESS;
}


void zend_shutdown_extensions()
{
	zend_llist_apply(&zend_extensions, (void (*)(void *)) zend_extension_shutdown);
	zend_llist_destroy(&zend_extensions);
}


void zend_extension_dtor(zend_extension *extension)
{
#if ZEND_EXTENSIONS_SUPPORT && !ZEND_DEBUG
	if (extension->handle) {
		DL_UNLOAD(extension->handle);
	}
#endif
}


static void zend_extension_message_dispatcher(zend_extension *extension, int num_args, va_list args)
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


ZEND_API zend_extension *zend_get_extension(char *extension_name)
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
