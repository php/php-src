/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to the Zend license, that is bundled     |
   | with this package in the file LICENSE.  If you did not receive a     |
   | copy of the Zend license, please mail us at zend@zend.com so we can  |
   | send you a copy immediately.                                         |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

#include "zend_extensions.h"

ZEND_API zend_llist zend_extensions;
static int last_resource_number;

int zend_load_extensions(char **extension_paths)
{
	char **p = extension_paths;

	if (!p) {
		return SUCCESS;
	}
	while (*p) {
		if (zend_load_extension(*p)==FAILURE) {
			return FAILURE;
		}
		p++;
	}
	return SUCCESS;
}


int zend_load_extension(char *path)
{
#if ZEND_EXTENSIONS_SUPPORT
	DL_HANDLE handle;
	zend_extension extension, *new_extension;
	zend_extension_version_info *extension_version_info;

	handle = DL_LOAD(path);
	if (!handle) {
		zend_printf("Failed loading %s\n", path);
		return FAILURE;
	}

	extension_version_info = (zend_extension_version_info *) DL_FETCH_SYMBOL(handle, "extension_version_info");
	new_extension = (zend_extension *) DL_FETCH_SYMBOL(handle, "zend_extension_entry");
	if (!extension_version_info || !new_extension) {
		zend_printf("%s doesn't appear to be a valid Zend extension\n", path);
		return FAILURE;
	}

	if (extension_version_info->zend_extension_api_no > ZEND_EXTENSION_API_NO) {
		zend_printf("%s requires Zend version %s or later\n"
					"Current version %s, API version %d\n",
					new_extension->name,
					extension_version_info->required_zend_version,
					ZEND_VERSION,
					ZEND_EXTENSION_API_NO);
		DL_UNLOAD(handle);
		return FAILURE;
	} else if (extension_version_info->zend_extension_api_no < ZEND_EXTENSION_API_NO) {
		/* we may be able to allow for downwards compatability in some harmless cases. */
		zend_printf("%s is outdated (API version %d, current version %d)\n"
					"Contact %s at %s for a later version of this module.\n",
					new_extension->name,
					extension_version_info->zend_extension_api_no,
					ZEND_EXTENSION_API_NO,
					new_extension->author,
					new_extension->URL);
		DL_UNLOAD(handle);
		return FAILURE;
	} else if (ZTS_V!=extension_version_info->thread_safe) {
		zend_printf("Cannot load %s - it %s thread safe, whereas Zend %s\n",
					new_extension->name,
					(extension_version_info->thread_safe?"is":"isn't"),
					(ZTS_V?"is":"isn't"));
		DL_UNLOAD(handle);
		return FAILURE;
	}

	if (new_extension->startup) {
		if (new_extension->startup(new_extension)!=SUCCESS) {
			DL_UNLOAD(handle);
			return FAILURE;
		}
	}
	extension = *new_extension;
	extension.handle = handle;

	zend_llist_add_element(&zend_extensions, &extension);

	/*zend_printf("Loaded %s, version %s\n", extension.name, extension.version);*/

	zend_append_version_info(&extension);
	return SUCCESS;
#else
	zend_printf("Extensions are not supported on this platform.\n");
	return FAILURE
#endif
}

static void zend_extension_shutdown(zend_extension *extension)
{
#if ZEND_EXTENSIONS_SUPPORT
	if (extension->shutdown) {
		extension->shutdown(extension);
	}
#endif
}


void zend_shutdown_extensions()
{
	zend_llist_apply(&zend_extensions, (void (*)(void *)) zend_extension_shutdown);
	zend_llist_destroy(&zend_extensions);
}


void zend_extension_dtor(zend_extension *extension)
{
#if ZEND_EXTENSIONS_SUPPORT
	DL_UNLOAD(extension->handle);
#endif
}


ZEND_API int zend_get_resource_handle(zend_extension *extension)
{
	if (last_resource_number<4) {
		extension->resource_number = last_resource_number;
		return last_resource_number;
	} else {
		return -1;
	}
}
