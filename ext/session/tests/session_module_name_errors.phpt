--TEST--
session_module_name(): errors
--EXTENSIONS--
session
--FILE--
<?php

try {
	var_dump(session_module_name("user"));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
	var_dump(session_module_name("fi\0le"));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: session_module_name(): Argument #1 ($module) must not be "user"
ValueError: session_module_name(): Argument #1 ($module) must not contain any null bytes
