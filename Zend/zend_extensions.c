/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2003 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

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
		fprintf(stderr, "Failed loading %s:  %s\n", path, DL_ERROR());
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
	if (extension_version_info->zend_extension_api_no != ZEND_EXTENSION_API_NO &&(!new_extension->api_no_check || new_extension->api_no_check(ZEND_EXTENSION_API_NO) != SUCCESS)) {
		if (extension_version_info->zend_extension_api_no > ZEND_EXTENSION_API_NO) {
			fprintf(stderr, "%s requires Zend Engine API version %d.\n"
					"The Zend Engine API version %d which is installed, is outdated.\n\n",
					new_extension->name,
					extension_version_info->zend_extension_api_no,
					ZEND_EXTENSION_API_NO);
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

	/*fprintf(stderr, "Loaded %s, version %s\n", extension.name, extension.version);*/
#endif

	return SUCCESS;
}


static void zend_extension_shutdown(zend_extension *extension TSRMLS_DC)
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
	last_resource_number = 0;
	return SUCCESS;
}


int zend_startup_extensions()
{
	zend_llist_apply_with_del(&zend_extensions, (int (*)(void *)) zend_extension_startup);
	return SUCCESS;
}


void zend_shutdown_extensions(TSRMLS_D)
{
	zend_llist_apply(&zend_extensions, (llist_apply_func_t) zend_extension_shutdown TSRMLS_CC);
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


static void zend_extension_message_dispatcher(zend_extension *extension, int num_args, va_list args TSRMLS_DC)
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
	TSRMLS_FETCH();

	zend_llist_apply_with_arguments(&zend_extensions, (llist_apply_with_args_func_t) zend_extension_message_dispatcher TSRMLS_CC, 2, message, arg);
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

/*
 * Support for dynamic loading of MH_BUNDLEs on Darwin / Mac OS X
 *
 */
 
#if HAVE_MACH_O_DYLD_H

void *zend_mh_bundle_load(char* bundle_path) 
{
	NSObjectFileImage bundle_image;
	NSModule bundle_handle;
	NSSymbol bundle_init_nssymbol;
	void (*bundle_init)(void);

	if (NSCreateObjectFileImageFromFile(bundle_path, &bundle_image) != NSObjectFileImageSuccess) {
		return NULL;
	}
	
	bundle_handle = NSLinkModule(bundle_image, bundle_path, NSLINKMODULE_OPTION_PRIVATE);
	NSDestroyObjectFileImage(bundle_image);
	
	/* call the init function of the bundle */
	bundle_init_nssymbol = NSLookupSymbolInModule(bundle_handle, "__init");
	if (bundle_init_nssymbol != NULL) {
		bundle_init = NSAddressOfSymbol(bundle_init_nssymbol);
		bundle_init();
	}
	
	return bundle_handle;
}

int zend_mh_bundle_unload(void *bundle_handle)
{
	NSSymbol bundle_fini_nssymbol;
	void (*bundle_fini)(void);
	
	/* call the fini function of the bundle */
	bundle_fini_nssymbol = NSLookupSymbolInModule(bundle_handle, "__fini");
	if (bundle_fini_nssymbol != NULL) {
		bundle_fini = NSAddressOfSymbol(bundle_fini_nssymbol);
		bundle_fini();
	}
	
	return (int) NSUnLinkModule(bundle_handle, NULL);
}

void *zend_mh_bundle_symbol(void *bundle_handle, const char *symbol_name)
{
	NSSymbol symbol;
	symbol = NSLookupSymbolInModule(bundle_handle, symbol_name);
	return NSAddressOfSymbol(symbol);
}

const char *zend_mh_bundle_error(void)
{
	/* Witness the state of the art error reporting */
	return NULL;
}

#endif /* HAVE_MACH_O_DYLD_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
