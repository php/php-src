--TEST--
Bug GH-15552 (Signed integer overflow in ext/standard/scanf.c)
--FILE--
<?php

try {
	var_dump(sscanf('hello','%2147483648$s'));
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
ValueError: sscanf(): Argument #2 ($format) argument index %2147483648$ is out of range
