# The embed SAPI

A server application programming interface (SAPI) is the entry point into the Zend Engine. The embed SAPI is a lightweight SAPI for calling into the Zend Engine from C or other languages that have C bindings.

## Basic Example

Below is a basic example in C that uses the embed SAPI to boot up the Zend Engine, start a request, and print the number of functions loaded in the function table.

```c
/* embed_sapi_basic_example.c */

#include <sapi/embed/php_embed.h>

int main(int argc, char **argv)
{
	/* Invokes the Zend Engine initialization phase: SAPI (SINIT), modules
	 * (MINIT), and request (RINIT). It also opens a 'zend_try' block to catch
	 * a zend_bailout().
	 */
	PHP_EMBED_START_BLOCK(argc, argv)

	php_printf(
		"Number of functions loaded: %d\n",
		zend_hash_num_elements(EG(function_table))
	);

	/* Close the 'zend_try' block and invoke the shutdown phase: request
	 * (RSHUTDOWN), modules (MSHUTDOWN), and SAPI (SSHUTDOWN).
	 */
	PHP_EMBED_END_BLOCK()
}
```

To compile this, we must point the compiler to the PHP header files. The paths to the header files are listed from `php-config --includes`.

We must also point the linker and the runtime loader to the `libphp.so` shared lib for linking PHP (`-lphp`) which is located at `$(php-config --prefix)/lib`. So the complete command to compile ends up being:

```bash
$  gcc \
	$(php-config --includes) \
	-L$(php-config --prefix)/lib \
	embed_sapi_basic_example.c \
	-lphp \
	-Wl,-rpath=$(php-config --prefix)/lib
```

> :memo: The embed SAPI is disabled by default. In order for the above example to compile, PHP must be built with the embed SAPI enabled. To see what SAPIs are installed, run `php-config --php-sapis`. If you don't see `embed` in the list, you'll need to rebuild PHP with `./configure --enable-embed`. The PHP shared library `libphp.so` is built when the embed SAPI is enabled.

If all goes to plan you should be able to run the program.

```bash
$ ./a.out 
Number of functions loaded: 1046
```

## Function call example

The following example calls `mt_rand()` and `var_dump()`s the return value.

```c
#include <main/php.h>
#include <ext/standard/php_var.h>
#include <sapi/embed/php_embed.h>

int main(int argc, char **argv)
{
	PHP_EMBED_START_BLOCK(argc, argv)

	zval retval = {0};
	zend_fcall_info fci = {0};
	zend_fcall_info_cache fci_cache = {0};

	zend_string *func_name = zend_string_init(ZEND_STRL("mt_rand"), 0);
	ZVAL_STR(&fci.function_name, func_name);

	fci.size = sizeof fci;
	fci.retval = &retval;

	if (zend_call_function(&fci, &fci_cache) == SUCCESS) {
		php_var_dump(&retval, 1);
	}

	zend_string_release(func_name);

	PHP_EMBED_END_BLOCK()
}
```

## Execute a PHP script example

```php
<?php

# example.php

echo 'Hello from userland!' . PHP_EOL;
```

```c
#include <sapi/embed/php_embed.h>

int main(int argc, char **argv)
{
	PHP_EMBED_START_BLOCK(argc, argv)

	zend_file_handle file_handle;
	zend_stream_init_filename(&file_handle, "example.php");

	if (php_execute_script(&file_handle) == FAILURE) {
		php_printf("Failed to execute PHP script.\n");
	}

	PHP_EMBED_END_BLOCK()
}
```

## INI defaults

The default value for 'error_prepend_string' is 'NULL'. The following example sets the INI default for 'error_prepend_string' to 'Embed SAPI error:'.

```c
#include <sapi/embed/php_embed.h>

/* This callback is invoked as soon as the configuration hash table is
 * allocated so any INI settings added via this callback will have the lowest
 * precedence and will allow INI files to overwrite them.
 */
static void example_ini_defaults(HashTable *configuration_hash)
{
	zval ini_value;
	ZVAL_NEW_STR(&ini_value, zend_string_init(ZEND_STRL("Embed SAPI error:"), /* persistent */ 1));
	zend_hash_str_update(configuration_hash, ZEND_STRL("error_prepend_string"), &ini_value);
}

int main(int argc, char **argv)
{
	php_embed_module.ini_defaults = example_ini_defaults;

	PHP_EMBED_START_BLOCK(argc, argv)

	zval retval;

	/* Generates an error by accessing an undefined variable '$a'. */
	if (zend_eval_stringl(ZEND_STRL("var_dump($a);"), &retval, "example") == FAILURE) {
		php_printf("Failed to eval PHP.\n");
	}

	PHP_EMBED_END_BLOCK()
}
```

After compiling and running, you should see:

```
Embed SAPI error:
Warning: Undefined variable $a in example on line 1
NULL
```

This default value is overwritable from INI files. We'll update one of the INI files (which can be found by running `$ php --ini`), and set `error_prepend_string="Oops!"`. We don't have to recompile the program, we can just run it again and we should see:

```
Oops!
Warning: Undefined variable $a in example on line 1
NULL
```
