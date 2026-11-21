--TEST--
OOM printf() does not print the partial result
--INI--
memory_limit=64M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php

// The text before the specifier is already in the buffer when the growth for
// the field width fails. It must not be printed.
$format = "abc%1000000000d";

echo "Setup done", PHP_EOL;

$result = printf($format, 1);

echo "Not reached", PHP_EOL;
?>
--EXPECTF--
Setup done

Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line 9
